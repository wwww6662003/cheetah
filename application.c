/*
  +----------------------------------------------------------------------+
  | Cheetah  Framework                                                   |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: wangwei(wwww6662003@163.com)                                                             |
  +----------------------------------------------------------------------+
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "ext/standard/url.h"
#include "SAPI.h"
#include "zend/zend.h"

#include "cheetah_const.h"
#include "php_cheetah.h"
#include "application.h"
#include "router.h"
#include "util.h"

/** {{{ current CheetahApplicationCore object
 */
zend_class_entry * cheetah_application_core_ce;
/* }}} */

/** {{{ don't find file and return 0。 
*/
zend_bool cheetah_eval(long type, const char * src, uint len, zval ** ret TSRMLS_DC) {
    zval src_z;
    zend_bool failure_retval = 0;
    char * eval_desc;
    zend_op_array * new_op_array = NULL;
    if(!src || ((ZEND_EVAL!=type) && !cheetah_is_readable(src))) return 0;
    if(!len) len = strlen(src);
    INIT_ZVAL(src_z);
    ZVAL_STRINGL(&src_z, (char*)src, len, 0);
    switch (type) {
        case ZEND_INCLUDE_ONCE:
        case ZEND_REQUIRE_ONCE: {
                zend_file_handle fh;
                if (IS_ABSOLUTE_PATH(src, len)) {
                    cwd_state state;
                    state.cwd_length = 0;
                    state.cwd = (char*)malloc(1);
                    state.cwd[0] = 0;
                    failure_retval = (!virtual_file_ex(&state, src, NULL, 1) &&
                        zend_hash_exists(&EG(included_files), state.cwd, state.cwd_length+1));
                    free(state.cwd);
                }
                if (failure_retval) { 
					/* do nothing */
                } else if (SUCCESS == zend_stream_open(src, &fh TSRMLS_CC)) {
                    if (!fh.opened_path) fh.opened_path = estrndup(src, len);
                    if (zend_hash_add_empty_element(&EG(included_files), fh.opened_path, 
                        strlen(fh.opened_path)+1)==SUCCESS) {
                        new_op_array = zend_compile_file(&fh, 
                            (type==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
                        zend_destroy_file_handle(&fh TSRMLS_CC);
                    } else {
                        zend_file_handle_dtor(&fh);
                        failure_retval=1;
                    }
                } else {
                    zend_message_dispatcher(((type==ZEND_INCLUDE_ONCE) ?
                        ZMSG_FAILED_INCLUDE_FOPEN : ZMSG_FAILED_REQUIRE_FOPEN), (char*)src);
                }
            }
            break;
        case ZEND_INCLUDE:
        case ZEND_REQUIRE:
            new_op_array = compile_filename(type, &src_z TSRMLS_CC);
            break;
        case ZEND_EVAL: {
                eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);
                new_op_array = zend_compile_string(&src_z, eval_desc TSRMLS_CC);
                efree(eval_desc);
            }
            break;
        EMPTY_SWITCH_DEFAULT_CASE()
    }
    if (new_op_array) { /** NOTE: 借鉴于 eaccelerator。 */
        zend_execute_data execute_data;
        zval ** org_retval, * org_exception = NULL, * tmp_ret = NULL;
        zend_function_state * org_func_state;
        zend_op_array * org_op_array;
        zend_op ** org_opline_ptr;
        execute_data = * EG(current_execute_data);
        // backup 
        org_retval = EG(return_value_ptr_ptr);
        org_exception = EG(exception);
        org_func_state = EG(function_state_ptr);
        org_op_array = EG(active_op_array);
        org_opline_ptr = EG(opline_ptr);
        // set 
        EG(return_value_ptr_ptr) = &tmp_ret;
        EG(exception) = NULL;
        EG(active_op_array) = new_op_array;
        zend_execute(new_op_array TSRMLS_CC);
        // restore 
        EG(return_value_ptr_ptr) = org_retval;
        EG(function_state_ptr) = org_func_state;
        EG(active_op_array) = org_op_array;
        EG(opline_ptr) = org_opline_ptr;
        if (!ret) {
            if (tmp_ret) zval_ptr_dtor(&tmp_ret);
        } else { /* return value is used */
            if (!tmp_ret) { /* there was no return statement */
                ALLOC_ZVAL(tmp_ret);
                INIT_PZVAL(tmp_ret);
                Z_LVAL_P(tmp_ret) = 1;
                Z_TYPE_P(tmp_ret) = IS_BOOL;
            }
            *ret = tmp_ret;
        }
        destroy_op_array(new_op_array TSRMLS_CC);
        efree(new_op_array);
        if(org_exception) {
            if(EG(exception)) zval_ptr_dtor(&EG(exception));
            EG(exception) = org_exception;
        } else if(EG(exception)) return 0;
    } else if (ret) {
        zval_ptr_dtor(ret);
        MAKE_STD_ZVAL(*ret);
        ZVAL_BOOL(*ret, failure_retval);
    }
    return 1;
}

/* }}} */
/** 文件是否存在以及是否可读。 */
zend_bool cheetah_is_readable(char * path) {
    FILE * fp;
    if(!path || '\0'==*path) return 0;
    fp = fopen(path, "r");
    if(fp) fclose(fp);
    return fp != 0;
}
/* }}} */
/** 类名为空也返回0。 */
zval * cheetah_create_object(const char * name TSRMLS_DC) {
    zend_class_entry ** ce;
    uint name_len;
    zval * ins;
    char * func/* = "__construct"*/;
    uint flag;
	
    if(!(name) || '\0'==*(name)) return NULL;
    name_len = strlen(name);
    if (zend_lookup_class((char*)name, name_len, &ce TSRMLS_CC) == FAILURE
        || (*ce)->ce_flags & ZEND_ACC_INTERFACE
        || (*ce)->ce_flags & ZEND_ACC_ABSTRACT)
        return NULL;
	
    MAKE_STD_ZVAL(ins);
    if(object_init_ex(ins, *ce) == FAILURE) {
        zval_ptr_dtor(&ins);
        return NULL;
    }
    func = (*ce)->constructor->common.function_name; /* 由于基类，constructor总是存在 */
    flag = cheetah_call_user_function(&ins, func, NULL, 0, NULL TSRMLS_CC);
    if(FAILURE == flag) {
        zval_ptr_dtor(&ins);
        ins = NULL;
    }
    return ins;
}
/* }}} */
/** 返回FAILURE或SUCCESS，如果之前有异常产生则立刻返回FAILURE。 */

uint cheetah_call_user_function(zval ** obj, const char * func, zval ** ret, 
        zend_uint p_cnt, zval *ps[] TSRMLS_DC) {
    /** char * tip; */
    zval f, * r;
    uint flag;
    if(EG(exception)) {
        /*if(obj)
            spprintf(&tip, 0, "调用%s::%s()方法前有异常发生，因此无法正确执行。", 
                Z_OBJCE_PP(obj)->name, func);
        else spprintf(&tip, 0, "调用函数%s()前有异常发生，因此无法正确执行。", func);
        zend_error(E_WARNING, tip); E_ERROR: 终止运行 
        efree(tip); */
        return FAILURE;
    }
    MAKE_STD_ZVAL(r);
    ZVAL_STRING(&f, (char*)func, 0);
    if(!obj) {
        flag = call_user_function(EG(function_table), NULL, &f, r, p_cnt, ps TSRMLS_CC);
        if(FAILURE == flag)
            zend_throw_exception_ex(NULL, NULL TSRMLS_CC, 
                "call %s function fail. ", func);
    } else {
        flag = call_user_function(NULL, (zval**)obj, &f, r, p_cnt, ps TSRMLS_CC);
        if(FAILURE == flag)
            zend_throw_exception_ex(NULL, NULL TSRMLS_CC, 
                "call %s::%s() method fail.", Z_OBJCE_PP(obj)->name, func);
    }
    if(!ret || FAILURE == flag) zval_ptr_dtor(&r);
    else *ret = r;
    return flag;

}
/* }}} */

/** {{{ public CheetahApplicationCore::__construct()
 */
PHP_METHOD(cheetah_application_core, __construct) {
    zval *self = NULL;
    zval *controller = NULL;
    zval *action = NULL;
    zval *param = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zzz"
                             , &controller
                             , &action
                             , &param
                             ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    self = getThis();
	if (controller) {
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_CONTROLLER , controller);
	}
    if (action) {
        CHEETAH_UPDATE_PROPERTY(self, CHEETAH_ACTION , action);
    }
    if (param) {
        CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PARAM , param);
    }
}
/* }}} */

