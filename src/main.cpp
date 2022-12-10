#include <cstdint>
#include <fmt/core.h>

union instr {
    uint32_t instruction;
    struct __attribute__((__packed__)) r_type {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint8_t rs2 : 5;
        uint8_t funct7 : 7;
    } r_type;
    struct __attribute__((__packed__)) i_type {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint16_t imm : 12;
    } i_type;
    struct __attribute__((__packed__)) s_type {
        uint8_t opcode : 7;
        uint8_t imm4_0 : 5;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint8_t rs2 : 5;
        uint8_t imm11_5 : 7;
    } s_type;
    struct __attribute__((__packed__)) b_type {
        uint8_t opcode : 7;
        uint8_t imm11 : 1;
        uint8_t imm4_1 : 4;
        uint8_t funct3 : 3;
        uint8_t rs1 : 5;
        uint8_t rs2 : 5;
        uint8_t imm10_5 : 6;
        uint8_t imm12 : 1;
    } b_type;
    struct __attribute__((__packed__)) u_type {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint32_t imm31_12 : 20;
    } u_type;
    struct __attribute__((__packed__)) j_type {
        uint8_t opcode : 7;
        uint8_t rd : 5;
        uint8_t imm19_12 : 8;
        uint8_t imm11 : 1;
        uint16_t imm10_1 : 10;
        uint8_t imm20 : 1;
    } j_type;
};

constexpr size_t tech = sizeof(instr);

int main()
{
    instr test;
    test.instruction = 0x001101b3;

    fmt::print("opcode in binary: {:07b}\n", (uint8_t)test.r_type.opcode);
    fmt::print("rd in binary, and num: {:05b}, {:d}\n", (uint8_t)test.r_type.rd, (uint8_t)test.r_type.rd);
    fmt::print("funct3 in binary: {:03b}\n", (uint8_t)test.r_type.funct3);
    fmt::print("rs1 in binary, and num: {:05b}, {:d}\n", (uint8_t)test.r_type.rs1, (uint8_t)test.r_type.rs1);
    fmt::print("rs2 in binary, and num: {:05b}, {:d}\n", (uint8_t)test.r_type.rs2, (uint8_t)test.r_type.rs2);
    fmt::print("funct7 in binary: {:07b}\n", (uint8_t)test.r_type.funct7);

    // What I'd expect
    // 0000000 00001 00010 000 00011 0110011

    return 0;
}