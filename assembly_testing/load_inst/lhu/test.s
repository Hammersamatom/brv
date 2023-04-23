.section .data
    db:
        .word 0xDEADBEEF

.section .text
.globl _start

_start:
    lui x1, %hi(db)
    ori x1, x1, %lo(db)
    lhu x2, 0(x1)
    lhu x3, 0(x1)
    lhu x4, 0(x1)
    lhu x5, 0(x1)
    lhu x6, 0(x1)
    lhu x7, 0(x1)
    lhu x8, 0(x1)
    lhu x9, 0(x1)
    lhu x10, 0(x1)
    lhu x11, 0(x1)
    lhu x12, 0(x1)
    lhu x13, 0(x1)
    lhu x14, 0(x1)
    lhu x15, 0(x1)
    lhu x16, 0(x1)
    lhu x17, 0(x1)
    lhu x18, 0(x1)
    lhu x19, 0(x1)
    lhu x20, 0(x1)
    lhu x21, 0(x1)
    lhu x22, 0(x1)
    lhu x23, 0(x1)
    lhu x24, 0(x1)
    lhu x25, 0(x1)
    lhu x26, 0(x1)
    lhu x27, 0(x1)
    lhu x28, 0(x1)
    lhu x29, 0(x1)
    lhu x30, 0(x1)
    lhu x31, 0(x1)
    ebreak