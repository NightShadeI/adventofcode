#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <unordered_map>

int main()
{
    const auto myRows = FileReader::readColumns(NonCopyablePath{"p1/input.txt"});
    const auto myDecodedRows = rowsToInts(myRows);

    std::unordered_map<std::int64_t, std::int64_t> myValueCounts;

    for (const auto& myDecodedRow : myDecodedRows)
    {
        ++myValueCounts[myDecodedRow[1]];
    }

    const std::int64_t myTotalScore{std::ranges::fold_left(myDecodedRows, 0,
        [&myValueCounts](std::int64_t aCurrentCount, const auto& aCurrentRow)
    {
        const std::int64_t myLocationId{aCurrentRow[0]}; 
        return aCurrentCount + myLocationId * myValueCounts[myLocationId];
    })};

    std::cout << "Solution: " << myTotalScore << '\n';
}