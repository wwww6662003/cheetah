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
  | Author: wangwei(wwww6662003@163.com)								 |
  | util.h: refer to the framework of zoeey(moxie(system128@gmail.com)). |
  +----------------------------------------------------------------------+
*/
#ifndef CHEETAH_UTIL_H
#define CHEETAH_UTIL_H

#define CHEETAH_LABEL     "label"
#define CHEETAH_NAME      "name"
#define CHEETAH_BRIEF     "brief"
#define CHEETAH_VALUES     "values"

#define CHEETAH_TRACK_VARS_SESSION 10000

zval ** get_global_by_name(zval * values, char * name, int name_len, char * key, int key_len, zval * def TSRMLS_DC);
zval ** get_track(zval * values, char * key, int key_len, zval * def, int track TSRMLS_DC);
zval  * get_ip(zend_bool is_format TSRMLS_DC);
void    from_ip(zval * fields, zval * names, zend_bool is_format TSRMLS_DC);
void    from_track_var(zval * values,zval * fields, zval * names, zval * def, int track TSRMLS_DC);

zval * cheetah_call_method(zval ** object_pp, zend_class_entry * obj_ce , zend_function ** fn_proxy, char * function_name , int function_name_len, zval ** retval_ptr_ptr, int param_count, zval *** params TSRMLS_DC);
void cheetah_error(int type TSRMLS_DC, const char * name, ...);

extern zend_class_entry * cheetah_util_core_ce;
PHP_MINIT_FUNCTION(cheetah_util_core);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */