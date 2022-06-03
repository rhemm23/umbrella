[global gdt_flush]

CODE_SEGMENT equ 0x08
DATA_SEGMENT equ 0x10

gdt_flush:
  mov eax, [esp + 4]
  lgdt [eax]

  mov ax, DATA_SEGMENT
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  jmp CODE_SEGMENT:flush 

flush:
  ret
