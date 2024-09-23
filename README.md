# Advent of Code 2023

Total Solutions: 10

Check out `solutions/2023` for implementations.

## Build

We depend on:

- [fmtlib](https://github.com/fmtlib/fmt)
- [argparse](https://github.com/p-ranav/argparse)

Use your preferred package manager to install them.

Note: this is only tested on x86_64 Linux using GCC.

### Build All Solutions

```bash
make all
```

### Compile Flags for clangd

If you use clangd for LSP, run the following for LSP configuration:

```bash
make compileflags
```

## Usage

The build folder will contain the solution runner `aoc`.

### Run all Solutions

Note: some solutions might take long to complete.

```bash
aoc 2023

```

### Run Specific Solutions

The syntax is as follows: `aoc [<year> <day> <part> [data]]`

```bash
# Run Year 2023, Day 1, Part 1
aoc 2023 1 1

# Run Year 2023, Day 5, Part 2 with custom data file
aoc 2023 5 2 ./mydata/foo.txt

```

___
