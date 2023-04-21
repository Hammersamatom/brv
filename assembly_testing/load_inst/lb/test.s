.global _start
_start:
    lb x1, 8(x0)
    ebreak
da:
    .word 0xDEADBEEF