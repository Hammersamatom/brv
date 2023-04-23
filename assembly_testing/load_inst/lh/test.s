.section .data
    db:
        .word 0xDEADBEEF

.section .text
.globl _start

_start:
    lui x1, %hi(db)
    ori x1, x1, %lo(db)
    lh x2, 0(x1)
    lh x3, 0(x1)
    lh x4, 0(x1)
    lh x5, 0(x1)
    lh x6, 0(x1)
    lh x7, 0(x1)
    lh x8, 0(x1)
    lh x9, 0(x1)
    lh x10, 0(x1)
    lh x11, 0(x1)
    lh x12, 0(x1)
    lh x13, 0(x1)
    lh x14, 0(x1)
    lh x15, 0(x1)
    lh x16, 0(x1)
    lh x17, 0(x1)
    lh x18, 0(x1)
    lh x19, 0(x1)
    lh x20, 0(x1)
    lh x21, 0(x1)
    lh x22, 0(x1)
    lh x23, 0(x1)
    lh x24, 0(x1)
    lh x25, 0(x1)
    lh x26, 0(x1)
    lh x27, 0(x1)
    lh x28, 0(x1)
    lh x29, 0(x1)
    lh x30, 0(x1)
    lh x31, 0(x1)
    ebreak