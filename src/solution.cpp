#include <expected>
#include <format>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "solution.hpp"

Solution::Solution(int year_id,
                   int day_id,
                   int part_id,
                   fn_type solution_function,
                   std::initializer_list<std::pair<std::string_view, std::string_view>> solution_inputs,
                   fn_input_parser_type solution_input_parser)
    : year_id(year_id)
    , day_id(day_id)
    , part_id(part_id)
    , solution(solution_function)
    , inputs({solution_inputs.begin(), solution_inputs.end()})
    , input_parser(solution_input_parser)
{}

auto Solution::operator()(std::string_view input_selection) const -> std::expected<return_type, std::string> {
    if (this->inputs.contains(input_selection)) {
        auto solution_result = this->input_parser(this->inputs.at(input_selection))
            .and_then([this](input_type parsed_input) {
                    return std::optional(this->solution(parsed_input));
                    });

        if (!solution_result) {
            return std::unexpected(std::format("failed to parse solution for aoc {} day, part {}",
                                               this->year_id,
                                               this->day_id,
                                               this->part_id));
        }

        return *solution_result;
    }

    return std::unexpected(std::format("no input file path found @ {} for solution aoc {} day {}, part {}",
                                       input_selection,
                                       this->year_id,
                                       this->day_id,
                                       this->part_id));
}

auto Solution::year() const -> int {
    return this->year_id;
}

auto Solution::day() const -> int {
    return this->day_id;
}

auto Solution::part() const -> int {
    return this->part_id;
}   

