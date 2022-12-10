#include <cstdint>

union r_type {
    uint32_t original_t;
    struct breakdown_t {
        uint8_t funct7 : 7;
        uint8_t rs2 : 5;
        uint8_t rs1 : 5;
        uint8_t funct3 : 3;
        uint8_t rd : 5;
        uint8_t opcode : 7;
    } broken_down;
};