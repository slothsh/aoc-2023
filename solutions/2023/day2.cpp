#include <algorithm>
#include <cctype>
#include <iterator>
#include <tuple>
#include <numeric>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <ranges>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

struct RoundCube {
    int r = 0;
    int g = 0;
    int b = 0;
};

struct GameRound {
    int id = 0;
    std::vector<RoundCube> round_cubes = {};
    RoundCube minimum = {};
};

template<typename T>
auto parse_game_rounds(std::vector<T> const& input) -> std::vector<GameRound> {
    auto game_rounds = input
        | std::views::transform([](std::string_view game_round) {
              auto game_id_start = std::ranges::search(game_round, std::string_view{" "}).begin() + 1;
              auto game_round_start = std::ranges::search(game_round, std::string_view{":"}).begin();

              auto id = std::stoi(std::ranges::subrange(game_id_start, game_round_start).data());
              GameRound round {id, {}};

              for (auto const draws : std::ranges::subrange(game_round_start + 1, game_round.end())
                                    | std::views::split(std::string_view{";"}))
              {
                  RoundCube round_cube{};

                  for (auto const hand_pairs : std::views::split(std::string_view{draws}, std::string_view{","})) {
                      auto trimmed_pairs = hand_pairs
                          | std::views::drop_while([](char c) {
                              return c == ' ';
                            })
                          | std::views::reverse
                          | std::views::drop_while([](char c) {
                              return c == ' ';
                            })
                          | std::views::reverse
                          | std::views::split(std::string_view{" "});

                      std::string value_string{};
                      std::size_t i = 0;
                      int cube_count = 0;

                      for (auto const value : trimmed_pairs) {
                          for (auto const c : value) {
                              value_string += c;
                          }
                          switch (i) {
                              case 0: {
                                  cube_count = std::stoi(value_string);
                              } break;
                              case 1: { 
                                  if (value_string == "red") {
                                      round_cube.r += cube_count;
                                  } else if (value_string == "green") {
                                      round_cube.g += cube_count;
                                  } else if (value_string == "blue") {
                                      round_cube.b += cube_count;
                                  }
                              } break;
                              default: break;
                          }
                          ++i;
                          value_string.clear();
                      }

                  }

                  if (round_cube.r > round.minimum.r) { round.minimum.r = round_cube.r; }
                  if (round_cube.g > round.minimum.g) { round.minimum.g = round_cube.g; }
                  if (round_cube.b > round.minimum.b) { round.minimum.b = round_cube.b; }

                  round.round_cubes.push_back(round_cube);
              }

              return round;
          });

    std::vector<GameRound> all_rounds{};
    for (auto const& round : game_rounds) {
        all_rounds.push_back(round);
    }

    return all_rounds;
}

auto AoC2023::day2_part1(SolutionInput input) -> SolutionReturn {
    int max_red = 12;
    int max_green = 13;
    int max_blue = 14;

    auto game_rounds = parse_game_rounds(input);
    int acc = 0;
    for (auto const& round : game_rounds) {
        bool valid_round = true;
        for (auto const& cubes : round.round_cubes) {
            if (cubes.r > max_red ||
                cubes.g > max_green ||
                cubes.b > max_blue)
            {
                valid_round = false;
            }
        }

        if (valid_round) {
            acc += round.id;
        }
    }

    return acc;
}

auto AoC2023::day2_part2(SolutionInput input) -> SolutionReturn {
    auto game_rounds = parse_game_rounds(input);

    int acc = 0;
    for (auto const& round : game_rounds) {
        const auto [r, g, b] = round.minimum;
        acc += (r * g * b);
    }

    return acc;
}
