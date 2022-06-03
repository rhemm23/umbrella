CC=i386-elf-gcc
LD=i386-elf-ld

BOOT_LINK := src/boot/link.ld
BOOT_SRCS := $(shell find src/boot/src -name "*.c")
BOOT_OBJS := $(patsubst src/boot/src/%.c, bin/boot/%.o, $(BOOT_SRCS))

KERNEL_LINK := src/kernel/link.ld
KERNEL_SRCS := $(shell find src/kernel/src -name "*.c")
KERNEL_OBJS := $(patsubst src/kernel/src/%.c, bin/kernel/%.o, $(KERNEL_SRCS))

run: bin/os.bin
	qemu-system-i386 -m 2G -serial stdio -vga std -hda $<

clean:
	rm -rf bin

bin/os.bin: bin/sect.bin bin/boot.bin bin/kernel.bin
	mkdir -p $(dir $@)
	cat $^ > $@

bin/kernel.bin: bin/kernel/entry.o $(KERNEL_OBJS)
	mkdir -p $(dir $@)
	$(LD) -o $@ -T $(KERNEL_LINK) $^ /usr/local/i386elfgcc/lib/gcc/i386-elf/4.9.1/libgcc.a --oformat binary
	dd if=/dev/null of=$@ bs=1 count=1 seek=16384

bin/kernel/entry.o: src/kernel/entry.asm
	mkdir -p $(dir $@)
	nasm -f elf $< -o $@

bin/kernel/%.o: src/kernel/src/%.c
	mkdir -p $(dir $@)
	$(CC) -fno-pic -nostdlib -nostdinc -std=c99 -ffreestanding -I src/kernel/include -c $< -o $@

bin/boot.bin: bin/boot/entry.o bin/boot/exit.o $(BOOT_OBJS)
	mkdir -p $(dir $@)
	$(LD) -o $@ -T $(BOOT_LINK) $^ /usr/local/i386elfgcc/lib/gcc/i386-elf/4.9.1/libgcc.a --oformat binary
	dd if=/dev/null of=$@ bs=1 count=1 seek=4096

bin/boot/%.o: src/boot/%.asm
	mkdir -p $(dir $@)
	nasm -f elf $< -o $@

bin/boot/%.o: src/boot/src/%.c
	mkdir -p $(dir $@)
	$(CC) -fno-pic -nostdlib -nostdinc -m16 -std=c99 -ffreestanding -I src/boot/include -c $< -o $@

bin/sect.bin: src/sect.asm
	mkdir -p $(dir $@)
	nasm -f bin $< -o $@
