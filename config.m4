dnl $Id$
dnl config.m4 for extension secret

PHP_ARG_ENABLE(secret, whether to enable secret support,
[  --enable-secret           Enable secret support])

if test "$PHP_SECRET" != "no"; then
  PHP_ADD_LIBRARY(mysqlclient, 1, SECRET_SHARED_LIBADD)
  PHP_SUBST(SECRET_SHARED_LIBADD)
  PHP_NEW_EXTENSION(secret, secret.c, $ext_shared)
fi
