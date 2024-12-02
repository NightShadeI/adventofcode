#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

template <typename R, typename InequalityCheckT>
[[nodiscard]] bool validateRange(R&& aRange, InequalityCheckT anInequalityCheck)
{
    const auto myGapSafe{[anInequalityCheck](int aFirst, int aSecond){ 
        return std::abs(aFirst - aSecond) <= 3 && anInequalityCheck(aFirst, aSecond); }};
    
    const auto myPairedView{aRange | std::views::adjacent<3>};
    const auto myValidForwardsIt{std::ranges::find_if_not(myPairedView, [&](const auto aPoint){
        const auto [myFirst, mySecond, _] = aPoint;
        return myGapSafe(myFirst, mySecond);
    })};

    const auto myReversePairView{myPairedView | std::views::reverse};
    const auto myValidBackwardsIt{std::ranges::find_if_not(myReversePairView, [&](const auto aPoint){
        const auto [_, mySecond, myThird] = aPoint;
        return myGapSafe(mySecond, myThird);
    })};

    if (myValidForwardsIt == myPairedView.cend() || myValidBackwardsIt == myReversePairView.cend())
        return true;

    if (const int64_t myPairGap{std::distance(myValidForwardsIt, myValidBackwardsIt.base()+1)-2}; myPairGap <= 0)
    {
        const auto [myFirstFwdSafe, mySecondFwdSafe, myThirdFwdSafe] = *myValidForwardsIt;
        const auto [myFirstBackSafe, mySecondBackSafe, myThirdBackSafe] = *myValidBackwardsIt;
        return myGapSafe(myFirstFwdSafe, myThirdFwdSafe) || myGapSafe(myFirstBackSafe, myThirdBackSafe);
    }
    
    return false;
}

int main()
{
    const auto myRows{FileReader::readColumns(NonCopyablePath{"p2/input.txt"}, " ")};
    const auto myDecodedRows{rowsToInts(myRows)};

    const int64_t myTotal{std::ranges::count_if(myDecodedRows, [](const auto& aRow){
        return validateRange(aRow, std::greater<int>{}) || validateRange(aRow, std::less<int>{});
    })};

    std::cout << "Solution: " << myTotal << '\n';
}