/** {{{ public CheetahApplicationCore::getController()
 */
PHP_METHOD(cheetah_application_core, getController) {
    zval *self = NULL;
	char *controller = NULL;
    self = getThis();
	if(CHEETAH_G(controller)){
		controller = CHEETAH_G(controller);
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_CONTROLLER , controller);
		RETURN_STRING(controller, 1);
	}else{
		controller = NULL;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_CONTROLLER , controller);
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ public CheetahApplicationCore::getAction()
 */
PHP_METHOD(cheetah_application_core, getAction) {
    zval *self = NULL;
	char *action = NULL;
    self = getThis();
    if(CHEETAH_G(action)){
		action = CHEETAH_G(action);
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_ACTION , action);
		RETURN_STRING(action, 1);
	}else{
		action=NULL;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_ACTION , action);
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ public CheetahApplicationCore::getParam()
 */
PHP_METHOD(cheetah_application_core, getParam) {
	zval *self=NULL;
    zval *param = NULL;
	zval *get_t=NULL;
	zval *post_t=NULL;
	self=getThis();
    if(CHEETAH_G(re_get)&&CHEETAH_G(re_post)==NULL){
		param=CHEETAH_G(re_get);
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PARAM , param);
		RETURN_ZVAL(param, 1,0);
	}else if(CHEETAH_G(re_get)==NULL&&CHEETAH_G(re_post)){
		param=CHEETAH_G(re_post);
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PARAM , param);
		RETURN_ZVAL(param, 1,0);
	}else if(CHEETAH_G(re_get)&&CHEETAH_G(re_post)){
		get_t=CHEETAH_G(re_get);
		post_t=CHEETAH_G(re_post);
		php_array_merge(Z_ARRVAL_P(get_t), Z_ARRVAL_P(post_t), 0 TSRMLS_CC);
		param=get_t;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PARAM , param);
		RETURN_ZVAL(param, 1,0);
	}else{
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PARAM , NULL);
		RETURN_NULL();
	}
    
}
/* }}} */

