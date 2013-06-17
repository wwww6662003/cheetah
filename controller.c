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
  | Author: wangwei(wwww6662003@163.com)                                                                |
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
#include "main/SAPI.h"

#include "cheetah_const.h"
#include "php_cheetah.h"
#include "router.h"
#include "controller.h"


/** {{{ current CheetahControllerCore object
 */
zend_class_entry * cheetah_controller_core_ce;
/* }}} */

/** {{{ public CheetahControllerCore::__construct()
 */
PHP_METHOD(cheetah_controller_core, __construct) {
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

/** {{{ public CheetahControllerCore::__construct()
 */
PHP_METHOD(cheetah_controller_core, display) {
	char *template_path=NULL;
	char *template_name=NULL;
	uint template_name_len=0;
	char base_dir_t[255];
	zend_bool flag;
	uint c_len=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s"
		, &template_name
		, &template_name_len
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if(template_name==NULL){
		template_name=CHEETAH_DEFAULT_TEMPLATE_NAME;
	}
	strncpy(base_dir_t,CHEETAH_G(base_dir),strlen(CHEETAH_G(base_dir))-CHEETAH_TEMPLATE_DIR_LENGTH);
	base_dir_t[strlen(CHEETAH_G(base_dir))-CHEETAH_TEMPLATE_DIR_LENGTH]='\0';
	c_len = spprintf(&template_path, 0, "%s%s%s%s%s%s", base_dir_t,CHEETAH_PATH_DELIMITER,CHEETAH_TEMPLATE_DIR,CHEETAH_PATH_DELIMITER,template_name, CHEETAH_DEFAULT_TEMPLATE_EXTNAME);
	flag = cheetah_eval(ZEND_REQUIRE_ONCE,template_path,c_len,NULL TSRMLS_CC);
	if(!flag) {
		if(!EG(exception)) zend_throw_exception_ex(NULL,
			NULL TSRMLS_CC, "don't load template %s file.", template_path);
		return 0;
    }
}
/***}}}*/

/** {{{ public CheetahControllerCore::getGet($key)
 */
PHP_METHOD(cheetah_controller_core, getGet) {
	char *key=NULL;
	uint key_len=0;
	uint c_len=0;
	zval **get_value=NULL;
	char *re_value=NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s"
		, &key
		, &key_len
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if(key&&CHEETAH_G(re_get)){
		if(zend_hash_find(Z_ARRVAL_P(CHEETAH_G(re_get)), (char*)key, strlen(key)+1, (void**)&get_value)==SUCCESS){
			re_value=Z_STRVAL_PP(get_value);
			RETURN_STRING(re_value,1);
		}else{
			zend_throw_exception_ex(NULL, NULL TSRMLS_CC, "no %s param's value.", key);
		}
	}else{
		RETURN_NULL();
	}
}
/***}}}*/

/** {{{ public CheetahControllerCore::getPost($key)
 */
PHP_METHOD(cheetah_controller_core, getPost) {
	char *key=NULL;
	uint key_len=0;
	uint c_len=0;
	zval **post_value=NULL;
	char *re_value=NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s"
		, &key
		, &key_len
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if(key&&CHEETAH_G(re_post)){
		if(zend_hash_find(Z_ARRVAL_P(CHEETAH_G(re_post)), (char*)key, strlen(key)+1, (void**)&post_value)==SUCCESS){
			re_value=Z_STRVAL_PP(post_value);
			RETURN_STRING(re_value,1);
		}else{
			zend_throw_exception_ex(NULL, NULL TSRMLS_CC, "no %s param's value.", key);
		}
	}else{
		RETURN_NULL();
	}
}
/***}}}*/

/***}}}*/

/** {{{ public CheetahControllerCore::assign($key,$value)
 */
PHP_METHOD(cheetah_controller_core, assign) {
	char *key=NULL;
	zval *k_value=NULL;
	uint key_len=0,k_value_len=0;
	zval *tpl_var_arr=NULL;
	//char *php_sentence=NULL;
	//uint c_len;
	//zend_bool flag;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz"
		, &key
		, &key_len
		,&k_value
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if(key&&k_value){
		Z_ADDREF_P(k_value);
		add_assoc_zval(CHEETAH_G(tpl_var), key, k_value);
		//php_array_merge(CHEETAH_G(tpl_var), tpl_var_arr, 0 TSRMLS_CC);
		//php_var_dump(&CHEETAH_G(tpl_var), 3 TSRMLS_CC);
	}else{
		RETURN_NULL();
	}
	/*c_len=spprintf(&php_sentence, 0, "%s%s%s%s%s", "$",key,"='",k_value,"';");
	php_printf("sentence=%s\n",php_sentence);
	flag = cheetah_eval(ZEND_REQUIRE_ONCE,php_sentence,c_len,NULL TSRMLS_CC);
	if(!flag) {
		if(!EG(exception)) zend_throw_exception_ex(NULL,
			NULL TSRMLS_CC, "don't find %s variable's value.", key);
		return 0;
    }*/

}
/***}}}*/
/** {{{ public CheetahControllerCore::assignArr($key,$value)
 */
PHP_METHOD(cheetah_controller_core, assignArr) {
	char *key=NULL;
	zval *k_value=NULL;
	uint key_len=0,k_value_len=0;
	zval *tpl_var_arr=NULL;
	//char *php_sentence=NULL;
	//uint c_len;
	//zend_bool flag;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa"
		, &key
		, &key_len
		,&k_value
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if(key&&k_value){
		Z_ADDREF_P(k_value);
		add_assoc_zval(CHEETAH_G(tpl_var), key, k_value);
		//php_array_merge(CHEETAH_G(tpl_var), tpl_var_arr, 0 TSRMLS_CC);
		//php_var_dump(&CHEETAH_G(tpl_var), 3 TSRMLS_CC);
	}else{
		RETURN_NULL();
	}
	/*c_len=spprintf(&php_sentence, 0, "%s%s%s%s%s", "$",key,"='",k_value,"';");
	php_printf("sentence=%s\n",php_sentence);
	flag = cheetah_eval(ZEND_REQUIRE_ONCE,php_sentence,c_len,NULL TSRMLS_CC);
	if(!flag) {
		if(!EG(exception)) zend_throw_exception_ex(NULL,
			NULL TSRMLS_CC, "don't find %s variable's value.", key);
		return 0;
    }*/

}
/***}}}*/

/** {{{ public CheetahControllerCore::getRequest($key)
 */
PHP_METHOD(cheetah_controller_core, getRequest) {
	char *key=NULL;
	uint key_len=0;
	uint c_len=0;
	zval **request_value=NULL;
	char *re_value=NULL;
	zval *request_t=NULL,*get_tt=NULL,*post_tt=NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s"
		, &key
		, &key_len
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if(key){
		get_tt=CHEETAH_G(re_get);
		post_tt=CHEETAH_G(re_post);
		php_array_merge(Z_ARRVAL_P(get_tt), Z_ARRVAL_P(post_tt), 0 TSRMLS_CC);
		if(zend_hash_find(Z_ARRVAL_P(get_tt), (char*)key, strlen(key)+1, (void**)&request_value)==SUCCESS){
			re_value=Z_STRVAL_PP(request_value);
			RETURN_STRING(re_value,1);
		}else{
			zend_throw_exception_ex(NULL, NULL TSRMLS_CC, "no %s param's value.", key);
		}
	}else{
		RETURN_NULL();
	}
}
/***}}}*/

/** {{{ public CheetahControllerCore::getParam()
 */
PHP_METHOD(cheetah_controller_core, getParam) {
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
/** {{{ public CheetahControllerCore::toArray()
 */
PHP_METHOD(cheetah_controller_core, toArray) {
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
/** {{{ public CheetahControllerCore::getController()
 */
PHP_METHOD(cheetah_controller_core, getController) {
    zval *self = NULL;
    zval *controller_z = NULL;
	char *controller = NULL;
    self = getThis();
    controller_z = CHEETAH_READ_PROPERTY(self, CHEETAH_CONTROLLER);
	if(controller_z){
		controller = Z_STRVAL_P(controller_z);
		RETURN_STRING(controller, 1);
	}else{
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ public CheetahControllerCore::getAction()
 */
PHP_METHOD(cheetah_controller_core, getAction) {
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

/** {{{ $controller='',$action='',$param=''
 */
ZEND_BEGIN_ARG_INFO(cheetah_controller_core_construct_args, 0)
    ZEND_ARG_INFO(0, controller)
    ZEND_ARG_INFO(0, action)
    ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ getController($controller)
 */
ZEND_BEGIN_ARG_INFO(cheetah_controller_core_getController_args, 0)
    ZEND_ARG_INFO(0, controller)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ getGet($key)
 */
ZEND_BEGIN_ARG_INFO(cheetah_controller_core_getGet_args, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
/* }}} */
/** {{{ getPost($key)
 */
ZEND_BEGIN_ARG_INFO(cheetah_controller_core_getPost_args, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ getRequest($key)
 */
ZEND_BEGIN_ARG_INFO(cheetah_controller_core_getRequest_args, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ assign($key,$value)
 */
ZEND_BEGIN_ARG_INFO(cheetah_controller_core_assign_args, 0)
    ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, k_value)
ZEND_END_ARG_INFO()
/* }}} */
/** {{{ assignArr($key,$value)
 */
ZEND_BEGIN_ARG_INFO(cheetah_controller_core_assignArr_args, 0)
    ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, k_value)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{
 */
zend_function_entry cheetah_controller_core_methods[] = {
    PHP_ME(cheetah_controller_core, __construct ,		cheetah_controller_core_construct_args			, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cheetah_controller_core, getController , NULL		, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_controller_core, getAction  ,	 NULL         , ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_controller_core, getParam ,		 NULL			, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_controller_core, toArray  ,		 NULL			, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_controller_core, display ,		NULL		, ZEND_ACC_PUBLIC)
	PHP_ME(cheetah_controller_core, getGet ,		cheetah_controller_core_getGet_args		, ZEND_ACC_PUBLIC)
	PHP_ME(cheetah_controller_core, getPost ,		cheetah_controller_core_getPost_args		, ZEND_ACC_PUBLIC)
	PHP_ME(cheetah_controller_core, getRequest ,		cheetah_controller_core_getRequest_args		, ZEND_ACC_PUBLIC)
	PHP_ME(cheetah_controller_core, assign ,		cheetah_controller_core_assign_args		, ZEND_ACC_PUBLIC)
	PHP_ME(cheetah_controller_core, assignArr ,		cheetah_controller_core_assignArr_args		, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};
/* }}} */

/** {{{
 */
PHP_MINIT_FUNCTION(cheetah_controller_core) {
	zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "CheetahControllerCore", cheetah_controller_core_methods);
    cheetah_controller_core_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	zend_declare_property_null(cheetah_controller_core_ce, ZEND_STRL(CHEETAH_CONTROLLER) , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_controller_core_ce, ZEND_STRL(CHEETAH_ACTION)  , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_controller_core_ce, ZEND_STRL(CHEETAH_PARAM) , ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(cheetah_controller_core_ce, ZEND_STRL(CHEETAH_BASE_DIR) , ZEND_ACC_PRIVATE TSRMLS_CC);
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