[global idt_flush]
idt_flush:
  lidt [idt_desc]
  ret

idt_desc:
  dw 2047
  dd 0x5000
