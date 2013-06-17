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
  | paeg.c: refer to the framework of zoeey(moxie(system128@gmail.com)). |                                               |
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
#include "page.h"


/** {{{ current cheetah_page_core object
 */
zend_class_entry * cheetah_page_core_ce;
/* }}} */

/** {{{ public CheetahPageCore::__construct()
 */
PHP_METHOD(cheetah_page_core, __construct) {
    zval * self           = NULL;
    long   pagesize      = 0;
    zval * pagesize_z     = NULL;
    zval * current_z      = NULL;
    long   current        = 0;
    zval * record_count_z  = NULL;
    zval * page_count_z    = NULL;
    zval * start_row_z     = NULL;
    zval * end_row_z       = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll"
                             , &pagesize
                             , &current
                            ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    self = getThis();
    ALLOC_INIT_ZVAL(pagesize_z);
    ALLOC_INIT_ZVAL(current_z);
    ALLOC_INIT_ZVAL(record_count_z);
    ALLOC_INIT_ZVAL(page_count_z);
    ALLOC_INIT_ZVAL(start_row_z);
    ALLOC_INIT_ZVAL(end_row_z);

    ZVAL_LONG(pagesize_z   , pagesize);
    ZVAL_LONG(current_z    , current);
    ZVAL_LONG(record_count_z, 0);
    ZVAL_LONG(page_count_z  , 0);
    ZVAL_LONG(start_row_z   , 0);
    ZVAL_LONG(end_row_z     , 0);

	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PAGE_SIZE , pagesize_z);
	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_CURRENT , current_z);
	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_RECORD_COUNT , record_count_z);
	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PAGE_COUNT , page_count_z);
	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_START_ROW , start_row_z);
	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_END_ROW , end_row_z);

    CHEETAH_PTR_DTOR(pagesize_z);
    CHEETAH_PTR_DTOR(current_z);
    CHEETAH_PTR_DTOR(record_count_z);
    CHEETAH_PTR_DTOR(page_count_z);
    CHEETAH_PTR_DTOR(start_row_z);
    CHEETAH_PTR_DTOR(end_row_z);



}
/* }}} */


/** {{{ public CheetahPageCore::setRecordCount($record_count)
 */
PHP_METHOD(cheetah_page_core, setRecordCount) {
    zval * self          = NULL;
    zval * pagesize_z    = NULL;
    long   page_size      = 0;
    zval * current_z     = NULL;
    long   current       = 0;
    zval * record_count_z = NULL;
    long   record_count   = 0;
    zval * page_count_z   = NULL;
    long   page_count     = 0;
    zval * start_row_z    = NULL;
    long   start_row      = 0;
    zval * end_row_z      = NULL;
    long   end_row        = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l"
                            , &record_count
                            ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    self = getThis();

    ALLOC_INIT_ZVAL(record_count_z);
    ZVAL_LONG(record_count_z, record_count);
    
	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_RECORD_COUNT , record_count_z);
	CHEETAH_PTR_DTOR(record_count_z);
    pagesize_z  = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_SIZE);
	current_z   = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    /* record_count */
    page_count_z = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    start_row_z  = CHEETAH_READ_PROPERTY(self, CHEETAH_START_ROW);
    end_row_z    = CHEETAH_READ_PROPERTY(self, CHEETAH_END_ROW);

    page_size    = Z_LVAL_P(pagesize_z);
    current     = Z_LVAL_P(current_z);
    // record_count = Z_LVAL_P(record_count_z);
    page_count   = Z_LVAL_P(page_count_z);
    start_row    = Z_LVAL_P(start_row_z);
    end_row      = Z_LVAL_P(end_row_z);



    /*  reckonPageCount */
    {
        page_count = record_count / page_size;
        if ((record_count % page_size) > 0) {
            page_count = page_count + 1;
        }
        page_count = page_count == 0 ? 1 : floor(page_count);

        Z_LVAL_P(page_count_z) = page_count;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_PAGE_COUNT , page_count_z);
    }
    /* reckonCurrent */
    {
        current = current < page_count ? current : page_count;
        current = current > 0 ? current : 1;

        Z_LVAL_P(current_z) = current;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_CURRENT , current_z);
    }
    /* reckonStartRow */
    {
        start_row = page_size * (current - 1);
        start_row = start_row > 0 ? start_row : 0;

        Z_LVAL_P(start_row_z) = start_row;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_START_ROW , start_row_z);
    }
    /* reckonEndRow */
    {
        end_row =start_row +page_size;
        end_row = end_row <record_count ? end_row :record_count;

        Z_LVAL_P(end_row_z) = end_row;
		CHEETAH_UPDATE_PROPERTY(self, CHEETAH_END_ROW , end_row_z);
    }

}
/* }}} */


