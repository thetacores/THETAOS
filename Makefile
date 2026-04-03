TARGET  := thetaos
ISO     := $(TARGET).iso
KERNEL  := build/$(TARGET).elf

AS      := nasm
CC      := i686-elf-gcc
LD      := i686-elf-gcc

ASFLAGS := -f elf32
CFLAGS  := -m32 -ffreestanding -O2 -Wall -Wextra -std=c11 \
           -fno-stack-protector -Ikernel/include
LDFLAGS := -m32 -ffreestanding -nostdlib \
           -T linker.ld

BOOT_OBJ := build/boot/boot.o
KERN_OBJ := build/kernel/kernel.o build/kernel/drivers/vga.o \
            build/kernel/klib/kprintf.o build/kernel/klib/string.o

.PHONY: all clean iso run

all: $(KERNEL)

build/boot/boot.o: boot/boot.asm
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

build/kernel/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(BOOT_OBJ) $(KERN_OBJ)
	mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) $^ -lgcc -o $@

iso: $(KERNEL)
	mkdir -p build/iso/boot/grub
	cp $(KERNEL) build/iso/boot/
	cp boot/grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) build/iso

run: iso
	qemu-system-i386 -cdrom $(ISO) -display gtk

clean:
	rm -rf build $(ISO)
