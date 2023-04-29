import json as js;

BITS = "32";
ARCHITECTURE = "riscv" + BITS;
SUBSET = "rv" + BITS + "i";
ABI = "ilp32"

TEST_FILE_NAME = "test"; # The name of the assembly file to assemble and run
RESULT_FILE_NAME = "expect_results"; # The name of the json file with the expect results, fail if they don't match
LINKER_SCRIPT = "linker.ld";

clang_build_command = "clang --target=" + ARCHITECTURE + " -march=" + SUBSET + " " + TEST_FILE_NAME + ".s -c -o obj.o -nostdlib"; # Compile to object file
clang_link_command  = "ld.lld --script=" + LINKER_SCRIPT + "obj.o -o linked.bin"; # Link it
clang_strip_command = "llvm-objcopy-10 -O binary linked.bin " + TEST_FILE_NAME + ".stripped"; # Strip out everything but the actual instructions

gcc_build_command = "riscv64-unknown-elf-gcc -march=" + SUBSET + " -mabi=" + ABI + " -g -ffreestanding -O0 -Wl,--gc-sections -nostartfiles -nostdlib -nodefaultlibs -Wl,-T," + LINKER_SCRIPT + " " + TEST_FILE_NAME + ".s"
gcc_strip_command = "riscv64-unknown-elf-objcopy -O binary a.out " + TEST_FILE_NAME + ".stripped"

































if __name__ == "__main__":
    main();