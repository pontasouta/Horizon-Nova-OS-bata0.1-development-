srcdir = ../lib/glibc

TEST_CC = gcc
TEST_CXX = g++ -std=gnu++11
test-cc-option-wimplicit-fallthrough = -Wimplicit-fallthrough
test-config-cflags-mprefer-vector-width = yes
test-config-cflags-signaling-nans = -fsignaling-nans
test-cc-option-wfree-labels = -Wfree-labels
test-cc-option-wmissing-parameter-name = -Wmissing-parameter-name
test-enable-cet = no
test-have-static-pie = yes
test-supported-fortify = 3

# Uncomment the line below if you want to do parallel build.
# PARALLELMFLAGS = -j 4

.PHONY: all install bench

all .DEFAULT:
	$(MAKE) -r PARALLELMFLAGS="$(PARALLELMFLAGS)" -C $(srcdir) objdir=`pwd` $@

check xcheck test:
	$(MAKE) -r PARALLELMFLAGS="$(PARALLELMFLAGS)" -C $(srcdir) \
		CC="$(TEST_CC)" CXX="$(TEST_CXX)" \
		cc-option-wimplicit-fallthrough="$(test-cc-option-wimplicit-fallthrough)" \
		config-cflags-mprefer-vector-width="$(test-config-cflags-mprefer-vector-width)" \
		config-cflags-signaling-nans="$(test-config-cflags-signaling-nans)" \
		cc-option-wfree-labels="$(test-cc-option-wfree-labels)" \
		cc-option-wmissing-parameter-name="$(test-cc-option-wmissing-parameter-name)" \
		enable-cet="$(test-enable-cet)" \
		have-static-pie="$(test-have-static-pie)" \
		supported-fortify="$(test-supported-fortify)" \
		objdir=`pwd` $@

install:
	LC_ALL=C; export LC_ALL; \
	$(MAKE) -r PARALLELMFLAGS="$(PARALLELMFLAGS)" -C $(srcdir) objdir=`pwd` $@

bench bench-clean bench-build:
	$(MAKE) -C $(srcdir)/benchtests $(PARALLELMFLAGS) objdir=`pwd` $@
