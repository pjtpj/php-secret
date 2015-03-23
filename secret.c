/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id: header,v 1.10 2002/02/28 08:25:27 sebastian Exp $ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_secret.h"
#include "SAPI.h"
#include <mysql/mysql.h>


/* If you declare any globals in php_secret.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(secret)
*/

/* True global resources - no need for thread safety here */
static int le_secret;
static char* g_szHost;
static char* g_szDatabase;
static char* g_szUsername;
static char* g_szPassword;
static char* g_szHomeRoot;
static int g_fDebug = 0;
static char* g_szLastError;


/* {{{ secret_functions[]
 *
 * Every user visible function must have an entry in secret_functions[].
 */
zend_function_entry secret_functions[] = {
   	PHP_FE(exs_get_secret,	NULL)
	PHP_FE(exs_put_secret,	NULL)
	PHP_FE(confirm_secret_compiled,	NULL)		/* For testing, remove later. */
	{NULL, NULL, NULL}	/* Must be the last line in secret_functions[] */
};
/* }}} */

/* {{{ secret_module_entry
 */
zend_module_entry secret_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"secret",
	secret_functions,
	PHP_MINIT(secret),
	PHP_MSHUTDOWN(secret),
	PHP_RINIT(secret),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(secret),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(secret),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SECRET
