#include <string_view>

#include "inputs.hpp"
#include "aocprogram.hpp"
#include "solution.hpp"
#include "parsing.hpp"
#include "aoc2023.hpp"

const std::map<std::string_view, Solution> AocProgram::solutions {
    { "2023:day1:part1", Solution(2023, 1, 1, &AoC2023::day1_part1, AOC2023_DAY1_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day1:part2", Solution(2023, 1, 2, &AoC2023::day1_part2, AOC2023_DAY1_INPUTS_INITIALIZER, &parsing::parse_lines) },
    
    { "2023:day2:part1", Solution(2023, 2, 1, &AoC2023::day2_part1, AOC2023_DAY2_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day2:part2", Solution(2023, 2, 2, &AoC2023::day2_part2, AOC2023_DAY2_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day3:part1", Solution(2023, 3, 1, &AoC2023::day3_part1, AOC2023_DAY3_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day3:part2", Solution(2023, 3, 2, &AoC2023::day3_part2, AOC2023_DAY3_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day4:part1", Solution(2023, 4, 1, &AoC2023::day4_part1, AOC2023_DAY4_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day4:part2", Solution(2023, 4, 2, &AoC2023::day4_part2, AOC2023_DAY4_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day5:part1", Solution(2023, 5, 1, &AoC2023::day5_part1, AOC2023_DAY5_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day5:part2", Solution(2023, 5, 2, &AoC2023::day5_part2, AOC2023_DAY5_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day6:part1", Solution(2023, 6, 1, &AoC2023::day6_part1, AOC2023_DAY6_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day6:part2", Solution(2023, 6, 2, &AoC2023::day6_part2, AOC2023_DAY6_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day7:part1", Solution(2023, 7, 1, &AoC2023::day7_part1, AOC2023_DAY7_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day7:part2", Solution(2023, 7, 2, &AoC2023::day7_part2, AOC2023_DAY7_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day8:part1", Solution(2023, 8, 1, &AoC2023::day8_part1, AOC2023_DAY8_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day8:part2", Solution(2023, 8, 2, &AoC2023::day8_part2, AOC2023_DAY8_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day9:part1", Solution(2023, 9, 1, &AoC2023::day9_part1, AOC2023_DAY9_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day9:part2", Solution(2023, 9, 2, &AoC2023::day9_part2, AOC2023_DAY9_INPUTS_INITIALIZER, &parsing::parse_lines) },

    { "2023:day10:part1", Solution(2023, 10, 1, &AoC2023::day10_part1, AOC2023_DAY10_INPUTS_INITIALIZER, &parsing::parse_lines) },
    { "2023:day10:part2", Solution(2023, 10, 2, &AoC2023::day10_part2, AOC2023_DAY10_INPUTS_INITIALIZER, &parsing::parse_lines) }
};
