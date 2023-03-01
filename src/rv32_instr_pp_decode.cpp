#include <cstdint>
#include <string>
#include <fmt/core.h>

#include "unions.hpp"

#include "rv32_instr_pp_decode.hpp"

void print_pretty_instr(INSTR_TYPE type, instr i)
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

// TODO: Finished implementation of decoding. Only ALU R and I type instructions from RV32I are handled atm
void rv_pp_decode(const uint32_t& word, REG_TYPE reg_type = X_TYPE)
{
    instr i; i.instruction = word;
    switch (i.op_only.opcode)
    {
        case 0b0110011: // ALU R-Type - Fallthrough case
        case 0b0010011: // ALU I-Type
        {
            bool rs2_or_imm = word & 0x20; // True for rs2, false for immediate value

            uint8_t rd = i.r_type.rd;
            uint8_t rs1 = i.r_type.rs1;
            uint8_t rs2 = i.r_type.rs2;
            int32_t imm = word >> 20;

            std::string repl_text = "{} {}{}, {}{}, {}{}\n";
            std::string instr;

            // r_type is used since it provides easy access to funct7, even if this is the ALU I-Type case
            // Regardless, funct3 is located in the same place in both r_type and i_type.
            switch (i.r_type.funct3)
            {
                case 0x0: instr = rs2_or_imm ? ((i.r_type.funct7 == 0x20) ? "SUB" : "ADD") : "ADDI"; break; // ADD/I + SUB
                case 0x4: instr = rs2_or_imm ? "XOR" : "XORI"; break; // XOR/I
                case 0x6: instr = rs2_or_imm ? "OR" : "ORI"; break; // OR/I
                case 0x7: instr = rs2_or_imm ? "AND" : "ANDI"; break; // AND/I
                case 0x1: instr = rs2_or_imm ? "SLL" : "SLLI"; break; // SLL/I
                case 0x5: instr = rs2_or_imm ? ((i.r_type.funct7 == 0x20) ? "SRA" : "SRL") : ((i.r_type.funct7 == 0x20) ? "SRAI" : "SRLI"); break; // SRA/I + SRL/I
                case 0x2: instr = rs2_or_imm ? "SLT" : "SLTI"; break; // SLT/I
                case 0x3: instr = rs2_or_imm ? "SLTU" : "SLTIU"; break; // SLTU/SLTIU
                default:  instr = "UNK"; break;
            }

            // FIXME: There has to be a better way to do this...
            fmt::print(repl_text,
                       instr,
                       reg_type ? "" : "x",
                       reg_type ? abi_names[rd] : std::to_string(rd),
                       reg_type ? "" : "x",
                       reg_type ? abi_names[rs1] : std::to_string(rs1),
                       rs2_or_imm ? "x" : "",
                       rs2_or_imm ? (reg_type ? abi_names[rs2] : std::to_string(rs2)) : ((i.r_type.funct3 == 0x5) ? std::to_string(imm & 0x1F) : std::to_string(imm)));
            break;
        }
        case 0b0000011: // Integer Load I-Type
        case 0b0100011: // Integer Store S-Type
        case 0b1100011: // Integer Branch B-Type
        case 0b1101111: // Integer JAL J-Type
        case 0b1100111: // Integer JALR I-Type
        case 0b0110111: // Integer LUI U-Type
        case 0b0010111: // Integer AUIPC U-Type
        case 0b1110011: // Integer ECALL/EBREAK I-Type
        default:
            break;
    }
}
