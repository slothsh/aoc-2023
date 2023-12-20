#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <numeric>
#include <stdexcept>
#include <string>
#include <string_view>
#include <array>
#include <utility>
#include <list>
#include <vector>
#include <ranges>
#include <format>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

enum IDChunkType {
    Single,
    Pair
};

struct AlmanacRange {
    std::int64_t start;
    std::int64_t length;
    std::int64_t overlap;

    static auto slices(AlmanacRange lhs, AlmanacRange rhs) -> std::array<AlmanacRange, 3> {
        std::int64_t lhs_end = lhs.start + lhs.length;
        std::int64_t rhs_end = rhs.start + rhs.length;

        // |---A---|
        //     |-x-B---|
        //       ^
        //       |______ = (s_A + l_A) - s_B
        bool inner_right = lhs.start <= rhs.start && rhs.start < lhs_end && lhs_end <= rhs_end;

        //     |---A---|
        // |---B-x-|
        //       ^
        //       |______ = (s_B + l_B) - s_A
        bool inner_left = rhs.start <= lhs.start && lhs.start < rhs_end && rhs_end <= lhs_end;

        // |---A---|
        //  |--B--|
        //     ^
        //     |________ = l_B
        bool all_of = lhs.start < rhs.start && rhs_end < lhs_end;

        //  |--A--|
        // |---B---|
        //     ^
        //     |________ = l_A
        bool some_of = rhs.start < lhs.start && lhs_end < rhs_end;

        //         |--A--|
        // |---B---|
        //
        // OR
        //
        // |--A--|
        //       |---B---|
        // bool none = lhs.start >= rhs_end || lhs_end <= rhs.start;


        // TODO: Better way to express this logic?
        if (inner_right) {
            return {
                AlmanacRange{ lhs.start, rhs.start - lhs.start, 0 },
                AlmanacRange{ rhs.start, lhs_end - rhs.start, lhs_end - rhs.start },
                AlmanacRange{ 0, 0, 0 },
            };
        } else if (inner_left) {
            return {
                AlmanacRange{ 0, 0, 0 },
                AlmanacRange{ lhs.start, rhs_end - lhs.start, rhs_end - lhs.start },
                AlmanacRange{ rhs_end, lhs_end - rhs_end, lhs_end - rhs_end }
            };
        } else if (all_of) {
            return {
                AlmanacRange{ lhs.start, lhs.start - rhs.start, 0 },
                AlmanacRange{ rhs.start, rhs.length, rhs.length },
                AlmanacRange{ rhs_end, lhs_end - rhs_end, 0 }
            };
        } else if (some_of) {
            return {
                AlmanacRange{ 0, 0, 0 },
                AlmanacRange{ lhs.start, lhs.length, lhs.length },
                AlmanacRange{ 0, 0, 0 }
            };
        }

        return {
            AlmanacRange{ 0, 0, 0 },
            AlmanacRange{ lhs.start, lhs.length, 0 },
            AlmanacRange{ 0, 0, 0 }
        };
    }
};

struct AlmanacEntry {
    using entry_type = std::pair<AlmanacRange, bool>;
    entry_type seed;
    entry_type soil;
    entry_type fertilizer;
    entry_type water;
    entry_type light;
    entry_type temperature;
    entry_type humidity;
    entry_type location;

    AlmanacEntry(AlmanacRange const& seed)
        : seed({seed, true})
        , soil({{}, false})
        , fertilizer({{}, false})
        , water({{}, false})
        , light({{}, false})
        , temperature({{}, false})
        , humidity({{}, false})
        , location({{}, false})
    {}

    explicit AlmanacEntry(AlmanacEntry const& almanac_entry, std::size_t index, std::int64_t length, std::int64_t offset)
        : seed({{}, false})
        , soil({{}, false})
        , fertilizer({{}, false})
        , water({{}, false})
        , light({{}, false})
        , temperature({{}, false})
        , humidity({{}, false})
        , location({{}, false})
    {
        for (std::size_t j = 0; j <= index; ++j) {
            this->set(j, { almanac_entry.value(j).start + offset, length, 0 });
        }
    }

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

    auto set(std::size_t column, AlmanacRange value) -> void {
        auto& entry = (*this)[column];
        entry.first = value;
        entry.second = true;
    }

    auto value(std::size_t column) const -> AlmanacRange {
        return this->at(column).first;
    }

    auto check(std::size_t column) const -> bool {
        return this->at(column).second;
    }

    auto enable_until(std::size_t column) -> void {
        for (std::size_t i = 0; i < std::min(7uz, column); ++i) {
            (*this)[i].second = true;
        }
    }
};

struct SourceDestinationRange {
    std::int64_t destination_start;
    std::int64_t source_start;
    std::int64_t range;
};

