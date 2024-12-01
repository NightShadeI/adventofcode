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

template<size_t NumColumns>
[[nodiscard]] inline std::vector<std::array<int, NumColumns>> rowsToInts(
    const std::vector<std::array<std::string, NumColumns>>& aRows)
{
    std::vector<std::array<int, NumColumns>> myDecodedInts;

    for (const auto& [myRowIdx, myRow] : aRows | std::views::enumerate)
    {
        auto& myDecodedRow = myDecodedInts.emplace_back();
        for (const auto& [myColIdx, myNumStr] : myRow | std::views::enumerate)
        {
            const auto myDecodedNum = toInt(myNumStr).or_else([myRowIdx]{
                throw std::runtime_error{
                    std::string{"Line "} + std::to_string(myRowIdx+1) + " was not decodable."};
                return std::optional{0};
            }).value();

            myDecodedRow[myColIdx] = myDecodedNum;
        }
    }

    return myDecodedInts;
}