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

#ifndef CHEETAH_CONFIG_H
#define CHEETAH_CONFIG_H

#define CHEETAH_DEBUG						"CHEETAH_DEBUG"
#define CHEETAH_VERSION						"0.6.0.0"
#define CHEETAH_SUPPORT_URL					"http://code.google.com/p/heavenmvc/"
#define CHEETAH_AUTHOR_EMAIL				"wangwei(wwww6662003@163.com)"

#define CHEETAH_DEFAULT_CONTROLLER			"index"
#define CHEETAH_DEFAULT_ACTION				"index"
#define CHEETAH_DEFAULT_CONTROLLER_SUFFIX	"Controller"
#define CHEETAH_DEFAULT_CONTROLLER_KEY	"controller"
#define CHEETAH_DEFAULT_ACTION_KEY	"action"
#define CHEETAH_DEFAULT_MODEL_SUFFIX		"Model"
#define CHEETAH_DEFAULT_LIB_SUFFIX			"Lib"
#define CHEETAH_DEFAULT_TEMPLATE_EXTNAME			".tpl.php"
#define CHEETAH_DEFAULT_TEMPLATE_NAME			"index"
#define CHEETAH_DEFAULT_PHP_EXTNAME			".php"
#define CHEETAH_PATH_DELIMITER			"/"
#define CHEETAH_PATH_CONSOLE			"console"
#define CHEETAH_PATH_CONSOLE_LENGTH			7
#define CHEETAH_TEMPLATE_DIR			"template"
#define CHEETAH_TEMPLATE_DIR_LENGTH			8

/** {{{ CHEETAH_read_property
 */
#define CHEETAH_READ_PROPERTY(obj, property) \
zend_read_property(Z_OBJCE_P(obj), obj, ZEND_STRL(property), 0 TSRMLS_CC)
/* }}} */

/** {{{ CHEETAH_update_property
 */
#define CHEETAH_UPDATE_PROPERTY(obj, property, value) \
	do { \
	zend_update_property(Z_OBJCE_P(obj), obj, ZEND_STRL(property), value TSRMLS_CC); \
	} while(0)
/* }}} */

/** {{{ CHEETAH_update_property_stringl
 */
#define CHEETAH_UPDATE_PROPERTY_STRINGL(obj, property, value, len) \
	do { \
	zend_update_property_stringl(Z_OBJCE_P(obj), obj, ZEND_STRL(property), value, len TSRMLS_CC); \
	} while(0)
/* }}} */

/** {{{ zend_update_static_property
 */
#define CHEETAH_UPDATE_STATIC_PROPERTY(ce, property, value) \
	do {\
	zend_update_static_property(ce, ZEND_STRL(property), value TSRMLS_CC); \
	} while(0)
/* }}} */

/** {{{ CHEETAH_read_static_property
 */
#define CHEETAH_READ_STATIC_PROPERTY(ce, property) \
zend_read_static_property(ce, ZEND_STRL(property), 0 TSRMLS_CC)
/* }}} */

/** {{{ new array
 */
#define CHEETAH_NEW_ARRAY(arr_name)                                                 \
        MAKE_STD_ZVAL(arr_name);												\
        array_init(arr_name);
/* }}} */

/** {{{ ptr dtor
 */
#define CHEETAH_PTR_DTOR(zp)                                                        \
        if (zp) {                                                              \
            zval_ptr_dtor(&zp);                                                \
        }
/* }}} */

/** {{{ dtor
 */
#define CHEETAH_DTOR(z)                                                             \
        if (&z) {                                                              \
            zval_dtor(&z);                                                     \
        }
/* }}} */

/** {{{ redefine
 */
#define CHEETAH_RE_DEFINE(zp)                                                       \
          CHEETAH_PTR_DTOR(zp)                                                      \
          MAKE_STD_ZVAL(zp)
/* }}} */

/** {{{ efree
 */
#define CHEETAH_FREE(v)                                                             \
        if (v) {                                                               \
            efree(v);                                                          \
        }
/* }}} */

#ifndef Z_ADDREF_P
#define Z_ADDREF_P 	 ZVAL_ADDREF
#define Z_REFCOUNT_P ZVAL_REFCOUNT
#define Z_DELREF_P 	 ZVAL_DELREF
#endif

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
