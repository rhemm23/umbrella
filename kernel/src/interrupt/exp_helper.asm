%macro EXCEPTION_NOERR 1
global exception%1
exception%1:
  cli
  push byte 0
  push %1
  jmp exception_handler_helper
%endmacro

%macro EXCEPTION_ERR 1
global exception%1
exception%1:
  cli
  push %1
  jmp exception_handler_helper
%endmacro

EXCEPTION_NOERR 0
EXCEPTION_NOERR 1
EXCEPTION_NOERR 2
EXCEPTION_NOERR 3
EXCEPTION_NOERR 4
EXCEPTION_NOERR 5
EXCEPTION_NOERR 6
EXCEPTION_NOERR 7
EXCEPTION_ERR 8
EXCEPTION_NOERR 9
EXCEPTION_ERR 10
EXCEPTION_ERR 11
EXCEPTION_ERR 12
EXCEPTION_ERR 13
EXCEPTION_ERR 14
EXCEPTION_NOERR 15
EXCEPTION_NOERR 16
EXCEPTION_NOERR 17
EXCEPTION_NOERR 18
EXCEPTION_NOERR 19
EXCEPTION_NOERR 20
EXCEPTION_NOERR 21
EXCEPTION_NOERR 22
EXCEPTION_NOERR 23
EXCEPTION_NOERR 24
EXCEPTION_NOERR 25
EXCEPTION_NOERR 26
EXCEPTION_NOERR 27
EXCEPTION_NOERR 28
EXCEPTION_NOERR 29
EXCEPTION_NOERR 30
EXCEPTION_NOERR 31

[extern exception_handler]
exception_handler_helper:
  pusha
  mov ax, ds
  push eax
  
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  call exception_handler

  pop ebx
  mov ds, bx
  mov es, bx
  mov fs, bx
  mov gs, bx

  popa
  add esp, 0x8

  sti
  iret
