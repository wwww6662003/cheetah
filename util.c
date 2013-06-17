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
  | util.c: refer to the framework of zoeey(moxie(system128@gmail.com)). |
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
#include "util.h"


/** {{{ current cheetah_util_core object
 */
zend_class_entry * cheetah_util_core_ce;
/* }}} */

void cheetah_error(int type TSRMLS_DC, const char * name, ... ) {
    va_list ap                ;
    int     len             = 0;
    char *  buf             = NULL;
    char *  msg             = NULL;

    if (EG(exception)) {
        zend_throw_exception_object(EG(exception) TSRMLS_CC);
        return;
    }

    va_start(ap, name);
    len = vspprintf(&buf, 0, name, ap);
    va_end(ap);
    spprintf(&msg, 0, INI_STR("errors_doc_url"), buf);

    php_error(type, msg);
    efree(msg);
    efree(buf);
}


zval * cheetah_call_method(zval **object_pp, zend_class_entry *obj_ce
                             , zend_function **fn_proxy, char *function_name
                             , int function_name_len, zval **retval_ptr_ptr
                             , int param_count, zval *** params TSRMLS_DC) {

        int result;
        zend_fcall_info fci;
        zval z_fname;
        zval *retval;
        HashTable *function_table;

        fci.size = sizeof(fci);
        /*fci.function_table = NULL; will be read form zend_class_entry of object if needed */
        #if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 2
        fci.object_pp = object_pp;
        #elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3
        fci.object_ptr = object_pp ? *object_pp : NULL;
        #endif

        fci.function_name = &z_fname;
        fci.retval_ptr_ptr = retval_ptr_ptr ? retval_ptr_ptr : &retval;
        fci.param_count = param_count;
        fci.params = params;
        fci.no_separation = 1;
        fci.symbol_table = NULL;

        if (!fn_proxy && !obj_ce) {
                /* no interest in caching and no information already present that is
                 * needed later inside zend_call_function. */
                ZVAL_STRINGL(&z_fname, function_name, function_name_len, 0);
                fci.function_table = !object_pp ? EG(function_table) : NULL;
                result = zend_call_function(&fci, NULL TSRMLS_CC);
        } else {
                zend_fcall_info_cache fcic;

                fcic.initialized = 1;
                if (!obj_ce) {
                        obj_ce = object_pp ? Z_OBJCE_PP(object_pp) : NULL;
                }
                if (obj_ce) {
                        function_table = &obj_ce->function_table;
                } else {
                        function_table = EG(function_table);
                }
                if (!fn_proxy || !*fn_proxy) {
                        if (zend_hash_find(function_table, function_name, function_name_len+1, (void **) &fcic.function_handler) == FAILURE) {
                                /* error at c-level */
                                zend_error(E_CORE_ERROR, "Couldn't find implementation for method %s%s%s", obj_ce ? obj_ce->name : "", obj_ce ? "::" : "", function_name);
                        }
                        if (fn_proxy) {
                                *fn_proxy = fcic.function_handler;
                        }
                } else {
                        fcic.function_handler = *fn_proxy;
                }
                fcic.calling_scope = obj_ce;

                #if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 2
                fcic.object_pp = object_pp;
                #elif PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3
                if (object_pp) {
                        fcic.called_scope = Z_OBJCE_PP(object_pp);
                } else if (obj_ce &&
                        !(EG(called_scope) &&
                        instanceof_function(EG(called_scope), obj_ce TSRMLS_CC)))
                {
                        fcic.called_scope = obj_ce;
                } else {
                        fcic.called_scope = EG(called_scope);
                }
                fcic.object_ptr = object_pp ? *object_pp : NULL;
                #endif
                result = zend_call_function(&fci, &fcic TSRMLS_CC);
        }
        if (result == FAILURE) {
                /* error at c-level */
                if (!obj_ce) {
                        obj_ce = object_pp ? Z_OBJCE_PP(object_pp) : NULL;
                }
                if (!EG(exception)) {
                        zend_error(E_CORE_ERROR, "Couldn't execute method %s%s%s", obj_ce ? obj_ce->name : "", obj_ce ? "::" : "", function_name);
                }
        }
        if (!retval_ptr_ptr) {
                if (retval) {
                        zval_ptr_dtor(&retval);
                }
                return NULL;
        }
        return *retval_ptr_ptr;
}

