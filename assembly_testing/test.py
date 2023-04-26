import json as js;

BITS = "32";
ARCHITECTURE = "riscv" + BITS;
SUBSET = "rv" + BITS + "i";

TEST_FILE_NAME = "test"; # The name of the assembly file to assemble and run
RESULT_FILE_NAME = "expect_results"; # The name of the json file with the expect results, fail if they don't match
LINKER_SCRIPT = "linker.ld";

build_command = "clang --target=" + ARCHITECTURE + " -march=" + SUBSET + " " + TEST_FILE_NAME + ".s -c -o obj.o -nostdlib"; # Compile to object file
link_command  = "ld.lld --script=" + LINKER_SCRIPT + "obj.o -o linked.bin"; # Link it
strip_command = "llvm-objcopy-10 -O binary linked.bin " + TEST_FILE_NAME + ".stripped"; # Strip out everything but the actual instructions



































if __name__ == "__main__":
    main();