/** {{{
 * public CheetahPageCore::getCurrent()
 * public CheetahPageCore::current()
 */
PHP_METHOD(cheetah_page_core, getCurrent) {
    zval * self       = NULL;
    zval * current    = NULL;
    self = getThis();
    current = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    RETURN_LONG(Z_LVAL_P(current));
}
/* }}} */

/** {{{ public CheetahPageCore::getPageCount()
 */
PHP_METHOD(cheetah_page_core, getPageCount) {
    zval * self       = NULL;
    zval * page_count    = NULL;
    self = getThis();
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    RETURN_LONG(Z_LVAL_P(page_count));
}
/* }}} */

/** {{{ public CheetahPageCore::getPageSize()
 */
PHP_METHOD(cheetah_page_core, getPageSize) {
    zval * self        = NULL;
    zval * page_size = NULL;
    self = getThis();
    page_size = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_SIZE);
    RETURN_LONG(Z_LVAL_P(page_size));
}
/* }}} */

/** {{{ public CheetahPageCore::getPageSize()
 */
PHP_METHOD(cheetah_page_core, getRecordCount) {
    zval * self        = NULL;
    zval * record_count = NULL;
    self = getThis();
    record_count = CHEETAH_READ_PROPERTY(self, CHEETAH_RECORD_COUNT);
    RETURN_LONG(Z_LVAL_P(record_count));
}
/* }}} */

/** {{{ public CheetahPageCore::getStartRow()
 */
PHP_METHOD(cheetah_page_core, getStartRow) {
    zval * self        = NULL;
    zval * start_row    = NULL;
    zval * record_count = NULL;
    self = getThis();
    start_row =CHEETAH_READ_PROPERTY(self, CHEETAH_START_ROW);
    record_count = CHEETAH_READ_PROPERTY(self, CHEETAH_RECORD_COUNT);
    RETURN_LONG(Z_LVAL_P(record_count) > 0 ? Z_LVAL_P(start_row) + 1 : 0);
}
/* }}} */


/** {{{ public CheetahPageCore::getOffset()
 */
PHP_METHOD(cheetah_page_core, getOffset) {
    zval * self        = NULL;
    zval * start_row    = NULL;
    self = getThis();
    start_row = CHEETAH_READ_PROPERTY(self, CHEETAH_START_ROW);
    RETURN_LONG(Z_LVAL_P(start_row));
}
/* }}} */

/** {{{ public CheetahPageCore::getEndRow()
 */
PHP_METHOD(cheetah_page_core, getEndRow) {
    zval * self        = NULL;
    zval * end_row      = NULL;
    self = getThis();
    end_row = CHEETAH_READ_PROPERTY(self, CHEETAH_END_ROW);
    RETURN_LONG(Z_LVAL_P(end_row));
}
/* }}} */

/** {{{ public CheetahPageCore::hasPrev()
 */
PHP_METHOD(cheetah_page_core, hasPrev) {
    zval * self        = NULL;
    zval * current     = NULL;
    self = getThis();
    current = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    RETURN_BOOL(Z_LVAL_P(current) > 1);
}
/* }}} */

/** {{{ public CheetahPageCore::hasNext()
 */
PHP_METHOD(cheetah_page_core, hasNext) {
    zval * self        = NULL;
    zval * page_count   = NULL;
    zval * current     = NULL;
    self = getThis();
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    current   = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    RETURN_BOOL(Z_LVAL_P(page_count) > Z_LVAL_P(current));
}
/* }}} */

/** {{{ public CheetahPageCore::isLast()
 */
PHP_METHOD(cheetah_page_core, isLast) {
    zval * self        = NULL;
    zval * page_count   = NULL;
    zval * current     = NULL;
    self = getThis();
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    current   = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    RETURN_BOOL(Z_LVAL_P(page_count) <= Z_LVAL_P(current));
}
/* }}} */

/** {{{ public CheetahPageCore::isFirst()
 */
