.global _start
_start:
    addi x1, x0, 1000
    addi x2, x0, 2000
    addi x3, x0, -1000
    xor x4, x2, x1
    xori x5, x3, -1950
    ebreak