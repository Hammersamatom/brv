#include <cstdint>
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

#include <fmt/core.h>

// Rapidjson stuff for exporting registers
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#include "unions.hpp"

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

std::string spit_registers_json(const uint32_t* regs, const uint32_t& pc)
{
    rapidjson::Document d; d.SetObject();
    rapidjson::Value key(rapidjson::kObjectType), val(rapidjson::kObjectType);

    for (uint32_t i = 0; i < 32; i++)
    {
        key.SetString(std::string("x" + std::to_string(i)).c_str(), d.GetAllocator());
        val.SetString(std::to_string(regs[i]).c_str(), d.GetAllocator());
        d.AddMember(key, val, d.GetAllocator());
    }

    key.SetString("PCR", d.GetAllocator());
    val.SetString(std::to_string(pc).c_str(), d.GetAllocator());
    d.AddMember(key, val, d.GetAllocator());

    rapidjson::StringBuffer strbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
    d.Accept(writer);

    return std::string(strbuf.GetString());
}

int main(int argc, char* argv[])
{
    fmt::print("Got {} arguments\n", argc);
    for (std::size_t i = 0; i < argc; i++)
        fmt::print("Argument {}: {}\n", i, argv[i]);

    // UINT16_MAX 32-bit words
    std::array<uint8_t, UINT16_MAX * 4> memory;
    memory.fill(0);
    component _ebreak; _ebreak.word = 0x00100073;
    for(std::size_t m = 0; m < 4; m++) memory[m] = _ebreak.byte[m];

    std::ifstream binary;
    if (argc > 1)
    {
        binary.open(argv[1], std::ios::in | std::ios::binary | std::ios::ate);

        if (!binary.is_open())
        {
            fmt::print("File does not exist or could not be opened\n");
            return 0;
        }

        std::streampos diff = binary.tellg();
        binary.seekg(0);

        if (diff > UINT16_MAX * 4)
        {
            fmt::print("Input file size larger than RISCV memory\n");
            return 0;
        }

        std::unique_ptr<char[]> filebuff = std::make_unique<char[]>(diff);;
        std::memset(filebuff.get(), 0, diff * sizeof(*filebuff.get()));
        binary.read(filebuff.get(), diff);
        binary.close();

        for (std::size_t f = 0; f < diff; f++)
            memory[f] = filebuff[f];
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
                                    gp_regs[test.r_type.rd] = (int32_t)gp_regs[test.r_type.rs1] < (int32_t)gp_regs[test.r_type.rs2] ? 1 : 0;
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
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] + imm.word_s;
                        }
                        pc_reg += 4;
                        break;
                    case 0x4: // XORI -- TESTED
                        fmt::print("XORI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] ^ imm.word_s;
                        }
                        pc_reg += 4;
                        break;
                    case 0x6: // ORI
                        fmt::print("ORI Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] | imm.word_s;
                        }
                        pc_reg += 4;
                        break;
                    case 0x7: // ANDI
                        fmt::print("ANDI Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] & imm.word_s;
                        }
                        pc_reg += 4;
                        break;
                    case 0x1: // SLLI
                        fmt::print("SLLI Instr (UNTESTED)\n");
                        if (test.i_type.rd != 0)
                        {
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] << (imm.word_s & 0x1F); // first 5 bits
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
                                    imm_reconstruct imm;
                                    imm.word = test.i_type.imm << 20;
                                    imm.word_s = imm.word_s >> 20;
                                    gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] >> (imm.word_s & 0x1F); // Imm [4:0]
                                }
                                pc_reg += 4;
                                break;
                            case 0x20:
                                fmt::print("SRAI Instr (UNTESTED)\n");
                                if (test.i_type.rd != 0)
                                {
                                    imm_reconstruct imm;
                                    imm.word = test.i_type.imm << 20;
                                    imm.word_s = imm.word_s >> 20;
                                    gp_regs[test.i_type.rd] = (int32_t)gp_regs[test.i_type.rs1] >> (imm.word_s & 0x1F);
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
                            gp_regs[test.i_type.rd] = (int32_t)gp_regs[test.i_type.rs1] < (int32_t)(test.i_type.imm >> 20) ? 1 : 0;
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
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            component t;
                            t.byte[3] = memory[gp_regs[test.i_type.rs1] + imm.word_s];
                            gp_regs[test.i_type.rd] = t.word_s >> 24;
                        }
                        pc_reg += 4;
                        break;
                    case 0x1: // LH (Load Half, sign-extended)
                        fmt::print("LH Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            // Need the lowest 16-bits, sign extended, immediate value
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            component t;
                            t.byte[2] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 0];
                            t.byte[3] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 1];
                            gp_regs[test.i_type.rd] = t.word_s >> 16;
                        }
                        pc_reg += 4;
                        break;
                    case 0x2: // LW (Load Word)
                        fmt::print("LW Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            component t;
                            t.byte[0] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 0];
                            t.byte[1] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 1];
                            t.byte[2] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 2];
                            t.byte[3] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 3];
                            gp_regs[test.i_type.rd] = t.word;
                        }
                        pc_reg += 4;
                        break;
                    case 0x4: // LBU (Load Byte Unsigned)
                        fmt::print("LBU Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            gp_regs[test.i_type.rd] = (uint32_t)memory[gp_regs[test.i_type.rs1] + imm.word_s];
                        }
                        pc_reg += 4;
                        break;
                    case 0x5: // LHU (Load Half Unsigned)
                        fmt::print("LHU Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            imm_reconstruct imm;
                            imm.word = test.i_type.imm << 20;
                            imm.word_s = imm.word_s >> 20;
                            component t;
                            t.byte[0] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 0];
                            t.byte[1] = memory[gp_regs[test.i_type.rs1] + imm.word_s + 1];
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
                            imm_reconstruct imm;
                            imm.s_imm = {test.s_type.imm4_0, test.s_type.imm11_5, 0};
                            imm.word = imm.word << 20;
                            imm.word_s = imm.word_s >> 20;
                            component t; t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 0] = t.byte[0];
                        }
                        pc_reg += 4;
                        break;
                    case 0x1: // SH
                        fmt::print("SH Instr (UNTESTED)\n");
                        {
                            imm_reconstruct imm;
                            imm.s_imm = {test.s_type.imm4_0, test.s_type.imm11_5, 0};
                            imm.word = imm.word << 20;
                            imm.word_s = imm.word_s >> 20;
                            component t; t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 0] = t.byte[0];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 1] = t.byte[1];
                        }
                        pc_reg += 4;
                        break;
                    case 0x2: //SW
                        fmt::print("SW Instr (UNTESTED)\n");
                        {
                            imm_reconstruct imm;
                            imm.s_imm = {test.s_type.imm4_0, test.s_type.imm11_5, 0};
                            imm.word = imm.word << 20;
                            imm.word_s = imm.word_s >> 20;
                            component t; t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 0] = t.byte[0];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 1] = t.byte[1];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 2] = t.byte[2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 3] = t.byte[3];
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
                                pc_reg += 4;
                                spit_registers(gp_regs, pc_reg);
                                fmt::print("{}\n", spit_registers_json(gp_regs, pc_reg));
                                return 0;
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
            default:
                unimplemented_instr(test, gp_regs, pc_reg);
                pc_reg += 4;
                break;
        }
    }
    return 0;
}
