.global _start
_start:
    lw x1, 8(x0)
    ebreak
da:
    .word 0xDEADBEEF