/** {{{ public CheetahApplicationCore::toArray()
 */
PHP_METHOD(cheetah_application_core, toArray) {
    zval *self = NULL;
    zval *array =NULL;
    zval *controller = NULL;
    zval *action = NULL;
    zval *param = NULL;
    self = getThis();
    controller = CHEETAH_READ_PROPERTY(self, CHEETAH_CONTROLLER);
    action  = CHEETAH_READ_PROPERTY(self, CHEETAH_ACTION);
    param = CHEETAH_READ_PROPERTY(self, CHEETAH_PARAM);
    CHEETAH_NEW_ARRAY(array);
	if (controller){
		Z_ADDREF_P(controller);
		add_assoc_zval(array, "controller", controller);
	}
    if (action){
       Z_ADDREF_P(action);
       add_assoc_zval(array, "action", action);
    } else {
       add_assoc_null(array, "action");
    }
    if (param){
       Z_ADDREF_P(param);
       add_assoc_zval(array, "param", param);
    } else {
       add_assoc_null(array, "action");
    }
    RETURN_ZVAL(array, 1, 1);
}
/* }}} */

/** {{{ public CheetahApplicationCore::runWeb()
 */
PHP_METHOD(cheetah_application_core, runWeb) {
	char *controller_name=NULL;
	char *controller_path=NULL;
	char *action_name=NULL;
	char fm, fc;
    uint c_len;
	zend_bool flag;
	zval * ins;
	char * func = NULL;
	
	char base_dir_t[255];
	cheetah_request_init();
	
	fc=*(CHEETAH_G(controller));
	if('a'<=fc&&'z'>=fc) fc-= 32;
	c_len = spprintf(&controller_name, 0, "%c%s", fc, CHEETAH_G(controller)+1);
	if(controller_name){
		c_len = spprintf(&controller_name, 0, "%s%s", controller_name, CHEETAH_DEFAULT_CONTROLLER_SUFFIX);
	}
	strncpy(base_dir_t,CHEETAH_G(base_dir),strlen(CHEETAH_G(base_dir))-CHEETAH_PATH_CONSOLE_LENGTH);
	base_dir_t[strlen(CHEETAH_G(base_dir))-CHEETAH_PATH_CONSOLE_LENGTH]='\0';
	c_len = spprintf(&controller_path, 0, "%s%s%s%s%s", base_dir_t,CHEETAH_DEFAULT_CONTROLLER_KEY,CHEETAH_PATH_DELIMITER,controller_name, CHEETAH_DEFAULT_PHP_EXTNAME);
	flag = cheetah_eval(ZEND_REQUIRE_ONCE,controller_path,c_len,NULL TSRMLS_CC);
	if(!flag) {
		if(!EG(exception)) zend_throw_exception_ex(NULL,
			NULL TSRMLS_CC, "don't find %s class file.", controller_path);
		return 0;
    }
	ins = cheetah_create_object(controller_name TSRMLS_CC);
	if(!ins || !IS_ZEND_STD_OBJECT(*ins) 
	#ifndef PHP_WIN32
		|| !instanceof_function(Z_OBJCE_P(ins), NULL TSRMLS_CC)
	#endif
        ) {
		zend_throw_exception_ex(NULL, NULL TSRMLS_CC, 
			"don't create %s class's object.", controller_path);
		goto FREE;
        }
	
	spprintf(&func, 0, "%s", CHEETAH_G(action));
    if(FAILURE==cheetah_call_user_function(&ins, func, NULL, 0, NULL TSRMLS_CC)) goto FREE;

	//php_printf("call end\n");
	CHEETAH_FREE(controller_name);
	CHEETAH_FREE(controller_path);
	CHEETAH_FREE(func);
FREE:
	CHEETAH_FREE(controller_name);
	CHEETAH_FREE(controller_path);
	CHEETAH_FREE(func);

}
/* }}} */

