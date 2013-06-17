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
#ifndef CHEETAH_ROUTE_H
#define CHEETAH_ROUTE_H

#define CHEETAH_CONTROLLER     "controller"
#define CHEETAH_ACTION			"action"
#define CHEETAH_PARAM			"param"


#define DOC_ROOT "DOCUMENT_ROOT"
#define DOC_ROOT_LEN (sizeof(DOC_ROOT)-1) /** 13 */
#define REQUEST_METHOD "REQUEST_METHOD"
#define REQUEST_METHOD_LEN (sizeof(REQUEST_METHOD)-1) /** 14 */
#define REQUEST_URI "REQUEST_URI" /** for apache2 */
#define REQUEST_URI_LEN (sizeof(REQUEST_URI)-1) /** 11 */
#define HTTP_HOST "HTTP_HOST"
#define HTTP_HOST_LEN (sizeof(HTTP_HOST)-1) /** 9 */
#define _GET "_GET"
#define _GET_SIZE 5
#define _POST "_POST"
#define _POST_SIZE 6
zend_bool cheetah_request_init();
zend_bool cheetah_absolute_path(const char * path, char ** abs_path,uint flag TSRMLS_DC);

extern zend_class_entry * cheetah_router_core_ce;
PHP_MINIT_FUNCTION(cheetah_router_core);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */