# THETAOS

A 32-bit x86 operating system kernel, booted via Multiboot2/GRUB.

## Build

```
make          # build kernel ELF
make iso      # build bootable ISO
make run      # build ISO and launch in QEMU (i386, GTK display)
make clean    # remove build artifacts
```

## Toolchain

- **Assembler:** nasm (elf32)
- **Compiler:** i686-elf-gcc (C11, -ffreestanding, -O2)
- **Linker:** i686-elf-gcc with `linker.ld` (kernel loaded at 1M)

## Project structure

```
boot/
  boot.asm          Multiboot2 header + _start entry point
  grub.cfg          GRUB configuration
kernel/
  kernel.c          kernel_main entry
  cpu/
    gdt.c / gdt.asm GDT setup + segment reload (gdt_flush)
    idt.c / idt.asm  IDT, PIC remap, ISR/IRQ stubs
  drivers/
    vga.c           VGA text-mode driver
    keyboard.c      PS/2 keyboard driver (IRQ1)
    timer.c         PIT timer driver (IRQ0)
  input/
    input.c         Input event layer (keycodes → events)
  klib/
    kprintf.c       Kernel printf (%d, %x, %s, %c)
    string.c        String utilities
  include/          Headers mirroring the above layout
linker.ld           Linker script
Makefile            Build system
```

## Conventions

- Kernel C code uses `-std=c11`, `-Wall -Wextra`, `-fno-stack-protector`.
- Assembly stubs go in `.asm` files alongside their `.c` counterparts; Makefile builds them as `%.asm.o`.
- GDT selector 0x08 = kernel code, 0x10 = kernel data.
- PIC is remapped so IRQ0-7 map to vectors 32-39, IRQ8-15 to 40-47.
- ISR/IRQ stubs push a uniform `isr_frame` (see `idt.h`) before calling C handlers.
