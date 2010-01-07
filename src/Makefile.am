sieve_plugindir = $(moduledir)/sieve

sieve_plugin_LTLIBRARIES = lib90_sieve_extdata_plugin.la

lib90_sieve_extdata_plugin_la_LDFLAGS = -module -avoid-version

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(dovecot_incdir) \
	-I$(dovecot_incdir)/src/lib \
	-I$(dovecot_incdir)/src/lib-mail \
	-I$(dovecot_incdir)/src/lib-dict \
	-I$(dovecot_incdir)/src/lib-storage \
	-I$(pigeonhole_incdir) \
	-I$(sieve_incdir) \
	-I$(sieve_variables_incdir) \
	-DPKG_RUNDIR=\""$(rundir)"\"

tests = \
	tst-extdata.c

extensions = \
	ext-extdata.c

lib90_sieve_extdata_plugin_la_SOURCES = \
	$(tests) \
	$(extensions) \
	ext-extdata-common.c \
	ext-extdata-variables.c \
	sieve-extdata-plugin.c

noinst_HEADERS = \
	ext-extdata-common.h \
	sieve-extdata-plugin.h

