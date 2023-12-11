#include <algorithm>
#include <numeric>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <ranges>
#include <map>
#include <unordered_map>
#include <set>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

static constexpr std::array SYMBOLS_TABLE {
    '@', '#', '$', '%', '&', '*', '-', '=', '+', '/'
};

struct SchematicNumber {
    std::string_view number;
    int value;
    std::size_t row;
    std::size_t column;
};

struct SymbolCoordinate {
    char symbol;
    std::size_t row;
    std::size_t column;
};

template<typename T>
auto parse_schematic_numbers(T const& schematic_lines) -> std::vector<SchematicNumber> {
    std::vector<SchematicNumber> schematic_numbers{};

    std::size_t i = 0;
    for (auto const& line : schematic_lines) {
        bool cursor_enable = std::isdigit(line.front());
        bool insert_number = false;
        auto cursor_start = line.begin();
        std::size_t j = 0;
        for (auto c = line.begin(); c != line.end(); ++c) {
            if (std::isdigit(*c) && !cursor_enable) {
                cursor_enable = true;
                cursor_start = c;
            } else if (!std::isdigit(*c) && cursor_enable) {
                cursor_enable = false;
                insert_number = true;
            }
            if (insert_number || (j == line.size() - 1 && cursor_enable)) {
                insert_number = false;
                const std::string_view number{cursor_start, (cursor_enable) ? line.end() : c};
                schematic_numbers.emplace_back(
                    number,
                    std::stoi(std::string{number}),
                    i,
                    std::max(0uz, j - number.size())
                );
            }
            ++j;
        }
        ++i;
    }

    return schematic_numbers;
}

using schematic_map_type = std::map<int, std::vector<SchematicNumber>>;
using schematic_map_parse_type = std::tuple<schematic_map_type, std::vector<SymbolCoordinate>>;

template<typename T>
auto parse_schematic_map(T const& schematic_lines, char search_symbol = '\0') -> schematic_map_parse_type {
    schematic_map_type number_map{};
    std::vector<SymbolCoordinate> symbol_coordinates{};

    std::size_t i = 0;
    for (auto const& line : schematic_lines) {
        bool cursor_enable = std::isdigit(line.front());
        bool insert_number = false;
        auto cursor_start = line.begin();
        std::size_t j = 0;
        for (auto c = line.begin(); c != line.end(); ++c) {
            if (*c == search_symbol) { symbol_coordinates.emplace_back(*c, i, j); }
            if (std::isdigit(*c) && !cursor_enable) {
                cursor_enable = true;
                cursor_start = c;
            } else if (!std::isdigit(*c) && cursor_enable) {
                cursor_enable = false;
                insert_number = true;
            }
            if (insert_number || (j == line.size() - 1 && cursor_enable)) {
                insert_number = false;
                const std::string_view number{cursor_start, (cursor_enable) ? line.end() : c};
                number_map[i].emplace_back(
                    number,
                    std::stoi(std::string{number}),
                    i,
                    std::max(0uz, j - number.size())
                );
            }
            ++j;
        }
        ++i;
    }

    return { number_map, symbol_coordinates };
}

auto AoC2023::day3_part1(SolutionInput input) -> SolutionReturn {
    auto const schematic_numbers = parse_schematic_numbers(input);
    int acc = 0;

    for (auto const& number : schematic_numbers) {
        std::size_t r = (number.row == 0) ? 0 : number.row - 1;
        std::size_t c = (number.column == 0) ? 0 : number.column - 1;
        bool is_part_number = false;

        for (std::size_t i = r; i <= std::min(input.size() - 1, number.row + 1) && !is_part_number; ++i) {
            for (std::size_t j = c; j <= std::min(input[i].size() - 1, number.column + number.number.size()) && !is_part_number; ++j) {
                for (auto const symbol : SYMBOLS_TABLE) {
                    if (input[i][j] == symbol) {
                        acc += number.value;
                        is_part_number = true;
                        break;
                    }
                }
            }
        }
    }

    return acc;
}

auto AoC2023::day3_part2(SolutionInput input) -> SolutionReturn {
    auto const [schematic_map, symbol_coordinates] = parse_schematic_map(input, '*');
    int acc = 0;

    for (auto const symbol : symbol_coordinates) {
        std::size_t min_r = (symbol.row == 0) ? 0 : symbol.row - 1;
        std::size_t min_c = (symbol.column == 0) ? 0 : symbol.column - 1;
        std::size_t numbers_found = 0;
        int multiplicand = 0;
        int multiplier = 0;

        for (std::size_t i = min_r; i <= std::min(input.size() - 1, symbol.row + 1); ++i) {
            for (const auto& number : schematic_map.at(i)) {
                std::size_t max_c = std::min(input[i].size() - 1, symbol.column + 1);
                bool start_between_range = min_c <= number.column && number.column <= max_c;
                bool end_between_range =  min_c <= number.column + number.number.size() - 1 && number.column + number.number.size() - 1 <= max_c;
                if (start_between_range || end_between_range) {
                    multiplicand = (numbers_found == 0) ? number.value : multiplicand;
                    multiplier = (numbers_found == 1) ? number.value : multiplier;
                    ++numbers_found;
                }
            }
        }

        if (numbers_found == 2) {
            acc += multiplicand * multiplier;
            numbers_found = 0;
        }
    }

    return acc;
}
