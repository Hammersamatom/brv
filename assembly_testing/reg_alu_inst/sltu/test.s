.section .text
.globl _start

_start:
    addi x1, x0, -1456
    addi x2, x0, -1276
    sltu x3, x1, x2
    ebreak