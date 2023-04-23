.section .data
    db:
        .word 0xDEADBEEF

.section .text
.globl _start

_start:
    lui x1, %hi(db)
    ori x1, x1, %lo(db)
    lbu x2, 0(x1)
    lbu x3, 0(x1)
    lbu x4, 0(x1)
    lbu x5, 0(x1)
    lbu x6, 0(x1)
    lbu x7, 0(x1)
    lbu x8, 0(x1)
    lbu x9, 0(x1)
    lbu x10, 0(x1)
    lbu x11, 0(x1)
    lbu x12, 0(x1)
    lbu x13, 0(x1)
    lbu x14, 0(x1)
    lbu x15, 0(x1)
    lbu x16, 0(x1)
    lbu x17, 0(x1)
    lbu x18, 0(x1)
    lbu x19, 0(x1)
    lbu x20, 0(x1)
    lbu x21, 0(x1)
    lbu x22, 0(x1)
    lbu x23, 0(x1)
    lbu x24, 0(x1)
    lbu x25, 0(x1)
    lbu x26, 0(x1)
    lbu x27, 0(x1)
    lbu x28, 0(x1)
    lbu x29, 0(x1)
    lbu x30, 0(x1)
    lbu x31, 0(x1)
    ebreak