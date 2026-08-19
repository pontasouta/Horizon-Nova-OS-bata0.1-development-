# config.make.  Generated from config.make.in by configure.
# Don't edit this file.  Put configuration parameters in configparms instead.

version = 2.43.9000
release = development

# Installation prefixes.
install_root = $(DESTDIR)
prefix = /home/sota/myos/boot/glibc-install
exec_prefix = ${prefix}
datadir = ${datarootdir}
libdir = ${exec_prefix}/lib
bindir = ${exec_prefix}/bin
slibdir = 
rtlddir = 
complocaledir = 
sysconfdir = ${prefix}/etc
libexecdir = ${exec_prefix}/libexec
rootsbindir = 
infodir = ${datarootdir}/info
includedir = ${prefix}/include
datarootdir = ${prefix}/share
localstatedir = ${prefix}/var
localedir = ${datarootdir}/locale
multidir= .

# Should we use and build ldconfig?
use-ldconfig = yes

# Maybe the `ldd' script must be rewritten.
ldd-rewrite-script = sysdeps/unix/sysv/linux/x86_64/ldd-rewrite.sed

# System configuration.
config-machine = x86_64
base-machine = x86_64
config-vendor = pc
config-os = linux-gnu
config-sysdirs =  sysdeps/unix/sysv/linux/x86_64/64 sysdeps/x86_64/64/nptl sysdeps/unix/sysv/linux/x86_64 sysdeps/unix/sysv/linux/x86 sysdeps/x86/nptl sysdeps/unix/sysv/linux/wordsize-64 sysdeps/x86_64/nptl sysdeps/unix/sysv/linux sysdeps/nptl sysdeps/pthread sysdeps/gnu sysdeps/unix/inet sysdeps/unix/sysv sysdeps/unix/x86_64 sysdeps/unix sysdeps/posix sysdeps/x86_64/64 sysdeps/x86_64/fpu/multiarch sysdeps/x86_64/fpu sysdeps/x86/fpu sysdeps/x86_64/multiarch sysdeps/x86_64 sysdeps/x86 sysdeps/ieee754/float128 sysdeps/ieee754/ldbl-96 sysdeps/ieee754/dbl-64 sysdeps/ieee754/flt-32 sysdeps/wordsize-64 sysdeps/ieee754 sysdeps/generic
cflags-cpu = 
asflags-cpu = 

config-extra-cflags = 
config-extra-cppflags = 
config-cflags-nofma = -ffp-contract=off

defines = 
sysheaders = 
sysincludes = 
c++-sysincludes = 
c++-cstdlib-header = /usr/include/c++/16.1.1/cstdlib
c++-cmath-header = /usr/include/c++/16.1.1/cmath
c++-bits-std_abs-h = /usr/include/c++/16.1.1/bits/std_abs.h
enable-werror = no

have-z-execstack = yes
have-no-error-execstack = @libc_cv_no_error_execstack@
have-protected-data = no
have-insert = yes
have-glob-dat-reloc = yes
with-fp = yes
enable-timezone-tools = yes
unwind-find-fde = no
have-fpie = yes
have-ssp = yes
stack-protector = -fno-stack-protector
no-stack-protector = -fno-stack-protector -DSTACK_PROTECTOR_LEVEL=0
fortify-source = -U_FORTIFY_SOURCE
no-fortify-source = -U_FORTIFY_SOURCE
have-selinux = no
have-libaudit = 
have-libcap = 
bind-now = no
have-cxx-thread_local = yes
have-loop-to-function = yes
have-textrel_ifunc = yes

multi-arch = default

mach-interface-list = 

memory-tagging = no

# Configuration options.
build-shared = yes
build-profile = no
build-static-nss = no
cross-compiling = no
force-install = yes
build-nscd = yes
use-nscd = yes
build-hardcoded-path-in-tests= no
build-pt-chown = no
man-pages-version = 6.9.1

