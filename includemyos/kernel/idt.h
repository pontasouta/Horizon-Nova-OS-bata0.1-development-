#ifndef IDT_H
#define IDT_H
#include <stdint.h>
struct InterruptFrame { uint64_t rip; uint64_t cs; uint64_t flags; uint64_t rsp; uint64_t ss; };
void init_gdt();
void set_idt_entry(int vector, void (*handler)());
void load_idt();
__attribute__((interrupt)) void dummy_handler(struct InterruptFrame* frame);
#endif // IDT_H