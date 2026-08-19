$(common-objpfx)string/rtld-memcmp-sse2.os: \
 ../sysdeps/x86_64/multiarch/memcmp-sse2.S ../include/stdc-predef.h \
 $(common-objpfx)libc-modules.h \
 ../include/libc-symbols.h ../include/libc-misc.h \
 $(common-objpfx)config.h \
 ../sysdeps/generic/libc-symver.h ../include/libc-diag.h \
 ../sysdeps/generic/symbol-hacks.h ../sysdeps/x86/isa-level.h
../include/stdc-predef.h:
$(common-objpfx)libc-modules.h:
../include/libc-symbols.h:
../include/libc-misc.h:
$(common-objpfx)config.h:
../sysdeps/generic/libc-symver.h:
../include/libc-diag.h:
../sysdeps/generic/symbol-hacks.h:
../sysdeps/x86/isa-level.h:
