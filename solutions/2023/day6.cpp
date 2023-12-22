#include <algorithm>
#include <bits/ranges_algobase.h>
#include <bits/ranges_base.h>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>
#include <string_view>
#include <array>
#include <utility>
#include <map>
#include <vector>
#include <ranges>
#include <format>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

auto parse_race_records(SolutionInput race_table) -> std::map<int, int> {
    std::map<int, int> race_records{};

    auto parsed_records = race_table
        | std::views::transform([](auto const& row) {
                std::vector<int> values{};
                std::ranges::subrange row_values(std::ranges::find(row, ':') + 1, row.end());

                auto parsed_values = row_values
                    | std::views::split(' ')
                    | std::views::filter([](auto c) { return std::string_view{c} != ""; });

                for (auto const parsed_value : parsed_values) {
                    std::string_view value{parsed_value};
                    values.emplace_back(std::stol(std::string{ value.begin(), value.end() }));
                }

                return values;
            });

    for (auto const [i, value] : parsed_records.front() | std::views::enumerate) {
        race_records[value] = parsed_records.back().at(i);
    }

    return race_records;
}

auto parse_race_records_ignore_kerning(SolutionInput race_table) -> std::pair<std::int64_t, std::int64_t> {
    std::pair<std::int64_t, std::int64_t> race_record{};

    auto parsed_record = race_table
        | std::views::transform([](auto const& row) {
                std::ranges::subrange row_values(std::ranges::find(row, ':') + 1, row.end());

                auto parsed_value = row_values
                    | std::views::filter([](auto c) { return c != ' '; });

                return std::stoll(std::string{parsed_value.begin(), parsed_value.end()});
            });

    race_record.first = parsed_record.front();
    race_record.second = parsed_record.back();

    return race_record;
}

auto AoC2023::day6_part1(SolutionInput input) -> SolutionReturn {
    auto const race_records = parse_race_records(input);
    std::vector<int> winning_durations{};

    for (auto const [time, record] : race_records) {
        int win_count = 0;
        for (int t = 0; t < time; ++t) {
            int duration = (time - t) * t;
            if (duration > record) {
                ++win_count;
            }
        }
        winning_durations.push_back(win_count);
    }

    return std::accumulate(winning_durations.begin(), winning_durations.end(), 1, std::multiplies<int>());
}

auto AoC2023::day6_part2(SolutionInput input) -> SolutionReturn {
    auto const [time, record] = parse_race_records_ignore_kerning(input);

    std::int64_t t = time / 2;
    std::int64_t offset = time - t;
    std::int64_t minimum = t;
    std::int64_t maximum = 0;

    while (offset != 0) {
        std::int64_t duration = (time - t) * t;

        if (duration > record) {
            maximum = std::max(t, maximum);
        }

        offset /= 2;
        t += (duration > record) ? offset : -offset;
    }

    return (maximum - minimum) * 2 + ((time % 2 == 0) ? 1 : 0);
}
