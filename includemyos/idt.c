void init_gdt() {
    struct GDTR gdtr;
    gdt[0].limit_low = 0; gdt[0].base_low = 0; gdt[0].base_middle = 0; gdt[0].access_byte = 0; gdt[0].flags_limit_high = 0; gdt[0].base_high = 0;
    gdt[1].limit_low = 0xFFFF; gdt[1].base_low = 0; gdt[1].base_middle = 0; gdt[1].access_byte = 0x9A; gdt[1].flags_limit_high = 0xA0; gdt[1].base_high = 0;
    gdt[2].limit_low = 0xFFFF; gdt[2].base_low = 0; gdt[2].base_middle = 0; gdt[2].access_byte = 0x92; gdt[2].flags_limit_high = 0xC0; gdt[2].base_high = 0;
    gdtr.limit = sizeof(gdt) - 1; gdtr.base = (uint64_t)&gdt;
    __asm__ volatile("lgdt %0\n\t" "mov $0x10, %%ax\n\t" "mov %%ax, %%ds\n\t" "mov %%ax, %%es\n\t" "mov %%ax, %%ss\n\t" "mov %%ax, %%fs\n\t" "mov %%ax, %%gs\n\t" "pushq $0x08\n\t" "leaq 1f(%%rip), %%rax\n\t" "pushq %%rax\n\t" "lretq\n\t" "1:\n\t" : : "m"(gdtr) : "rax", "ax");
}

void set_idt_entry(int vector, void (*handler)()) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].offset_low = addr & 0xFFFF; idt[vector].selector = 0x08; idt[vector].ist = 0; idt[vector].type_attr = 0x8E; idt[vector].offset_mid = (addr >> 16) & 0xFFFF; idt[vector].offset_high = (addr >> 32) & 0xFFFFFFFF; idt[vector].zero = 0;
}

void load_idt() { struct IDTR idtr; idtr.limit = sizeof(idt) - 1; idtr.base = (uint64_t)&idt; __asm__ volatile("lidt %0" : : "m"(idtr)); }
__attribute__((interrupt)) void dummy_handler(struct InterruptFrame* frame) {}
