#include <cpu/gdt.h>
#include <klib/kprintf.h>

#define GDTBASE 0x00000800

struct gdtr gdtr;
struct gdt_desc gdt[7];

void gdt_init() {
    /* Null descriptor */
    gdt[0] = (struct gdt_desc){ 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00 };

    /* Kernel code segment (ring 0) */
    gdt[1] = (struct gdt_desc){ 0xFFFF, 0x0000, 0x00, 0x9A, 0xCF, 0x00 };

    /* Kernel data segment (ring 0) */
    gdt[2] = (struct gdt_desc){ 0xFFFF, 0x0000, 0x00, 0x92, 0xCF, 0x00 };

    /* User code segment (ring 3) */
    gdt[3] = (struct gdt_desc){ 0xFFFF, 0x0000, 0x00, 0xFA, 0xCF, 0x00 };

    /* User data segment (ring 3) */
    gdt[4] = (struct gdt_desc){ 0xFFFF, 0x0000, 0x00, 0xF2, 0xCF, 0x00 };

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base  = GDTBASE;

    __asm__ volatile("lgdt %0" : : "m"(gdtr));
}

void gdt_debug() {
    kprintf("[GDT] GDTR base=0x%x limit=0x%x\n", gdtr.base, gdtr.limit);
    for (int i = 0; i < 5; i++) {
        uint32_t base = gdt[i].base0 | (gdt[i].base16 << 16) | (gdt[i].base24 << 24);
        uint32_t limit = gdt[i].limit0 | ((gdt[i].flags_limit & 0x0F) << 16);
        kprintf("[GDT] [%d] base=0x%x limit=0x%x access=0x%x flags=0x%x\n",
            i, base, limit, gdt[i].access, (gdt[i].flags_limit >> 4));
    }
}