/** {{{ public CheetahUtilCore::__construct()
 */
PHP_METHOD(cheetah_util_core, __construct) {

}
/* }}} */
/** {{{ public CheetahUtilCore::setVar()
 */
PHP_METHOD(cheetah_util_core, setVar) {
	zval * self           = NULL;
    zval * label          = NULL;
    zval * name           = NULL;
    zval * brief          = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|zz"
		, &label
		, &name
		, &brief
		) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    self = getThis();
	CHEETAH_UPDATE_PROPERTY(self, CHEETAH_LABEL, label);
    if (name) {
       CHEETAH_UPDATE_PROPERTY(self, CHEETAH_NAME, name);
    }
    if (brief) {
        CHEETAH_UPDATE_PROPERTY(self, CHEETAH_BRIEF, brief);
    }
}
/* }}} */

/** {{{ public CheetahUtilCore::getLabel()
 */
PHP_METHOD(cheetah_util_core, getLabel) {
    zval *   self         = NULL;
    zval *   label        = NULL;
    self = getThis();
    label = CHEETAH_READ_PROPERTY(self, CHEETAH_LABEL);
    RETURN_ZVAL(label, 1, 0);
}
/* }}} */

/** {{{ public CheetahUtilCore::getName()
 */
PHP_METHOD(cheetah_util_core, getName) {
    zval * self        = NULL;
    zval * name        = NULL;
    self = getThis();
    name = CHEETAH_READ_PROPERTY(self, CHEETAH_NAME);
    if (name) {
        RETURN_ZVAL(name, 1, 0);
    }
    RETURN_NULL();
}
/* }}} */

/** {{{ public CheetahUtilCore::getBrief()
 */
PHP_METHOD(cheetah_util_core, getBrief) {
    zval *  self        = NULL;
    zval *  brief       = NULL;
    zval *  name_z      = NULL;
    char *  name        = NULL;
    int     name_len    = 0;
    zval ** msg_pp      = NULL;
    zval *  msg_p       = NULL;
    zval *  msg            = NULL;
    zval    is_debug          ;
    char *  debug_msg   = NULL;
    int     len         = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s"
                            , &name, &name_len
                            ) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    self = getThis();
    brief = CHEETAH_READ_PROPERTY(self, CHEETAH_BRIEF);
    if (!brief) {
        RETURN_NULL();
    } 
    if (Z_TYPE_P(brief) == IS_ARRAY && name_len) {
    
        if (zend_hash_find(Z_ARRVAL_P(brief), name, name_len + 1, (void **) &msg_pp) == SUCCESS) {           
            ALLOC_INIT_ZVAL(msg);
            *msg = **msg_pp;
        }
    } else if (Z_TYPE_P(brief) == IS_OBJECT && name_len) {
            msg_p = zend_read_property(Z_OBJCE_P(brief), brief, name, name_len , 1 TSRMLS_CC);
            ALLOC_INIT_ZVAL(msg);
            *msg = *msg_p;
    } else if (Z_TYPE_P(brief) == IS_STRING) {
            ALLOC_INIT_ZVAL(msg);
            *msg = *brief;
    } 
    if (!msg) {
        RETURN_NULL();
    }
    zval_copy_ctor(msg);
    INIT_PZVAL(msg);
    if (zend_get_constant(ZEND_STRL(CHEETAH_DEBUG), & is_debug TSRMLS_CC)) {
        name_z = zend_read_property(cheetah_util_core_ce, self, ZEND_STRL(CHEETAH_NAME), 0 TSRMLS_CC);
        if (Z_BVAL(is_debug) && Z_TYPE_P(name_z) == IS_STRING) {
            len = spprintf(&debug_msg, len, "[%s]%s", Z_STRVAL_P(name_z), Z_STRVAL_P(msg));
            CHEETAH_RE_DEFINE(msg);
            ZVAL_STRINGL(msg, debug_msg, len, 1);
            efree(debug_msg);
        }
        CHEETAH_DTOR(is_debug);
    }
    RETURN_ZVAL(msg, 1, 1);
}
/* }}} */

