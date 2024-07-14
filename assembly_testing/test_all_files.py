import json as js;
import sys;
import subprocess;

BITS = "32";
ARCHITECTURE = "riscv" + BITS;
SUBSET = "rv" + BITS + "i";
ABI = "ilp32"

TEST_FILE_NAME = "test"; # The name of the assembly file to assemble and run
RESULT_FILE_NAME = "expect_results"; # The name of the json file with the expect results, fail if they don't match
LINKER_SCRIPT = "linker.ld";

#build_command = "clang --target=" + ARCHITECTURE + " -march=" + SUBSET + " " + TEST_FILE_NAME + ".s -c -o obj.o -nostdlib"; # Compile to object file
#link_command  = "riscv32-elf-ld --script=" + LINKER_SCRIPT + "obj.o -o linked.bin"; # Link it
#strip_command = "riscv32-elf-objcopy -O binary linked.bin " + TEST_FILE_NAME + ".stripped"; # Strip out everything but the actual instructions

def compile_asm(path: str):
    build_output = subprocess.run(
        [
            "clang",
            "--target=" + ARCHITECTURE,
            "-march=" + SUBSET,
            path + TEST_FILE_NAME + ".s",
            "-c",
            "-o",
            path + "obj.o",
            "-nostdlib"
        ]
    );
    linker_output = subprocess.run(
        [
            "riscv32-elf-ld",
            "--script=" + path + LINKER_SCRIPT,
            path + "obj.o",
            "-o",
            path + "linked.bin"
        ]
    );
    strip_output = subprocess.run(
        [
            "riscv32-elf-objcopy",
            "-O",
            "binary",
            path + "linked.bin",
            path + TEST_FILE_NAME + ".stripped"
        ]
    );

tests = [
    "upper_imm_inst/lui",
    "upper_imm_inst/auipc",

    "imm_alu_inst/addi",
    "imm_alu_inst/xori",
    "imm_ali_inst/andi",
    "imm_alu_inst/ori",
    "imm_alu_inst/slti",
    "imm_alu_inst/sltiu",
    "imm_alu_inst/slli",
    "imm_alu_inst/srli",
    "imm_alu_inst/srai",

    "reg_alu_inst/add",
    "reg_alu_inst/sub",
    "reg_alu_inst/xor",
    "reg_alu_inst/and",
    "reg_alu_inst/or",
    "reg_alu_inst/slt",
    "reg_alu_inst/sltu",
    "reg_alu_inst/sll",
    "reg_alu_inst/srl",
    "reg_alu_inst/sra",

    "load_inst/lb",
    "load_inst/lbu",
    "load_inst/lh",
    "load_inst/lhu",
    "load_inst/lw",

    "branch_inst/beq",
    "branch_inst/bge",
    "branch_inst/bgeu",
    "branch_inst/blt",
    "branch_inst/bltu",
    "branch_inst/bne",
];