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
        int32_t t = *(int32_t*)&regs[i]; // Evil recasting / FIXME: Find another way
        val.SetString(std::to_string(t).c_str(), d.GetAllocator());
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

inline int32_t sign_extend(uint32_t a, uint8_t shift)
{
    return (int32_t)(a << shift) >> shift;
}

int main(int argc, char* argv[])
{
    fmt::print("Got {} arguments\n", argc);
    for (std::size_t i = 0; i < argc; i++)
        fmt::print("Argument {}: {}\n", i, argv[i]);

    auto mem_power_calc = [](uint64_t base, uint64_t exponent) constexpr {
        uint64_t x = 1;
        for (uint64_t i = 0; i < exponent; i++)
        {
            x *= base;
        }
        return x;
    };
    const uint64_t MEM_MAX = mem_power_calc(2, 23);

    std::unique_ptr<uint8_t[]> memory = std::make_unique<uint8_t[]>(MEM_MAX);
    std::memset(memory.get(), 0, MEM_MAX * sizeof(*memory.get()));

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
    uint32_t gp_regs[32] = { 0 };
    uint32_t pc_reg = 0;

    for (;;)
    {
        component helper;
        helper.byte[0] = memory[pc_reg + 0];
        helper.byte[1] = memory[pc_reg + 1];
        helper.byte[2] = memory[pc_reg + 2];
        helper.byte[3] = memory[pc_reg + 3];
        test.instruction = helper.word;

        bool branched = false;

        imm_reconstruct imm; imm.word = 0;
        component t; t.word = 0;

        switch (test.op_only.opcode)
        {
            case 0b0110011: // Integer ALU R-Type
                switch (test.r_type.funct3)
                {
                    case 0x0: // ADD / SUB
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] + gp_regs[test.r_type.rs2]; break;
                            case 0x20: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] - gp_regs[test.r_type.rs2]; break;
                        }
                        break;
                    case 0x4: // XOR (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] ^ gp_regs[test.r_type.rs2]; break;
                        }
                        break;
                    case 0x6: // OR (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] | gp_regs[test.r_type.rs2]; break;
                        }
                        break;
                    case 0x7: // AND (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] & gp_regs[test.r_type.rs2]; break;
                        }
                        break;
                    case 0x1: // SLL (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] << gp_regs[test.r_type.rs2]; break;
                        }
                        break;
                    case 0x5: // SRL / SRA (Should be working fine)
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] >> gp_regs[test.r_type.rs2]; break;
                            case 0x20: gp_regs[test.r_type.rd] = (int32_t)gp_regs[test.r_type.rs1] >> gp_regs[test.r_type.rs2]; break;
                        }
                        break;
                    case 0x2: // SLT (Set Less Than [signed?]) (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = (int32_t)gp_regs[test.r_type.rs1] < (int32_t)gp_regs[test.r_type.rs2] ? 1 : 0; break;
                        }
                        break;
                    case 0x3: // SLTU (Set Less Than Unsigned) (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] < gp_regs[test.r_type.rs2] ? 1 : 0; break;
                        }
                        break;
                }
                break;
            case 0b0010011: // Integer ALU I-Type NOTE: Immediates are sign-extended (they're signed by DEFAULT, their MSB gets extended to the left [all ones to the left])
                switch (test.i_type.funct3)
                {
                    // ADDI -- TESTED
                    case 0x0: gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20); break;
                    // XORI -- TESTED
                    case 0x4: gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] ^ sign_extend(test.i_type.imm, 20); break;
                    // ORI
                    case 0x6: gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] | sign_extend(test.i_type.imm, 20); break;
                    // ANDI
                    case 0x7: gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] & sign_extend(test.i_type.imm, 20); break;
                    // SLLI / first 5 bits
                    case 0x1: gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] << (sign_extend(test.i_type.imm, 20) & 0x1F); break;
                    // SRLI / SRAI
                    case 0x5:
                        switch ((test.i_type.imm & 0b111111100000) >> 5) // Subdivide the IMM value again with a union?
                        {
                            case 0x00: gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] >> (sign_extend(test.i_type.imm, 20) & 0x1F); break;
                            case 0x20: gp_regs[test.i_type.rd] = (int32_t)gp_regs[test.i_type.rs1] >> (sign_extend(test.i_type.imm, 20) & 0x1F); break;
                        }
                    // SLTI (Set Less Than Immediate)
                    case 0x2: gp_regs[test.i_type.rd] = (int32_t)gp_regs[test.i_type.rs1] < (int32_t)(test.i_type.imm >> 20) ? 1 : 0; break;
                    // SLTIU (Set Less Than Immediate Unsigned)
                    case 0x3: gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] < (test.i_type.imm >> 20) ? 1 : 0; break;
                }
                break;
            case 0b0000011: // Integer Load I-Type -- ~~TESTED~~ / Needs retesting, switched to a UNION
                switch (test.i_type.funct3)
                {
                    case 0x0: // LB (Load Byte, sign extended)
                        // Need the lowest 8-bits, sign extended, immediate value
                        t.byte[3] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 0];
                        gp_regs[test.i_type.rd] = t.word_s >> 24;
                        break;
                    case 0x1: // LH (Load Half, sign-extended)
                        // Need the lowest 16-bits, sign extended, immediate value
                        t.byte[2] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 0];
                        t.byte[3] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 1];
                        gp_regs[test.i_type.rd] = t.word_s >> 16;
                        break;
                    case 0x2: // LW (Load Word)
                        t.byte[0] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 0];
                        t.byte[1] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 1];
                        t.byte[2] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 2];
                        t.byte[3] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 3];
                        gp_regs[test.i_type.rd] = t.word;
                        break;
                    case 0x4: // LBU (Load Byte Unsigned)
                        gp_regs[test.i_type.rd] = (uint32_t)memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20)];
                        break;
                    case 0x5: // LHU (Load Half Unsigned)
                        t.byte[0] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 0];
                        t.byte[1] = memory[gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20) + 1];
                        gp_regs[test.i_type.rd] = t.word;
                        break;
                }
                break;
            case 0b0100011: // Integer Store S-Type
                switch (test.s_type.funct3)
                {
                    case 0x0: // SB
                        {
                            imm.s_imm = {test.s_type.imm4_0, test.s_type.imm11_5, 0};
                            imm.word = imm.word << 20;
                            imm.word_s = imm.word_s >> 20;
                            t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 0] = t.byte[0];
                        }
                        break;
                    case 0x1: // SH
                        {
                            imm.s_imm = {test.s_type.imm4_0, test.s_type.imm11_5, 0};
                            imm.word = imm.word << 20;
                            imm.word_s = imm.word_s >> 20;
                            t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 0] = t.byte[0];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 1] = t.byte[1];
                        }
                        break;
                    case 0x2: //SW
                        {
                            imm.s_imm = {test.s_type.imm4_0, test.s_type.imm11_5, 0};
                            imm.word = imm.word << 20;
                            imm.word_s = imm.word_s >> 20;
                            t.word = gp_regs[test.s_type.rs2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 0] = t.byte[0];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 1] = t.byte[1];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 2] = t.byte[2];
                            memory[gp_regs[test.s_type.rs1] + imm.word_s + 3] = t.byte[3];
                        }
                        break;
                }
                break;
            case 0b1100011: // Integer Branch B-Type
                imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                switch (test.b_type.funct3)
                {
                    case 0x0: // BEQ -- Signed or unsigned doesn't matter, since either it's equal or not.
                        if (gp_regs[test.b_type.rs1] == gp_regs[test.b_type.rs2])
                        {
                            branched = true;
                            pc_reg += sign_extend(imm.word, 19);
                        }
                        break;
                    case 0x1: // BNE -- Signed or unsigned doesn't matter, since either it's equal or not.
                        if (gp_regs[test.b_type.rs1] != gp_regs[test.b_type.rs2])
                        {
                            branched = true;
                            pc_reg += sign_extend(imm.word, 19);
                        }
                        break;
                    case 0x4: // BLT
                        if ((signed)gp_regs[test.b_type.rs1] < (signed)gp_regs[test.b_type.rs2])
                        {
                            branched = true;
                            pc_reg += sign_extend(imm.word, 19);
                        }
                        break;
                    case 0x5: // BGE
                        if ((signed)gp_regs[test.b_type.rs1] >= (signed)gp_regs[test.b_type.rs2])
                        {
                            branched = true;
                            pc_reg += sign_extend(imm.word, 19);
                        }
                        break;
                    case 0x6: // BLTU
                        if (gp_regs[test.b_type.rs1] < gp_regs[test.b_type.rs2])
                        {
                            branched = true;
                            pc_reg += sign_extend(imm.word, 19);
                        }
                        break;
                    case 0x7: // BGEU
                        if (gp_regs[test.b_type.rs1] >= gp_regs[test.b_type.rs2])
                        {
                            branched = true;
                            pc_reg += sign_extend(imm.word, 19);
                        }
                        break;
                }
                break;
            case 0b1101111: // Integer JAL J-Type
                {
                    imm.j_imm = {0, test.j_type.imm10_1, test.j_type.imm11, test.j_type.imm19_12, test.j_type.imm20, 0};
                    gp_regs[test.j_type.rd] = pc_reg + 4;
                    branched = true;
                    pc_reg += sign_extend(imm.word, 11);
                }
                break;
            // Integer JALR I-Type
            case 0b1100111:
                switch (test.i_type.funct3)
                {
                    case 0x0:
                        {
                            gp_regs[test.i_type.rd] = pc_reg + 4;
                            branched = true;
                            pc_reg = gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20);
                        }
                        break;
                }
                break;
            // Integer LUI U-Type
            case 0b0110111: if (test.u_type.rd != 0) gp_regs[test.u_type.rd] = (uint32_t)test.u_type.imm31_12 << 12; break;
            // Integer AUIPC U-Type
            case 0b0010111: if (test.u_type.rd != 0) gp_regs[test.u_type.rd] = pc_reg + ((uint32_t)test.u_type.imm31_12 << 12); break;
            case 0b1110011: // Integer ECALL/EBREAK I-Type
                switch (test.i_type.funct3)
                {
                    case 0x0: // Funct3 is always 0x0 in the base instruction set
                        switch (test.i_type.imm)
                        {
                            // ECALL
                            case 0x0: break;
                            // EBREAK
                            case 0x1: spit_registers(gp_regs, pc_reg); fmt::print("{}\n", spit_registers_json(gp_regs, pc_reg)); return 0; break;
                        }
                        break;
                }
                break;
        }

        // Reset zero/x0 register to 0. Prevent branching.
        gp_regs[0] = 0;

        // Increment PC if we haven't branched/jumped
        if (!branched)
            pc_reg += 4;

    }

    return 0;
}
