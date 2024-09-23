#include <algorithm>
#include <bits/ranges_algo.h>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <numeric>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <ranges>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

// TODO: Refactor ALL of this to not be so hacky and haphazard
// TODO: Ensure that axis convention is consistent x = columns, y = rows => i = y, j = x
// TODO: Possibly use trigometric functions to determine directions and offsets

struct Vec2 {
    int x;
    int y;

    friend auto operator==(Vec2 const& lhs, Vec2 const& rhs) -> bool {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    friend auto operator!=(Vec2 const& lhs, Vec2 const& rhs) -> bool {
        return lhs.x != rhs.x || lhs.y != rhs.y;
    }

    friend auto operator+=(Vec2& lhs, Vec2 const& rhs) -> void {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
    }
};

enum Direction : int {
    None       = 0,
    North      = 1,
    East       = 2,
    NorthEast  = 3,
    South      = 4,
    NorthSouth = 5,
    SouthEast  = 6,
    West       = 8,
    NorthWest  = 9,
    EastWest   = 10,
    SouthWest  = 12,
};

auto opposite(Direction direction) -> Direction {
    using type = std::underlying_type_t<Direction>;
    bool wrap_around = direction >= Direction::South;
    return static_cast<Direction>(
        (wrap_around)
            ? static_cast<type>(direction) >> 2
            : static_cast<type>(direction) << 2
    );
}

auto can_connect(Direction our_direction, Direction their_pipe) -> bool {
    if (our_direction == Direction::None) return false;

    using type = std::underlying_type_t<Direction>;
    auto const our_opposite_value = static_cast<type>(opposite(our_direction));
    auto const their_pipe_value = static_cast<type>(their_pipe);
    return (our_opposite_value & their_pipe_value) == our_opposite_value;
}

auto from_relative_offsets(int x, int y) -> Direction {
    using type = std::underlying_type_t<Direction>;

    auto vertical = (x == 0)
        ? static_cast<type>(Direction::None)
        : (-x > 0)
            ? static_cast<type>(Direction::North)
            : static_cast<type>(Direction::South);

    auto horizontal = (y == 0)
        ? static_cast<type>(Direction::None)
        : (y > 0)
            ? static_cast<type>(Direction::East)
            : static_cast<type>(Direction::West);

    return static_cast<Direction>(vertical | horizontal);
}

auto to_relative_offsets(Direction const direction) -> std::pair<int, int> {
    std::array<int, 4> directions{};
    std::size_t directions_index = 0;

    for (auto const d : { North, East, South, West }) {
        auto d_value = static_cast<int>(d);
        int direction_present = (direction & d_value) == d_value;
        if (bool(direction_present)) {
            directions[directions_index++] = (d == South || d == West) ? -direction_present : direction_present;
        }
    }

    return {
        directions.front(),
        directions.back()
    };
}

auto to_pair(Direction const direction) -> std::pair<Direction, Direction> {
    std::array<Direction, 2> directions{};
    std::size_t directions_index = 0;

    for (auto const d : { North, East, South, West }) {
        auto d_value = static_cast<int>(d);
        int direction_present = (direction & d_value) == d_value;
        if (bool(direction_present)) {
            directions[directions_index++] = d;
        }
    }

    return {
        directions.front(),
        directions.back()
    };
}

auto to_name(Direction const direction) -> std::string_view {
    switch (direction) {
        case None:       { return "None"; }
        case North:      { return "North"; }
        case East:       { return "East"; }
        case NorthEast:  { return "NorthEast"; }
        case South:      { return "South"; }
        case NorthSouth: { return "NorthSouth"; }
        case SouthEast:  { return "SouthEast"; }
        case West:       { return "West"; }
        case NorthWest:  { return "NorthWest"; }
        case EastWest:   { return "EastWest"; }
        case SouthWest:  { return "SouthWest"; }
    }

    return "<UNKNOWN>";
}

struct Pipe {
    enum Type : int {
        Vertical   = '|',
        Horizontal = '-',
        BendNE     = 'L',
        BendNW     = 'J',
        BendSW     = '7',
        BendSE     = 'F',
        Ground     = '.',
        Unknown    = 'S'
    };

    static auto valid_symbol(char c) -> bool {
        switch (static_cast<Type>(c)) {
            case Vertical:   // FALLTHROUGH
            case Horizontal: // FALLTHROUGH
            case BendNE:     // FALLTHROUGH
            case BendNW:     // FALLTHROUGH
            case BendSW:     // FALLTHROUGH
            case BendSE:     // FALLTHROUGH
            case Ground:     // FALLTHROUGH
            case Unknown:    { return true; }
            default:         { return false; }
        }

        return false;
    }

