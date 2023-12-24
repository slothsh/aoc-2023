#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <numeric>
#include <string>
#include <string_view>
#include <array>
#include <utility>
#include <unordered_map>
#include <vector>
#include <ranges>
#include <format>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

enum Card : int {
    Two = '2',
    Three = '3',
    Four = '4',
    Five = '5',
    Six = '6',
    Seven = '7',
    Eight = '8',
    Nine = '9',
    Ten = 'T',
    Jack = 'J',
    Queen = 'Q',
    King = 'K',
    Ace = 'A'
};

enum HandType : int {
    None = 0,
    HighCard,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    FullHouse,
    FourOfAKind,
    FiveOfAKind
};

struct Hand {
    using cards_type = std::array<Card, 5>;
    using verifier_type = std::vector<bool(*)(cards_type const&)>;

    cards_type cards;
    int bid;
    HandType hand;

    Hand() = default;

    Hand(std::string_view cards, int bid)
        : cards(Hand::fill_cards(cards, this->cards))
        , bid(bid)
        , hand(Hand::compute_hand(*this))
    {
    }

    static const verifier_type verifiers;
    static bool enable_joker;

    static auto compute_hand(Hand const& hand_in) -> HandType {
        for (auto const [i, verify] : Hand::verifiers | std::views::reverse | std::views::enumerate) {
            if (verify(hand_in.cards)) {
                return static_cast<HandType>(i + 2);
            }
        }

        return HandType::HighCard;
    }

    static auto to_sequence_value(Card card) -> int {
        switch (card) {
            case Card::Two:   { return 0; } break;
            case Card::Three: { return 1; } break;
            case Card::Four:  { return 2; } break;
            case Card::Five:  { return 3; } break;
            case Card::Six:   { return 4; } break;
            case Card::Seven: { return 5; } break;
            case Card::Eight: { return 6; } break;
            case Card::Nine:  { return 7; } break;
            case Card::Ten:   { return 8; } break;
            case Card::Jack:  { return (Hand::enable_joker) ? -1 : 9; } break;
            case Card::Queen: { return 10; } break;
            case Card::King:  { return 11; } break;
            case Card::Ace:   { return 12; } break;
            default: return -1; // UNREACHABLE
        }

        // UNREACHABLE
        return -1;
    }

    static auto check_joker_and_modify(std::unordered_map<Card, int>& cards_map) -> void {
        bool contains_joker = std::find_if(cards_map.begin(), cards_map.end(),
                                           [](auto const& entry) { return entry.first == Card::Jack; }) != cards_map.end();

        if (contains_joker) {
            auto max_key = std::max_element(cards_map.begin(), cards_map.end(),
                                            [](auto const& lhs, auto const& rhs) {
                                                if (lhs.first == Card::Jack && rhs.first != Card::Jack) return true;
                                                if (lhs.first != Card::Jack && rhs.first == Card::Jack) return false;

                                                if (lhs.second == rhs.second) {
                                                    return Hand::to_sequence_value(lhs.first) < Hand::to_sequence_value(rhs.first);
                                                }

                                                return lhs.second < rhs.second;
                                            })->first;

            if (max_key != Card::Jack) {
                cards_map[max_key] += cards_map.at(Card::Jack);
                cards_map.erase(Card::Jack);
            }
        }
    }

    static auto verify_five_of_a_kind(cards_type const& cards_in) -> bool {
        std::unordered_map<Card, int> cards_map{};

        for (auto const c : cards_in) {
            cards_map[c] += 1;
        }

        if (Hand::enable_joker) {
            check_joker_and_modify(cards_map);
        }

        bool five_of_a_kind = std::find_if(cards_map.begin(), cards_map.end(),
                                           [](auto const& entry) {
                                               return entry.second == 5;
                                           }) != cards_map.end();

        return cards_map.size() == 1 && five_of_a_kind;
    }

    static auto verify_four_of_a_kind(cards_type const& cards_in) -> bool {
        std::unordered_map<Card, int> cards_map{};

        for (auto const c : cards_in) {
            cards_map[c] += 1;
        }

        if (Hand::enable_joker) {
            check_joker_and_modify(cards_map);
        }

        bool four_of_a_kind = std::find_if(cards_map.begin(), cards_map.end(),
                                           [](auto const& entry) {
                                               return entry.second == 4;
                                           }) != cards_map.end();

        return cards_map.size() == 2 && four_of_a_kind;
    }

