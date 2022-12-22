#include <cstdint>

union component
{
    int32_t word_s;
    uint32_t word;
    uint16_t half[2];
    int16_t half_s[2];
    uint8_t byte[4];
    int8_t byte_s[4];
};

union imm_reconstruct
{
    uint32_t word;
    int32_t word_s;
    struct __attribute__((__packed__)) b_imm
    {
        uint8_t unused_1 : 1;
        uint8_t imm4_1 : 4;
        uint8_t imm10_5 : 6;
        uint8_t imm11 : 1;
        uint8_t imm12 : 1;
        uint32_t unused_2 : 19;
    } b_imm;
    struct __attribute__((__packed__)) j_imm
    {
        uint8_t unused_1 : 1;
        uint16_t imm10_1 : 10;
        uint8_t imm11 : 1;
        uint8_t imm19_12 : 8;
        uint8_t imm20 : 1;
        uint16_t unused_2 : 11;
    } j_imm;
};

union instr
{
    //uint8_t inst_byte[4];
    //uint16_t inst_half[2];
    uint32_t instruction;
    struct __attribute__((__packed__)) op_only
    {
        uint8_t opcode : 7;
        uint32_t unused : 25;
    } op_only;
    struct __attribute__((__packed__)) r_type
    {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint8_t rs2 : 5;
        uint8_t funct7 : 7;
    } r_type;
    struct __attribute__((__packed__)) i_type
    {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint16_t imm : 12;
    } i_type;
    struct __attribute__((__packed__)) s_type
    {
        uint8_t opcode : 7;
        uint8_t imm4_0 : 5;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint8_t rs2 : 5;
        uint8_t imm11_5 : 7;
    } s_type;
    struct __attribute__((__packed__)) b_type
    {
        uint8_t opcode : 7;
        uint8_t imm11 : 1;
        uint8_t imm4_1 : 4;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint8_t rs2 : 5;
        uint8_t imm10_5 : 6;
        uint8_t imm12 : 1;
    } b_type;
    struct __attribute__((__packed__)) u_type
    {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint32_t imm31_12 : 20;
    } u_type;
    struct __attribute__((__packed__)) j_type
    {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint8_t imm19_12 : 8;
        uint8_t imm11 : 1;
        uint16_t imm10_1 : 10;
        uint8_t imm20 : 1;
    } j_type;
};