#!/usr/bin/env python3

import os
import sys
import json
from common import write_macros


def main():
    STUBS_DIRECTORY = os.path.abspath(sys.argv[1])
    definition_data = [(2023, day) for day in range(1, 32)]

    with open(os.path.join(STUBS_DIRECTORY, "input_defines.stub.hpp"), "w") as stub_handle:
        stub_handle.write("#pragma once\n\n")
        write_macros(stub_handle)
        stub_handle.write("\n")

        for definition in definition_data:
            stub_handle.write(f"#define INPUTS_{definition[0]}_DAY{definition[1]} \"\", \"\"\n")
        stub_handle.write("\n")

        for definition in definition_data:
            stub_handle.write(f"#define AOC{definition[0]}_DAY{definition[1]}_INPUTS_INITIALIZER INITILIAZER(INPUTS_{definition[0]}_DAY{definition[1]})\n")


if __name__ == "__main__":
    main()
