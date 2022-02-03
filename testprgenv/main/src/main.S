    .section .text.entry
    .global _start
_start:
    li x1, 0
    li x2, 0
    li x3, 0
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0
    li x9, 0
    li x10, 0
loop:
    addi x1, x1, 1
    addi x2, x2, 1
    addi x3, x3, 1
    addi x4, x4, 1
    addi x5, x5, 1
    addi x6, x6, 1
    addi x7, x7, 1
    addi x8, x8, 1
    addi x9, x9, 1
    addi x10, x10, 1
    addi x1, x1, 1
    addi x2, x2, 1
    addi x3, x3, 1
    addi x4, x4, 1
    addi x5, x5, 1
    addi x6, x6, 1
    addi x7, x7, 1
    addi x8, x8, 1
    addi x9, x9, 1
    addi x10, x10, 1
    j loop