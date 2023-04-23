.section .data
    db:
        .word 0xDEADBEEF

.section .text
.globl _start

_start:
    lui x1, %hi(db)
    ori x1, x1, %lo(db)
    lb x2, 0(x1)
    lb x3, 0(x1)
    lb x4, 0(x1)
    lb x5, 0(x1)
    lb x6, 0(x1)
    lb x7, 0(x1)
    lb x8, 0(x1)
    lb x9, 0(x1)
    lb x10, 0(x1)
    lb x11, 0(x1)
    lb x12, 0(x1)
    lb x13, 0(x1)
    lb x14, 0(x1)
    lb x15, 0(x1)
    lb x16, 0(x1)
    lb x17, 0(x1)
    lb x18, 0(x1)
    lb x19, 0(x1)
    lb x20, 0(x1)
    lb x21, 0(x1)
    lb x22, 0(x1)
    lb x23, 0(x1)
    lb x24, 0(x1)
    lb x25, 0(x1)
    lb x26, 0(x1)
    lb x27, 0(x1)
    lb x28, 0(x1)
    lb x29, 0(x1)
    lb x30, 0(x1)
    lb x31, 0(x1)
    ebreak