    static auto verify_full_house(cards_type const& cards_in) -> bool {
        std::unordered_map<Card, int> cards_map{};

        for (auto const c : cards_in) {
            cards_map[c] += 1;
        }

        if (Hand::enable_joker) {
            check_joker_and_modify(cards_map);
        }

        bool three_of_a_kind = std::find_if(cards_map.begin(), cards_map.end(),
                                            [](auto const& entry) {
                                                return entry.second == 3;
                                            }) != cards_map.end();

        bool two_of_a_kind = std::find_if(cards_map.begin(), cards_map.end(),
                                          [](auto const& entry) {
                                              return entry.second == 2;
                                          }) != cards_map.end();

        return cards_map.size() == 2 && three_of_a_kind && two_of_a_kind;
    }

    static auto verify_three_of_a_kind(cards_type const& cards_in) -> bool {
        std::unordered_map<Card, int> cards_map{};

        for (auto const c : cards_in) {
            cards_map[c] += 1;
        }

        if (Hand::enable_joker) {
            check_joker_and_modify(cards_map);
        }

        bool three_of_a_kind = std::find_if(cards_map.begin(), cards_map.end(),
                                            [](auto const& entry) {
                                                return entry.second == 3;
                                            }) != cards_map.end();

        return cards_map.size() == 3 && three_of_a_kind;
    }

    static auto verify_two_pair(cards_type const& cards_in) -> bool {
        std::unordered_map<Card, int> cards_map{};

        for (auto const c : cards_in) {
            cards_map[c] += 1;
        }

        if (Hand::enable_joker) {
            check_joker_and_modify(cards_map);
        }

        bool three_of_a_kind = std::find_if(cards_map.begin(), cards_map.end(),
                                            [](auto const& entry) {
                                                return entry.second == 3;
                                            }) != cards_map.end();

        return cards_map.size() == 3 && !three_of_a_kind;
    }

    static auto verify_one_pair(cards_type const& cards_in) -> bool {
        std::unordered_map<Card, int> cards_map{};

        for (auto const c : cards_in) {
            cards_map[c] += 1;
        }

        if (Hand::enable_joker) {
            check_joker_and_modify(cards_map);
        }

        bool two_of_a_kind = std::find_if(cards_map.begin(), cards_map.end(),
                                          [](auto const& entry) {
                                              return entry.second == 2;
                                          }) != cards_map.end();

        return cards_map.size() == 4 && two_of_a_kind;
    }

    friend auto operator<(Hand const& lhs, Hand const& rhs) -> bool {
        if (lhs.hand == rhs.hand) {
            for (std::size_t i = 0; i < lhs.cards.size(); ++i) {
                if (lhs.cards.at(i) != rhs.cards.at(i)) {
                    return Hand::to_sequence_value(lhs.cards.at(i)) < Hand::to_sequence_value(rhs.cards.at(i));
                }
            }

            // UNREACHABLE
            return false;
        }

        return lhs.hand < rhs.hand;
    }

private:
    static auto fill_cards(std::string_view cards_view, cards_type& cards) -> cards_type& {
        for (auto const [i, c] : cards_view | std::views::enumerate) {
            cards[i] = static_cast<Card>(c);
        }
        
        return cards;
    }
};

Hand::verifier_type const Hand::verifiers {
    &Hand::verify_five_of_a_kind,
    &Hand::verify_four_of_a_kind,
    &Hand::verify_full_house,
    &Hand::verify_three_of_a_kind,
    &Hand::verify_two_pair,
    &Hand::verify_one_pair
};

bool Hand::enable_joker = false;

auto parse_game_hands(SolutionInput game_hands) -> std::vector<Hand> {
    std::vector<Hand> hands{};

    auto parsed_hands = game_hands
        | std::views::transform([](auto game_hand) {
                auto hand_bid_parts = game_hand
                    | std::views::split(' ');

                std::string_view cards{};
                int bid = 0;
                for (auto const [i, part] : hand_bid_parts | std::views::enumerate)  {
                    std::string_view value{part};
                    if (i == 0) {
                        cards = value;
                    } else if (i == 1) {
                        bid = std::stol(std::string{value.begin(), value.end()});
                    }
                }

                return Hand {
                    cards,
                    bid
                };
            });

    std::ranges::copy(parsed_hands.begin(), parsed_hands.end(), std::back_inserter(hands));

    return hands;
}

auto AoC2023::day7_part1(SolutionInput input) -> SolutionReturn {
    auto game_hands = parse_game_hands(input);
    std::sort(game_hands.begin(), game_hands.end());

    int total_winnings = 0;
    for (auto const& [i, hand] : game_hands | std::views::enumerate) {
        total_winnings += hand.bid * (i + 1);
    }

    return total_winnings;
}

auto AoC2023::day7_part2(SolutionInput input) -> SolutionReturn {
    Hand::enable_joker = true;
    auto game_hands = parse_game_hands(input);
    std::sort(game_hands.begin(), game_hands.end());

    int total_winnings = 0;
    for (auto const& [i, hand] : game_hands | std::views::enumerate) {
        total_winnings += hand.bid * (i + 1);
    }

    return total_winnings;
}