ZEND_GET_MODULE(secret)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("secret.global_value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_secret_globals, secret_globals)
    STD_PHP_INI_ENTRY("secret.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_secret_globals, secret_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_secret_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_secret_init_globals(zend_secret_globals *secret_globals)
{
	secret_globals->global_value = 0;
	secret_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(secret)
{
	/* If you have INI entries, uncomment these lines 
	ZEND_INIT_MODULE_GLOBALS(secret, php_secret_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	*/
   
	FILE* pfSecret;
	char szLine[1024];

	pfSecret = fopen("/etc/php_secret.conf", "r");
	if(pfSecret) {
		while(fgets(szLine, sizeof(szLine), pfSecret)) {
			char szArg1[sizeof(szLine)];
			char szArg2[sizeof(szLine)];
			int cArguments;
			
			/* Comments and empty lines */
			if(szLine[0] == 0 || szLine[0] == '\n' || szLine[0] == ';' || szLine[0] == '#')
				continue;

			cArguments = sscanf(szLine, "%s %s", szArg1, szArg2);
			if(cArguments != 2)
				continue;

			if(!strcmp(szArg1, "Host"))
				g_szHost = strdup(szArg2);
			else if(!strcmp(szArg1, "Database"))
				g_szDatabase = strdup(szArg2);
			else if(!strcmp(szArg1, "Username"))
				g_szUsername = strdup(szArg2);
			else if(!strcmp(szArg1, "Password"))
				g_szPassword = strdup(szArg2);
			else if(!strcmp(szArg1, "HomeRoot"))
				g_szHomeRoot = strdup(szArg2);
			else if(!strcmp(szArg1, "Debug") && !strcasecmp(szArg2, "TRUE"))
				g_fDebug = 1;
		}
		fclose(pfSecret);
	}
	else {
		g_szLastError = strerror(errno);
	}
   
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(secret)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
   
	if(g_szHost)
		free(g_szHost);
	if(g_szDatabase)
		free(g_szDatabase);
	if(g_szUsername)
		free(g_szUsername);
	if(g_szPassword)
		free(g_szPassword);
	if(g_szHomeRoot)
		free(g_szHomeRoot);

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(secret)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(secret)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(secret)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Secret Support", "enabled");
	php_info_print_table_header(2, "Last Error", g_szLastError);
	if(g_fDebug) {
		php_info_print_table_header(2, "Host", g_szHost);
		php_info_print_table_header(2, "Database", g_szDatabase);
		php_info_print_table_header(2, "Username", g_szUsername);
		php_info_print_table_header(2, "Password", g_szPassword);	
		php_info_print_table_header(2, "HomeRoot", g_szHomeRoot);
	}
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_secret_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_secret_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char string[256];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = sprintf(string, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "secret", arg);
	RETURN_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

static char* GetScriptPath()
{
	return strdup(SG(request_info).path_translated);
}

static char* GetAccountName()
{
	char* szScriptPath = GetScriptPath();
	char* szAccountName = NULL;
	char* szSearch;
	int cchHomeRoot = g_szHomeRoot ? strlen(g_szHomeRoot) : 0;

	if(szScriptPath) {
		if(strlen(szScriptPath) > cchHomeRoot) {
			szAccountName = strdup(szScriptPath + cchHomeRoot);
			szSearch = strchr(szAccountName, '/');
			if(szSearch) *szSearch = 0;
		}
		else {
			g_szLastError = "ScriptPath is too short";
		}
		free(szScriptPath);
	}
	else {
		g_szLastError = "Cannot get ScriptPath";
	}
		
	return szAccountName;
}

/* {{{ proto string exs_get_secret()
   Returns the secret associated with a hosting account */
PHP_FUNCTION(exs_get_secret)
{
	static char szQueryFmt[] = "SELECT WebServerSecret FROM Accounts WHERE WebServerUser = '%s'";
	static char szErrorFmt[] = "Cannot get row with account name '%s'";

	char* szQuery = NULL;
	char* szAccountName = NULL;
	char* szSecret = NULL;

	MYSQL mysql;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	/* Calculate username from script path */

	szAccountName = GetAccountName();
	if(szAccountName) {

		/* Lookup Secret for Account */

		szQuery = malloc(strlen(szQueryFmt) + strlen(szAccountName) + 1);
		sprintf(szQuery, szQueryFmt, szAccountName);
	
		if(mysql_init(&mysql)) {
			if(mysql_real_connect(&mysql, g_szHost, g_szUsername, 
								  g_szPassword, g_szDatabase, 0, NULL, 0)) {
				if(mysql_real_query(&mysql, szQuery, strlen(szQuery)) == 0) {
					MYSQL_RES* pmyres = mysql_use_result(&mysql);
					if(pmyres) {
						MYSQL_ROW myrow = mysql_fetch_row(pmyres);
						if(myrow){
							szSecret = estrdup(myrow[0]);
							mysql_free_result(pmyres);
						}
						else {
							g_szLastError = malloc(strlen(szErrorFmt) + strlen(szAccountName) + 1);
							sprintf(g_szLastError, szErrorFmt, szAccountName);
						}
					}
					else {
						g_szLastError = strdup(mysql_error(&mysql));
					}
					mysql_close(&mysql);
				}
				else {
					g_szLastError = strdup(mysql_error(&mysql));
				}
				mysql_close(&mysql);
			}
			else {
				g_szLastError = strdup(mysql_error(&mysql));
			}
		}
		else {
			g_szLastError = strdup(mysql_error(&mysql));
		}
		
		free(szQuery);
		free(szAccountName);
	}
	
	if(szSecret) {
		RETURN_STRING(szSecret, 0);
	}
	else {
		RETURN_STRING("", 1);
	}
}
/* }}} */

/* {{{ proto void exs_put_secret(string secret)
   Updates the secret associated with a hosting account */
PHP_FUNCTION(exs_put_secret)
{
	zval **secret;

	static char szQueryFmt[] = "Update WebSiteUsers SET Secret = '%s' WHERE Username = '%s'";

	char* szQuery = NULL;
	char* szAccountName = NULL;

	MYSQL mysql;

	char* szSecret;
	char* szSecretEsc;
	int cchSecretEsc;


	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &secret) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(secret);
	szSecret = Z_STRVAL_PP(secret);

	cchSecretEsc = strlen(szSecret) * 2 + 1;
	szSecretEsc = malloc(cchSecretEsc);
	mysql_escape_string(szSecretEsc, szSecret, strlen(szSecret));

	/* Calculate username from script path */

	szAccountName = GetAccountName();
	if(szAccountName) {

		/* Update Secret for Account */
		
		szQuery = malloc(strlen(szQueryFmt) + strlen(szSecretEsc) + strlen(szAccountName) + 1);
		sprintf(szQuery, szQueryFmt, szSecretEsc, szAccountName);
	
		if(mysql_init(&mysql)) {
			if(mysql_real_connect(&mysql, g_szHost, g_szUsername, 
								  g_szPassword, g_szDatabase, 0, NULL, 0)) {
				mysql_real_query(&mysql, szQuery, strlen(szQuery));
			}
			mysql_close(&mysql);
		}
	}
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
