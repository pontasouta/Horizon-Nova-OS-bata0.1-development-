$(common-objpfx)nptl/pthread_kill_other_threads.os: \
 pthread_kill_other_threads.c ../include/stdc-predef.h \
 $(common-objpfx)libc-modules.h \
 ../include/libc-symbols.h ../include/libc-misc.h \
 $(common-objpfx)config.h \
 ../sysdeps/generic/libc-symver.h ../include/libc-diag.h \
 ../sysdeps/generic/symbol-hacks.h ../include/shlib-compat.h \
 $(common-objpfx)abi-versions.h
../include/stdc-predef.h:
$(common-objpfx)libc-modules.h:
../include/libc-symbols.h:
../include/libc-misc.h:
$(common-objpfx)config.h:
../sysdeps/generic/libc-symver.h:
../include/libc-diag.h:
../sysdeps/generic/symbol-hacks.h:
../include/shlib-compat.h:
$(common-objpfx)abi-versions.h:
