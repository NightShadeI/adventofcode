#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

[[nodiscard]] constexpr std::int64_t customPow(std::int64_t aBase, std::int64_t aExponent) noexcept
{
    if (aExponent <= 0)
    {
        return 1;
    }
    return aBase * customPow(aBase, aExponent-1);
}

[[nodiscard]] std::int64_t totalDigits(std::int64_t aNumber)
{
    std::int64_t myCount{1};
    while (aNumber /= 10) {
        ++myCount;
    }
    return myCount;
}

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

        static constexpr std::int64_t POSSIBLE_OPERATORS{3};
        const std::int64_t myTotalPermutations{customPow(POSSIBLE_OPERATORS, myOperands.size()-1)};
        for (std::int32_t myPermutation{0}; myPermutation < myTotalPermutations; ++myPermutation)
        {
            const std::int64_t myPermutationCalc{std::ranges::fold_left(myOperands | std::views::drop(1) | std::views::enumerate, myOperands.front(),
                [myPermutation, myCurrentPermutation = myPermutation](std::int64_t aCalculatedVal, auto aElementPair) mutable
            {
                const auto [myIdx, myOperand] = aElementPair;
                const std::int64_t myOperatorId{myCurrentPermutation % POSSIBLE_OPERATORS};
                myCurrentPermutation /= POSSIBLE_OPERATORS;
                if (myOperatorId == 2)
                {
                    const std::int64_t myTotalDigits{totalDigits(myOperand)};
                    return (aCalculatedVal * customPow(10, myTotalDigits) + myOperand);
                }
                else if (myOperatorId == 1)
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