SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	COPYING.LGPL \
	ChangeLog \
	doc

if MAINTAINER_MODE
ChangeLog: .hg/dirstate
	hg log --style=changelog > ChangeLog
endif

if PIGEONHOLE_TESTSUITE_AVAILABLE

TESTSUITE_BIN = $(PIGEONHOLE_TESTSUITE) -P src/.libs/sieve_extdata

if PIGEONHOLE_TESTSUITE_VALGRIND
TEST_BIN = valgrind -q --error-exitcode=1 --show-reachable=yes --leak-check=full $(TESTSUITE_BIN)
else
TEST_BIN = $(TESTSUITE_BIN)
endif

test_cases = \
	tests/extdata-test.svtest \
	tests/extdata-namespace.svtest

$(test_cases):
	@$(TEST_BIN) $@

.PHONY: $(test_cases)
test: $(test_cases)

else

test:
	@echo "Pigeonhole test suite not compiled."

endif
