if BUILD_DOCS
DOCS = doc
endif

SUBDIRS = src $(DOCS)

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	tests \
	ChangeLog

dist-hook:
	rm -rf `find $(distdir)/tests -type f -name '*.svbin'`

if MAINTAINER_MODE
ChangeLog: .hg/dirstate
	hg log --style=changelog > ChangeLog
endif

if PIGEONHOLE_TESTSUITE_AVAILABLE

TESTSUITE_BIN = $(PIGEONHOLE_TESTSUITE) -P src/.libs/sieve_extdata \
	$(TESTSUITE_OPTIONS)

if PIGEONHOLE_TESTSUITE_VALGRIND
TEST_BIN = libtool --mode=execute valgrind -q --error-exitcode=1 --show-reachable=yes --leak-check=full $(TESTSUITE_BIN)
else
TEST_BIN = $(TESTSUITE_BIN)
endif

test_cases = \
	$(top_srcdir)/tests/extdata-namespace.svtest \
	$(top_srcdir)/tests/extdata-test.svtest

$(test_cases):
	@$(TEST_BIN) $@

.PHONY: $(test_cases)
test: $(test_cases)

check: check-am test all-am

else

test:
	@echo "Pigeonhole test suite not compiled."

endif
