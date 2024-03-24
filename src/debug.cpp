#include <string>
#include <iostream>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "debug.hpp"
#include "unions.hpp"

void unimplemented_instr(instr unimp, uint32_t* regs, uint32_t pc)
{
    fmt::print("! UNIMPLEMENTED INSTRUCTION !\nHEX: {:08x}\n> Opcode: {:07b}\n", unimp.instruction, (uint8_t)unimp.op_only.opcode);
    spit_registers(regs, pc);
    std::cin.get();
}

void spit_registers(uint32_t* regs, uint32_t pc)
{
    for (uint32_t c = 0; c < 32; c++)
        fmt::print("> x{0:02}: {1:032b} {1:08x} {1} {2}\n", c, regs[c], (int32_t)regs[c]);
    fmt::print("> PCR: {}\n", pc);
}

std::string spit_registers_json(const uint32_t* regs, const uint32_t& pc)
{
    // Create DOM instance
    nlohmann::ordered_json json;
    // For loop, set key to x# and value to register value
    // Push the two into the json
    for (uint32_t i = 0; i < 32; i++)
        json.emplace("x"+ std::to_string(i), std::to_string(regs[i]));
    // Make a key value pair of the PC Register and it's contents
    json.emplace("PCR", std::to_string(pc));
    // Pretty print it
    return json.dump(4);
}