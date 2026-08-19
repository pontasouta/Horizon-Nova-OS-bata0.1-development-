$(common-objpfx)string/rtld-memmove-avx-unaligned-erms-rtm.os: \
 ../sysdeps/x86_64/multiarch/memmove-avx-unaligned-erms-rtm.S \
 ../include/stdc-predef.h $(common-objpfx)libc-modules.h \
 ../include/libc-symbols.h ../include/libc-misc.h \
 $(common-objpfx)config.h \
 ../sysdeps/generic/libc-symver.h ../include/libc-diag.h \
 ../sysdeps/generic/symbol-hacks.h
../include/stdc-predef.h:
$(common-objpfx)libc-modules.h:
../include/libc-symbols.h:
../include/libc-misc.h:
$(common-objpfx)config.h:
../sysdeps/generic/libc-symver.h:
../include/libc-diag.h:
../sysdeps/generic/symbol-hacks.h:
