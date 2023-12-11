#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <fmt/core.h>

#include "parsing.hpp"
#include "solution.hpp"

auto parsing::parse_lines(std::string_view input) -> std::optional<SolutionInputValue> {
    namespace fs = std::filesystem;

    std::vector<std::string> input_lines {};
    try {
        auto input_path = fs::absolute(fs::path(input));
        std::ifstream input_file(input_path, std::ios::in);
        if (!input_file) {
            return std::nullopt;
        }

        for (std::string line; std::getline(input_file, line, '\n');) {
            input_lines.emplace_back(line);
        }
    }

    catch (std::exception& error) {
        return std::nullopt;
    }

    return input_lines;
}