/** {{{ public CheetahUtilCore::toArray()
 */
PHP_METHOD(cheetah_util_core, toArray) {
    zval *   self        = NULL;
    zval *   array       = NULL;
    zval *   label       = NULL;
    zval *   name        = NULL;
    zval *   brief       = NULL;
    self = getThis();
    label = CHEETAH_READ_PROPERTY(self, CHEETAH_LABEL);
    name  = CHEETAH_READ_PROPERTY(self, CHEETAH_NAME);
    brief = CHEETAH_READ_PROPERTY(self, CHEETAH_BRIEF);
    CHEETAH_NEW_ARRAY(array);
    Z_ADDREF_P(label);
    add_assoc_zval(array, "label", label);
    if (name){
       Z_ADDREF_P(name);
       add_assoc_zval(array, "name", name);
    } else {
       add_assoc_null(array, "name");
    }
    if (brief){
       Z_ADDREF_P(brief);
       add_assoc_zval(array, "brief", brief);
    } else {
       add_assoc_null(array, "brief");
    }
    RETURN_ZVAL(array, 1, 1);
}
/* }}} */

#define CHEETAH_IS_METHOD(method_name)                                              \
    zval ** method = NULL;                                                     \
    method = get_track(NULL, ZEND_STRL("REQUEST_METHOD")                       \
                     , NULL, TRACK_VARS_SERVER TSRMLS_CC);                     \
    if (method                                                                 \
            && Z_TYPE_PP(method) == IS_STRING                                  \
            && strcmp(Z_STRVAL_PP(method),method_name) == 0                    \
            ) {                                                                \
        RETURN_TRUE;                                                           \
    }                                                                          \
    RETURN_FALSE;


#define CHEETAH_GET_TRACK(track)                                                    \
    char *   key         = NULL;                                               \
    int      key_len     = 0;                                                  \
    zval *   def         = NULL;                                               \
    zval *   self        = NULL;                                               \
    zval **  val         = NULL;                                               \
    zval *   values      = NULL;                                               \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z"                 \
            , &key, &key_len                                                   \
            , &def                                                             \
            ) == FAILURE) {                                                    \
        RETURN_FALSE;                                                          \
    }                                                                          \
    self = getThis();                                                                          \
    values = zend_read_property(cheetah_util_core_ce, self, ZEND_STRL(CHEETAH_VALUES)   , 1 TSRMLS_CC); \
    val = get_track(values, key,key_len, def,track TSRMLS_CC);                 \
    if (val) {                                                                 \
        RETURN_ZVAL(*val, 1, 0);                                               \
    }else{                                                                     \
        RETURN_NULL();                                                         \
    }


#define CHEETAH_FROM_TRACK(track)                                                   \
    zval *  fields      = NULL;                                                \
    zval *  names       = NULL;                                                \
    zval *  def         = NULL;                                                \
    zval *   self        = NULL;                                               \
    zval *   values      = NULL;                                               \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z"                \
            , &fields                                                          \
            , &names                                                           \
            , &def                                                             \
            ) == FAILURE) {                                                    \
        RETURN_FALSE;                                                          \
    }                                                                          \
    self = getThis();                                                          \
    values = zend_read_property(cheetah_util_core_ce, self, ZEND_STRL(CHEETAH_VALUES)   , 1 TSRMLS_CC); \
    from_track_var(values, fields, names, def, track TSRMLS_CC);                       \
    RETURN_TRUE;


/** {{{  names to list
 *  title,content to ["title","content"]
 */
static zval * to_list(char * names,int names_len TSRMLS_DC) {
    zval * list         = NULL;
    char   ch           = 0;
    char * name         = NULL;
    int    name_len     = 0;
    int    i            = 0;

    name = emalloc(sizeof(char) * names_len);

    CHEETAH_NEW_ARRAY(list);

    for(i = 0; i < names_len; i++) {
        ch = *(names + i);
         /* [a-zA-Z0-9_]+ */
        if (   (ch > 96 && ch < 123)
            || (ch > 64 && ch < 91)
            || (ch > 47 && ch < 58)
            ||  ch == 95
        ) {
            *(name + name_len) = ch;
            name_len++;
        } else {
            if (name_len > 0) {
                add_next_index_stringl(list, name, name_len, 1);
            }
            name_len = 0;
        }
    }

    if (name_len > 0) {
        add_next_index_stringl(list, name, name_len, 1);
    }

    CHEETAH_FREE(name);

    return list;
}
/* }}} */

