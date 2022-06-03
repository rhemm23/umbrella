[org 0x7C00]
[bits 16]

xor ax, ax
mov ss, ax
mov es, ax
mov ds, ax
mov fs, ax
mov gs, ax
mov sp, 0x7C00
mov bp, sp

mov si, disk_packet
mov ah, 0x42
int 0x13

jmp 0x7E00

disk_packet:
  db 0x10
  db 0
  dw 8
  dw 0x7E00
  dw 0
  dd 1
  dd 0

times 510-($-$$) db 0
dw 0xAA55
