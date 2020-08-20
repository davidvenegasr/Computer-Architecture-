section .rodata
ALIGN 16
half_white1: dd 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000
section .text

global _start

_start:
  movdqa xmm0, [half_white1]
  movdqa xmm0, [half_white1]
  movdqa xmm0, [half_white1]