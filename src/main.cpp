#include <cstdint>
#include <array>
#include <vector>
#include <iostream>

#include <fmt/core.h>

enum instr_types {
    R_TYPE,
    I_TYPE,
    S_TYPE,
    B_TYPE,
    U_TYPE,
    J_TYPE,
    RAW
};

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

void spit_registers(uint32_t* regs, uint32_t pc)
{
    for (uint32_t c = 0; c < 32; c++)
        fmt::print("> x{0:02}: {1:032b} {1:08x} {1} {2}\n", c, regs[c], (int32_t)regs[c]);
    fmt::print("> PCR: {}\n", pc);
}

void unimplemented_instr(instr unimp, uint32_t* regs, uint32_t pc)
{
    fmt::print("! UNIMPLEMENTED INSTRUCTION !\nHEX: {:08x}\n> Opcode: {:07b}\n", unimp.instruction, (uint8_t)unimp.op_only.opcode);
    spit_registers(regs, pc);
    std::cin.get();
}

void print_pretty_instr(instr_types type, instr i)
{
    switch (type)
    {
        case RAW:
            fmt::print("{:08b} {:08b} {:08b} {:08b}\n", i.instruction >> 24 & 0xFF, i.instruction >> 16 & 0xFF, i.instruction >> 8 & 0xFF, i.instruction & 0xFF);
            break;
        case R_TYPE:
            fmt::print("{:07b} {:05b} {:05b} {:03b} {:05b} {:07b}\n", (uint8_t)i.r_type.funct7, (uint8_t)i.r_type.rs2, (uint8_t)i.r_type.rs1, (uint8_t)i.r_type.funct3, (uint8_t)i.r_type.rd, (uint8_t)i.r_type.opcode);
            break;
        case I_TYPE:
            fmt::print("{:012b} {:05b} {:03b} {:05b} {:07b}\n", (uint16_t)i.i_type.imm, (uint8_t)i.i_type.rs1, (uint8_t)i.i_type.funct3, (uint8_t)i.i_type.rd, (uint8_t)i.i_type.opcode);
            break;
        case S_TYPE:
            fmt::print("{:07b} {:05b} {:05b} {:03b} {:05b} {:07b}\n", (uint8_t)i.s_type.imm11_5, (uint8_t)i.s_type.rs2, (uint8_t)i.s_type.rs1, (uint8_t)i.s_type.funct3, (uint8_t)i.s_type.imm4_0, (uint8_t)i.s_type.opcode);
            break;
        case B_TYPE:
            fmt::print("{:01b} {:06b} {:05b} {:05b} {:03b} {:04b} {:01b} {:07b}\n", (uint8_t)i.b_type.imm12, (uint8_t)i.b_type.imm10_5, (uint8_t)i.b_type.rs2, (uint8_t)i.b_type.rs1, (uint8_t)i.b_type.funct3, (uint8_t)i.b_type.imm4_1, (uint8_t)i.b_type.imm11, (uint8_t)i.b_type.opcode);
            break;
        case U_TYPE:
            fmt::print("{:020b} {:05b} {:07b}\n", (uint8_t)i.u_type.imm31_12, (uint8_t)i.u_type.rd, (uint8_t)i.u_type.opcode);
            break;
        case J_TYPE:
            fmt::print("{:01b} {:010b} {:01b} {:08b} {:05b} {:07b}\n", (uint8_t)i.j_type.imm20, (uint8_t)i.j_type.imm10_1, (uint8_t)i.j_type.imm11, (uint8_t)i.j_type.imm19_12, (uint8_t)i.j_type.rd, (uint8_t)i.j_type.opcode);
            break;
    }
}







#define EBREAK 0x00100073
#define ECONT 0x00200073





