    .section .text.entry
    .global _start
_start:
    li x1, 1
    li x5, 0x20000000
    sw x1, 0(x5)
    la x2, exception_entry
    csrw mtvec, x2
    c.li x5, 1
    li x6, 0x8
    /*csrw mie, x6
    csrsi mstatus, 0x8*/
    csrr x2, 0x123

loop:
    j loop

exception_entry:
    //li x1, 100
    mv x1, x2
    mret