/** {{{ get global by name
 */
zval ** get_global_by_name(zval * values, char * name, int name_len, char * key, int key_len, zval * def TSRMLS_DC) {
    zval ** val         = NULL;
    zval ** datas       = NULL;
    zend_bool is_def    = 0;
    zend_bool jit_init  = 0;


    do{
        jit_init = (PG(auto_globals_jit) && !PG(register_globals) && !PG(register_long_arrays));

        if (jit_init) {
            zend_is_auto_global(name, name_len TSRMLS_CC);
        }

        if (zend_hash_find(&EG(symbol_table), name, name_len+1, (void **) &datas) == SUCCESS
            && Z_TYPE_PP(datas) == IS_ARRAY
            && zend_hash_find(Z_ARRVAL_PP(datas), key, key_len + 1, (void **) &val) == SUCCESS
            ) {
            break;
        }


        if (strcmp(name,"_REQUEST") == 0
               && values && Z_TYPE_P(values) == IS_ARRAY
               &&
               zend_hash_find(Z_ARRVAL_P(values)
                         , key , key_len + 1
                         , (void **) &val) == SUCCESS) {
               break;
         }

        is_def = 1;
    } while(0);

    if (is_def && def) {
       val = &def;
    }

    return val;
}
/* }}} */

/** {{{ get track
 */
zval ** get_track(zval * values, char * key, int key_len, zval * def, int track TSRMLS_DC) {
    zval **   val       = NULL;
    zend_bool is_def    = 0;

    switch(track) {
        case TRACK_VARS_REQUEST:

              val = get_global_by_name(values, ZEND_STRL("_REQUEST")
                                      , key, key_len, def TSRMLS_CC);

              break;
        case TRACK_VARS_SERVER:

              val = get_global_by_name(values, ZEND_STRL("_SERVER")
                                      , key, key_len, def TSRMLS_CC);
              break;
        case TRACK_VARS_FILES:

              val = get_global_by_name(values, ZEND_STRL("_FILES")
                                      , key, key_len, def TSRMLS_CC);
              break;
        /*case CHEETAH_TRACK_VARS_SESSION:
              if (PS(session_status) != php_session_active &&
                  PS(session_status) != php_session_disabled) {
                  is_def = 1;
                  break;
              }

              val = get_global_by_name(values, ZEND_STRL("_SESSION")
                                      , key, key_len, def TSRMLS_CC);
              break;*/
        default:
              /* POST GET COOKIE */
               if (PG(http_globals)[track]
                   &&
                   zend_hash_find(Z_ARRVAL_P(PG(http_globals)[track])
                             , key , key_len + 1
                             , (void **) &val) == SUCCESS) {
                   break;
               }

               if (track == TRACK_VARS_GET
                   && values && Z_TYPE_P(values) == IS_ARRAY
                   &&
                   zend_hash_find(Z_ARRVAL_P(values)
                             , key , key_len + 1
                             , (void **) &val) == SUCCESS) {
                   break;
               }

               is_def = 1;
       }

    if (track != TRACK_VARS_FILES
        && is_def
        && def) {
       val = &def;
    }

    if (val && *val){
        Z_ADDREF_P(*val);
    }

    return val;
}
/* }}} */

/** {{{ get client ip
 */