PHP_METHOD(cheetah_page_core, isFirst) {
    zval * self        = NULL;
    zval * page_count   = NULL;
    zval * current     = NULL;
    self = getThis();
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    current   = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    RETURN_BOOL(Z_LVAL_P(current) <= 1);
}
/* }}} */

/** {{{ public CheetahPageCore::hasList()
 */
PHP_METHOD(cheetah_page_core, hasList) {
    zval * self        = NULL;
    zval * record_count   = NULL;

    self = getThis();
    record_count = CHEETAH_READ_PROPERTY(self, CHEETAH_RECORD_COUNT);

    RETURN_BOOL(Z_LVAL_P(record_count) > 0);
}
/* }}} */

/** {{{ public CheetahPageCore::first()
 */
PHP_METHOD(cheetah_page_core, first) {
    RETURN_LONG(1);
}
/* }}} */

/** {{{ public CheetahPageCore::prev()
 */
PHP_METHOD(cheetah_page_core, prev) {
    zval * self        = NULL;
    zval * end_row      = NULL;
    zval * current     = NULL;
    int    pre_page     = 0;
    self = getThis();
    current = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    pre_page = Z_LVAL_P(current) - 1;
    pre_page = (pre_page >= 1) ? pre_page : 1;
    RETURN_LONG(pre_page);
}
/* }}} */

/** {{{ public CheetahPageCore::next()
 */
PHP_METHOD(cheetah_page_core, next) {
    zval * self        = NULL;
    zval * end_row      = NULL;
    zval * current     = NULL;
    zval * page_count    = NULL;
    int    next_page     = 0;
    self = getThis();
    current = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
	next_page = Z_LVAL_P(current) + 1;
	next_page = (next_page < Z_LVAL_P(page_count)) ? next_page : Z_LVAL_P(page_count);
    RETURN_LONG(next_page);
}
/* }}} */

/** {{{ public CheetahPageCore::last()
 */
PHP_METHOD(cheetah_page_core, last) {
    zval * self          = NULL;
    zval * end_row       = NULL;
    zval * page_count    = NULL;
    self = getThis();
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    RETURN_LONG(Z_LVAL_P(page_count));
}
/* }}} */

/** {{{ public CheetahPageCore::last()
 */
PHP_METHOD(cheetah_page_core, all) {
    zval * self         = NULL;
    zval * page_count   = NULL;
    zval * pages        = NULL;
    int    i            = 0;
    self = getThis();
    CHEETAH_NEW_ARRAY(pages);
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    if (Z_LVAL_P(page_count) <= 1) {
        add_next_index_long(pages, 1);
    }else{
       for(i = 0;i < Z_LVAL_P(page_count);i++) {
            add_next_index_long(pages, i + 1);
        }
    }
    RETURN_ZVAL(pages, 1, 1);
}
/* }}} */

/** {{{ public CheetahPageCore::siblings()
 */
PHP_METHOD(cheetah_page_core, siblings) {
    zval * self          = NULL;
    zval * page_count     = NULL;
    zval * current       = NULL;
    zval * pages         = NULL;
    int    i             = 0;
    int    count         = 0;
    int    jumper        = 0;
    int    startPage     = 0;
    int    endPage       = 0;
    zend_bool isContinue = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l"
                            , &count
                            ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    self = getThis();
    CHEETAH_NEW_ARRAY(pages);
    page_count = CHEETAH_READ_PROPERTY(self, CHEETAH_PAGE_COUNT);
    current   = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    if (Z_LVAL_P(page_count) > 0 && count > 0) {
            /**
             * 去除当前页
             */
            count--;
            startPage = Z_LVAL_P(current) - ((((count % 2) > 0) ? count - 1 : count) / 2);
            endPage   = count + startPage;
            jumper    = 0;

            /**
             * 可缩小的滑块
             */
            do {
                jumper++;
                isContinue = 0;
                /**
                 * 向后走 1 位
                 */
                if (startPage < 1) {
                    startPage++;
                    if (endPage < Z_LVAL_P(page_count)) {
                        endPage++;
                    }
                    isContinue = 1;
                }
                /**
                 * 向前走 1 位
                 */
                if (endPage > Z_LVAL_P(page_count)) {
                    endPage--;
                    if (startPage > 1) {
                        startPage--;
                    }
                    isContinue = 1;
                }
                /**
                 * 防循环锁
                 * 该方法已经严格测试。
                 * 请勿为此举感到担心，纯属极端癖好。
                 */
                if (jumper > 500) {
                    isContinue = 0;
                }
            } while (isContinue);

            for (i = startPage; i <= endPage; i++) {
                 add_next_index_long(pages, i);
            }
        } else {
            add_next_index_long(pages, 1);
        }

    RETURN_ZVAL(pages, 1, 1);
}
/* }}} */

