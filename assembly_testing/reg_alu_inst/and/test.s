.section .text
.globl _start

_start:
    addi x1, x0, -1456
    addi x2, x0, -1276
    and  x3, x2, x1
    ebreak