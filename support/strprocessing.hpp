#pragma once

#include <string_view>
#include <ranges>
#include <vector>

[[nodiscard]] std::vector<std::string_view> splitBy(
    std::string_view aLine, std::string_view aDelim)
{
    std::vector<std::string_view> myParts;
    for (const auto myPart : std::views::split(aLine, aDelim))
    {
        myParts.emplace_back(myPart);
    }
    return myParts;
}