/** {{{ public CheetahPageCore::compare($page, $show, $else_show)
 */
PHP_METHOD(cheetah_page_core, compare) {
    zval * self        = NULL;
    zval * current     = NULL;
    int    page        = 0;
    zval * ret         = NULL;
    zval * else_ret    = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lzz"
                            , &page
                            , &ret
                            , &else_ret
                            ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    self = getThis();
    current = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);

    if (Z_LVAL_P(current) != page) {
        RETURN_ZVAL(else_ret, 1, 0);
    }
    RETURN_ZVAL(ret, 1, 0);
}
/* }}} */


/** {{{ public CheetahPageCore::isCurrent($page)
 */
PHP_METHOD(cheetah_page_core, isCurrent) {
    zval * self        = NULL;
    zval * current     = NULL;
    int    page        = 0;
    zval * show        = NULL;
    zval * else_show    = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l"
                            , &page
                            ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    self = getThis();
    current = CHEETAH_READ_PROPERTY(self, CHEETAH_CURRENT);
    if (Z_LVAL_P(current) != page) {
       RETURN_FALSE;
    }
    RETURN_TRUE;
}
/* }}} */

/** {{{ $page_size, $page
 */
ZEND_BEGIN_ARG_INFO(pageset_construct_args, 0)
    ZEND_ARG_INFO(0, page_size)
    ZEND_ARG_INFO(0, page)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $record_count
 */
ZEND_BEGIN_ARG_INFO(pageset_setRecordCount_args, 0)
    ZEND_ARG_INFO(0, record_count)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $count
 */
ZEND_BEGIN_ARG_INFO(pageset_siblings_args, 0)
    ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $page, $show, $else_show
 */
ZEND_BEGIN_ARG_INFO(pageset_compare_args, 0)
    ZEND_ARG_INFO(0, page)
    ZEND_ARG_INFO(0, show)
    ZEND_ARG_INFO(0, else_show)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $page
 */
ZEND_BEGIN_ARG_INFO(pageset_isCurrent_args, 0)
    ZEND_ARG_INFO(0, page)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{
 */
zend_function_entry cheetah_page_core_methods[] = {
    PHP_ME(cheetah_page_core, __construct     , pageset_construct_args  , ZEND_ACC_PUBLIC  | ZEND_ACC_CTOR)
    PHP_ME(cheetah_page_core, setRecordCount  , pageset_setRecordCount_args    , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, getCurrent      , NULL    , ZEND_ACC_PUBLIC)
    PHP_MALIAS(cheetah_page_core, current     , getCurrent    , NULL              , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, getPageCount    , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, getPageSize     , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, getRecordCount  , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, getStartRow     , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, getOffset       , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, getEndRow       , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, hasPrev         , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, hasNext         , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, isLast          , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, isFirst         , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, hasList         , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, first           , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, prev            , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, next            , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, last            , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, all             , NULL       , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, siblings        , pageset_siblings_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, compare         , pageset_compare_args   , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_page_core, isCurrent       , pageset_isCurrent_args , ZEND_ACC_PUBLIC) {
        NULL, NULL, NULL
    }

};
/* }}} */

/** {{{
 */
PHP_MINIT_FUNCTION(cheetah_page_core) {
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "CheetahPageCore", cheetah_page_core_methods);
    cheetah_page_core_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

    zend_declare_property_null(cheetah_page_core_ce, ZEND_STRL(CHEETAH_PAGE_SIZE)    , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_page_core_ce, ZEND_STRL(CHEETAH_CURRENT)      , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_page_core_ce, ZEND_STRL(CHEETAH_RECORD_COUNT) , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_page_core_ce, ZEND_STRL(CHEETAH_PAGE_COUNT)   , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_page_core_ce, ZEND_STRL(CHEETAH_START_ROW)    , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_page_core_ce, ZEND_STRL(CHEETAH_END_ROW)      , ZEND_ACC_PRIVATE TSRMLS_CC);

}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */