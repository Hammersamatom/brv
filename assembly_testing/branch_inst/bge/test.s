.section .text
.globl _start

_start:
    addi x1, x0, -1456
    addi x2, x0, 1456
    bgeu x2, x1, TEST
    addi x3, x0, 69
    ebreak
TEST:
    addi x3, x0, 2000
    ebreak