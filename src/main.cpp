#include <cstdint>
#include <array>
#include <fmt/core.h>

union instr
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
};

constexpr size_t tech = sizeof(instr);

void unimplemented_instr(instr unimp)
{
    fmt::print("! UNIMPLEMENTED INSTRUCTION !\n");
    fmt::print("HEX: {:08x}\n", unimp.instruction);
    fmt::print("> Opcode: {:07b}\n", (uint8_t)unimp.op_only.opcode);
}

int main()
{
    instr test;

    std::array<uint32_t, 3> rom_code = {
        0x06408093, // ADDI x1, x1, 100
        0x0c810113, // ADDI x2, x2, 200
        0x001101b3 // ADD  x3, x2, x1
    };

    uint32_t gp_regs[32];
    uint32_t pc_reg = 0;

    for (uint32_t i = 0; i < rom_code.size(); i++)
    {
        // test.instruction = (rom_code[pc_reg + 3] << 24) | (rom_code[pc_reg + 2]
        // << 16) | (rom_code[pc_reg + 1] << 8) | (rom_code[pc_reg]);
        test.instruction = rom_code[i]; // pc_reg];
        switch (test.op_only.opcode)
        {
            case 0b0110011: // Integer ALU R-Type
                switch (test.r_type.funct3)
                {
                    case 0x0: // ADD / SUB
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("ADD instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] + gp_regs[test.r_type.rs2];
                                }
                                break;
                            case 0x20:
                                fmt::print("SUB instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] - gp_regs[test.r_type.rs2];
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    case 0x4: // XOR (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("XOR instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] ^ gp_regs[test.r_type.rs2];
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    case 0x6: // OR (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("OR instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] | gp_regs[test.r_type.rs2];
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    case 0x7: // AND (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("AND instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] & gp_regs[test.r_type.rs2];
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    case 0x1: // SLL (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SLL instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] << gp_regs[test.r_type.rs2];
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    case 0x5: // SRL / SRA (Should be working fine)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SRL instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] >> gp_regs[test.r_type.rs2];
                                }
                                break;
                            case 0x20:
                                fmt::print("SRA instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = (int32_t)gp_regs[test.r_type.rs1] >> gp_regs[test.r_type.rs2];
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    case 0x2: // SLT (Set Less Than [signed?]) (RV32I only has one
                        // instruction
                        // for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SLT instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = (signed)gp_regs[test.r_type.rs1] < (signed)gp_regs[test.r_type.rs2] ? 1 : 0;
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    case 0x3: // SLTU (Set Less Than Unsigned) (RV32I only has one instruction for funct3)
                        switch (test.r_type.funct7)
                        {
                            case 0x00:
                                fmt::print("SLTU instr\n");
                                if (test.r_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.r_type.rd] = gp_regs[test.r_type.rs1] < gp_regs[test.r_type.rs2] ? 1 : 0;
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                        break;
                    default:
                        unimplemented_instr(test);
                        break;
                }
                break;
            case 0b0010011: // Integer ALU I-Type -- NOTE: Immediates are
                // sign-extended
                // (they're signed by DEFAULT, their MSB gets extended to
                // the left [all ones to the left])
                switch (test.i_type.funct3)
                {
                    case 0x0: // ADDI
                        fmt::print("ADDI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] + (test.i_type.imm >> 20);
                        }
                        break;
                    case 0x4: // XORI
                        fmt::print("XORI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] ^ (test.i_type.imm >> 20);
                        }
                        break;
                    case 0x6: // ORI
                        fmt::print("ORI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] | (test.i_type.imm >> 20);
                        }
                        break;
                    case 0x7: // ANDI
                        fmt::print("ANDI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] & (test.i_type.imm >> 20);
                        }
                        break;
                    case 0x1: // SLLI -- NOTE: TEST
                        fmt::print("SLLI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] << ((test.i_type.imm >> 20) & 0b11111);
                        }
                        break;
                    case 0x5: // SRLI / SRAI -- NOTE: TEST
                        switch ((test.i_type.imm & 0b111111100000) >> 5) // Subdivide the IMM value again with a union?
                        {
                            case 0x00:
                                fmt::print("SRLI instr\n");
                                if (test.i_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] >> (test.i_type.imm & 0b0000000111111);
                                }
                                break;
                            case 0x20:
                                fmt::print("SRAI instr\n");
                                if (test.i_type.rd != 0) // Prevent write to x0 (AKA, hardwire 0)
                                {
                                    gp_regs[test.i_type.rd] = (int32_t)gp_regs[test.i_type.rs1] >> (test.i_type.imm & 0b0000000111111);
                                }
                                break;
                            default:
                                unimplemented_instr(test);
                                break;
                        }
                    case 0x2: // SLTI (Set Less Than Immediate) -- NOTE: TEST
                        fmt::print("SLTI Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = (signed)gp_regs[test.i_type.rs1] < (signed)(test.i_type.imm >> 20) ? 1 : 0;
                        }
                        break;
                    case 0x3: // SLTIU (Set Less Than Immediate Unsigned) -- NOTE: TEST
                        fmt::print("SLTIU Instr\n");
                        if (test.i_type.rd != 0)
                        {
                            gp_regs[test.i_type.rd] = gp_regs[test.i_type.rs1] < (test.i_type.imm >> 20) ? 1 : 0;
                        }
                        break;
                    default:
                        unimplemented_instr(test);
                        break;
                }
                break;
            case 0b0000011: // Integer Load I-Type
                break;
            case 0b0100011: // Integer Store S-Type
                break;
            case 0b1100011: // Integer Branch B-Type
                break;
            case 0b1101111: // Integer JAL J-Type
                break;
            case 0b1100111: // Integer JALR I-Type
                break;
            case 0b0110111: // Integer LUI U-Type
                break;
            case 0b0010111: // Integer AUIPC U-Type
                break;
            case 0b1110011: // Integer ECALL/EBREAK I-Type
                break;
            default:
                unimplemented_instr(test);
                break;
        }
    }
    return 0;
}
