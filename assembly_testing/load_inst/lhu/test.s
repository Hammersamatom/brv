.global _start
_start:
    lhu x1, 8(x0)
    ebreak
da:
    .word 0xDEADBEEF