    .section .text.entry
    .global _start
_start:
    li x1, 10

loop:
    addi x1, x1, -1
    bne x1, x0, loop
    li x2, 1
infinite_loop:
    j infinite_loop