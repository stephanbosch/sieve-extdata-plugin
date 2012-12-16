# pigeonhole.m4 - Check presence of pigeonhole -*-Autoconf-*-
#
# Current implementation is ugly and needs improvement

AC_DEFUN([DC_PIGEONHOLE],[
	DC_DOVECOT
	DC_DOVECOT_MODULEDIR

	LIBDOVECOT_INCLUDE="$LIBDOVECOT_INCLUDE $LIBDOVECOT_STORAGE_INCLUDE"
	CFLAGS="$DOVECOT_CFLAGS"
	LIBS="$DOVECOT_LIBS"
	AC_SUBST(LIBDOVECOT_INCLUDE)

	AC_ARG_WITH(pigeonhole,
	[  --with-pigeonhole=DIR   Pigeonhole base directory],
	pigeonholedir="$withval",
	pigeonholedir="$dovecot_pkgincludedir/sieve"
	)

	AC_MSG_CHECKING([for pigeonhole in "$pigeonholedir"])

	top=`pwd`
	cd $pigeonholedir
	pigeonholedir=`pwd`
	cd $top
	AC_SUBST(pigeonholedir)

	PIGEONHOLE_TESTSUITE=
	if test -f "$pigeonholedir/src/lib-sieve/sieve.h"; then
		AC_MSG_RESULT([found])
		pigeonhole_incdir="$pigeonholedir"
		LIBSIEVE_INCLUDE='\
			-I$(pigeonhole_incdir) \
			-I$(pigeonhole_incdir)/src/lib-sieve \
			-I$(pigeonholedir)/src/lib-sieve/plugins/copy \
			-I$(pigeonholedir)/src/lib-sieve/plugins/enotify \
			-I$(pigeonholedir)/src/lib-sieve/plugins/variables'
		if test -f "$pigeonholedir/src/testsuite/testsuite"; then
			PIGEONHOLE_TESTSUITE="${pigeonholedir}/src/testsuite/testsuite"
  		fi
	elif test -f "$pigeonholedir/sieve.h"; then
		AC_MSG_RESULT([found])
		pigeonhole_incdir="$pigeonholedir"
		LIBSIEVE_INCLUDE='-I$(pigeonhole_incdir)'
	else
		AC_MSG_RESULT([not found])
		AC_MSG_NOTICE([
			Pigeonhole Sieve headers not found from $pigeonholedir and they
			are not installed in the Dovecot include path, use --with-pigeonhole=PATH
 			to give path to Pigeonhole sources or installed headers.])
		AC_MSG_ERROR([pigeonhole not found])
	fi
	
	AM_CONDITIONAL(PIGEONHOLE_TESTSUITE_AVAILABLE, ! test -z "$PIGEONHOLE_TESTSUITE")

	pigeonhole_incdir="$pigeonholedir"

	AC_ARG_ENABLE(valgrind,
	[AC_HELP_STRING([--enable-valgrind], [Enable Valgrind memory leak checks in testsuite [default=no]])],
	    if test x$enableval = xno || test x$enableval = xauto; then
    	    want_valgrind=$enableval
	    else
    	    want_valgrind=yes
	    fi,
	want_valgrind=no)
	AM_CONDITIONAL(PIGEONHOLE_TESTSUITE_VALGRIND, test "$want_valgrind" = "yes")

	AC_SUBST(pigeonhole_incdir)

	AC_SUBST(LIBSIEVE_INCLUDE)
	AC_SUBST(PIGEONHOLE_TESTSUITE)
])
