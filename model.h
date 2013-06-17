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
  | Author: wangwei(wwww6662003@163.com)								 |                                                               |
  +----------------------------------------------------------------------+
*/
#ifndef CHEETAH_MODEL_H
#define CHEETAH_MODEL_H

#define CHEETAH_ITEMS   "items"
#define CHEETAH_ACTIVES "actives"

#define CHEETAH_CONN     "conn"
#define CHEETAH_STMT     "stmt"
#define CHEETAH_KEYS     "keys"

extern zend_class_entry * cheetah_model_core_ce;
PHP_MINIT_FUNCTION(cheetah_model_core);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */