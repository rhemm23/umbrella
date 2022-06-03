OUTPUT_FORMAT("elf32-i386")
SECTIONS {
  . = 0x7E00;
  .text : AT(0x7E00) {
    bin/boot/entry.o(.text*);
    *(.text*);
  }
  .data : {
    *(.bss*);
    *(.data*);
    *(.rodata*);
  }
  /DISCARD/ : {
    *(.comment);
    *(.eh_frame);
  }
}
