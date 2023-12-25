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
#include <map>
#include <vector>
#include <ranges>
#include <format>
#include <thread>
#include <fmt/core.h>

#include "aoc2023.hpp"
#include "solution.hpp"

struct Network {
    using node_type = std::pair<std::string, std::string>;
    using network_type = std::map<std::string, node_type>;

    std::string steps;
    network_type network;
    std::size_t cursor;

    auto next(std::string const& current_node) -> std::string_view {
        auto& node = (this->steps.at(this->cursor) == 'L')
            ? this->network.at(current_node).first
            : this->network.at(current_node).second;

        this->advance();

        return node;
    }

    auto peek(std::string const& current_node) -> std::string_view {
        return (this->steps.at(this->cursor) == 'L')
            ? this->network.at(current_node).first
            : this->network.at(current_node).second;
    }

    auto advance() -> void {
        if (this->cursor >= steps.size() - 1) {
            this->cursor = 0;
        } else {
            ++this->cursor;
        }
    }

    auto reset_cursor() -> void {
        this->cursor = 0;
    }

    static auto parse_node(std::string_view node_expression) -> std::pair<std::string, node_type> {
        auto expression_parts = node_expression
            | std::views::filter([](auto c) { return c != ' ' && c != '(' && c != ')'; })
            | std::views::split('=');

        std::string node_key{expression_parts.front().begin(), expression_parts.front().end()};
        std::string left_edge;
        std::string right_edge;

        for (auto const left_right : expression_parts | std::views::drop(1)) {
            auto edge_node_parts = left_right
                | std::views::split(',');

            for (auto const [i, edge] : edge_node_parts | std::views::enumerate) {
                std::string& edge_container = (i == 0)
                    ? left_edge
                    : right_edge;
                edge_container = std::string{edge.begin(), edge.end()};
            }
        }

        return {
            node_key,
            { left_edge, right_edge }
        };
    }

    static auto parse_network(SolutionInput network_description) -> Network {
    Network network_map{};

    auto network_chunks = network_description
        | std::views::chunk_by([](auto lhs, auto rhs) {
                return lhs != "" && rhs != "";
            })
        | std::views::filter([](auto chunk) {
                return chunk[0] != "";
            });

    for (auto const [i, chunk] : network_chunks | std::views::enumerate) {
        if (i == 0) {
            network_map.steps = chunk[0];
        } else {
            for (auto const& segment : chunk) {
                auto node = Network::parse_node(segment);
                network_map.network[node.first] = node.second;
            }
        }
    }

    return network_map;
}

};

auto AoC2023::day8_part1(SolutionInput input) -> SolutionReturn {
    auto network_map = Network::parse_network(input);

    std::string current_node{ "AAA" };
    std::string_view destination_node{ "ZZZ" };

    int total_steps = 0;
    while (current_node != destination_node) {
        current_node = network_map.next(current_node);
        ++total_steps;
    }

    return total_steps;
}

auto AoC2023::day8_part2(SolutionInput input) -> SolutionReturn {
    auto network_map = Network::parse_network(input);

    auto starting_nodes_view = network_map.network
        | std::views::filter([](auto const& node) { return node.first.back() == 'A'; })
        | std::views::transform([](auto const& node) { return std::tuple { node.first, 0ll, 0ll }; });

    std::vector<std::tuple<std::string, std::int64_t, std::int64_t>> starting_nodes {
        starting_nodes_view.begin(),
        starting_nodes_view.end()
    };

    auto constexpr identical_nodes = [](auto const& node) {
        return node.back() == 'Z';
    };

    for (auto& [current_node, steps_to_z, total_steps] : starting_nodes) {
        while (!identical_nodes(current_node)) {
            current_node = network_map.next(current_node);
            ++steps_to_z;
        }
        network_map.reset_cursor();
    }

    auto const highest_steps = std::get<1>(
        *std::max_element(starting_nodes.begin(), starting_nodes.end(),
                          [](auto const& lhs, auto const& rhs) { return std::get<1>(lhs) < std::get<1>(rhs); })
    );

    auto const mod_total_steps = [highest_steps](auto const& node) {
        auto const total_steps = std::get<2>(node);
        if (total_steps == 0)
            return false;
        auto const steps_to_z = std::get<1>(node);
        auto const step = total_steps / steps_to_z;
        return (highest_steps * step) % steps_to_z == 0;
    };

    std::int64_t step = 1;
    while (std::find_if_not(starting_nodes.begin(), starting_nodes.end(), mod_total_steps) != starting_nodes.end()) {
        for (auto& [current_node, steps_to_z, total_steps] : starting_nodes) {
            total_steps = steps_to_z * step;
        }
        ++step;
    }

    return std::get<2>(
        *std::max_element(starting_nodes.begin(), starting_nodes.end(),
                          [](auto const& lhs, auto const& rhs) { return std::get<2>(lhs) < std::get<2>(rhs); })
    );
}
