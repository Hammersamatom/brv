#include <string>

void spit_registers(uint32_t* regs, uint32_t pc);
std::string spit_registers_json(const uint32_t* regs, const uint32_t& pc);