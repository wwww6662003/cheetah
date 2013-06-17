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
  | Author: wangwei(wwww6662003@163.com)                                                               |
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
#include "view.h"


/** {{{ current cheetah_view_core object
 */
zend_class_entry * cheetah_view_core_ce;
/* }}} */

/** {{{ public CheetahViewCore::__construct()
 */
PHP_METHOD(cheetah_view_core, __construct) {
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

/** {{{ public CheetahViewCore::getParam()
 */
PHP_METHOD(cheetah_view_core, getParam) {
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
/** {{{ public CheetahViewCore::toArray()
 */
PHP_METHOD(cheetah_view_core, toArray) {
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
/** {{{ public CheetahViewCore::getController()
 */
PHP_METHOD(cheetah_view_core, getController) {
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

/** {{{ public CheetahViewCore::getAction()
 */
PHP_METHOD(cheetah_view_core, getAction) {
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

/** {{{ public CheetahViewCore::getVar($key)
 */
PHP_METHOD(cheetah_view_core, getVar) {
	char *key=NULL;
	uint k_len=0;
	zval **var_value=NULL;
	zval *var_re_value=NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s"
		, &key
		, &k_len
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if(zend_hash_find(Z_ARRVAL_P(CHEETAH_G(tpl_var)), (char*)key, strlen(key)+1, (void**)&var_value)==SUCCESS){
		var_re_value=Z_STRVAL_PP(var_value);
		RETURN_STRING(var_re_value,1);
	}else{
		zend_throw_exception_ex(NULL, NULL TSRMLS_CC, "no %s param's value.", key);
		RETURN_NULL();
	}
}
/***}}}*/
/** {{{ public CheetahViewCore::getVarArr($key)
 */
PHP_METHOD(cheetah_view_core, getVarArr) {
	char *key=NULL;
	uint k_len=0;
	zval **var_value=NULL;
	zval *var_re_value=NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s"
		, &key
		, &k_len
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	CHEETAH_NEW_ARRAY(var_re_value);
	if(zend_hash_find(Z_ARRVAL_P(CHEETAH_G(tpl_var)), (char*)key, strlen(key)+1, (void**)&var_value)==SUCCESS){
		var_re_value=*var_value;
		//ZVAL_ADDREF(*var_value);
		//add_next_index_zval(var_re_value,var_value);
		//php_var_dump(var_re_value, 3 TSRMLS_CC);
		RETURN_ZVAL(var_re_value,1,1);
	}else{
		zend_throw_exception_ex(NULL, NULL TSRMLS_CC, "no %s param's value.", key);
		RETURN_NULL();
	}
}
/***}}}*/

/** {{{ $controller='',$action='',$param=''
 */
ZEND_BEGIN_ARG_INFO(cheetah_view_core_construct_args, 0)
    ZEND_ARG_INFO(0, controller)
    ZEND_ARG_INFO(0, action)
    ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ getController($controller)
 */
ZEND_BEGIN_ARG_INFO(cheetah_view_core_getController_args, 0)
    ZEND_ARG_INFO(0, controller)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ getVar($key)
 */
ZEND_BEGIN_ARG_INFO(cheetah_view_core_getVar_args, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ getVarArr($key)
 */
ZEND_BEGIN_ARG_INFO(cheetah_view_core_getVarArr_args, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{
 */
zend_function_entry cheetah_view_core_methods[] = {
    PHP_ME(cheetah_view_core, __construct ,		cheetah_view_core_construct_args			, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cheetah_view_core, getController , NULL		, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_view_core, getAction  ,	 NULL         , ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_view_core, getParam ,		 NULL			, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_view_core, toArray  ,		 NULL			, ZEND_ACC_PUBLIC )
	PHP_ME(cheetah_view_core, getVar ,		cheetah_view_core_getVar_args		, ZEND_ACC_PUBLIC)
	PHP_ME(cheetah_view_core, getVarArr ,		cheetah_view_core_getVarArr_args		, ZEND_ACC_PUBLIC)
	
	{ NULL, NULL, NULL }
};
/* }}} */

/** {{{
 */
PHP_MINIT_FUNCTION(cheetah_view_core) {
	zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "CheetahViewCore", cheetah_view_core_methods);
    cheetah_view_core_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	zend_declare_property_null(cheetah_view_core_ce, ZEND_STRL(CHEETAH_CONTROLLER) , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_view_core_ce, ZEND_STRL(CHEETAH_ACTION)  , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_view_core_ce, ZEND_STRL(CHEETAH_PARAM) , ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(cheetah_view_core_ce, ZEND_STRL(CHEETAH_BASE_DIR) , ZEND_ACC_PRIVATE TSRMLS_CC);
	 return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */