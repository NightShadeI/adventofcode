#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

int main()
{
    const auto myRows = FileReader::readColumns(NonCopyablePath{"p7/input.txt"}, ": ");
    std::int64_t myTotalVal{std::ranges::fold_left(myRows, 0ll, [](std::int64_t aCurrent, const auto aRow){
        const std::int64_t myDesiredNumber{toInt(aRow[0])};
        std::vector<std::int64_t> myOperands;
        for (const auto myOperandStr : aRow[1] | std::views::split(' '))
        {
            myOperands.emplace_back(toInt(std::string_view{myOperandStr}));
        }

        const std::int32_t myTotalPermutations{1 << (myOperands.size()-1)};
        for (std::int32_t myPermutation{0}; myPermutation < myTotalPermutations; ++myPermutation)
        {
            const std::int64_t myPermutationCalc{std::ranges::fold_left(myOperands | std::views::drop(1) | std::views::enumerate, myOperands.front(),
                [myPermutation](std::int64_t aCalculatedVal, auto aElementPair)
            {
                const auto [myIdx, myOperand] = aElementPair;
                const bool myUseMult{((1 << myIdx) & myPermutation) > 0};
                if (myUseMult)
                {
                    return aCalculatedVal * myOperand;
                }
                else
                {
                    return aCalculatedVal + myOperand;
                }
            })};
            if (myPermutationCalc == myDesiredNumber)
            {
                return aCurrent + myPermutationCalc;
            }
        }
        return aCurrent;
    })};
    std::cout << "Solution: " << myTotalVal << '\n';
}