#!/bin/bash
builddir=`dirname "$0"`
GCONV_PATH="${builddir}/iconvdata"

usage () {
cat << EOF
Usage: $0 [OPTIONS] <program> [ARGUMENTS...]

  --tool=TOOL  Run with the specified TOOL. It can be strace, rpctrace,
               valgrind or container. The container will run within
               support/test-container.  For strace and valgrind,
               additional arguments can be passed after the tool name.
EOF

  exit 1
}

toolname=default
while test $# -gt 0 ; do
  case "$1" in
    --tool=*)
      toolname="${1:7}"
      shift
      ;;
    --*)
      usage
      ;;
    *)
      break
      ;;
  esac
done

if test $# -eq 0 ; then
  usage
fi

case "$toolname" in
  default)
    exec   env GCONV_PATH="${builddir}"/iconvdata LOCPATH="${builddir}"/localedata LC_ALL=C  "${builddir}"/elf/ld-linux-x86-64.so.2 --library-path "${builddir}":"${builddir}"/math:"${builddir}"/elf:"${builddir}"/dlfcn:"${builddir}"/nss:"${builddir}"/nis:"${builddir}"/rt:"${builddir}"/resolv:"${builddir}"/mathvec:"${builddir}"/support:"${builddir}"/misc:"${builddir}"/debug:"${builddir}"/nptl:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/ ${1+"$@"}
    ;;
  strace*)
    exec $toolname  -EGCONV_PATH=/home/sota/myos/boot/glibc-build/iconvdata  -ELOCPATH=/home/sota/myos/boot/glibc-build/localedata  -ELC_ALL=C  /home/sota/myos/boot/glibc-build/elf/ld-linux-x86-64.so.2 --library-path /home/sota/myos/boot/glibc-build:/home/sota/myos/boot/glibc-build/math:/home/sota/myos/boot/glibc-build/elf:/home/sota/myos/boot/glibc-build/dlfcn:/home/sota/myos/boot/glibc-build/nss:/home/sota/myos/boot/glibc-build/nis:/home/sota/myos/boot/glibc-build/rt:/home/sota/myos/boot/glibc-build/resolv:/home/sota/myos/boot/glibc-build/mathvec:/home/sota/myos/boot/glibc-build/support:/home/sota/myos/boot/glibc-build/misc:/home/sota/myos/boot/glibc-build/debug:/home/sota/myos/boot/glibc-build/nptl:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/ ${1+"$@"}
    ;;
  rpctrace)
    exec rpctrace  -EGCONV_PATH=/home/sota/myos/boot/glibc-build/iconvdata  -ELOCPATH=/home/sota/myos/boot/glibc-build/localedata  -ELC_ALL=C  /home/sota/myos/boot/glibc-build/elf/ld-linux-x86-64.so.2 --library-path /home/sota/myos/boot/glibc-build:/home/sota/myos/boot/glibc-build/math:/home/sota/myos/boot/glibc-build/elf:/home/sota/myos/boot/glibc-build/dlfcn:/home/sota/myos/boot/glibc-build/nss:/home/sota/myos/boot/glibc-build/nis:/home/sota/myos/boot/glibc-build/rt:/home/sota/myos/boot/glibc-build/resolv:/home/sota/myos/boot/glibc-build/mathvec:/home/sota/myos/boot/glibc-build/support:/home/sota/myos/boot/glibc-build/misc:/home/sota/myos/boot/glibc-build/debug:/home/sota/myos/boot/glibc-build/nptl:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/ ${1+"$@"}
    ;;
  valgrind*)
    exec env GCONV_PATH=/home/sota/myos/boot/glibc-build/iconvdata LOCPATH=/home/sota/myos/boot/glibc-build/localedata LC_ALL=C $toolname  /home/sota/myos/boot/glibc-build/elf/ld-linux-x86-64.so.2 --library-path /home/sota/myos/boot/glibc-build:/home/sota/myos/boot/glibc-build/math:/home/sota/myos/boot/glibc-build/elf:/home/sota/myos/boot/glibc-build/dlfcn:/home/sota/myos/boot/glibc-build/nss:/home/sota/myos/boot/glibc-build/nis:/home/sota/myos/boot/glibc-build/rt:/home/sota/myos/boot/glibc-build/resolv:/home/sota/myos/boot/glibc-build/mathvec:/home/sota/myos/boot/glibc-build/support:/home/sota/myos/boot/glibc-build/misc:/home/sota/myos/boot/glibc-build/debug:/home/sota/myos/boot/glibc-build/nptl:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/ ${1+"$@"}
    ;;
  container)
    exec env GCONV_PATH=/home/sota/myos/boot/glibc-build/iconvdata LOCPATH=/home/sota/myos/boot/glibc-build/localedata LC_ALL=C  /home/sota/myos/boot/glibc-build/elf/ld-linux-x86-64.so.2 --library-path /home/sota/myos/boot/glibc-build:/home/sota/myos/boot/glibc-build/math:/home/sota/myos/boot/glibc-build/elf:/home/sota/myos/boot/glibc-build/dlfcn:/home/sota/myos/boot/glibc-build/nss:/home/sota/myos/boot/glibc-build/nis:/home/sota/myos/boot/glibc-build/rt:/home/sota/myos/boot/glibc-build/resolv:/home/sota/myos/boot/glibc-build/mathvec:/home/sota/myos/boot/glibc-build/support:/home/sota/myos/boot/glibc-build/misc:/home/sota/myos/boot/glibc-build/debug:/home/sota/myos/boot/glibc-build/nptl:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/ /home/sota/myos/boot/glibc-build/support/test-container env GCONV_PATH=/home/sota/myos/boot/glibc-build/iconvdata LOCPATH=/home/sota/myos/boot/glibc-build/localedata LC_ALL=C  /home/sota/myos/boot/glibc-build/elf/ld-linux-x86-64.so.2 --library-path /home/sota/myos/boot/glibc-build:/home/sota/myos/boot/glibc-build/math:/home/sota/myos/boot/glibc-build/elf:/home/sota/myos/boot/glibc-build/dlfcn:/home/sota/myos/boot/glibc-build/nss:/home/sota/myos/boot/glibc-build/nis:/home/sota/myos/boot/glibc-build/rt:/home/sota/myos/boot/glibc-build/resolv:/home/sota/myos/boot/glibc-build/mathvec:/home/sota/myos/boot/glibc-build/support:/home/sota/myos/boot/glibc-build/misc:/home/sota/myos/boot/glibc-build/debug:/home/sota/myos/boot/glibc-build/nptl:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/:/usr/lib/gcc/x86_64-pc-linux-gnu/16.1.1/../../../../lib/ ${1+"$@"}
    ;;
  *)
    usage
    ;;
esac
