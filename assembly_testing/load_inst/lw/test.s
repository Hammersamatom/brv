.section .data
    db:
        .word 0xDEADBEEF

.section .text
.globl _start

_start:
    lui x1, %hi(db)
    ori x1, x1, %lo(db)
    lw x2, 0(x1)
    lw x3, 0(x1)
    lw x4, 0(x1)
    lw x5, 0(x1)
    lw x6, 0(x1)
    lw x7, 0(x1)
    lw x8, 0(x1)
    lw x9, 0(x1)
    lw x10, 0(x1)
    lw x11, 0(x1)
    lw x12, 0(x1)
    lw x13, 0(x1)
    lw x14, 0(x1)
    lw x15, 0(x1)
    lw x16, 0(x1)
    lw x17, 0(x1)
    lw x18, 0(x1)
    lw x19, 0(x1)
    lw x20, 0(x1)
    lw x21, 0(x1)
    lw x22, 0(x1)
    lw x23, 0(x1)
    lw x24, 0(x1)
    lw x25, 0(x1)
    lw x26, 0(x1)
    lw x27, 0(x1)
    lw x28, 0(x1)
    lw x29, 0(x1)
    lw x30, 0(x1)
    lw x31, 0(x1)
    ebreak