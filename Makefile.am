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

ChangeLog:
	git log --name-status \
		--pretty="format:%ai %aN <%aE> (%h)%n%n%w(80,4,4)%s%n%n%b" > ChangeLog \
			|| rm -f ChangeLog

if PIGEONHOLE_TESTSUITE_AVAILABLE

TESTSUITE_PLUGIN = -P src/.libs/sieve_extdata

TESTSUITE_BIN = $(PIGEONHOLE_TESTSUITE) \
	$(TESTSUITE_PLUGIN) \
	$(TESTSUITE_OPTIONS)

TEST_BIN = $(RUN_TEST) $(TESTSUITE_BIN)

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
