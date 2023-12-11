#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <numeric>
#include <stdexcept>
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

enum IDChunkType {
    Single,
    Pair
};

struct AlmanacEntry {
    using entry_type = std::pair<std::int64_t, bool>;
    entry_type seed;
    entry_type soil;
    entry_type fertilizer;
    entry_type water;
    entry_type light;
    entry_type temperature;
    entry_type humidity;
    entry_type location;

    AlmanacEntry(std::int64_t seed,
                 std::int64_t soil,
                 std::int64_t fertilizer,
                 std::int64_t water,
                 std::int64_t light,
                 std::int64_t temperature,
                 std::int64_t humidity,
                 std::int64_t location)
        : seed({seed, false})
        , soil({soil, false})
        , fertilizer({fertilizer, false})
        , water({water, false})
        , light({light, false})
        , temperature({temperature, false})
        , humidity({humidity, false})
        , location({location, false})
    {}

    auto operator[](std::size_t column) -> entry_type& {
        switch(column) {
            case 0: { return this->seed; } break;
            case 1: { return this->soil; } break;
            case 2: { return this->fertilizer; } break;
            case 3: { return this->water; } break;
            case 4: { return this->light; } break;
            case 5: { return this->temperature; } break;
            case 6: { return this->humidity; } break;
            case 7: { return this->location; } break;
            default: throw std::out_of_range(std::format("index is out bounds for almanac entry: index {} does not exist", column));
        }
    }

    auto at(std::size_t column) const -> entry_type {
        if (column > 7) {
            throw std::out_of_range(std::format("index is out bounds for almanac entry: index {} does not exist", column));
        }
        return static_cast<entry_type>((*const_cast<AlmanacEntry*>(this))[column]);
    }

    auto set(std::size_t column, std::int64_t value) -> void {
        auto& entry = (*this)[column];
        entry.first = value;
        entry.second = true;
    }

    auto value(std::size_t column) const -> std::int64_t {
        return this->at(column).first;
    }

    auto check(std::size_t column) const -> bool {
        return this->at(column).second;
    }
};

struct SourceDestinationRange {
    std::int64_t destination_start;
    std::int64_t source_start;
    std::int64_t range;
};

auto check_range_bound_and_update_almanac(SourceDestinationRange const& range, std::vector<AlmanacEntry>& almanac, std::size_t const index) {
    for (auto& almanac_entry : almanac) {
        std::int64_t range_min = range.source_start;
        std::int64_t range_max = range.source_start + range.range;
        if (range_min <= almanac_entry.value(index) && almanac_entry.value(index) < range_max) {
            almanac_entry.set(index + 1, range.destination_start + (almanac_entry.value(index) - range_min));
        } else if (!almanac_entry.check(index + 1)) {
            almanac_entry.set(index + 1, almanac_entry.value(index));
        }
    }
}

auto parse_ids_singles(std::ranges::sized_range auto&& id_chunk, std::vector<AlmanacEntry>& almanac) -> void {
    auto seeds_id_part = id_chunk
        | std::views::split(':')
        | std::views::drop(1)
        | std::views::transform([](auto id_numbers) {
                return id_numbers
                    | std::views::split(' ')
                    | std::views::filter([](auto id) { return std::string_view{id} != ""; });
            });

    for (auto&& id_numbers : seeds_id_part) {
        for (auto const id : id_numbers) {
            almanac.emplace_back(std::stoul(std::string{id.begin(), id.end()}), 0, 0, 0, 0, 0, 0, 0);
        }
    }
}

auto parse_range_entry(std::ranges::sized_range auto&& range_chunk) -> SourceDestinationRange {
    auto split_values = range_chunk
        | std::views::split(' ')
        | std::views::filter([](auto number) { return std::string_view{number} != ""; })
        | std::views::transform([](auto number) { return std::string{number.begin(), number.end()}; })
        | std::views::transform([](auto number) { return std::stoul(number); });

    std::vector<std::int64_t> values{split_values.begin(), split_values.end()};
    return {
        values.at(0),
        values.at(1),
        values.at(2)
    };
    return {};
}

auto parse_almanac_table(SolutionInput almanac_input, int id_type = IDChunkType::Single) -> std::vector<AlmanacEntry> {
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
            (id_type == IDChunkType::Single)
                ? parse_ids_singles(section_chunk[0], almanac)
                : void();
        } else {
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
    return std::min_element(almanac.begin(), almanac.end(),
                            [](auto const& current, auto const& next) { 
                                return current.location.first < next.location.first;
                            })
        ->location.first;
}

auto AoC2023::day5_part2(SolutionInput input) -> SolutionReturn {
    return -1;
}
