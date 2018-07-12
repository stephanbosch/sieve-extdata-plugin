AC_INIT([Pigeonhole Sieve Extdata plugin], [0.2.devel], [dovecot@dovecot.org], [pigeonhole-0.6-sieve-extdata])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([no-define foreign])

# Autoheader is not needed and does more harm than good for this package.
# However, it is tightly integrated in autoconf/automake and therefore it is
# difficult not to use it. As a workaround we give autoheader a dummy config
# header to chew on and we handle the real config header ourselves.
AC_CONFIG_HEADERS([dummy-config.h sieve-extdata-config.h])

AC_DEFINE_UNQUOTED(SIEVE_EXTDATA_NAME, "$PACKAGE_NAME",
    [Define to the full name of this plugin implementation.])
AC_DEFINE_UNQUOTED(SIEVE_EXTDATA_VERSION, "$PACKAGE_VERSION",
    [Define to the version of this plugin implementation.])

AM_MAINTAINER_MODE

AC_PROG_LIBTOOL

DC_DOVECOT
DC_DOVECOT_MODULEDIR

LIBDOVECOT_INCLUDE="$LIBDOVECOT_INCLUDE $LIBDOVECOT_STORAGE_INCLUDE"
CFLAGS="$DOVECOT_CFLAGS"
LIBS="$DOVECOT_LIBS"
AC_SUBST(LIBDOVECOT_INCLUDE)

DC_PIGEONHOLE

AC_ARG_ENABLE(valgrind,
[AC_HELP_STRING([--enable-valgrind], [Enable Valgrind memory leak checks in testsuite [default=no]])],
    if test x$enableval = xno || test x$enableval = xauto; then
        want_valgrind=$enableval
    else
        want_valgrind=yes
    fi,
    want_valgrind=no)
AM_CONDITIONAL(TESTSUITE_VALGRIND, test "$want_valgrind" = "yes")

AC_ARG_WITH(docs,
[  --with-docs             Install documentation (default)],
    if test x$withval = xno; then
        want_docs=no
    else
        want_docs=yes
    fi,
    want_docs=yes)
AM_CONDITIONAL(BUILD_DOCS, test "$want_docs" = "yes")

AC_CONFIG_FILES([
Makefile
src/Makefile
doc/Makefile
stamp.h])

AC_OUTPUT