    static auto to_direction(Type const pipe) -> Direction {
        using enum Direction;
        switch (pipe) {
            case Vertical:   { return NorthSouth; }
            case Horizontal: { return EastWest; }
            case BendNE:     { return NorthEast; }
            case BendNW:     { return NorthWest; }
            case BendSW:     { return SouthWest; }
            case BendSE:     { return SouthEast; }
            case Ground:     { return None; }
            case Unknown:    { return None; }
            default:         { return None; }
        }

        return None;
    }

    static auto from_direction(Direction direction) -> Pipe::Type {
        using enum Direction;
        switch (direction) {
            case NorthSouth: { return Vertical; }
            case EastWest:   { return Horizontal; }
            case NorthWest:  { return BendNW; }
            case NorthEast:  { return BendNE; }
            case SouthWest:  { return BendSW; }
            case SouthEast:  { return BendSE; }
            default:         { return Ground; }
        }

        return Ground;
    }

    static auto offsets(Type const pipe_in) -> Vec2 {
        auto const [x_offset, y_offset] = to_relative_offsets(Pipe::to_direction(pipe_in));
        return {
            x_offset,
            y_offset
        };
    }

    static auto split_directions(Type const pipe_in) -> std::pair<Direction, Direction> {
        auto const [first, second] = to_pair(Pipe::to_direction(pipe_in));

        return {
            first,
            second
        };
    }

