[global exit]

CODE_SEGMENT equ 0x08
DATA_SEGMENT equ 0x10

[bits 16]
exit:
  cli
  lgdt [gdt_desc]
  mov eax, cr0
  or eax, 1
  mov cr0, eax
  jmp CODE_SEGMENT:exit32

[bits 32]
exit32:
  mov ax, DATA_SEGMENT
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov ebp, 0x7C00
  mov esp, ebp

  jmp 0x8E00

gdt_desc:
  dw 23
  dd 0x0500
