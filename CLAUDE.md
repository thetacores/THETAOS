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
    io.c / io.h     Port I/O helpers (inb/outb/inw/outw, static inline)
  drivers/
    vga.c           VGA text-mode driver
    keyboard.c      PS/2 keyboard driver (IRQ1)
    timer.c         PIT timer driver (IRQ0)
    ata.c           ATA PIO disk driver (primary bus, LBA28, polling)
  input/
    input.c         Input event layer (keycodes → events)
  klib/
    kprintf.c       Kernel printf (%d, %x, %s, %c)
    string.c        String utilities
    kmalloc.c       Free-list heap allocator (4 MB static heap, 8-byte aligned)
  shell/
    shell.c         Interactive shell (prompt, input loop, command dispatch)
  include/          Headers mirroring the above layout
linker.ld           Linker script
Makefile            Build system
```

## Boot sequence

`kernel_main` runs: `kmalloc_init` → `gdt_init` → `idt_init` → `timer_init` → `keyboard_init` → `ata_init` → `shell()`

## Shell

`shell()` runs a blocking read loop via `keyboard_getc()`, parses whitespace-delimited tokens into `argv[]`, and dispatches to `shell_exec()`.

Built-in commands: `help`, `clear`, `credit`, `echo`, `poweroff`, `reboot`, `write`, `read`, `test`

- `poweroff` — QEMU ACPI shutdown via port 0x604 (fallback 0xB004)
- `reboot` — keyboard controller reset (port 0x64 / 0xFE), triple-fault fallback
- `write {lba} {string}` — write a string to the given LBA sector via ATA PIO
- `read {lba}` — read and print a string from the given LBA sector via ATA PIO
- `test {arg}` — debug/test command

## Memory allocator

`kmalloc` / `kfree` use a free-list allocator over a static 4 MB heap (`heap[]` in `kmalloc.c`).

- Allocations are 8-byte aligned; each block is preceded by a `block_t` header.
- `kfree` coalesces adjacent free blocks.
- `kdebug()` dumps the block list via `kprintf`.
- Call `kmalloc_init()` before any other subsystem that may allocate.

## ATA PIO disk driver

ATA PIO (Programmed I/O) driver in `kernel/drivers/ata.c` / `kernel/include/drivers/ata.h`.

- Primary ATA bus: I/O base `0x1F0`, control `0x3F6`
- Polling BSY/DRQ status bits; 28-bit LBA, PIO mode — no DMA, no IRQ
- 400ns delay via 4× alt-status reads before polling
- `ata_init()` — soft reset + master drive select
- `ata_read_sector(lba, buf)` / `ata_write_sector(lba, buf)` — 256×16-bit word transfers
- `ata_read_string(lba, buf)` / `ata_write_string(lba, buf)` — convenience wrappers for null-terminated strings

## Conventions

- Kernel C code uses `-std=c11`, `-Wall -Wextra`, `-fno-stack-protector`.
- Assembly stubs go in `.asm` files alongside their `.c` counterparts; Makefile builds them as `%.asm.o`.
- GDT selector 0x08 = kernel code, 0x10 = kernel data.
- PIC is remapped so IRQ0-7 map to vectors 32-39, IRQ8-15 to 40-47.
- ISR/IRQ stubs push a uniform `isr_frame` (see `idt.h`) before calling C handlers.
