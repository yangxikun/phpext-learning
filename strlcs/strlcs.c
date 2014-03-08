/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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
#include "php_strlcs.h"

/* If you declare any globals in php_strlcs.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(strlcs)
*/

/* True global resources - no need for thread safety here */
static int le_strlcs;

/* {{{ strlcs_functions[]
 *
 * Every user visible function must have an entry in strlcs_functions[].
 */
const zend_function_entry strlcs_functions[] = {
	PHP_FE(strlcs,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in strlcs_functions[] */
};
/* }}} */

/* {{{ strlcs_module_entry
 */
zend_module_entry strlcs_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"strlcs",
	strlcs_functions,
	PHP_MINIT(strlcs),
	PHP_MSHUTDOWN(strlcs),
	PHP_RINIT(strlcs),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(strlcs),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(strlcs),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_STRLCS_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_STRLCS
ZEND_GET_MODULE(strlcs)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("strlcs.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_strlcs_globals, strlcs_globals)
    STD_PHP_INI_ENTRY("strlcs.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_strlcs_globals, strlcs_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_strlcs_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_strlcs_init_globals(zend_strlcs_globals *strlcs_globals)
{
	strlcs_globals->global_value = 0;
	strlcs_globals->global_string = NULL;
}
*/
/* }}} */
/* {{{ str_lcs 最长公共子序列
 */
static char *str_lcs(const char *str1, int str1_len ,const char *str2, int str2_len)
{
	/*初始化*/
	int i, j;
	int **c = (int **)emalloc((str1_len+1)*sizeof(int *));
	for (i = 0; i <= str1_len; ++i) {
		c[i] = (int *)ecalloc(str2_len+1, sizeof(int));
	}

	for (i = 1; i <= str1_len; ++i) {
		for (j = 1; j <= str2_len; ++j) {
			if (str1[i-1] == str2[j-1]) {
				c[i][j] = c[i-1][j-1] + 1;
			} else if (c[i-1][j] > c[i][j-1]) {
				c[i][j] = c[i-1][j];
			} else {
				c[i][j] = c[i][j-1];
			}
		}
	}

	/*先计算长度*/
	int common_len = 0;
	i = str1_len;
	j = str2_len;
	while (1) {
		if (i == 0 || j == 0) {
			break;
		}
		if (c[i][j] == c[i-1][j-1]+1 && str1[i-1] == str2[j-1]) {
			i--;j--;
			common_len++;
		} else if (c[i][j] == c[i-1][j]) {
			i--;
		} else {
			j--;
		}
	}

	/*再求最长公共子串*/
	char *lcs = (char *)0;
	if (common_len > 0) {
		lcs = (char *)emalloc((common_len+1)*sizeof(char));
		lcs[common_len--] = '\0';
		i = str1_len;
		j = str2_len;
		while (i != 0 && j != 0) {
			if (c[i][j] == c[i-1][j-1]+1 && str1[i-1] == str2[j-1]) {
				i--;j--;
				lcs[common_len--] = str1[i];
			} else if (c[i][j] == c[i-1][j]) {
				i--;
			} else {
				j--;
			}
		}
	}

	for (i = 0; i <= str1_len; ++i) {
		efree(c[i]);
	}
	efree(c);

	return lcs;
}

/*{{{ str_lccs 最长连续公共子序列
 */
static char *str_lccs(const char *str1, int str1_len ,const char *str2, int str2_len)
{
	const char *strA = str1_len > str2_len ? str2 : str1;
	const char *strB = str1_len > str2_len ? str1 : str2;
	int min_len = str1_len > str2_len ? str2_len : str1_len;

	int i = 0, step = min_len;
	char *s = (char *)emalloc((min_len+1)*sizeof(char));
	/*选择strA中的连续子序列，并在strB中查找是否存在*/
	while (step != 0) {
		for (i = 0; i+step <= min_len; ++i) {
			strncpy(s, strA+i, step);
			s[step] = '\0';
			if (strstr(strB, s) != (char *)0) {
				return s;
			}
		}
		step--;
	}
	efree(s);

	return (char *)0;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(strlcs)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(strlcs)
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
PHP_RINIT_FUNCTION(strlcs)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(strlcs)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(strlcs)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "strlcs support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_strlcs_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(strlcs)
{
	char *str1 = NULL, *str2 = NULL, *lcs = NULL;
	int str1_len, str2_len;
	zend_bool continues = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &str1, &str1_len, &str2, &str2_len, &continues) == FAILURE) {
		return;
	}

	if (continues) {
		lcs = str_lccs(str1, str1_len, str2, str2_len);
	} else {
		lcs = str_lcs(str1, str1_len, str2, str2_len);		
	}

	if (lcs == (char *)0) {
		RETURN_EMPTY_STRING();
	} else {
		RETURN_STRING(lcs, 0);
	}
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
