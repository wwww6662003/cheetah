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
#include "main/SAPI.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_array.h"

#include "cheetah_const.h"
#include "php_cheetah.h"
#include "router.h"


/** {{{ current CheetahRouterCore object
 */
zend_class_entry * cheetah_router_core_ce;
/* }}} */

/** {{{ cheetah_request_init
 */
zend_bool cheetah_request_init() {
	char *uri='\0';
	char *method='\0';
	char *host='\0';
	char *fragment='\0';
	char *path='\0';
	char *abs_path='\0';
	char *controller_key=CHEETAH_DEFAULT_CONTROLLER_KEY;
	char *action_key=CHEETAH_DEFAULT_ACTION_KEY;

	zval *get=NULL;
	zval *post=NULL;
	zval **get_p=NULL;
	zval **post_p = NULL;
	zval ** controller_v = NULL;
	zval ** action_v = NULL;
	php_url * url;
	TSRMLS_FETCH(); 
	if(uri) { // CLI
        method = "GET";
    } else {
        method = zend_getenv(REQUEST_METHOD, REQUEST_METHOD_LEN TSRMLS_CC);
        // for apache2, sapi_getenv
        uri = zend_getenv(REQUEST_URI, REQUEST_URI_LEN TSRMLS_CC);
    }
    url = php_url_parse(uri);
    path = estrdup(url->path ? url->path : "/");
    if(url->fragment) fragment = estrdup(url->fragment);
    // SERVER_NAME
    host = (url->host ? url->host : zend_getenv(HTTP_HOST, HTTP_HOST_LEN TSRMLS_CC));
    zend_hash_find(&EG(symbol_table), _GET, _GET_SIZE, (void**)&get_p);
    /** 对于terminal，$_GET和$_POST为空数组，不加判断产生内存泄露。 */
    if(get_p && Z_TYPE_PP(get_p)==IS_ARRAY /*&& zend_hash_num_elements(Z_ARRVAL_PP(get))*/) {
        get = *get_p;
        zval_ptr_dtor(&get);
        get->is_ref = 1;
        ZVAL_ADDREF(get);
    } else {
        MAKE_STD_ZVAL(get);
        array_init(get);
    }
    zend_hash_find(&EG(symbol_table), _POST, _POST_SIZE, (void**)&post_p);
    if(post_p && Z_TYPE_PP(post_p)==IS_ARRAY /*&& zend_hash_num_elements(Z_ARRVAL_PP(post))*/) {
        post = *post_p;
        zval_ptr_dtor(&post);
		post->is_ref = 1;
        ZVAL_ADDREF(post);
    } else {
        MAKE_STD_ZVAL(post);
        array_init(post);
    }
	
	cheetah_absolute_path(path,&abs_path,1 TSRMLS_CC);
	zend_hash_find(Z_ARRVAL_P(get), (char*)controller_key, strlen(controller_key)+1, (void**)&controller_v);
	zend_hash_find(Z_ARRVAL_P(get), (char*)action_key, strlen(action_key)+1, (void**)&action_v);
	
	CHEETAH_G(script_name)=path;
	CHEETAH_G(host)=host;
	//CHEETAH_G(controller) =Z_STRVAL_PP(controller_v)?Z_STRVAL_PP(controller_v):CHEETAH_DEFAULT_CONTROLLER;
    //CHEETAH_G(action) = Z_STRVAL_PP(action_v)?Z_STRVAL_PP(action_v):CHEETAH_DEFAULT_ACTION;
	CHEETAH_G(controller) =Z_STRVAL_PP(controller_v);
	CHEETAH_G(action) = Z_STRVAL_PP(action_v);
	//CHEETAH_G(param) = NULL;
	CHEETAH_G(base_dir) = abs_path;
	CHEETAH_G(uri) = uri;
    CHEETAH_G(method) = method;
	CHEETAH_G(re_get) = get;
	CHEETAH_G(re_post) = post;

	/*CHEETAH_FREE(uri);
	CHEETAH_FREE(host);
	CHEETAH_FREE(method);
	CHEETAH_FREE(fragment);
	CHEETAH_FREE(path);
	CHEETAH_FREE(abs_path);
	CHEETAH_FREE(controller_key);
	CHEETAH_FREE(action_key);

	CHEETAH_PTR_DTOR(get);
	CHEETAH_PTR_DTOR(post);
	CHEETAH_PTR_DTOR(get_p);
	CHEETAH_PTR_DTOR(post_p);
	CHEETAH_PTR_DTOR(controller_v);
	CHEETAH_PTR_DTOR(action_v);
    php_url_free(url);*/
	return 1;
}
/**}}}}*/

