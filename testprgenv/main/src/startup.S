    .section .text.entry
    .global _start
_start:
    li x1, 11
    li x2, 11
    li x3, 11
    li x4, 11
    li x5, 11
    li x6, 11
    li x7, 11
    li x8, 11
    li x9, 11
    li x11, 11
    li x11, 11
    li x12, 11
    li x13, 11
    li x14, 11
    li x15, 11
    li x16, 11
    li x17, 11
    li x18, 11
    li x19, 11
    la x20, 0x80001000
    sw x20, 0(x20)
    fence
    la x20, 0x80001010
    sw x20, 0(x20)
    la x20, 0x80001020
    fence
    sw x20, 0(x20)
    fence.i
    li x1, 11
    li x2, 11
    li x3, 11
    li x4, 11
    li x5, 11
    li x6, 11
    li x7, 11
    li x8, 11
    li x9, 11
    li x11, 11
    li x11, 11
    li x12, 11
    li x13, 11
    li x14, 11
    li x15, 11
    li x16, 11
    li x17, 11
    li x18, 11
    li x19, 11

loop:
    addi x1, x1, -1
    bne x1, x0, loop
    li x2, 1
infinite_loop:
    j infinite_loop