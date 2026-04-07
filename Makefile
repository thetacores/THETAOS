# SPDX-License-Identifier: GPL-2.0

TARGET	:= thetaos
ISO	:= $(TARGET).iso
KERNEL	:= build/$(TARGET).elf

AS	:= nasm
CC	:= i686-elf-gcc
LD	:= i686-elf-gcc

ASFLAGS	:= -f elf32
CFLAGS	:= -m32 -ffreestanding -O2 -Wall -Wextra -std=c11 \
	   -fno-stack-protector -Ikernel/include
LDFLAGS	:= -m32 -ffreestanding -nostdlib -T linker.ld

# Auto-discover sources
BOOT_ASM := $(wildcard boot/*.asm)
KERN_C   := $(shell find kernel -name '*.c')
KERN_ASM := $(shell find kernel -name '*.asm')

# Generate object paths under build/
BOOT_OBJ := $(patsubst %.asm,build/%.o,$(BOOT_ASM))
KERN_OBJ := $(patsubst kernel/%.c,build/kernel/%.o,$(KERN_C)) \
	     $(patsubst kernel/%.asm,build/kernel/%.asm.o,$(KERN_ASM))

.PHONY: all clean iso run

all: $(KERNEL)

build/boot/%.o: boot/%.asm
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

build/kernel/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel/%.asm.o: kernel/%.asm
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(KERNEL): $(BOOT_OBJ) $(KERN_OBJ)
	mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) $^ -lgcc -o $@

iso: $(KERNEL)
	mkdir -p build/iso/boot/grub
	cp $(KERNEL) build/iso/boot/
	cp boot/grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) build/iso

disk.img:
	dd if=/dev/zero of=disk.img bs=512 count=2048

run: iso disk.img
	qemu-system-i386 -cdrom $(ISO) \
		-drive file=disk.img,format=raw,if=ide,index=0 \
		-display gtk

clean:
	rm -rf build $(ISO) disk.img
