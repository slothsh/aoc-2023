#include <algorithm>
#include <cctype>
#include <cmath>
#include <numeric>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <ranges>
#include <map>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

struct ScratchCard {
    int id;
    std::vector<int> winning_numbers;
    std::vector<int> draw_numbers;
};

template<typename T>
auto parse_scratch_cards(T const& scratch_cards_input) -> std::vector<ScratchCard> {
    std::vector<ScratchCard> scratch_cards{};

    auto parsed_scratch_cards = scratch_cards_input
        | std::views::transform([](std::string_view scratch_card_line) {
                ScratchCard scratch_card{};

                auto scratch_card_split = std::views::split(scratch_card_line, ':');

                for (auto card_id_part : scratch_card_split | std::views::take(1)) {
                    auto id_number_part = card_id_part
                        | std::views::split(' ')
                        | std::views::drop(1);

                    for (auto id_view : id_number_part
                                      | std::views::filter([](auto&& value) { return std::string_view{value} != ""; }))
                    {
                        scratch_card.id = std::stoi(std::string{id_view.begin(), id_view.end()});
                    }
                }

                for (auto scratch_card_numbers_part : scratch_card_split | std::views::drop(1)) {
                    auto number_parts = scratch_card_numbers_part
                        | std::views::split('|');

                    for (auto [i, numbers_range] : number_parts | std::views::enumerate) {
                        std::string_view numbers_view{numbers_range};
                        auto single_numbers = numbers_view
                            | std::views::split(' ');

                        for (auto number_range : single_numbers
                                               | std::views::filter([](auto&& value) { return std::string_view{value} != ""; }))
                        {
                            int n = std::stoi(std::string(number_range.begin(), number_range.end()));
                            if (i == 0) {
                                scratch_card.winning_numbers.emplace_back(n);
                            } else if (i == 1) {
                                scratch_card.draw_numbers.emplace_back(n);
                            } 
                        }
                    }
                }

                return scratch_card;
            });

    std::ranges::copy(parsed_scratch_cards, std::back_inserter(scratch_cards));

    return scratch_cards;
}

template<typename T>
auto map_total_scratch_cards(T const& scratch_cards) -> std::map<int, int> {
    std::map<int, int> total_scratch_cards{};

    for (const auto& card : scratch_cards) {
        total_scratch_cards[card.id] += 1;
    }

    return total_scratch_cards;
}

auto AoC2023::day4_part1(SolutionInput input) -> SolutionReturn {
    auto scratch_cards = parse_scratch_cards(input);
    int acc = 0;

    for (auto const& card : scratch_cards) {
        int exponent = 0;

        for (auto const winning_number : card.winning_numbers) {
            for (auto const draw_number : card.draw_numbers) {
                if (winning_number == draw_number) {
                    ++exponent;
                } 
            }
        }

        acc += (exponent > 0)
            ? std::pow(2, exponent - 1)
            : 0;
    }

    return acc;
}

auto AoC2023::day4_part2(SolutionInput input) -> SolutionReturn {
    auto scratch_cards = parse_scratch_cards(input);
    auto total_scratch_cards = map_total_scratch_cards(scratch_cards);
    int max_id = scratch_cards.back().id;

    for (auto const& card : scratch_cards) {
        int winning_numbers = 0;

        for (auto const winning_number : card.winning_numbers) {
            for (auto const draw_number : card.draw_numbers) {
                if (winning_number == draw_number) {
                    ++winning_numbers;
                } 
            }
        }

        for (int i = total_scratch_cards.at(card.id); i > 0; --i) {
            for (int id = card.id + 1; id <= std::min(max_id, card.id + winning_numbers); ++id) {
                total_scratch_cards[id] += 1;
            }
        }
    }

    return std::accumulate(total_scratch_cards.begin(), total_scratch_cards.end(), 0,
                           [](int acc, std::pair<int, int> card_totals) {
                               return acc + card_totals.second;
                           });
}
