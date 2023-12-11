#pragma once

#include <optional>
#include <string_view>

#include "solution.hpp"

namespace parsing {
    auto parse_lines(std::string_view path) -> std::optional<SolutionInputValue>;
} // END of namespace parsing

