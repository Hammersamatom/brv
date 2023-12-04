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

template <typename T>
class registerFile
{
    private:
        T registers[32] = { 0 };
    public: 
        T* data() {
            return registers;
        }
    public:
        void setReg(uint8_t acc, T in)
        {
            if (acc) registers[acc & 0x1F] = in;
        }

        const T& operator[](uint8_t acc)
        {
            return registers[acc & 0x1F];
        }
};

int main(int argc, char* argv[])
{
    fmt::print("Got {} arguments\n", argc);
    for (std::size_t i = 0; i < argc; i++)
        fmt::print("Argument {}: {}\n", i, argv[i]);

    const uint64_t MEM_MAX = 1 << 23;

    std::unique_ptr<uint8_t[]> memory = std::make_unique<uint8_t[]>(MEM_MAX);
    std::memset(memory.get(), 0, MEM_MAX * sizeof(*memory.get()));

    //uint8_t memory[MEM_MAX] = { 0 };

    //component _ebreak; _ebreak.word = 0x00100073;
    //for(std::size_t m = 0; m < 4; m++) memory[m] = _ebreak.byte[m];
    memory[0] = 0x73;
    memory[2] = 0x10;

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

        if (diff > MEM_MAX)
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
    //uint32_t gp_regs[32] = { 0 };
    registerFile<uint32_t> gp_regs;
    uint32_t pc_reg = 0;

    for (;;)
    {
        component helper;
        helper.byte[0] = memory[pc_reg + 0];
        helper.byte[1] = memory[pc_reg + 1];
        helper.byte[2] = memory[pc_reg + 2];
        helper.byte[3] = memory[pc_reg + 3];
        test.instruction = helper.word;

        uint8_t branched = 0;

        bool rs2_or_imm = helper.word & 0x20;
        uint32_t val = rs2_or_imm ? gp_regs[test.r_type.rs2] : sign_extend(test.i_type.imm, 20);

        imm_reconstruct imm; imm.word = 0;
        component t; t.word = 0;
        uint32_t ls_offset = 0;

        switch (test.op_only.opcode)
        {
            case 0b0110011: // Integer ALU R-Type - Fallthrough
            case 0b0010011: // Integer ALU I-Type NOTE: Immediates are sign-extended (they're signed by DEFAULT, their MSB gets extended to the left [all ones to the left])
                switch (test.i_type.funct3)
                {
                    // ADDI -- TESTED
                    case 0x0: 
                        switch (test.r_type.funct7)
                        {
                            case 0x00: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] + val); break;
                            case 0x20: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] - gp_regs[test.r_type.rs2]); break;
                        }
                        break;
                    // XORI -- TESTED
                    case 0x4: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] ^ val); break;
                    // ORI
                    case 0x6: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] | val); break;
                    // ANDI
                    case 0x7: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] & val); break;
                    // SLLI / first 5 bits
                    case 0x1: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] << (rs2_or_imm ? val : val & 0x1F)); break;
                    // SRLI / SRAI
                    case 0x5:
                        switch ((test.i_type.imm & 0xFE0) >> 5) // Subdivide the IMM value again with a union?
                        {
                            case 0x00: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] >> (rs2_or_imm ? val : val & 0x1F)); break;
                            case 0x20: gp_regs.setReg(test.r_type.rd, (int32_t)gp_regs[test.i_type.rs1] >> (rs2_or_imm ? val : val & 0x1F)); break;
                        }
                        break;
                    // SLTI (Set Less Than Immediate)
                    case 0x2: gp_regs.setReg(test.r_type.rd, (int32_t)gp_regs[test.i_type.rs1] < (signed)val ? 1 : 0); break;
                    // SLTIU (Set Less Than Immediate Unsigned)
                    case 0x3: gp_regs.setReg(test.r_type.rd, gp_regs[test.i_type.rs1] < val ? 1 : 0); break;
                }
                break;
            case 0b0000011: // Integer Load I-Type -- ~~TESTED~~ / Needs retesting, switched to a UNION
                ls_offset = gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20);
                switch (test.i_type.funct3)
                {

                    case 0x1: // LH (Load Half, sign-extended)
                        // Need the lowest 16-bits, sign extended, immediate value
                        t.byte[3] = memory[ls_offset + 1];
                        t.byte[2] = memory[ls_offset + 0];
                        gp_regs.setReg(test.r_type.rd, t.word_s >> 16);
                        break;
                    case 0x0: // LB (Load Byte, sign extended)
                        // Need the lowest 8-bits, sign extended, immediate value
                        t.byte[3] = memory[ls_offset + 0];
                        gp_regs.setReg(test.r_type.rd, t.word_s >> 24);
                        break;
                    case 0x2: // LW (Load Word)
                        t.byte[3] = memory[ls_offset + 3];
                        t.byte[2] = memory[ls_offset + 2];
                    case 0x5: // LHU (Load Half Unsigned)
                        t.byte[1] = memory[ls_offset + 1];
                    case 0x4: // LBU (Load Byte Unsigned)
                        t.byte[0] = memory[ls_offset + 0];
                        gp_regs.setReg(test.r_type.rd, t.word);
                }
                break;
            case 0b0100011: // Integer Store S-Type
                imm.s_imm = {test.s_type.imm4_0, test.s_type.imm11_5, 0};
                t.word = gp_regs[test.s_type.rs2];
                ls_offset = gp_regs[test.s_type.rs1] + sign_extend(imm.word, 20);
                switch (test.s_type.funct3)
                {
                    case 0x2: //SW
                        memory[gp_regs[test.s_type.rs1] + ls_offset + 3] = t.byte[3];
                        memory[gp_regs[test.s_type.rs1] + ls_offset + 2] = t.byte[2];
                    case 0x1: // SH
                        memory[gp_regs[test.s_type.rs1] + ls_offset + 1] = t.byte[1];
                    case 0x0: // SB
                        memory[gp_regs[test.s_type.rs1] + ls_offset + 0] = t.byte[0];
                }
                break;
            case 0b1100011: // Integer Branch B-Type
                imm.b_imm = {0, test.b_type.imm4_1, test.b_type.imm10_5, test.b_type.imm11, test.b_type.imm12, 0};
                switch (test.b_type.funct3)
                {
                    // BEQ -- Signed or unsigned doesn't matter, since either it's equal or not.
                    case 0x0: if (gp_regs[test.b_type.rs1] == gp_regs[test.b_type.rs2]) branched = 1; break;
                    // BNE -- Signed or unsigned doesn't matter, since either it's equal or not.
                    case 0x1: if (gp_regs[test.b_type.rs1] != gp_regs[test.b_type.rs2]) branched = 1; break;
                    // BLT
                    case 0x4: if ((signed)gp_regs[test.b_type.rs1] < (signed)gp_regs[test.b_type.rs2]) branched = 1; break;
                    // BGE
                    case 0x5: if ((signed)gp_regs[test.b_type.rs1] >= (signed)gp_regs[test.b_type.rs2]) branched = 1; break;
                    // BLTU
                    case 0x6: if (gp_regs[test.b_type.rs1] < gp_regs[test.b_type.rs2]) branched = 1; break;
                    // BGEU
                    case 0x7: if (gp_regs[test.b_type.rs1] >= gp_regs[test.b_type.rs2]) branched = 1; break;
                }
                if (branched) pc_reg += sign_extend(imm.word, 19);
                break;
            case 0b1101111: // Integer JAL J-Type
                {
                    imm.j_imm = {0, test.j_type.imm10_1, test.j_type.imm11, test.j_type.imm19_12, test.j_type.imm20, 0};
                    gp_regs.setReg(test.j_type.rd, pc_reg + 4);
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
                            gp_regs.setReg(test.r_type.rd, pc_reg + 4);
                            branched = true;
                            //pc_reg = gp_regs[test.i_type.rs1] + sign_extend(test.i_type.imm, 20);
                            pc_reg = gp_regs[test.i_type.rs1] + ((int32_t)test.instruction >> 20);
                        }
                        break;
                }
                break;
            // Integer LUI U-Type
            case 0b0110111: gp_regs.setReg(test.u_type.rd, (uint32_t)test.u_type.imm31_12 << 12); break;
            // Integer AUIPC U-Type
            case 0b0010111: gp_regs.setReg(test.u_type.rd, pc_reg + ((uint32_t)test.u_type.imm31_12 << 12)); break;
            case 0b1110011: // Integer ECALL/EBREAK I-Type
                switch (test.i_type.funct3)
                {
                    case 0x0: // Funct3 is always 0x0 in the base instruction set
                        switch (test.i_type.imm)
                        {
                            // ECALL
                            case 0x0: break;
                            // EBREAK
                            case 0x1: spit_registers(gp_regs.data(), pc_reg); return 0; break;
                        }
                        break;
                }
                break;
        }

        // Reset zero/x0 register to 0. Prevent branching.
        //gp_regs[0] = 0;

        // Increment PC if we haven't branched/jumped
        if (!branched)
            pc_reg += 4;

    }

    return 0;
}