
from posixpath import basename, splitext
import string
from sys import exit, argv, stderr
from typing import BinaryIO, TextIO


# Code Generation


def top(file: TextIO):
    file.write("""/*! This code file is generated. Any changes will be overridden when regenerated. */

#include <cstddef>
#include <cstdint>

extern "C" {

""")


def blob(file: TextIO, name: str, data: BinaryIO):
    file.write(f"extern const uint8_t {name}[] = ")
    file.write("{ ")
    for i in data.read():
        file.write(f"0x{i:02X}, ")
    file.write(" };\n")

    file.write(f"extern const size_t {name}_size = {data.tell()};\n\n")


def end(file: TextIO):
    file.write("}\n")


def symbol(filename: str):
    return "data_" + splitext(basename(filename))[0]


# Command-line


def usage(argv0: str = argv[0], file: TextIO = stderr):
    print(f"Usage: {argv0} <output> <input...>", file=file)
    print(file=file)
    exit(1)


if __name__ == "__main__":
    if len(argv) < 3:
        usage()

    output = argv[1]
    with open(output, "w") as outfile:
        top(outfile)
        for input in argv[2:]:
            with open(input, "rb") as infile:
                blob(outfile, symbol(input), infile)
        end(outfile)
