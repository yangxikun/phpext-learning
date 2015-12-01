dnl $Id$
dnl config.m4 for extension varprofile

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(varprofile, for varprofile support,
dnl Make sure that the comment is aligned:
dnl [  --with-varprofile             Include varprofile support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(varprofile, whether to enable varprofile support,
[  --enable-varprofile           Enable varprofile support])

if test "$PHP_VARPROFILE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-varprofile -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/varprofile.h"  # you most likely want to change this
  dnl if test -r $PHP_VARPROFILE/$SEARCH_FOR; then # path given as parameter
  dnl   VARPROFILE_DIR=$PHP_VARPROFILE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for varprofile files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       VARPROFILE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$VARPROFILE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the varprofile distribution])
  dnl fi

  dnl # --with-varprofile -> add include path
  dnl PHP_ADD_INCLUDE($VARPROFILE_DIR/include)

  dnl # --with-varprofile -> check for lib and symbol presence
  dnl LIBNAME=varprofile # you may want to change this
  dnl LIBSYMBOL=varprofile # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $VARPROFILE_DIR/$PHP_LIBDIR, VARPROFILE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_VARPROFILELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong varprofile lib version or lib not found])
  dnl ],[
  dnl   -L$VARPROFILE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(VARPROFILE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(varprofile, varprofile.c, $ext_shared)
fi
