    .section .text.entry
    .global _start
_start:
    csrrwi x0, mtvec, 0xf
    j _start