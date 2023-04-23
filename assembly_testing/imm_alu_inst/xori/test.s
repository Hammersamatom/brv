.section .text
.globl _start

_start:
    xori x0, x0, 0x055
    xori x1, x0, 0x05A
    xori x2, x1, 0x0A5
    xori x3, x2, 0x0AA
    xori x4, x3, 0x055
    xori x5, x0, -1536
    xori x6, x0, -1408
    xori x7, x0, -1280
    xori x8, x0, -1152
    xori x9, x0, -1024
    xori x10, x0, -896
    xori x11, x0, -768
    xori x12, x0, -640
    xori x13, x0, -512
    xori x14, x0, -384
    xori x15, x0, -256
    xori x16, x0, -128
    xori x17, x0, 0
    xori x18, x0, 128
    xori x19, x0, 256
    xori x20, x0, 384
    xori x21, x0, 512
    xori x22, x0, 768
    xori x23, x0, 1024
    xori x24, x0, 1152
    xori x25, x0, 1280
    xori x26, x0, 1408
    xori x27, x0, 1536
    xori x28, x0, 1664
    xori x29, x0, 1792
    xori x30, x0, 1920
    xori x31, x0, 2047
    ebreak