#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

int main()
{
    const auto myRows = FileReader::readColumns(NonCopyablePath{"p2/input.txt"}, " ");
    const auto myDecodedRows = rowsToInts(myRows);

    int myTotal{0};
    for (const auto& myRow : myDecodedRows)
    {
        const auto myDiffSafe = [](std::int64_t aFirst, std::int64_t aSecond){ return aFirst - aSecond <= 3; };

        const bool mySafeIncrease{std::ranges::all_of(myRow | std::views::adjacent<2>, 
            [&](const auto& aWindow)
        {
            const auto [myFirst, mySecond] = aWindow;
            return myFirst > mySecond && myDiffSafe(myFirst, mySecond);
        })};

        const bool mySafeDecrease{std::ranges::all_of(myRow | std::views::adjacent<2>, 
            [&](const auto& aWindow)
        {
            const auto [myFirst, mySecond] = aWindow;
            return myFirst < mySecond && myDiffSafe(mySecond, myFirst);
        })};

        if (mySafeIncrease || mySafeDecrease) { ++myTotal; }
    }

    std::cout << "Solution: " << myTotal << '\n';
}