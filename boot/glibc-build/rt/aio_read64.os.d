$(common-objpfx)rt/aio_read64.os: aio_read64.c \
 ../include/stdc-predef.h $(common-objpfx)libc-modules.h \
 ../include/libc-symbols.h ../include/libc-misc.h \
 $(common-objpfx)config.h \
 ../sysdeps/generic/libc-symver.h ../include/libc-diag.h \
 ../sysdeps/generic/symbol-hacks.h ../sysdeps/x86/bits/wordsize.h
../include/stdc-predef.h:
$(common-objpfx)libc-modules.h:
../include/libc-symbols.h:
../include/libc-misc.h:
$(common-objpfx)config.h:
../sysdeps/generic/libc-symver.h:
../include/libc-diag.h:
../sysdeps/generic/symbol-hacks.h:
../sysdeps/x86/bits/wordsize.h:
