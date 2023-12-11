#include <algorithm>
#include <cctype>
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

auto AoC2023::day1_part1(SolutionInput input) -> SolutionReturn {
    std::vector<int> numbers{};
    numbers.reserve(input.size());

    auto digits = input
        | std::views::transform([](std::string_view calibration_value) {
                std::string digits{};
                std::ranges::copy(
                    calibration_value
                        | std::views::filter([](char v) { return std::isdigit(v); }),
                    std::back_inserter(digits)
                );

                std::size_t slice_start = 0;
                std::size_t slice_end = (digits.size() > 1) ? digits.size() - 1 : 0;
                std::string two_digits(2, '\0');
                std::array<char, 2> two_digits_buffer {
                    digits.at(slice_start),
                    digits.at(slice_end)
                };

                std::copy(two_digits_buffer.begin(), two_digits_buffer.end(),
                          two_digits.begin());

                return std::stoi(two_digits);
            });

    return std::accumulate(digits.begin(), digits.end(), 0);
}

auto AoC2023::day1_part2(SolutionInput input) -> SolutionReturn {
    static std::array<std::tuple<std::string_view, std::string_view>, 10> number_dictionary {
        std::tuple{"0", "zero"},
        std::tuple{"1", "one"},
        std::tuple{"2", "two"},
        std::tuple{"3", "three"},
        std::tuple{"4", "four"},
        std::tuple{"5", "five"},
        std::tuple{"6", "six"},
        std::tuple{"7", "seven"},
        std::tuple{"8", "eight"},
        std::tuple{"9", "nine"},
    };

    auto digits = input
        | std::views::transform([](std::string_view calibration_value) {
              int first_value = -1;
              int last_value = -1;
              auto first_word = std::ranges::subrange{calibration_value.end(), calibration_value.end()};
              auto last_word = std::ranges::subrange{std::views::reverse(calibration_value).end(), std::views::reverse(calibration_value).end()};

              for (auto const& [i, number_elements] : std::views::enumerate(number_dictionary)) {
                  auto const& [number_digit, number_word] = number_elements;
                  auto digit_slice_start = std::ranges::search(calibration_value, number_digit);
                  auto word_slice_start = std::ranges::search(calibration_value, number_word);

                  if (digit_slice_start.begin() < first_word.begin() || word_slice_start.begin() < first_word.begin()) {
                      first_word = (digit_slice_start.begin() < word_slice_start.begin()) ? digit_slice_start : word_slice_start;
                      first_value = i;
                  }

                  auto digit_slice_end = std::ranges::search(std::views::reverse(calibration_value), std::views::reverse(number_digit));
                  auto word_slice_end = std::ranges::search(std::views::reverse(calibration_value), std::views::reverse(number_word));

                  if (digit_slice_end.begin() < last_word.begin() || word_slice_end.begin() < last_word.begin()) {
                      last_word = (digit_slice_end.begin() < word_slice_end.begin()) ? digit_slice_end : word_slice_end;
                      last_value = i;
                  }
              }

              return (first_value * 10) + last_value;
          });

    return std::accumulate(digits.begin(), digits.end(), 0);
}