    Type pipe;
};

using pipe_entry_type = std::vector<Pipe>;
using pipe_map_type = std::vector<pipe_entry_type>;

auto parse_pipe_map(SolutionInput pipe_sketch) -> std::pair<Vec2, pipe_map_type> {
    pipe_map_type pipe_map{};
    pipe_map.resize(pipe_sketch.size());
    Vec2 animal_position{};

    for (auto const [i, r] : pipe_sketch | std::views::enumerate) {
        for (auto const [j, c] : r | std::views::enumerate) {
            if (static_cast<Pipe::Type>(c) == Pipe::Unknown) {
                animal_position.x = i;
                animal_position.y = j;
            }

            if (Pipe::valid_symbol(c)) {
                pipe_map.at(i).emplace_back(static_cast<Pipe::Type>(c));
            }
        }
    }

    return { animal_position, pipe_map };
}

auto fill_missing_pipe(Vec2 const animal_position, pipe_map_type& pipe_map) -> void {
    std::array<Direction, 2> missing_directions{
        Direction::None,
        Direction::None
    };

    std::size_t const size_max = std::numeric_limits<std::size_t>::max();
    int const x_max = pipe_map.size();
    int const y_max = pipe_map.front().size();

    int x_offset = 0;
    int y_offset = 1;
    std::size_t missing_index = 0;

    for (int steps = 4; steps > 0;
         --steps,
         (steps % 2 == 0)
            ? void(x_offset = -x_offset), void(y_offset = -y_offset)
            : void(std::swap(x_offset, y_offset)))
    {
        auto x_absolute = animal_position.x + x_offset;
        auto y_absolute = animal_position.y + y_offset;

        std::size_t i = (0 <= x_absolute && x_absolute < x_max)
            ? animal_position.x + x_offset
            : size_max;

        std::size_t j = (0 <= y_absolute && y_absolute < y_max)
            ? animal_position.y + y_offset
            : size_max;

        if (i == size_max || j == size_max) {
            continue;
        }

        auto const animal_direction = from_relative_offsets(x_offset, y_offset);
        auto const pipe_direction = Pipe::to_direction(pipe_map.at(i).at(j).pipe);

        if (can_connect(animal_direction, pipe_direction)) {
            missing_directions[missing_index++] = animal_direction;
        }
    }

    using direction_type = std::underlying_type_t<Direction>;
    pipe_map[animal_position.x][animal_position.y].pipe = Pipe::from_direction(
        static_cast<Direction>(
            static_cast<direction_type>(missing_directions.front()) | static_cast<direction_type>(missing_directions.back())
        )
    );
}

auto find_loop(Vec2 const animal_position, pipe_map_type const& pipe_map) -> std::vector<Direction> {
    std::vector<Direction> directions;

    Vec2 current_position = animal_position;

    Direction previous = None;
    do {
        auto& [x, y] = current_position;
        auto const [first, second] = Pipe::split_directions(pipe_map.at(x).at(y).pipe);

        auto& next_direction = const_cast<Direction&>(std::min(first, second));
        if (next_direction == opposite(previous)) {
            next_direction = std::max(first, second);
        }

        bool n_or_s = next_direction == North || next_direction == South;
        auto& offset_direction = (n_or_s) ? x : y;
        auto const [offset_x, offset_y] = to_relative_offsets(next_direction);

        bool n = first == North;
        bool e = first == East;
        bool s = first == South;
        bool w = first == West;

        bool take_x = (n && next_direction >= North)
                   || (e && next_direction >= East)
                   || (s && next_direction >= South)
                   || (w && next_direction >= West);

        auto const next_offset = (take_x) ? offset_x : offset_y;

        previous = next_direction;
        directions.emplace_back(next_direction);
        offset_direction += (n_or_s) ? -next_offset : next_offset;
    } while (current_position != animal_position);

    return directions;
}

auto pipe_map_bounding_box(Vec2 const animal_position,
                           std::vector<Direction> const& loop_directions) -> std::pair<Vec2, Vec2> {
    Vec2 lo {
        .x = std::numeric_limits<int>::max(),
        .y = std::numeric_limits<int>::max()
    };

    Vec2 hi {
        .x = 0,
        .y = 0
    };

    auto [y, x] = animal_position;
    for (auto const direction : loop_directions) {
        x += (direction == East || direction == West) ? (direction == West) ? -1 : 1 : 0;
        y += (direction == North || direction == South) ? (direction == South) ? 1 : -1 : 0;

        lo.x = std::min(lo.x, x);
        lo.y = std::min(lo.y, y);
        hi.x = std::max(hi.x, x);
        hi.y = std::max(hi.y, y);
    }

    return { lo, hi };
}

auto empty_tiles_inside_boundaries(std::pair<Vec2, Vec2> const boundaries,
                                   pipe_map_type const& pipe_map) -> std::vector<Vec2> {
    std::vector<Vec2> empty_tiles{};
    auto const [lo, hi] = boundaries;

    for (std::size_t i = lo.y; i <= static_cast<std::size_t>(hi.y); ++i) {
        for (std::size_t j = lo.x; j <= static_cast<std::size_t>(hi.x); ++j) {
            if (pipe_map.at(i).at(j).pipe == Pipe::Ground) {
                empty_tiles.emplace_back(j, i);
            }
        }
    }

    return empty_tiles;
}

auto AoC2023::day10_part1(SolutionInput input) -> SolutionReturn {
    auto [animal_position, pipe_map] = parse_pipe_map(input);
    fill_missing_pipe(animal_position, pipe_map);
    auto const loop_directions = find_loop(animal_position, pipe_map);
    return loop_directions.size() / 2;
}

auto AoC2023::day10_part2(SolutionInput input) -> SolutionReturn {
    auto [animal_position, pipe_map] = parse_pipe_map(input);
    fill_missing_pipe(animal_position, pipe_map);
    auto const loop_directions = find_loop(animal_position, pipe_map);
    auto const boundaries = pipe_map_bounding_box(animal_position, loop_directions);
    auto const empty_tiles = empty_tiles_inside_boundaries(boundaries, pipe_map);
    std::vector<Vec2> enclosed_tiles{};

    for (auto const tile : empty_tiles) {
        static constexpr std::size_t NORTH_INDEX = 0;
        static constexpr std::size_t EAST_INDEX = 1;
        static constexpr std::size_t SOUTH_INDEX = 2;
        static constexpr std::size_t WEST_INDEX = 3;

        std::array<bool, 4> intersections{
            // north
            // east
            // south
            // west
        };

        auto [y, x] = animal_position;
        for (auto const direction : loop_directions) {
            x += (direction == East || direction == West) ? (direction == West) ? -1 : 1 : 0;
            y += (direction == North || direction == South) ? (direction == South) ? 1 : -1 : 0;

            // TODO: WHAT?
            if (x == tile.x) {
                if (y < tile.y) {
                    intersections[NORTH_INDEX] = !intersections.at(NORTH_INDEX);
                } else {
                    intersections[SOUTH_INDEX] = !intersections.at(SOUTH_INDEX);
                }
            }

            if (y == tile.y) {
                if (x < tile.x) {
                    intersections[WEST_INDEX] = !intersections.at(WEST_INDEX);
                } else {
                    intersections[EAST_INDEX] = !intersections.at(EAST_INDEX);
                }
            }
        }

        bool const enclosed = std::ranges::fold_right(intersections | std::views::all, true,
                [](auto lhs, auto rhs) { return lhs && rhs;
            });

        if (enclosed) {
            enclosed_tiles.push_back(tile);
        };
    }

    for (std::size_t i = boundaries.first.y; i <= static_cast<std::size_t>(boundaries.second.y); ++i) {
        // fmt::print("    ");
        for (std::size_t j = boundaries.first.x; j <= static_cast<std::size_t>(boundaries.second.x); ++j) {
            auto const found_symbol = std::ranges::find_if(enclosed_tiles | std::views::all,
                                                           [i, j](auto const tile_position) {
                                                               return tile_position.y == static_cast<int>(i) && tile_position.x == static_cast<int>(j);
                                                           });
            auto const tile_symbol = (found_symbol != enclosed_tiles.end()) ? 'I' : static_cast<char>(pipe_map.at(i).at(j).pipe);
            // fmt::print("{}", tile_symbol);
        }
        // fmt::print("\n");
    }

    return -1;
}
