dnl $Id$
dnl config.m4 for extension cheetah

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(cheetah, for cheetah support,
dnl Make sure that the comment is aligned:
dnl [  --with-cheetah             Include cheetah support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(cheetah, whether to enable cheetah support,
dnl Make sure that the comment is aligned:
dnl [  --enable-cheetah           Enable cheetah support])

if test "$PHP_CHEETAH" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-cheetah -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/cheetah.h"  # you most likely want to change this
  dnl if test -r $PHP_CHEETAH/$SEARCH_FOR; then # path given as parameter
  dnl   CHEETAH_DIR=$PHP_CHEETAH
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for cheetah files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CHEETAH_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CHEETAH_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the cheetah distribution])
  dnl fi

  dnl # --with-cheetah -> add include path
  dnl PHP_ADD_INCLUDE($CHEETAH_DIR/include)

  dnl # --with-cheetah -> check for lib and symbol presence
  dnl LIBNAME=cheetah # you may want to change this
  dnl LIBSYMBOL=cheetah # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CHEETAH_DIR/lib, CHEETAH_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CHEETAHLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong cheetah lib version or lib not found])
  dnl ],[
  dnl   -L$CHEETAH_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl PHP_SUBST(CHEETAH_SHARED_LIBADD)

  PHP_NEW_EXTENSION(cheetah, cheetah.c, $ext_shared)
fi