zval * get_ip(zend_bool is_format TSRMLS_DC) {
    zval **   val           = NULL;
    char  *   ip            = NULL;
    zval  *   ip_long       = NULL;
    char  *   pos           = NULL;
    int       len           = 0;
    zval  *   params[1]        ;
    zval  *   ip2long       = NULL;
    zval  *   ret           = NULL;


    do {
        val = get_track(NULL, ZEND_STRL("HTTP_CLIENT_IP"), NULL,TRACK_VARS_SERVER TSRMLS_CC);

        if (val && Z_TYPE_PP(val) == IS_STRING) {
            break;
        }

        val = get_track(NULL, ZEND_STRL("HTTP_X_FORWARDED_FOR"), NULL,TRACK_VARS_SERVER TSRMLS_CC);

        if (val && Z_TYPE_PP(val) == IS_STRING) {
            break;
        }

        val = get_track(NULL, ZEND_STRL("REMOTE_ADDR"), NULL, TRACK_VARS_SERVER TSRMLS_CC);

        if (val && Z_TYPE_PP(val) == IS_STRING) {
            break;
        }

    } while (0);

    do {

        if (!val) {
            break;
        }
        /**
         * for test
         * ZVAL_STRING(*val,"127.0.0.2,127.0.0.3", 1);
         */

        pos = strchr(Z_STRVAL_PP(val), ',');

        if (pos) {
            len = pos - Z_STRVAL_PP(val);
            ip  = estrndup(Z_STRVAL_PP(val), len);
        }else{
            len = Z_STRLEN_PP(val);
            ip  = estrndup(Z_STRVAL_PP(val), len);
        }

        if (is_format) {
            ALLOC_INIT_ZVAL(ret);
            ZVAL_STRINGL(ret, ip, len, 1);
            break;
        }
        /* ip string */
        ALLOC_INIT_ZVAL(params[0]);
        ZVAL_STRINGL(params[0], ip, len, 1);
        /* function name */
        ALLOC_INIT_ZVAL(ip2long);
        ZVAL_STRINGL(ip2long  , "ip2long", 7, 1);

        ALLOC_INIT_ZVAL(ip_long);

        if (call_user_function(EG(function_table), NULL, ip2long, ip_long, 1, params TSRMLS_CC) == SUCCESS) {
            ALLOC_INIT_ZVAL(ret);
            ZVAL_LONG(ret, Z_LVAL_P(ip_long));
            CHEETAH_PTR_DTOR(ip_long);
        }

        CHEETAH_PTR_DTOR(params[0]);
        CHEETAH_PTR_DTOR(ip2long);

    } while (0);

    if (!ret && !is_format) {
        ALLOC_INIT_ZVAL(ret);
        ZVAL_LONG(ret, 0);
    }

    CHEETAH_FREE(ip);

    return ret;
}
/* }}} */

/** {{{  from track
 */
void from_track_var(zval * values, zval * fields, zval * names, zval * def, int track TSRMLS_DC) {
    zval      *  names_list     = NULL;
    zval     **  field          = NULL;
    HashTable *  names_table    = NULL;
    HashPosition names_pointer  = NULL;
    zval     **  val            = NULL;

    if (Z_TYPE_P(fields) == IS_NULL) {
        convert_to_array_ex(&fields);
    }

    if (Z_TYPE_P(fields) == IS_OBJECT || Z_TYPE_P(fields) == IS_ARRAY) {
        if (Z_TYPE_P(names) == IS_ARRAY) {
            names_table = Z_ARRVAL_P(names);
        } else if (Z_TYPE_P(names) == IS_STRING) {
            names_list = to_list(Z_STRVAL_P(names) ,Z_STRLEN_P(names) TSRMLS_CC);
            names_table = Z_ARRVAL_P(names_list);
        } else {
            return;
        }


        for (zend_hash_internal_pointer_reset_ex(names_table, &names_pointer);
                            zend_hash_get_current_data_ex(names_table, (void**) &field, &names_pointer) == SUCCESS;
                            zend_hash_move_forward_ex(names_table, &names_pointer)) {
            if (field == NULL || Z_TYPE_PP(field) == IS_NULL) {
                continue;
            }
            if (Z_TYPE_PP(field) != IS_STRING){
                convert_to_string_ex(field);
            }

            val = get_track(values, Z_STRVAL_PP(field),Z_STRLEN_PP(field), def, track TSRMLS_CC);

            if (Z_TYPE_P(fields) == IS_OBJECT) {
                if (val && *val) {
                     add_property_zval_ex(fields, Z_STRVAL_PP(field),Z_STRLEN_PP(field) + 1
                                    , *val TSRMLS_CC);
                }else{
                     add_property_null_ex(fields, Z_STRVAL_PP(field),Z_STRLEN_PP(field) + 1 TSRMLS_CC);
                }
            }
            if (Z_TYPE_P(fields) == IS_ARRAY) {
                if (val && *val) {
                     add_assoc_zval(fields, Z_STRVAL_PP(field), *val);
                }else{
                     add_assoc_null(fields, Z_STRVAL_PP(field));
                }
            }
        }
    }

    CHEETAH_PTR_DTOR(names_list);
}
/* }}} */




/** {{{  from ip
 */
void from_ip(zval * fields, zval * names, zend_bool is_format TSRMLS_DC) {
    zval      *  names_list     = NULL;
    zval     **  field          = NULL;
    HashTable *  names_table    = NULL;
    HashPosition names_pointer  = NULL;
    zval      *  val            = NULL;

    if (Z_TYPE_P(fields) == IS_NULL) {
        convert_to_array_ex(&fields);
    }

    if (Z_TYPE_P(fields) == IS_OBJECT || Z_TYPE_P(fields) == IS_ARRAY) {
         if (Z_TYPE_P(names) == IS_ARRAY) {
            names_table = Z_ARRVAL_P(names);
        } else if (Z_TYPE_P(names) == IS_STRING) {
            names_list = to_list(Z_STRVAL_P(names) ,Z_STRLEN_P(names) TSRMLS_CC);
            names_table = Z_ARRVAL_P(names_list);
        } else {
            return;
        }

        for (zend_hash_internal_pointer_reset_ex(names_table, &names_pointer);
                            zend_hash_get_current_data_ex(names_table, (void**) &field, &names_pointer) == SUCCESS;
                            zend_hash_move_forward_ex(names_table, &names_pointer)) {
            if (field == NULL || Z_TYPE_PP(field) == IS_NULL) {
                continue;
            }

            if (Z_TYPE_PP(field) != IS_STRING){
                convert_to_string_ex(field);
            }
            val = get_ip(is_format TSRMLS_CC);

            if (Z_TYPE_P(fields) == IS_OBJECT) {
                if (val) {
                     add_property_zval_ex(fields, Z_STRVAL_PP(field),Z_STRLEN_PP(field) + 1
                                    , val TSRMLS_CC);
                }else{
                     add_property_null_ex(fields, Z_STRVAL_PP(field),Z_STRLEN_PP(field) + 1 TSRMLS_CC);
                }
            }
            if (Z_TYPE_P(fields) == IS_ARRAY) {
                if (val) {
                     add_assoc_zval(fields, Z_STRVAL_PP(field), val);
                }else{
                     add_assoc_null(fields, Z_STRVAL_PP(field));
                }
            }

        }
    }
    CHEETAH_PTR_DTOR(names_list);
}
/* }}} */

/** {{{ public ZeLoader::isGet()
 */
PHP_METHOD(cheetah_util_core, isGet) {
   CHEETAH_IS_METHOD("GET");
}
/* }}} */

/** {{{ public ZeLoader::isPost()
 */
PHP_METHOD(cheetah_util_core, isPost) {
   CHEETAH_IS_METHOD("POST");
}
/* }}} */

/** {{{ public ZeLoader::isPut()
 */
PHP_METHOD(cheetah_util_core, isPut) {
   CHEETAH_IS_METHOD("PUT");
}
/* }}} */

/** {{{ public ZeLoader::isDelete()
 */
PHP_METHOD(cheetah_util_core, isDelete) {
   CHEETAH_IS_METHOD("DELETE");
}
/* }}} */


/** {{{ public ZeLoader::isTrace()
 */
PHP_METHOD(cheetah_util_core, isTrace) {
   CHEETAH_IS_METHOD("TRACE");
}
/* }}} */

/** {{{ public ZeLoader::getGet($key, $default = null)
 */
PHP_METHOD(cheetah_util_core, getGet) {
    CHEETAH_GET_TRACK(TRACK_VARS_GET);
}
/* }}} */

/** {{{ public ZeLoader::fromGet(&$fields, $names, $default = null)
 */
PHP_METHOD(cheetah_util_core, fromGet) {
    CHEETAH_FROM_TRACK(TRACK_VARS_GET);
}
/* }}} */

/** {{{ public ZeLoader::getPost($key, $default = null)
 */
PHP_METHOD(cheetah_util_core, getPost) {
    CHEETAH_GET_TRACK(TRACK_VARS_POST);
}
/* }}} */

/** {{{ public ZeLoader::fromPost(&$fields, $names, $default = null)
 */
PHP_METHOD(cheetah_util_core, fromPost) {
    CHEETAH_FROM_TRACK(TRACK_VARS_POST);
}
/* }}} */

/** {{{ public ZeLoader::getCookie($key, $default = null)
 */
PHP_METHOD(cheetah_util_core, getCookie) {
    CHEETAH_GET_TRACK(TRACK_VARS_COOKIE);
}
/* }}} */

/** {{{ public ZeLoader::fromCookie(&$fields, $names, $default = null)
 */
PHP_METHOD(cheetah_util_core, fromCookie) {
    CHEETAH_FROM_TRACK(TRACK_VARS_COOKIE);
}
/* }}} */


/** {{{ public ZeLoader::getRequest($key, $default = null)
 */
PHP_METHOD(cheetah_util_core, getRequest) {
    CHEETAH_GET_TRACK(TRACK_VARS_REQUEST)
}
/* }}} */


/** {{{ public ZeLoader::fromRequest(&$fields, $names, $default = null)
 */
PHP_METHOD(cheetah_util_core, fromRequest) {
    CHEETAH_FROM_TRACK(TRACK_VARS_REQUEST);
}
/* }}} */


/** {{{ public ZeLoader::getServer($key, $default = null)
 */
PHP_METHOD(cheetah_util_core, getServer) {
    CHEETAH_GET_TRACK(TRACK_VARS_SERVER)
}
/* }}} */


/** {{{ public ZeLoader::fromServer(&$fields, $names, $default = null)
 */
PHP_METHOD(cheetah_util_core, fromServer) {
    CHEETAH_FROM_TRACK(TRACK_VARS_SERVER);
}
/* }}} */

/** {{{ public ZeLoader::getFile($key, $default = null)
 */
PHP_METHOD(cheetah_util_core, getFile) {
    CHEETAH_GET_TRACK(TRACK_VARS_FILES)
}
/* }}} */


/** {{{ public ZeLoader::fromFile(&$fields, $names, $default = null)
 */
PHP_METHOD(cheetah_util_core, fromFile) {
    CHEETAH_FROM_TRACK(TRACK_VARS_FILES);
}
/* }}} */



/** {{{ public ZeLoader::getSession($key, $default = null)
 */
PHP_METHOD(cheetah_util_core, getSession) {
    CHEETAH_GET_TRACK(CHEETAH_TRACK_VARS_SESSION);
}
/* }}} */

/** {{{ public ZeLoader::fromSession(&$fields, $names, $default = null)
 */
PHP_METHOD(cheetah_util_core, fromSession) {
    CHEETAH_FROM_TRACK(CHEETAH_TRACK_VARS_SESSION);
}
/* }}} */


/** {{{ public ZeLoader::getIp($isFormat)
 */
PHP_METHOD(cheetah_util_core, getIp) {
    zend_bool is_format     = 0;
    zval * ret              = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b"
            , &is_format
            ) == FAILURE) {
        RETURN_FALSE;
    }
   ret = get_ip(is_format TSRMLS_CC);
   if (ret){
       RETURN_ZVAL(ret, 1, 1);
   } else {
       RETURN_NULL();
   }

}
/* }}} */

/** {{{ public ZeLoader::fromIp(&$fields, $names, $isFormat = false)
 */
PHP_METHOD(cheetah_util_core, fromIp) {
    zval *  fields          = NULL;
    zval *  names           = NULL;
    zend_bool  is_format    = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|b"
            , &fields
            , &names
            , &is_format
            ) == FAILURE) {
        RETURN_FALSE;
    }

    from_ip(fields, names, is_format TSRMLS_CC);

    RETURN_TRUE;
}
/* }}} */