# Build tools.
CC = gcc
CXX = g++ -std=gnu++11
BUILD_CC = 
CFLAGS = -g -O2
CPPFLAGS-config = 
extra-nonshared-cflags = 
rtld-early-cflags = 
ASFLAGS-config =  -Wa,--noexecstack
AR = ar
NM = nm
MAKEINFO = makeinfo
AS = $(CC) -c
BISON = bison
AUTOCONF = no
OBJDUMP = objdump
OBJCOPY = objcopy
GPROF = gprof
READELF = readelf
STRIP = strip

# Installation tools.
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_INFO = /usr/sbin/install-info
LN_S = ln -s
MSGFMT = msgfmt

# Script execution tools.
BASH = /usr/sbin/bash
AWK = gawk
PERL = /usr/sbin/perl
PYTHON = python3 -B

# Additional libraries.
LIBGD = yes

# Package versions and bug reporting configuration.
PKGVERSION = (GNU libc) 
PKGVERSION_TEXI = (GNU libc) 
REPORT_BUGS_TO = <https://www.gnu.org/software/libc/bugs.html>
REPORT_BUGS_TEXI = @uref{https://www.gnu.org/software/libc/bugs.html}

# More variables may be inserted below by configure.

static-cxx-tests = yes
with-lld = no
have-test-clang = no
have-test-clangxx = no
have-test-clang-19-or-less = 
have-gnu-retain = yes
have-cc-with-libunwind = no
have-z-start-stop-gc = yes
have-depaudit = yes
have-dt-relr = yes
have-no-dynamic-linker = yes
have-static-pie = yes
config-cflags-wno-discarded-qualifiers = -Wno-discarded-qualifiers
have-test-mtls-descriptor = gnu2
have-test-mtls-traditional = gnu
config-cflags-wno-maybe-uninitialized = -Wno-maybe-uninitialized
config-cflags-signaling-nans = -fsignaling-nans
have-test-cc-cflags-fsemantic-interposition = yes
test-config-cflags-float-store = -ffloat-store
config-cflags-cc-loop-to-function = yes
cc-option-wimplicit-fallthrough = -Wimplicit-fallthrough
cc-option-wfree-labels = -Wfree-labels
cc-option-wmissing-parameter-name = -Wmissing-parameter-name
have-test-cc-trampoline = yes
test-config-cflags-wno-restrict = -Wno-restrict
test-config-cflags-wno-fortify-source = -Wno-fortify-source
test-config-cflags-finput-charset-ascii = -finput-charset=ascii
test-config-cxxflags-finput-charset-ascii = -finput-charset=ascii
test-config-cxxflags-stdcxx26 = -std=c++26
have-test-stb-gnu-unique = yes
supported-fortify = 3
no-file-offset-bits-source = 
no-time-bits-source = 
aligned-65536 = yes
config-cflags-mprefer-vector-width = yes
have-z-mark-plt = yes
enable-cet = no
have-mamx-tile = yes
have-x86-apx = no
have-x86-isa-level = baseline
x86-isa-level-2-or-above = 2 3 4
x86-isa-level-3-or-above = 3 4
x86-isa-level-4-or-above = 4
enable-x86-isa-level = yes
have-test-cc-cflags-mfpmath-387 = yes
test-cc-cflags-no-direct-extern-access = -mno-direct-extern-access
build-mathvec = yes
have-ifunc = yes
have-gcc-ifunc = yes
build-pic-default = yes
cc-pie-default = yes
pde-load-address = 0x0000000000400000
load-address-ldflag = -Wl,-Ttext-segment
config-cflags-no-fp-builtin-inexact = -fno-fp-int-builtin-inexact
have-undefined-version = -Wl,--undefined-version
build-pie-default = yes
enable-static-pie = yes
have-libgcc_s = yes
libgcc-name = -lgcc
libgcc_eh-name = -lgcc_eh
enable-gsframe = no
abort-in-ld = no
