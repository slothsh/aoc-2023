#include <format>
#include <fmt/core.h>
#include <argparse/argparse.hpp>

#include "aocprogram.hpp"
#include "solution.hpp"

auto main(int argc, char** argv) -> int {
    argparse::ArgumentParser program("aoc");
    program.add_description("Advent of Code solutions by slothsh");

    program.add_argument("year")
        .default_value<int>(-1)
        .help("which year of AoC to access")
        .scan<'i', int>();

    program.add_argument("day")
        .default_value<int>(-1)
        .help("which day of AoC to access")
        .scan<'i', int>();

    program.add_argument("part")
        .default_value<int>(-1)
        .help("which part of AoC to access")
        .scan<'i', int>();

    program.add_argument("data")
        .default_value("main")
        .help("which data set of AoC to use");

    program.parse_args(argc, argv);

    if (program.is_used("help")) {
        fmt::print("{}\n", program.help().str());
        return 0;
    }

    if (program.get<int>("year") == -1 &&
        program.get<int>("day") == -1 &&
        program.get<int>("part") == -1)
    {
        for (auto const& [id, solution] : AocProgram::solutions) {
            auto solution_result = solution("main");
            fmt::print("{} Day {}, Part {}: {}\n", solution.year(), solution.day(), solution.part(), solution_result.value());
        }

        return 0;
    }

    std::string const aoc_id = std::format("{}:day{}:part{}",
                                           program.get<int>("year"),
                                           program.get<int>("day"),
                                           program.get<int>("part"));

    std::string const data = program.get("data");

    auto solution_result = AocProgram::solutions.at(aoc_id)(data);
    fmt::print("{} Day {}, Part {}: {}\n",
               AocProgram::solutions.at(aoc_id).year(),
               AocProgram::solutions.at(aoc_id).day(),
               AocProgram::solutions.at(aoc_id).part(),
               solution_result.value());

    return 0;
}
