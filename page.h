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
  | paeg.h: refer to the framework of zoeey(moxie(system128@gmail.com)). |                             |
  +----------------------------------------------------------------------+
*/
#ifndef CHEETAH_PAGE_H
#define CHEETAH_PAGE_H

#define CHEETAH_PAGE_SIZE     "page_size"
#define CHEETAH_CURRENT       "current"
#define CHEETAH_RECORD_COUNT  "record_count"
#define CHEETAH_PAGE_COUNT    "page_count"
#define CHEETAH_START_ROW     "start_row"
#define CHEETAH_END_ROW       "end_row"


extern zend_class_entry * cheetah_page_core_ce;
PHP_MINIT_FUNCTION(cheetah_page_core);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */