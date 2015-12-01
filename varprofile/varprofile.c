/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_varprofile.h"

/* If you declare any globals in php_varprofile.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(varprofile)
*/

/* True global resources - no need for thread safety here */
static int le_varprofile;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("varprofile.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_varprofile_globals, varprofile_globals)
    STD_PHP_INI_ENTRY("varprofile.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_varprofile_globals, varprofile_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ static int ref_zval_list_find(ref_zval **rz, zval *z) 
   Return 1 when z found, otherwise return 0 */
static int ref_zval_list_find(ref_zval **rz, zval *z)
{
	while((*rz)->next != NULL) {
		if ((*rz)->z == z)
			return 1;
		*rz = (*rz)->next;
	}
	return (*rz)->z == z ? 1 : 0;
}
/* }}} */

/* {{{ static zval* class_memory_use_ht(HashTable *hashtable, ref_list *rl)
   Return array */
static zval* class_memory_use_profile(HashTable *hashtable, ref_zval_list *rzl)
{
    HashPosition pos;
	char *key, *tmp_key;
	uint key_len, prop_len;
	const char *prop_name, *class_name;
	ulong num_index;
	zval **entry, *subarray;
	MAKE_STD_ZVAL(subarray);
	array_init(subarray);

	if (hashtable == NULL) return subarray;

	zend_hash_internal_pointer_reset_ex(hashtable, &pos);
    while (zend_hash_get_current_data_ex(hashtable, (void **)&entry, &pos) == SUCCESS) {
		if (Z_REFCOUNT_PP(entry) > 1) {
			ref_zval *rz = rzl->head;
			if (rzl->head == NULL) {
				rzl->head = emalloc(sizeof(ref_zval));
				rzl->head->next = NULL;
				rzl->head->z = *entry;
			} else if (ref_zval_list_find(&rz, *entry)) {
				zend_hash_move_forward_ex(hashtable, &pos);//skip this zval
				continue;
			} else {
				rz->next = emalloc(sizeof(ref_zval));
				rz->next->z = *entry;
				rz->next->next = NULL;
			}
		}
        if (zend_hash_get_current_key_ex(hashtable, &key, &key_len, &num_index, 0, &pos) == HASH_KEY_IS_STRING) {
			//non-public attributes key is empty
			zend_unmangle_property_name_ex(key, key_len - 1, &class_name, &prop_name, (int*) &prop_len);
            if (IS_INTERNED(key) && prop_name != key) {
                tmp_key = estrndup(prop_name, prop_len);
            } else {
                tmp_key = NULL;
            }
			switch (Z_TYPE_PP(entry)) {
				case IS_LONG:
				case IS_BOOL:                    
                    tmp_key == NULL ? add_assoc_long(subarray, prop_name, sizeof(long))
                        : add_assoc_long(subarray, tmp_key, sizeof(long));
					break;
				case IS_DOUBLE:
					tmp_key == NULL ? add_assoc_long(subarray, prop_name, sizeof(double))
                        :add_assoc_long(subarray, tmp_key, sizeof(double));
					break;
				case IS_STRING:
					tmp_key == NULL ? add_assoc_long(subarray, prop_name, Z_STRLEN_PP(entry))
                        :add_assoc_long(subarray, tmp_key, Z_STRLEN_PP(entry));
					break;
				case IS_ARRAY:
                    tmp_key == NULL ? add_assoc_zval(subarray, prop_name, array_memory_use_profile(Z_ARRVAL_PP(entry), rzl))
                        :add_assoc_zval(subarray, tmp_key, array_memory_use_profile(Z_ARRVAL_PP(entry), rzl));					
					break;
				case IS_OBJECT:
					tmp_key == NULL ? add_assoc_zval(subarray, prop_name, class_memory_use_profile(Z_OBJPROP_PP(entry), rzl))
                        :add_assoc_zval(subarray, tmp_key, class_memory_use_profile(Z_OBJPROP_PP(entry), rzl));
					break;
			}
            if (IS_INTERNED(key) && prop_name != key) {
                efree(tmp_key);
            }
        }
        zend_hash_move_forward_ex(hashtable, &pos);
    }

	return subarray;
}
/* }}} */

/* {{{ static zval* memory_use_ht(HashTable *hashtable, ref_list *rl)
   Return array */
static zval* array_memory_use_profile(HashTable *hashtable, ref_zval_list *rzl)
{
    HashPosition pos;
    zval **entry, key, *subarray;
	MAKE_STD_ZVAL(subarray);
	array_init(subarray);

    zend_hash_internal_pointer_reset_ex(hashtable, &pos);
    while (zend_hash_get_current_data_ex(hashtable, (void **)&entry, &pos) == SUCCESS) {
		if (Z_REFCOUNT_PP(entry) > 1) {
			ref_zval *rz = rzl->head;
			if (rzl->head == NULL) {
				rzl->head = emalloc(sizeof(ref_zval));
				rzl->head->next = NULL;
				rzl->head->z = *entry;
			} else if (ref_zval_list_find(&rz, *entry)) {
				zend_hash_move_forward_ex(hashtable, &pos);//skip this zval
				continue;
			} else {
				rz->next = emalloc(sizeof(ref_zval));
				rz->next->z = *entry;
				rz->next->next = NULL;
			}
		}
		zend_hash_get_current_key_zval_ex(hashtable, &key, &pos);
		switch (Z_TYPE_PP(entry)) {
			case IS_LONG:
			case IS_BOOL:
				Z_TYPE(key) == IS_STRING ? add_assoc_long(subarray, Z_STRVAL(key), sizeof(long))
					: add_index_long(subarray, Z_LVAL(key), sizeof(long));//add_assoc_long底层会对key进行处理
				break;
			case IS_DOUBLE:
				Z_TYPE(key) == IS_STRING ? add_assoc_long(subarray, Z_STRVAL(key), sizeof(double))
					: add_index_long(subarray, Z_LVAL(key), sizeof(double));
				break;
			case IS_STRING:
				Z_TYPE(key) == IS_STRING ? add_assoc_long(subarray, Z_STRVAL(key), Z_STRLEN_PP(entry))
					: add_index_long(subarray, Z_LVAL(key), Z_STRLEN_PP(entry));
				break;
			case IS_ARRAY:
				Z_TYPE(key) == IS_STRING ? add_assoc_zval(subarray, Z_STRVAL(key), array_memory_use_profile(Z_ARRVAL_PP(entry), rzl))
					: add_index_zval(subarray, Z_LVAL(key), array_memory_use_profile(Z_ARRVAL_PP(entry), rzl));
				break;
			case IS_OBJECT:
                if (Z_OBJ_HT_PP(entry)->get_properties != NULL) {
				    Z_TYPE(key) == IS_STRING ? add_assoc_zval(subarray, Z_STRVAL(key), class_memory_use_profile(Z_OBJPROP_PP(entry), rzl))
					   : add_index_zval(subarray, Z_LVAL(key), class_memory_use_profile(Z_OBJPROP_PP(entry), rzl));
				}
                break;
		}
        zend_hash_move_forward_ex(hashtable, &pos);
		if (Z_TYPE(key) == IS_STRING) {
			str_efree(Z_STRVAL(key));//有可用key的strval是动态emalloc的，需要释放
		}
    }

	return subarray;
}

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_varprofile_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(varprofile)
{
    ref_zval *rz;
	ref_zval_list *rzl;
	rzl = emalloc(sizeof(ref_zval_list));
	rzl->head = NULL;

	array_init(return_value);

    /* profile global variables */
    add_assoc_zval(return_value, "global", array_memory_use_profile(&EG(symbol_table), rzl));

    /* profile current scope variables */
    if (!EG(active_symbol_table)) {                                                                                                                                                
        zend_rebuild_symbol_table(TSRMLS_C);
    }
    if (EG(active_symbol_table) != &EG(symbol_table)) {
        add_assoc_zval(return_value, "active", array_memory_use_profile(EG(active_symbol_table), rzl));
    }

	/* profile function static variables */
    HashPosition pos;
    HashTable *hashtable;
	zval *parentarray;
	MAKE_STD_ZVAL(parentarray);
	array_init(parentarray);
	zend_function *zf;

	zend_hash_internal_pointer_reset_ex(EG(function_table), &pos);
	while(zend_hash_get_current_data_ex(EG(function_table), (void**)&zf, &pos) == SUCCESS) {
		if (zf->type == ZEND_USER_FUNCTION && zf->op_array.static_variables) {
            strcmp(zf->common.function_name, "{closure}") != 0
                ? add_assoc_zval(parentarray, zf->common.function_name, array_memory_use_profile(zf->op_array.static_variables, rzl))
                : add_next_index_zval(parentarray, array_memory_use_profile(zf->op_array.static_variables, rzl));
		}
		zend_hash_move_forward_ex(EG(function_table), &pos);
	}
	add_assoc_zval(return_value, "function.static", parentarray);

	/* profile class static variables */
	MAKE_STD_ZVAL(parentarray);
	array_init(parentarray);
	zend_class_entry **ce;
	zend_property_info *prop_info;
	zval key, *subarray, *prop;
    HashPosition static_var_pos;
    short should_new_subarray = 1;
	zend_hash_internal_pointer_reset_ex(EG(class_table), &pos);
	while(zend_hash_get_current_data_ex(EG(class_table), (void**)&ce, &pos) == SUCCESS) {
		if (should_new_subarray) {
			MAKE_STD_ZVAL(subarray);
			array_init(subarray);
		}
		should_new_subarray = 0;
		hashtable = &((*ce)->properties_info);
		zend_hash_internal_pointer_reset_ex(hashtable, &static_var_pos);
		while(zend_hash_get_current_data_ex(hashtable, (void **)&prop_info, &static_var_pos) == SUCCESS) {
			if (prop_info->offset >= 0 && (prop_info->flags & ZEND_ACC_STATIC) != 0) {
				prop = (*ce)->default_static_members_table[prop_info->offset];
				if (Z_REFCOUNT_P(prop) > 1) {
					rz = rzl->head;
					if (rzl->head == NULL) {
						rzl->head = emalloc(sizeof(ref_zval));
						rzl->head->next = NULL;
						rzl->head->z = prop;
					} else if (ref_zval_list_find(&rz, prop)) {
						zend_hash_move_forward_ex(hashtable, &static_var_pos);
						continue;
					} else {
						rz->next = emalloc(sizeof(ref_zval));
						rz->next->z = prop;
						rz->next->next = NULL;
					}
				}
				zend_hash_get_current_key_zval_ex(hashtable, &key, &static_var_pos);
                if (Z_TYPE(key) != IS_STRING) {
                    zend_hash_move_forward_ex(hashtable, &static_var_pos);
                    continue;
                }
				switch (Z_TYPE_P(prop)) {
					case IS_LONG:
					case IS_BOOL:
						add_assoc_long(subarray, Z_STRVAL(key), sizeof(long));
						should_new_subarray = 1;
						break;
					case IS_DOUBLE:
						add_assoc_long(subarray, Z_STRVAL(key), sizeof(double));
						should_new_subarray = 1;
						break;
					case IS_STRING:
						add_assoc_long(subarray, Z_STRVAL(key), Z_STRLEN_P(prop));
						should_new_subarray = 1;
						break;
					case IS_ARRAY:
						add_assoc_zval(subarray, Z_STRVAL(key), array_memory_use_profile(Z_ARRVAL_P(prop), rzl));
						should_new_subarray = 1;
						break;
					case IS_OBJECT:
						add_assoc_zval(subarray, Z_STRVAL(key), class_memory_use_profile(Z_OBJPROP_P(prop), rzl));
						should_new_subarray = 1;
						break;
				}
				str_efree(Z_STRVAL(key));
			}
			zend_hash_move_forward_ex(hashtable, &static_var_pos);
		}
		if (should_new_subarray) {
			add_assoc_zval(parentarray, (*ce)->name, subarray);
		}
		zend_hash_move_forward_ex(EG(class_table), &pos);
	}
	add_assoc_zval(return_value, "class.static", parentarray);
	if (!should_new_subarray) {
		FREE_HASHTABLE(Z_ARRVAL_P(subarray));
		FREE_ZVAL(subarray);
	}

	rz = rzl->head;
	ref_zval *tmp;
	while(rz != NULL) {
		tmp = rz;
		rz = rz->next;
		efree(tmp);
	}
	efree(rzl);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_varprofile_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_varprofile_init_globals(zend_varprofile_globals *varprofile_globals)
{
	varprofile_globals->global_value = 0;
	varprofile_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(varprofile)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(varprofile)
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
PHP_RINIT_FUNCTION(varprofile)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(varprofile)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(varprofile)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "varprofile support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ varprofile_functions[]
 *
 * Every user visible function must have an entry in varprofile_functions[].
 */
const zend_function_entry varprofile_functions[] = {
	PHP_FE(varprofile,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in varprofile_functions[] */
};
/* }}} */

/* {{{ varprofile_module_entry
 */
zend_module_entry varprofile_module_entry = {
	STANDARD_MODULE_HEADER,
	"varprofile",
	varprofile_functions,
	PHP_MINIT(varprofile),
	PHP_MSHUTDOWN(varprofile),
	PHP_RINIT(varprofile),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(varprofile),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(varprofile),
	PHP_VARPROFILE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_VARPROFILE
ZEND_GET_MODULE(varprofile)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