/** {{{ public CheetahApplicationCore::setBaseDir()
 */
PHP_METHOD(cheetah_application_core, setBaseDir) {
	zval *self=NULL;
    char *base_dir = NULL;
	uint base_dir_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s"
                             , &base_dir
							 , &base_dir_len
                             ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	self=getThis();
	if(base_dir){
		CHEETAH_G(base_dir)=base_dir;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_BASE_DIR , base_dir);
	}else{
		CHEETAH_G(base_dir)=NULL;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_BASE_DIR , NULL);
	}
}
/* }}} */

/** {{{ public CheetahApplicationCore::getBaseDir()
 */
PHP_METHOD(cheetah_application_core, getBaseDir) {
	zval *self=NULL;
	char *base_dir = NULL;
	self=getThis();
    if(CHEETAH_G(base_dir)){
		base_dir = CHEETAH_G(base_dir);
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_BASE_DIR , base_dir);
		RETURN_STRING(base_dir, 1);
	}else{
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_BASE_DIR , NULL);
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ public CheetahApplicationCore::initConn()
 */
PHP_METHOD(cheetah_application_core, initConn) {
	zval *self=NULL;
	zval *conn=NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z"
							 , &conn
                             ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if (conn && Z_TYPE_P(conn) == IS_RESOURCE) {
		php_printf("conn success\n");
        CHEETAH_G(conn)=conn;
    } else {
        /* PDO is not connected */
        //cheetah_error(E_ERROR TSRMLS_CC, "recorder.construct.conn.not_object");
		zend_throw_exception_ex(NULL, NULL TSRMLS_CC, 
			"don't create conn object.",NULL);
    }
}
/* }}} */

/** {{{ $controller='',$action='',$param=''
 */
ZEND_BEGIN_ARG_INFO(cheetah_application_core_initConn_args, 0)
    ZEND_ARG_INFO(0, conn)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $controller='',$action='',$param=''
 */
ZEND_BEGIN_ARG_INFO(cheetah_application_core_construct_args, 0)
    ZEND_ARG_INFO(0, controller)
    ZEND_ARG_INFO(0, action)
    ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()
/* }}} */
/** {{{ $base_dir
 */
ZEND_BEGIN_ARG_INFO(cheetah_application_core_setBaseDir_args, 0)
    ZEND_ARG_INFO(0, base_dir)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{
 */
zend_function_entry cheetah_application_core_methods[] = {
    PHP_ME(cheetah_application_core, __construct ,	 cheetah_application_core_construct_args			, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(cheetah_application_core, getController , NULL		, ZEND_ACC_PUBLIC )
    PHP_ME(cheetah_application_core, getAction  ,	 NULL         , ZEND_ACC_PUBLIC )
    PHP_ME(cheetah_application_core, getParam ,		 NULL			, ZEND_ACC_PUBLIC )
    PHP_ME(cheetah_application_core, toArray  ,		 NULL									, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_application_core, setBaseDir  ,	 cheetah_application_core_setBaseDir_args	, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_application_core, getBaseDir  ,	 NULL									, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_application_core, runWeb  ,		 NULL									, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_application_core, initConn  ,	cheetah_application_core_initConn_args	, ZEND_ACC_PUBLIC )
	{ NULL, NULL, NULL }
};
/* }}} */

/** {{{
 */
PHP_MINIT_FUNCTION(cheetah_application_core) {
	zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "CheetahApplicationCore", cheetah_application_core_methods);
    cheetah_application_core_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

    zend_declare_property_null(cheetah_application_core_ce, ZEND_STRL(CHEETAH_CONTROLLER) , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_application_core_ce, ZEND_STRL(CHEETAH_ACTION)  , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_application_core_ce, ZEND_STRL(CHEETAH_PARAM) , ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(cheetah_application_core_ce, ZEND_STRL(CHEETAH_BASE_DIR) , ZEND_ACC_PRIVATE TSRMLS_CC);
	return SUCCESS;
}

/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */