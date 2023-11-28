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

def main():
    num_args = len(sys.argv);
    if num_args < 2:
        print("Not enough arguemnts!");
        print("Feed the path to a test")
    build_output = subprocess.run(
        [
            "clang",
            "--target=" + ARCHITECTURE,
            "-march=" + SUBSET,
            sys.argv[1] + TEST_FILE_NAME + ".s",
            "-c",
            "-o",
            sys.argv[1] + "obj.o",
            "-nostdlib"
        ]
    );
    linker_output = subprocess.run(
        [
            "riscv32-elf-ld",
            "--script=" + sys.argv[1] + LINKER_SCRIPT,
            sys.argv[1] + "obj.o",
            "-o",
            sys.argv[1] + "linked.bin"
        ]
    );
    strip_output = subprocess.run(
        [
            "riscv32-elf-objcopy",
            "-O",
            "binary",
            sys.argv[1] + "linked.bin",
            sys.argv[1] + TEST_FILE_NAME + ".stripped"
        ]
    );


if __name__ == "__main__":
    main();