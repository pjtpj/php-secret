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

#ifndef PHP_SECRET_H
#define PHP_SECRET_H

extern zend_module_entry secret_module_entry;
#define phpext_secret_ptr &secret_module_entry

#ifdef PHP_WIN32
#define PHP_SECRET_API __declspec(dllexport)
#else
#define PHP_SECRET_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(secret);
PHP_MSHUTDOWN_FUNCTION(secret);
PHP_RINIT_FUNCTION(secret);
PHP_RSHUTDOWN_FUNCTION(secret);
PHP_MINFO_FUNCTION(secret);

PHP_FUNCTION(confirm_secret_compiled);	/* For testing, remove later. */
PHP_FUNCTION(exs_get_secret);
PHP_FUNCTION(exs_put_secret);


/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(secret)
	int   global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(secret)
*/

/* In every utility function you add that needs to use variables 
   in php_secret_globals, call TSRM_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as SECRET_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define SECRET_G(v) TSRMG(secret_globals_id, zend_secret_globals *, v)
#else
#define SECRET_G(v) (secret_globals.v)
#endif

#endif	/* PHP_SECRET_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