/** {{{ public ZeLoader::setValues($values)
 */
PHP_METHOD(cheetah_util_core, setValues) {
    zval *  self         = NULL;
    zval *  values       = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a"
            , &values
            ) == FAILURE) {
        RETURN_FALSE;
    }
    self = getThis();

    zend_update_property(cheetah_util_core_ce, self, ZEND_STRL(CHEETAH_VALUES), values TSRMLS_CC);

    RETURN_TRUE;
}
/* }}} */


/** {{{ setVar
 */
ZEND_BEGIN_ARG_INFO(cheetah_util_core_setVar_args, 0)
    ZEND_ARG_INFO(0, label)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, brief)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ getBrief
 */
ZEND_BEGIN_ARG_INFO(cheetah_util_core_getBrief_args, 0)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $name, $default = null
 */
ZEND_BEGIN_ARG_INFO(cheetah_util_core_get_track_args, 0)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, def)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ &$fields, $names, $default = null
 */
ZEND_BEGIN_ARG_INFO(cheetah_util_core_from_track_args, 0)
    ZEND_ARG_INFO(1, fields)
    ZEND_ARG_INFO(0, names)
    ZEND_ARG_INFO(0, def)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $isFormat = false
 */
ZEND_BEGIN_ARG_INFO(cheetah_util_core_getIp_args, 0)
    ZEND_ARG_INFO(0, is_format)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ &$fields, $names, $isFormat = false
 */
ZEND_BEGIN_ARG_INFO(cheetah_util_core_fromIp_args, 0)
    ZEND_ARG_INFO(1, fields)
    ZEND_ARG_INFO(0, names)
    ZEND_ARG_INFO(0, is_format)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ $values
 */
ZEND_BEGIN_ARG_INFO(cheetah_util_core_setValues_args, 0)
    ZEND_ARG_INFO(0, values)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{
 */
zend_function_entry cheetah_util_core_methods[] = {
    PHP_ME(cheetah_util_core, setVar     , cheetah_util_core_setVar_args  , ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(cheetah_util_core, getLabel        , NULL                   , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getName         , NULL                   , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getBrief        , cheetah_util_core_getBrief_args   , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, toArray         , NULL                   , ZEND_ACC_PUBLIC) 
    PHP_ME(cheetah_util_core, isGet       , NULL  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, isPost      , NULL  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, isPut       , NULL  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, isDelete    , NULL  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, isTrace     , NULL  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getGet      , cheetah_util_core_get_track_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromGet     , cheetah_util_core_from_track_args , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getPost     , cheetah_util_core_get_track_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromPost    , cheetah_util_core_from_track_args , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getCookie   , cheetah_util_core_get_track_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromCookie  , cheetah_util_core_from_track_args , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getRequest  , cheetah_util_core_get_track_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromRequest , cheetah_util_core_from_track_args , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getServer   , cheetah_util_core_get_track_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromServer  , cheetah_util_core_from_track_args , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getFile     , cheetah_util_core_get_track_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromFile    , cheetah_util_core_from_track_args , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getSession  , cheetah_util_core_get_track_args  , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromSession , cheetah_util_core_from_track_args , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, getIp       , cheetah_util_core_getIp_args      , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, fromIp      , cheetah_util_core_fromIp_args     , ZEND_ACC_PUBLIC)
    PHP_ME(cheetah_util_core, setValues   , cheetah_util_core_setValues_args  , ZEND_ACC_PUBLIC)
	{  NULL, NULL, NULL}
};
/* }}} */

/** {{{
 */
PHP_MINIT_FUNCTION(cheetah_util_core) {
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "CheetahUtilCore", cheetah_util_core_methods);
    cheetah_util_core_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
    zend_declare_property_null(cheetah_util_core_ce, ZEND_STRL(CHEETAH_LABEL) , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_util_core_ce, ZEND_STRL(CHEETAH_NAME)  , ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(cheetah_util_core_ce, ZEND_STRL(CHEETAH_BRIEF) , ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(cheetah_util_core_ce, ZEND_STRL(CHEETAH_VALUES) , ZEND_ACC_PRIVATE TSRMLS_CC);
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