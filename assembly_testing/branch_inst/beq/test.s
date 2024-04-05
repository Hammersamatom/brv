.section .text
.globl _start

_start:
    addi x1, x0, -1456
    addi x1, x1, 1456
    beq x1, x0, TEST
    addi x1, x0, 69
    ebreak
TEST:
    addi x2, x0, 2000
    ebreak