auto check_range_bound_and_update_almanac(SourceDestinationRange const& range, std::list<AlmanacEntry>& almanac, std::size_t const index) {
    AlmanacRange source { range.source_start, range.range, 0 };
    std::vector<typename std::list<AlmanacEntry>::iterator> invalid_entries{};

    for (auto almanac_entry = almanac.begin(); almanac_entry != almanac.end(); ++almanac_entry) {
        auto slices = AlmanacRange::slices(almanac_entry->value(index), source);
        bool inserted = false;
        std::int64_t offset = 0;

        for (auto const& [i, sub_slice] : slices | std::views::enumerate) {
            if (sub_slice.length > 0) {
                AlmanacEntry split_entry{ *almanac_entry, index, sub_slice.length, offset };

                auto split_start = (sub_slice.overlap == 0)
                    ? (!almanac_entry->check(index + 1))
                        ? split_entry.value(index).start
                        : almanac_entry->value(index + 1).start
                    : range.destination_start + (sub_slice.start - source.start);

                split_entry.set(index + 1, { split_start, sub_slice.length, 0 });
                almanac.insert(almanac_entry, split_entry);

                inserted = true;
                offset += sub_slice.length;
            }
        }

        if (inserted) invalid_entries.push_back(almanac_entry);
    }

    for (auto const& invalid : invalid_entries) {
        almanac.erase(invalid);
    }
}

auto parse_ids_singles(std::ranges::sized_range auto&& id_chunk, std::list<AlmanacEntry>& almanac) -> void {
    auto seeds_id_part = id_chunk
        | std::views::split(':')
        | std::views::drop(1)
        | std::views::transform([](auto id_numbers) {
                return id_numbers
                    | std::views::split(' ')
                    | std::views::filter([](auto id) { return std::string_view{id} != ""; });
            });

    for (auto id_numbers : seeds_id_part) {
        for (auto id : id_numbers) {
            almanac.emplace_back(AlmanacRange{ std::stoll(std::string{id.begin(), id.end()}), 1, 0 });
        }
    }
}

auto parse_ids_pairs(std::ranges::sized_range auto&& id_chunk, std::list<AlmanacEntry>& almanac) -> void {
    auto seeds_id_part = id_chunk
        | std::views::split(':')
        | std::views::drop(1)
        | std::views::transform([](auto id_numbers) {
                return id_numbers
                    | std::views::split(' ')
                    | std::views::filter([](auto id) { return std::string_view{id} != ""; });
            });


    for (auto id_numbers : seeds_id_part) {
        for (auto id_chunk : id_numbers | std::views::chunk(2)) {
            // TODO: FIX STUPID VIEWS::CHUNK HACK
            std::string start;
            std::string length;
            std::size_t i = 0;

            for (auto number : id_chunk) {
                if (i == 0) {
                    start = std::string{ number.begin(), number.end() };
                } else if (i == 1) {
                    length = std::string{ number.begin(), number.end() };
                }
                ++i;
            }

            almanac.emplace_back(AlmanacRange{ std::stoll(start), std::stoll(length), 0 });
        }
    }
}

auto parse_range_entry(std::ranges::sized_range auto&& range_chunk) -> SourceDestinationRange {
    auto split_values = range_chunk
        | std::views::split(' ')
        | std::views::filter([](auto number) { return std::string_view{number} != ""; })
        | std::views::transform([](auto number) { return std::string{number.begin(), number.end()}; })
        | std::views::transform([](auto number) { return std::stoll(number); });

    std::vector<std::int64_t> values{split_values.begin(), split_values.end()};
    return {
        values.at(0),
        values.at(1),
        values.at(2)
    };
    return {};
}

auto parse_almanac_table(SolutionInput almanac_input, int id_type = IDChunkType::Single) -> std::list<AlmanacEntry> {
    std::list<AlmanacEntry> almanac{};

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
                : parse_ids_pairs(section_chunk[0], almanac);
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
    auto almanac = parse_almanac_table(input, IDChunkType::Single);

    return std::min_element(almanac.begin(), almanac.end(),
                            [](auto const& current, auto const& next) { 
                               auto current_end = current.location.first.start + current.location.first.length;
                               auto next_end = next.location.first.start + next.location.first.length;

                                return current.location.first.start < next.location.first.start
                                    && current_end < next_end;
                            })
        ->location.first.start;
}

auto AoC2023::day5_part2(SolutionInput input) -> SolutionReturn {
    auto almanac = parse_almanac_table(input, IDChunkType::Pair);

    return std::min_element(almanac.begin(), almanac.end(),
                            [](auto const& current, auto const& next) { 
                               auto current_end = current.location.first.start + current.location.first.length;
                               auto next_end = next.location.first.start + next.location.first.length;

                                return current.location.first.start < next.location.first.start
                                    && current_end < next_end;
                            })
        ->location.first.start;
}