/** 获取网站绝对路径1是目录2是文件*/
zend_bool cheetah_absolute_path(const char * path, char ** abs_path,uint flag TSRMLS_DC) {
    char * root;
    if(IS_ABSOLUTE_PATH(path, strlen(path))) return 1;
    if(flag==1) {
        root = zend_getenv(DOC_ROOT, DOC_ROOT_LEN TSRMLS_CC);
        if(root){
			CHEETAH_G(base_dir)=root;
			spprintf(abs_path, 0, "%s%c", root, '\0');
		}else{
			CHEETAH_G(base_dir)=root;
			spprintf(abs_path, 0, "%s%c%s", root, '\0', path);
		}
    }
    return 0;
}
/**}}}}*/

/** {{{ public CheetahRouterCore::toArray()
 */
PHP_METHOD(cheetah_router_core, toArray) {
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

/** {{{ public CheetahRouterCore::parseUrl()
 */
PHP_METHOD(cheetah_router_core, parseUrl) {
	cheetah_request_init();
}
/* }}} */


/* }}} */

/** {{{ public CheetahRouterCore::__construct()
 */
PHP_METHOD(cheetah_router_core, __construct) {
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

/** {{{ public CheetahRouterCore::getController()
 */
PHP_METHOD(cheetah_router_core, getController) {
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

/** {{{ public CheetahRouterCore::getAction()
 */
PHP_METHOD(cheetah_router_core, getAction) {
    zval *self = NULL;
    zval *action_z = NULL;
	char *action = NULL;
    self = getThis();
    action_z = CHEETAH_READ_PROPERTY(self, CHEETAH_ACTION);
    if(action_z){
		action = Z_STRVAL_P(action_z);
		RETURN_STRING(action, 1);
	}else{
		RETURN_NULL();
	}
}
/* }}} */

/** {{{ public CheetahRouterCore::getParam()
 */
PHP_METHOD(cheetah_router_core, getParam) {
	zval *self = NULL;
    zval *param = NULL;
    self = getThis();
    param = CHEETAH_READ_PROPERTY(self, CHEETAH_PARAM);
    if(param){
		RETURN_ZVAL(param, 1,0);
	}else{
		RETURN_NULL();
	}
    
}
/* }}} */

/** {{{ $controller='',$action='',$param=''
 */
ZEND_BEGIN_ARG_INFO(cheetah_router_core_construct_args, 0)
    ZEND_ARG_INFO(0, controller)
    ZEND_ARG_INFO(0, action)
    ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $controller
 */
ZEND_BEGIN_ARG_INFO(cheetah_router_core_getController_args, 0)
    ZEND_ARG_INFO(0, controller)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $action
 */
ZEND_BEGIN_ARG_INFO(cheetah_router_core_getAction_args, 0)
    ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $param
 */
ZEND_BEGIN_ARG_INFO(cheetah_router_core_getParam_args, 0)
    ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{
 */
zend_function_entry cheetah_router_core_methods[] = {
    PHP_ME(cheetah_router_core, __construct ,		cheetah_router_core_construct_args			, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(cheetah_router_core, getController ,		cheetah_router_core_getController_args		, ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_router_core, getAction  ,			cheetah_router_core_getAction_args			, ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_router_core, getParam ,			cheetah_router_core_getParam_args			, ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_router_core, toArray  ,			NULL									, ZEND_ACC_PUBLIC)
	PHP_ME(cheetah_router_core, parseUrl  ,			NULL									, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};
/* }}} */

/** {{{
 */
PHP_MINIT_FUNCTION(cheetah_router_core) {
	zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "CheetahRouterCore", cheetah_router_core_methods);
    cheetah_router_core_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

    zend_declare_property_null(cheetah_router_core_ce, ZEND_STRL(CHEETAH_CONTROLLER) , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_router_core_ce, ZEND_STRL(CHEETAH_ACTION)  , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_router_core_ce, ZEND_STRL(CHEETAH_PARAM) , ZEND_ACC_PRIVATE TSRMLS_CC);
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