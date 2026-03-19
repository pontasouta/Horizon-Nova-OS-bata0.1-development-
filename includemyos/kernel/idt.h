#ifndef IDT_H
#define IDT_H
#include <stdint.h>
struct InterruptFrame { uint64_t rip; uint64_t cs; uint64_t flags; uint64_t rsp; uint64_t ss; };
#endif // IDT_H