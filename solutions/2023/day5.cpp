#include <algorithm>
#include <cctype>
#include <cmath>
#include <numeric>
#include <string>
#include <string_view>
#include <array>
#include <utility>
#include <vector>
#include <map>
#include <ranges>
#include <format>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

struct AlmanacEntry {
    int seed;
    int soil;
    int fertilizer;
    int water;
    int light;
    int temperature;
    int humidity;
    int location;

    AlmanacEntry(int seed,
                 int soil,
                 int fertilizer,
                 int water,
                 int light,
                 int temperature,
                 int humidity,
                 int location)
        : seed(seed)
        , soil(soil)
        , fertilizer(fertilizer)
        , water(water)
        , light(light)
        , temperature(temperature)
        , humidity(humidity)
        , location(location)
    {}

    auto operator[](int column) -> int& {
        switch(column) {
            case 0: { return this->seed; } break;
            case 1: { return this->soil; } break;
            case 2: { return this->fertilizer; } break;
            case 3: { return this->water; } break;
            case 4: { return this->light; } break;
            case 5: { return this->temperature; } break;
            case 6: { return this->humidity; } break;
            case 7: { return this->location; } break;
            default: throw std::range_error(std::format("index is out bounds for AlmanacEntry: index {} does not exist", column));
        }
    }
};

struct SourceDestinationRange {
    int destination_start;
    int source_start;
    int range;
};

auto check_range_bound_and_update_almanac(SourceDestinationRange const& range, std::vector<AlmanacEntry>& almanac, int const index) {
    for (auto& almanac_entry : almanac) {
        int range_min = range.source_start;
        int range_max = range.source_start + range.range;
        if (range_min <= almanac_entry[index] && almanac_entry[index] <= range_max) {
            almanac_entry[index + 1] = range.destination_start + (almanac_entry[index] - range_min);
        } else {
            almanac_entry[index + 1] = almanac_entry[index];
        }
        fmt::print("{}: i: {} next: {} -> min: {} max: {}\n", index, almanac_entry[index], almanac_entry[index + 1], range_min, range_max);
    }
    fmt::print("\n");
}

auto parse_range_entry(std::ranges::sized_range auto&& values_range) -> SourceDestinationRange {
    auto split_values = values_range
        | std::views::split(' ')
        | std::views::filter([](auto number) { return std::string_view{number} != ""; })
        | std::views::transform([](auto number) { return std::string{number.begin(), number.end()}; })
        | std::views::transform([](auto number) { return std::stoi(number); });

    std::vector<int> values{split_values.begin(), split_values.end()};
    return {
        values.at(0),
        values.at(1),
        values.at(2)
    };
    return {};
}

auto parse_almanac_table(SolutionInput almanac_input) -> std::vector<AlmanacEntry> {
    std::vector<AlmanacEntry> almanac{};

    auto almanac_sections = almanac_input
        | std::views::chunk_by([](auto lhs, auto rhs) {
                return lhs != "" && rhs != "";
            })
        | std::views::filter([](auto chunk) {
                return chunk[0] != "";
            });

    for (auto const [i, section_chunk] : almanac_sections | std::views::enumerate) {
        if (i == 0) {
            auto seeds_id_part = section_chunk[0]
                | std::views::split(':')
                | std::views::drop(1)
                | std::views::transform([](auto id_numbers) {
                        return id_numbers
                            | std::views::split(' ')
                            | std::views::filter([](auto id) { return std::string_view{id} != ""; });
                    });

            for (auto&& id_numbers : seeds_id_part) {
                for (auto const id : id_numbers) {
                    almanac.emplace_back(std::stoi(std::string{id.begin(), id.end()}), 0, 0, 0, 0, 0, 0, 0);
                }
            }
        }

        else {
            for (auto&& range_numbers : section_chunk | std::views::drop(1)) {
                auto id_range = parse_range_entry(range_numbers);
                check_range_bound_and_update_almanac(id_range, almanac, i - 1);
            }
        }
    }
    
    return almanac;
}

auto AoC2023::day5_part1(SolutionInput input) -> SolutionReturn {
    auto almanac = parse_almanac_table(input);

    for (auto const& entry : almanac) {
        fmt::print("seed: {}, soil: {}, fertilizer: {}, water: {}, light: {}, temperature: {}, humidity: {}, location: {}\n",
                   entry.seed, entry.soil, entry.fertilizer, entry.water, entry.light, entry.temperature, entry.humidity, entry.location);
    }

    return std::min_element(almanac.begin(), almanac.end(),
                            [](auto const& current, auto const& next) { 
                                return current.location < next.location;
                            })
        ->location;
}

auto AoC2023::day5_part2(SolutionInput input) -> SolutionReturn {
    return -1;
}
