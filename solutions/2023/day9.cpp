#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <numeric>
#include <functional>
#include <string>
#include <iterator>
#include <type_traits>
#include <string_view>
#include <utility>
#include <vector>
#include <ranges>
#include <memory>
#include <span>
#include <optional>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

struct PolynomialSequence {
    std::vector<std::int64_t> values;
    int depth;
    std::unique_ptr<PolynomialSequence> next;

    PolynomialSequence(std::span<std::int64_t> values_view, int depth_in)
        : values({})
        , depth(depth_in)
        , next(nullptr)
    {
        std::ranges::move(values_view, std::back_inserter(this->values));
        auto differences = this->values
            | std::views::pairwise
            | std::views::transform([](auto const& pair) {
                    return std::get<1>(pair) - std::get<0>(pair);
                });

        // https://en.wikipedia.org/wiki/Arithmetic_progression#Sum
        // Interestingly, the arithmetic sum only works for integers >= 0
        // auto arithmetic_sum = (differences.size() * (differences.front() + differences.back())) / 2;

        if (std::ranges::find_if_not(differences, [](auto const n) { return n == 0; }) != differences.end()) {
            std::vector<std::int64_t> next_values{};
            std::ranges::move(differences, std::back_inserter(next_values));
            this->next.reset(new PolynomialSequence{ next_values, depth_in + 1 });
        }
    }

    auto at(int depth) -> std::optional<std::reference_wrapper<PolynomialSequence>> {
        if (depth == this->depth) {
            return std::ref(*this);
        } else if (this->next != nullptr) {
            return this->next->at(depth);
        }

        return std::nullopt;
    }

    auto max_depth() -> int {
        if (this->next == nullptr) {
            return this->depth;
        }

        return this->next->max_depth();
    }
};

auto parse_oasis_report(SolutionInput oasis_report) -> std::vector<PolynomialSequence> {
    std::vector<PolynomialSequence> report_data{};
    report_data.reserve(oasis_report.size());

    std::ranges::for_each(oasis_report, [&report_data](auto const& report_entry) {
            auto report_values = report_entry
                | std::views::split(' ')
                | std::views::transform([](auto const value) {
                        std::string_view value_view{value};
                        return std::stol(std::string{value_view.begin(), value_view.end()});
                    });

            std::vector<std::int64_t> values{};
            std::ranges::move(report_values, std::back_inserter(values));
            report_data.emplace_back(values, 0);
        });

    return report_data;
}

auto AoC2023::day9_part1(SolutionInput input) -> SolutionReturn {
    auto report_data = parse_oasis_report(input);

    std::vector<int> predictions{};
    for (auto& sequence : report_data) {
        int depth = sequence.max_depth();
        std::int64_t prediction = 0;
        while (depth >= 0) {
            auto node = sequence.at(depth);
            if (node.has_value()) {
                prediction += node->get().values.back();
            }
            --depth;
        }
        predictions.push_back(prediction);
    }

    return std::accumulate(predictions.begin(), predictions.end(), 0, std::plus<std::int64_t>());
}

auto AoC2023::day9_part2(SolutionInput input) -> SolutionReturn {
    auto report_data = parse_oasis_report(input);

    std::vector<int> predictions{};
    for (auto& sequence : report_data) {
        int depth = sequence.max_depth();
        std::int64_t prediction = 0;
        while (depth >= 0) {
            auto node = sequence.at(depth);
            if (node.has_value()) {
                prediction = node->get().values.front() - prediction;
            }
            --depth;
        }
        predictions.push_back(prediction);
    }

    return std::accumulate(predictions.begin(), predictions.end(), 0, std::plus<std::int64_t>());
}
