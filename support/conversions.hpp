#pragma once

#include <charconv>
#include <optional>
#include <ranges>
#include <string_view>
#include <vector>

[[nodiscard]] inline std::optional<int> toInt(std::string_view aString)
{
    int myValue{};

    const auto myResult = std::from_chars(aString.data(), aString.data() + aString.size(), myValue);
    if (myResult.ptr != aString.cend())
    {
        return std::nullopt;
    }
    else if (myResult.ec == std::errc{})
    {
        return myValue;
    }
    else
    {
        return std::nullopt;
    }
};

[[nodiscard]] inline std::vector<std::vector<int>> rowsToInts(
    const std::vector<std::vector<std::string>>& aRows)
{
    std::vector<std::vector<int>> myDecodedInts;

    for (const auto& [myRowIdx, myRow] : aRows | std::views::enumerate)
    {
        auto& myDecodedRow = myDecodedInts.emplace_back();
        for (const std::string& myNumStr : myRow)
        {
            const auto myDecodedNum = toInt(myNumStr).or_else([myRowIdx]{
                throw std::runtime_error{
                    std::string{"Line "} + std::to_string(myRowIdx+1) + " was not decodable."};
                return std::optional{0};
            }).value();

            myDecodedRow.emplace_back(myDecodedNum);
        }
    }

    return myDecodedInts;
}