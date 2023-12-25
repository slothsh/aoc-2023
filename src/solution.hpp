#pragma once

#include <expected>
#include <cstdint>
#include <initializer_list>
#include <unordered_map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

class Solution {
public:
    using return_type = std::int64_t;
    using input_value_type = std::vector<std::string>;
    using input_type = std::add_const_t<input_value_type>&;
    using fn_type = return_type(*)(input_type);
    using fn_input_parser_type = std::optional<input_value_type>(*)(std::string_view);

    Solution() = delete;

    explicit Solution(int year_id,
                      int day_id,
                      int part_id,
                      fn_type solution_function,
                      std::initializer_list<std::pair<std::string_view, std::string_view>> solution_inputs,
                      fn_input_parser_type solution_input_parser);

    auto operator()(std::string_view input_selection) const -> std::expected<return_type, std::string>;

    auto year() const -> int;
    auto day() const -> int;
    auto part() const -> int;

private:
    const int year_id;
    const int day_id;
    const int part_id;
    const fn_type solution;
    const std::unordered_map<std::string_view, std::string_view> inputs;
    const fn_input_parser_type input_parser;
};

using SolutionInput = typename Solution::input_type;
using SolutionInputValue = typename Solution::input_value_type;
using SolutionReturn = typename Solution::return_type;
