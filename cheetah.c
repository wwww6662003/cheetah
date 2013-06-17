/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
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

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cheetah.h"

#include "cheetah_const.h"
#include "application.h"
#include "router.h"
#include "controller.h"

/* If you declare any globals in php_cheetah.h uncomment this:*/
ZEND_DECLARE_MODULE_GLOBALS(cheetah)


/* True global resources - no need for thread safety here */
static int le_cheetah;

/* {{{ cheetah_functions[]
 *
 * Every user visible function must have an entry in cheetah_functions[].
 */
zend_function_entry cheetah_functions[] = {
	PHP_FE(confirm_cheetah_compiled,	NULL)		/* For testing, remove later. */
	{NULL, NULL, NULL}	/* Must be the last line in cheetah_functions[] */
};
/* }}} */

/* {{{ cheetah_module_entry
 */
zend_module_entry cheetah_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"cheetah",
	cheetah_functions,
	PHP_MINIT(cheetah),
	PHP_MSHUTDOWN(cheetah),
	PHP_RINIT(cheetah),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(cheetah),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(cheetah),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CHEETAH
ZEND_GET_MODULE(cheetah)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("cheetah.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_cheetah_globals, cheetah_globals)
    STD_PHP_INI_ENTRY("cheetah.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_cheetah_globals, cheetah_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_cheetah_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_cheetah_init_globals(zend_cheetah_globals *cheetah_globals)
{
	cheetah_globals->global_value = 0;
	cheetah_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cheetah)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	PHP_MINIT(cheetah_application_core)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(cheetah_router_core)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(cheetah_controller_core)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(cheetah_view_core)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(cheetah_util_core)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(cheetah_page_core)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(cheetah_model_core)(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cheetah)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(cheetah)
{
	CHEETAH_G(host) =NULL;
	CHEETAH_G(script_name) =NULL;
	CHEETAH_G(controller) =NULL;
    CHEETAH_G(action) = NULL;
    CHEETAH_G(param) = NULL;
	CHEETAH_G(base_dir) = NULL;
	CHEETAH_G(uri) = NULL;
    CHEETAH_G(method) = NULL;
	CHEETAH_G(re_get) = NULL;
	CHEETAH_G(re_post) = NULL;
	CHEETAH_G(tpl_var) = NULL;
	CHEETAH_NEW_ARRAY(CHEETAH_G(tpl_var));
	CHEETAH_G(conn) = NULL;

/*	cheetah_globals->controller = NULL;
    cheetah_globals->action = NULL;
    cheetah_globals->param = NULL;
	cheetah_globals->base_uri = NULL;
	cheetah_globals->ext = NULL;*/

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(cheetah)
{
	CHEETAH_FREE(CHEETAH_G(host));
	CHEETAH_FREE(CHEETAH_G(script_name));
	CHEETAH_FREE(CHEETAH_G(controller));
	CHEETAH_FREE(CHEETAH_G(action));
	CHEETAH_FREE(CHEETAH_G(param));
	CHEETAH_FREE(CHEETAH_G(base_dir));
	CHEETAH_FREE(CHEETAH_G(uri));
	CHEETAH_FREE(CHEETAH_G(method));
	CHEETAH_PTR_DTOR(CHEETAH_G(re_get));
	CHEETAH_PTR_DTOR(CHEETAH_G(re_post));
	CHEETAH_PTR_DTOR(CHEETAH_G(tpl_var));
	CHEETAH_PTR_DTOR(CHEETAH_G(conn));

/*	CHEETAH_FREE(cheetah_globals->controller);
	CHEETAH_FREE(cheetah_globals->action);
	CHEETAH_FREE(cheetah_globals->param);
	CHEETAH_FREE(cheetah_globals->base_uri);
CHEETAH_FREE(cheetah_globals->ext);*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cheetah)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "cheetah support", "enabled");
	php_info_print_table_row(2, "Version", CHEETAH_VERSION);
	php_info_print_table_row(2, "Supports", CHEETAH_SUPPORT_URL);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_cheetah_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_cheetah_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "cheetah", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
