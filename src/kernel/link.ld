OUTPUT_FORMAT("elf32-i386")
SECTIONS {
  . = 0x8E00;
  .text : AT(0x8E00) {
    bin/kernel/entry.o(.text*);
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
