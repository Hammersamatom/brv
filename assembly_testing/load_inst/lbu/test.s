.global _start
_start:
    lbu x1, 8(x0)
    ebreak
da:
    .word 0xDEADBEEF