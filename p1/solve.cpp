#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

int main()
{
    const auto myRows = FileReader::readColumns<2>(NonCopyablePath{"p1/input.txt"});
    const auto myDecodedRows = rowsToInts(myRows);
    
    std::vector<int> myFirstColumn;
    std::vector<int> mySecondColumn;

    for (const auto& myDecodedRow : myDecodedRows)
    {
        myFirstColumn.emplace_back(myDecodedRow[0]);
        mySecondColumn.emplace_back(myDecodedRow[1]);
    }

    std::ranges::sort(myFirstColumn, std::ranges::less{});
    std::ranges::sort(mySecondColumn, std::ranges::less{});

    int myTotalDifference{0};
    for (const auto [myFirstNum, mySecondNum] : std::views::zip(myFirstColumn, mySecondColumn))
    {
        myTotalDifference += std::abs(myFirstNum - mySecondNum);
    }

    std::cout << "Solution: " << myTotalDifference << '\n';
}