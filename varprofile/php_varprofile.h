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

#ifndef PHP_VARPROFILE_H
#define PHP_VARPROFILE_H

extern zend_module_entry varprofile_module_entry;
#define phpext_varprofile_ptr &varprofile_module_entry

#define PHP_VARPROFILE_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_VARPROFILE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_VARPROFILE_API __attribute__ ((visibility("default")))
#else
#	define PHP_VARPROFILE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(varprofile)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(varprofile)
*/

/* In every utility function you add that needs to use variables 
   in php_varprofile_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as VARPROFILE_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define VARPROFILE_G(v) TSRMG(varprofile_globals_id, zend_varprofile_globals *, v)
#else
#define VARPROFILE_G(v) (varprofile_globals.v)
#endif

#endif	/* PHP_VARPROFILE_H */

/* struct for remember the variables that its refcount__gc > 1 */
struct _ref_zval {
	zval *z;
    struct _ref_zval *next;
};
struct _ref_zval_list {
	struct _ref_zval *head;
};
typedef struct _ref_zval ref_zval;
typedef struct _ref_zval_list ref_zval_list;

/* utility function */
static int ref_zval_list_find(ref_zval **rz, zval *entry);
static zval* array_memory_use_profile(HashTable *ht, ref_zval_list *rzl);
static zval* class_memory_use_profile(HashTable *ht, ref_zval_list *rzl);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
