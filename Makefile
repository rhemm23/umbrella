CC=i386-elf-gcc
LD=i386-elf-ld

BOOT_LINK := boot/link.ld
BOOT_SRCS := $(shell find boot/src -name "*.c")
BOOT_OBJS := $(patsubst boot/src/%.c, bin/boot/%.o, $(BOOT_SRCS))

KERNEL_LINK := kernel/link.ld
KERNEL_SRCS := $(shell find kernel/src -name "*.c")
KERNEL_OBJS := $(patsubst kernel/src/%.c, bin/kernel/%.o, $(KERNEL_SRCS))
KERNEL_ASM_SRCS := $(shell find kernel/src -name "*.asm")
KERNEL_ASM_OBJS := $(patsubst kernel/src/%.asm, bin/kernel/%.o, $(KERNEL_ASM_SRCS))

run: bin/os.bin
	qemu-system-i386 \
	-m 2G \
	-serial stdio \
	-vga std \
	-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
	-device rtl8139,netdev=net0,id=nic0,mac=66:d3:0e:2f:99:00 \
	-object filter-dump,id=f0,netdev=net0,file=/tmp/traffic.dat \
	-hda $<

clean:
	rm -rf bin

bin/os.bin: bin/sect.bin bin/boot.bin bin/kernel.bin
	mkdir -p $(dir $@)
	cat $^ > $@

bin/kernel.bin: bin/kernel/entry.o $(KERNEL_OBJS) $(KERNEL_ASM_OBJS)
	mkdir -p $(dir $@)
	$(LD) -o $@ -T $(KERNEL_LINK) $^ /usr/local/i386elfgcc/lib/gcc/i386-elf/4.9.1/libgcc.a --oformat binary
	dd if=/dev/null of=$@ bs=1 count=1 seek=32768

bin/kernel/%.o: kernel/src/%.asm
	mkdir -p $(dir $@)
	nasm -f elf $< -o $@

bin/kernel/entry.o: kernel/entry.asm
	mkdir -p $(dir $@)
	nasm -f elf $< -o $@

bin/kernel/%.o: kernel/src/%.c
	mkdir -p $(dir $@)
	$(CC) -fno-pic -nostdlib -nostdinc -std=c99 -ffreestanding -I kernel/include -c $< -o $@

bin/boot.bin: bin/boot/entry.o bin/boot/exit.o $(BOOT_OBJS)
	mkdir -p $(dir $@)
	$(LD) -o $@ -T $(BOOT_LINK) $^ /usr/local/i386elfgcc/lib/gcc/i386-elf/4.9.1/libgcc.a --oformat binary
	dd if=/dev/null of=$@ bs=1 count=1 seek=4096

bin/boot/%.o: boot/%.asm
	mkdir -p $(dir $@)
	nasm -f elf $< -o $@

bin/boot/%.o: boot/src/%.c
	mkdir -p $(dir $@)
	$(CC) -fno-pic -nostdlib -nostdinc -m16 -std=c99 -ffreestanding -I boot/include -c $< -o $@

bin/sect.bin: sect.asm
	mkdir -p $(dir $@)
	nasm -f bin $< -o $@
