dnl $Id$
dnl config.m4 for extension strlcs

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(strlcs, for strlcs support,
dnl Make sure that the comment is aligned:
dnl [  --with-strlcs             Include strlcs support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(strlcs, whether to enable strlcs support,
dnl Make sure that the comment is aligned:
[  --enable-strlcs           Enable strlcs support])

if test "$PHP_STRLCS" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-strlcs -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/strlcs.h"  # you most likely want to change this
  dnl if test -r $PHP_STRLCS/$SEARCH_FOR; then # path given as parameter
  dnl   STRLCS_DIR=$PHP_STRLCS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for strlcs files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       STRLCS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$STRLCS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the strlcs distribution])
  dnl fi

  dnl # --with-strlcs -> add include path
  dnl PHP_ADD_INCLUDE($STRLCS_DIR/include)

  dnl # --with-strlcs -> check for lib and symbol presence
  dnl LIBNAME=strlcs # you may want to change this
  dnl LIBSYMBOL=strlcs # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $STRLCS_DIR/lib, STRLCS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_STRLCSLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong strlcs lib version or lib not found])
  dnl ],[
  dnl   -L$STRLCS_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(STRLCS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(strlcs, strlcs.c, $ext_shared)
fi
