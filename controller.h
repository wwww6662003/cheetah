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

#ifndef CHEETAH_CONTROLLER_H
#define CHEETAH_CONTROLLER_H

#define CHEETAH_CONTROLLER     "controller"
#define CHEETAH_ACTION			"action"
#define CHEETAH_PARAM			"param"
#define CHEETAH_BASE_DIR		"base_dir"

extern zend_class_entry * cheetah_controller_core_ce;
PHP_MINIT_FUNCTION(cheetah_controller_core);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */