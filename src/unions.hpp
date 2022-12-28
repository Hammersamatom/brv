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
    struct __attribute__((__packed__)) s_imm
    {
        uint8_t imm4_0 : 5;
        uint8_t imm11_5 : 7;
        uint32_t unused : 20;
    } s_imm;
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

union instr_word
{
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
    // Atomics (A-Extension) support
    struct __attribute__((__packed__)) a_type
    {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint8_t rs2 : 5;
        uint8_t rl : 1;
        uint8_t aq : 1;
        uint8_t funct5 : 5;
    } a_type;
    // Compressed (C-extension) support
    struct __attribute__((__packed__)) cr_type
    {
        uint8_t op : 2;
        uint8_t rs2 : 5;
        uint8_t rd_rs1 : 5;
        uint8_t funct4 : 4;
    } cr_type;
    struct __attribute__((__packed__)) ci_type
    {
        uint8_t op : 2;
        uint8_t imm6_2 : 5;
        uint8_t rd_rs1 : 5;
        uint8_t imm12 : 1;
        uint8_t funct3 : 3;
    } ci_type;
    struct __attribute__((__packed__)) css_type
    {
        uint8_t op : 2;
        uint8_t rs2 : 5;
        uint8_t imm : 6;
        uint8_t funct3 : 3;
    } css_type;
    struct __attribute__((__packed__)) ciw_type
    {
        uint8_t op : 2;
        uint8_t rd : 3;
        uint8_t imm : 8;
        uint8_t funct3 : 3;
    } ciw_type;
    struct __attribute__((__packed__)) cl_type
    {
        uint8_t op : 2;
        uint8_t rd : 3;
        uint8_t imm6_5 : 2;
        uint8_t rs1 : 3;
        uint8_t imm12_10 : 3;
        uint8_t funct3 : 3;
    } cl_type;
    struct __attribute__((__packed__)) cs_type
    {
        uint8_t op : 2;
        uint8_t rs2 : 3;
        uint8_t imm6_5 : 2;
        uint8_t rd_rs1 : 3;
        uint8_t imm12_10 : 3;
        uint8_t funct3 : 3;
    } cs_type;
    struct __attribute__((__packed__)) cb_type
    {
        uint8_t op : 2;
        uint8_t imm6_2 : 5;
        uint8_t rs1 : 3;
        uint8_t imm12_10 : 3;
        uint8_t funct3 : 3;
    } cb_type;
    struct __attribute__((__packed__)) cj_type
    {
        uint8_t op : 2;
        uint16_t offset : 11;
        uint8_t funct3 : 3;
    } cj_type;
};