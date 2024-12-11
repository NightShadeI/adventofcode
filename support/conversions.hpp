#pragma once

#include <cstdint>
#include <charconv>
#include <optional>
#include <ranges>
#include <string_view>
#include <vector>

[[nodiscard]] inline std::int64_t toInt(std::string_view aString)
{
    std::int64_t myValue{};

    const auto myResult = std::from_chars(aString.data(), aString.data() + aString.size(), myValue);
    if (myResult.ptr != aString.cend())
    {
        throw std::runtime_error{"Could only partially decode integer."};
    }
    else if (myResult.ec == std::errc{})
    {
        return myValue;
    }
    else
    {
        throw std::runtime_error{"Could not decode integer."};
    }
};

[[nodiscard]] inline std::vector<std::vector<std::int64_t>> rowsToInts(
    const std::vector<std::vector<std::string>>& aRows)
{
    std::vector<std::vector<std::int64_t>> myDecodedInts;

    for (const auto& [myRowIdx, myRow] : aRows | std::views::enumerate)
    {
        auto& myDecodedRow = myDecodedInts.emplace_back();
        for (const std::string& myNumStr : myRow)
        {
            myDecodedRow.emplace_back(toInt(myNumStr));
        }
    }

    return myDecodedInts;
}