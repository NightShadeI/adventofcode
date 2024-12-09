#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

struct Space
{
    std::int64_t theMemoryId{};
    std::int64_t theOffset{};
    std::int64_t theLength{};
};

int main()
{
    std::string myMemoryRules{};
    std::vector<Space> myTakenMemory{};
    std::vector<Space> myFreeMemory{};
    std::int64_t myCurrentOffset{0};
    FileReader::processLines(NonCopyablePath{"p9/input.txt"}, [&](const std::string& aLine){
        myMemoryRules = aLine;
    });

    for (const auto [myMemoryId, myMemoryRule] : myMemoryRules | std::views::chunk(2) | std::views::enumerate)
    {
        if (myMemoryRule.size() < 2) break;

        const std::int64_t myTaken{myMemoryRule[0] - '0'};
        const std::int64_t myFree{myMemoryRule[1] - '0'};

        myTakenMemory.emplace_back(myMemoryId, myCurrentOffset, myTaken);
        myCurrentOffset += myTaken;

        myFreeMemory.emplace_back(myMemoryId, myCurrentOffset, myFree);
        myCurrentOffset += myFree;
    }

    const std::int64_t myFinalId{std::ssize(myMemoryRules)/2};
    if (myMemoryRules.size() % 2 == 1)
    {
        const std::int64_t myTaken{myMemoryRules.back() - '0'};
        myTakenMemory.emplace_back(myFinalId, myCurrentOffset, myTaken);
        myCurrentOffset += myTaken;
    }

    std::span myTakenView{myTakenMemory};

    while (!myTakenView.empty())
    {
        Space& myQuerySpace{myTakenView.back()};
        const auto myFirstFreeIt{std::ranges::find_if(myFreeMemory, [&](const auto& aFreeSpace){
            return myQuerySpace.theOffset > aFreeSpace.theOffset && aFreeSpace.theLength >= myQuerySpace.theLength;
        })};
        if (myFirstFreeIt != myFreeMemory.cend())
        {
            myQuerySpace.theOffset = myFirstFreeIt->theOffset;
            if (myFirstFreeIt->theLength == myQuerySpace.theLength)
            {
                myFreeMemory.erase(myFirstFreeIt);
            }
            else
            {
                myFirstFreeIt->theOffset += myQuerySpace.theLength;
                myFirstFreeIt->theLength -= myQuerySpace.theLength;
            }
        }
        myTakenView = myTakenView.subspan(0, myTakenView.size() - 1);
    }

    const std::int64_t myChecksum{std::ranges::fold_left(myTakenMemory, 0ll, [](std::int64_t aCurrent, const auto& myTakenSpace){
        std::int64_t myMemoryCost{0};
        for (std::int64_t myOffset{myTakenSpace.theOffset}; myOffset < myTakenSpace.theOffset + myTakenSpace.theLength; ++myOffset)
        {
            myMemoryCost += myOffset * myTakenSpace.theMemoryId;
        }
        return aCurrent + myMemoryCost;
    })};

    std::cout << "Solution: " << myChecksum << '\n';
}