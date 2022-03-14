    .section .text.entry
    .global _start
_start:
    li x1, 1
    li x5, 0x20000000
    sw x1, 0(x5)
    la x2, exception_entry
    csrw mtvec, x2
    li x6, 0x8
    csrw mie, x6
    csrsi mstatus, 0x8

loop:
    j loop

exception_entry:
    li x1, 100
    mret