int main()
{
    std::vector<uint32_t> translated_prog = {
        0xddccc0b7,
        0xbaa08093,
        0x00102823,
        0x00100073,
        0x00000000
    };

    // UINT16_MAX 32-bit words
    std::array<uint8_t, UINT16_MAX * 4> memory = {};

    for (std::size_t i = 0; i < translated_prog.size(); i++)
    {
        component t; t.word = translated_prog[i];
        memory[i * 4 + 0] = t.byte[0];
        memory[i * 4 + 1] = t.byte[1];
        memory[i * 4 + 2] = t.byte[2];
        memory[i * 4 + 3] = t.byte[3];
    }

    // BEGIN INTERPRETATION
    instr test;
    uint32_t gp_regs[32];
    uint32_t pc_reg = 0;

    for (;;)
    {
        component helper;
        helper.byte[0] = memory[pc_reg + 0];
        helper.byte[1] = memory[pc_reg + 1];
        helper.byte[2] = memory[pc_reg + 2];
        helper.byte[3] = memory[pc_reg + 3];
        test.instruction = helper.word;

        switch (test.op_only.opcode)
        {
            case 0b0110011: // Integer ALU R-Type
                switch (test.r_type.funct3)
                {
                    case 0x0: // ADD / SUB
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("ADD Instr\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] + gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            case 0x20:
                                fmt::print("SUB Instr\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] - gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    case 0x4: // XOR (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("XOR Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] ^ gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    case 0x6: // OR (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("OR Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] | gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    case 0x7: // AND (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("AND Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] & gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    case 0x1: // SLL (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SLL Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] << gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    case 0x5: // SRL / SRA (Should be working fine)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SRL Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] >> gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            case 0x20:
                                fmt::print("SRA Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = (int32_t)gp_regs[test.r_type.rs1] >> gp_regs[test.r_type.rs2];
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    case 0x2: // SLT (Set Less Than [signed?]) (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SLT Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = (signed)gp_regs[test.r_type.rs1] < (signed)gp_regs[test.r_type.rs2] ? 1 : 0;
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    case 0x3: // SLTU (Set Less Than Unsigned) (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SLTU Instr (UNTESTED)\n");
                                if (test.r_type.rd != 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] < gp_regs[test.r_type.rs2] ? 1 : 0;
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                        break;
                    default:
                        unimplemented_instr(test, gp_regs, pc_reg);
                        pc_reg += 4;
                        break;
                }
                break;
            case 0b0010011: // Integer ALU I-Type NOTE: Immediates are sign-extended (they're signed by DEFAULT, their MSB gets extended to the left [all ones to the left])
                switch (test.i_type.funct3)
                {
                    case 0x0: // ADDI -- TESTED
                        fmt::print("ADDI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] + sign_extend_imm;
                        }
                        pc_reg += 4;
                        break;
                    case 0x4: // XORI -- TESTED
                        fmt::print("XORI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] ^ sign_extend_imm;
                        }
                        pc_reg += 4;
                        break;
                    case 0x6: // ORI
                        fmt::print("ORI Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] | sign_extend_imm;
                        }
                        pc_reg += 4;
                        break;
                    case 0x7: // ANDI
                        fmt::print("ANDI Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] & sign_extend_imm;
                        }
                        pc_reg += 4;
                        break;
                    case 0x1: // SLLI
                        fmt::print("SLLI Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] << (sign_extend_imm & 0x1F); // first 5 bits
                        }
                        pc_reg += 4;
                        break;
                    case 0x5: // SRLI / SRAI
                        switch ((test.i_type.imm & 0b111111100000) >> 5) // Subdivide the IMM value again with a union?
                        {
                            case 0x00:
                                fmt::print("SRLI Instr (UNTESTED)\n");
                                if (test.i_type.rd != 0)
                                {
                                    int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                                    gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] >> (sign_extend_imm & 0x1F); // Imm [4:0]
                                }
                                pc_reg += 4;
                                break;
                            case 0x20:
                                fmt::print("SRAI Instr (UNTESTED)\n");
                                if (test.i_type.rd != 0)
                                {
                                    int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                                    gp_regs[test.i_type.rd] = (int32_t)gp_regs[test.i_type.rs1] >> (sign_extend_imm & 0x1F);
                                }
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                        }
                    case 0x2: // SLTI (Set Less Than Immediate)
                        fmt::print("SLTI Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = (signed)gp_regs[test.i_type.rs1] < (signed)(test.i_type.imm >> 20) ? 1 : 0;
                        }
                        pc_reg += 4;
                        break;
                    case 0x3: // SLTIU (Set Less Than Immediate Unsigned)
                        fmt::print("SLTIU Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] < (test.i_type.imm >> 20) ? 1 : 0;
                        }
                        pc_reg += 4;
                        break;
                    default:
                        unimplemented_instr(test, gp_regs, pc_reg);
                        pc_reg += 4;
                        break;
                }
                break;
            case 0b0000011: // Integer Load I-Type -- ~~TESTED~~ / Needs retesting, switched to a UNION
                switch (test.i_type.funct3)
                {
                    case 0x0: // LB (Load Byte, sign extended)
                        fmt::print("LB Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            // Need the lowest 8-bits, sign extended, immediate value
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            component t;
                            t.byte[3] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm];
                            gp_regs[test.i_type.rd] = t.word_s >> 24;
                        }
                        pc_reg += 4;
                        break;
                    case 0x1: // LH (Load Half, sign-extended)
                        fmt::print("LH Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            // Need the lowest 16-bits, sign extended, immediate value
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            component t;
                            t.byte[2] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 0];
                            t.byte[3] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 1];
                            gp_regs[test.i_type.rd] = t.word_s >> 16;
                        }
                        pc_reg += 4;
                        break;
                    case 0x2: // LW (Load Word)
                        fmt::print("LW Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            component t;
                            t.byte[0] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 0];
                            t.byte[1] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 1];
                            t.byte[2] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 2];
                            t.byte[3] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 3];
                            gp_regs[test.i_type.rd] = t.word;
                        }
                        pc_reg += 4;
                        break;
                    case 0x4: // LBU (Load Byte Unsigned)
                        fmt::print("LBU Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            gp_regs[test.i_type.rd] = (uint32_t)memory[gp_regs[test.i_type.rs1] + sign_extend_imm];
                        }
                        pc_reg += 4;
                        break;
                    case 0x5: // LHU (Load Half Unsigned)
                        fmt::print("LHU Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            component t;
                            t.byte[0] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 0];
                            t.byte[1] = memory[gp_regs[test.i_type.rs1] + sign_extend_imm + 1];
                            gp_regs[test.i_type.rd] = t.word;
                        }
                        pc_reg += 4;
                        break;
                    default:
                        unimplemented_instr(test, gp_regs, pc_reg);
                        pc_reg += 4;
                        break;
                }
                break;
            case 0b0100011: // Integer Store S-Type
                switch (test.s_type.funct3)
                {
                    case 0x0: // SB
                        fmt::print("SB Instr (UNTESTED)\n");
                        {
                            int32_t sign_extend_imm = (int32_t)((test.s_type.imm11_5 << 5 | test.s_type.imm4_0) << 20) >> 20;
                            component t;
                            t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + sign_extend_imm + 0] = t.byte[0];
                        }
                        pc_reg += 4;
                        break;
                    case 0x1: // SH
                        fmt::print("SH Instr (UNTESTED)\n");
                        {
                            int32_t sign_extend_imm = (int32_t)((test.s_type.imm11_5 << 5 | test.s_type.imm4_0) << 20) >> 20;
                            component t;
                            t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + sign_extend_imm + 0] = t.byte[0];
                            memory[gp_regs[test.s_type.rs1] + sign_extend_imm + 1] = t.byte[1];
                        }
                        pc_reg += 4;
                        break;
                    case 0x2: //SW
                        fmt::print("SW Instr (UNTESTED)\n");
                        {
                            int32_t sign_extend_imm = (int32_t)((test.s_type.imm11_5 << 5 | test.s_type.imm4_0) << 20) >> 20;
                            component t;
                            t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + sign_extend_imm + 0] = t.byte[0];
                            memory[gp_regs[test.s_type.rs1] + sign_extend_imm + 1] = t.byte[1];
                            memory[gp_regs[test.s_type.rs1] + sign_extend_imm + 2] = t.byte[2];
                            memory[gp_regs[test.s_type.rs1] + sign_extend_imm + 3] = t.byte[3];
                        }
                        pc_reg += 4;
                        break;
                    default:
                        unimplemented_instr(test, gp_regs, pc_reg);
                        pc_reg += 4;
                        break;
                }
                break;
            case 0b1100011: // Integer Branch B-Type
                switch (test.b_type.funct3)
                {
                    case 0x0: // BEQ -- Signed or unsigned doesn't matter, since either it's equal or not.
                        fmt::print("BEQ Instr (UNTESTED)\n");
                        if (gp_regs[test.b_type.rs1] == gp_regs[test.b_type.rs2])
                        {
                            imm_reconstruct imm;
                            imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                            imm.word = imm.word << 19;
                            imm.word_s = imm.word_s >> 19;
                            pc_reg += imm.word_s;
                        }
                        else
                        {
                            pc_reg += 4;
                        }
                        break;
                    case 0x1: // BNE -- Signed or unsigned doesn't matter, since either it's equal or not.
                        fmt::print("BNE Instr (UNTESTED)\n");
                        if (gp_regs[test.b_type.rs1] != gp_regs[test.b_type.rs2])
                        {
                            imm_reconstruct imm;
                            imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                            imm.word = imm.word << 19;
                            imm.word_s = imm.word_s >> 19;
                            pc_reg += imm.word_s;
                        }
                        else
                        {
                            pc_reg += 4;
                        }
                        break;
                    case 0x4: // BLT
                        fmt::print("BLT Instr (UNTESTED)\n");
                        if ((signed)gp_regs[test.b_type.rs1] < (signed)gp_regs[test.b_type.rs2])
                        {
                            imm_reconstruct imm;
                            imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                            imm.word = imm.word << 19;
                            imm.word_s = imm.word_s >> 19;
                            pc_reg += imm.word_s;
                        }
                        else
                        {
                            pc_reg += 4;
                        }
                        break;
                    case 0x5: // BGE
                        fmt::print("BGE Instr (UNTESTED)\n");
                        if ((signed)gp_regs[test.b_type.rs1] >= (signed)gp_regs[test.b_type.rs2])
                        {
                            imm_reconstruct imm;
                            imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                            imm.word = imm.word << 19;
                            imm.word_s = imm.word_s >> 19;
                            pc_reg += imm.word_s;
                        }
                        else
                        {
                            pc_reg += 4;
                        }
                        break;
                    case 0x6: // BLTU
                        fmt::print("BLTU Instr (UNTESTED)\n");
                        if (gp_regs[test.b_type.rs1] < gp_regs[test.b_type.rs2])
                        {
                            imm_reconstruct imm;
                            imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                            imm.word = imm.word << 19;
                            imm.word_s = imm.word_s >> 19;
                            pc_reg += imm.word_s;
                        }
                        else
                        {
                            pc_reg += 4;
                        }
                        break;
                    case 0x7: // BGEU
                        fmt::print("BGEU Instr (UNTESTED)\n");
                        if (gp_regs[test.b_type.rs1] >= gp_regs[test.b_type.rs2])
                        {
                            imm_reconstruct imm;
                            imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                            imm.word = imm.word << 19;
                            imm.word_s = imm.word_s >> 19;
                            pc_reg += imm.word_s;
                        }
                        else
                        {
                            pc_reg += 4;
                        }
                        break;
                    default:
                        unimplemented_instr(test, gp_regs, pc_reg);
                        pc_reg += 4;
                        break;
                }
                break;
            case 0b1101111: // Integer JAL J-Type
                fmt::print("JAL Instr (UNTESTED)\n");
                {
                    imm_reconstruct imm;
                    imm.j_imm = {0, test.j_type.imm10_1, test.j_type.imm11, test.j_type.imm19_12, test.j_type.imm20, 0};
                    imm.word = imm.word << 11;
                    imm.word_s = imm.word_s >> 11;
                    if (test.j_type.rd != 0)
                    {
                        gp_regs[test.j_type.rd] = pc_reg + 4;
                    }
                    pc_reg += imm.word_s;
                }
                break;
            case 0b1100111: // Integer JALR I-Type
                switch (test.i_type.funct3)
                {
                    case 0x0:
                        fmt::print("JALR Instr (UNTESTED)\n");
                        {
                            int32_t sign_extend_imm = (int32_t)(test.i_type.imm << 20) >> 20;
                            if (test.i_type.rd != 0)
                            {
                                gp_regs[test.i_type.rd] = pc_reg + 4;
                            }
                            pc_reg = gp_regs[test.i_type.rs1] + sign_extend_imm;
                        }
                        break;
                    default:
                        unimplemented_instr(test, gp_regs, pc_reg);
                        pc_reg += 4;
                        break;
                }
                break;
            case 0b0110111: // Integer LUI U-Type
                fmt::print("LUI Instr (UNTESTED)\n");
                if (test.u_type.rd != 0)
                {
                    gp_regs[test.u_type.rd] = (uint32_t)test.u_type.imm31_12 << 12;
                }
                pc_reg += 4;
                break;
            case 0b0010111: // Integer AUIPC U-Type
                fmt::print("AUIPC Instr (UNTESTED)\n");
                if (test.u_type.rd != 0)
                {
                    gp_regs[test.u_type.rd] = pc_reg + ((uint32_t)test.u_type.imm31_12 << 12);
                }
                pc_reg += 4;
                break;
            case 0b1110011: // Integer ECALL/EBREAK I-Type
                switch (test.i_type.funct3)
                {
                    case 0x0: // Funct3 is always 0x0 in the base instruction set
                        switch (test.i_type.imm)
                        {
                            case 0x0:
                                fmt::print("ECALL Instr, skipping\n");
                                pc_reg += 4;
                                break;
                            case 0x1:
                                fmt::print("EBREAK Instr\n");
                                spit_registers(gp_regs, pc_reg);
                                for (std::size_t i = 0; i < 8; i++)
                                    fmt::print("{}: {:08x}\n", i, (uint32_t)(memory[i*4 + 3] << 24 | memory[i*4 + 2] << 16 | memory[i*4 + 1] << 8 | memory[i*4]));
                                pc_reg += 4;
                                return 0;
                                break;
                            case 0x2: /// NONSTANDARD INSTRUCTION FOR DEBUGGING PURPOSES ONLY
                                fmt::print("ECONT Instr\n");
                                spit_registers(gp_regs, pc_reg);
                                pc_reg += 4;
                                break;
                            default:
                                unimplemented_instr(test, gp_regs, pc_reg);
                                break;
                        }
                        break;
                    default:
                        unimplemented_instr(test, gp_regs, pc_reg);
                        pc_reg += 4;
                        break;
                }
                break;
            default:
                unimplemented_instr(test, gp_regs, pc_reg);
                pc_reg += 4;
                break;
        }
    }
    return 0;
}
