#!/usr/bin/env python3

import os
import sys
import json
from common import write_macros

INPUTS_CONFIG_PATH = os.path.abspath("./inputs/inputs.json")

def main():
    INPUTS_IN_CONFIG_PATH = os.path.abspath(sys.argv[1])
    definition_data = []

    with open(INPUTS_CONFIG_PATH, "r") as config_handle:
        config = json.load(config_handle)

        for year, year_data in config.items():
            for day, day_data in year_data.items():
                day_definitions = []
                for day_entry in day_data:
                    day_definitions.append(f"\"{day_entry['id']}\"")
                    day_definitions.append(f"\"{day_entry['path']}\"")

                definition_data.append(
                    (year, day, f"INPUTS_{year}_{day.upper()} {','.join(day_definitions)}")
                )

    with open(INPUTS_IN_CONFIG_PATH, "w") as config_handle:
        config_handle.write("#pragma once\n\n")

        write_macros(config_handle)
        config_handle.write("\n")

        for definition in definition_data:
            config_handle.write(f"#define {definition[2]}\n")
        config_handle.write("\n")

        for definition in definition_data:
            config_handle.write(f"#define AOC{definition[0]}_{definition[1].upper()}_INPUTS_INITIALIZER INITILIAZER(INPUTS_{definition[0]}_{definition[1].upper()})\n")


if __name__ == "__main__":
    main()
