#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

enum class Availablity
{
    FREE,
    TAKEN
};

struct Space
{
    std::int64_t theMemoryId{};
    Availablity theAvailability{};
};

int main()
{
    std::string myMemoryRules{};
    std::vector<Space> myVirtualMemory{};
    FileReader::processLines(NonCopyablePath{"p9/input.txt"}, [&](const std::string& aLine){
        myMemoryRules = aLine;
    });

    for (const auto [myMemoryId, myMemoryRule] : myMemoryRules | std::views::chunk(2) | std::views::enumerate)
    {
        if (myMemoryRule.size() < 2) break;

        const std::int64_t myTaken{myMemoryRule[0] - '0'};
        const std::int64_t myFree{myMemoryRule[1] - '0'};

        std::ranges::fill_n(std::back_insert_iterator(myVirtualMemory), myTaken, Space{myMemoryId, Availablity::TAKEN});
        std::ranges::fill_n(std::back_insert_iterator(myVirtualMemory), myFree, Space{myMemoryId, Availablity::FREE});
    }

    const std::int64_t myFinalId{std::ssize(myMemoryRules)/2};
    if (myMemoryRules.size() % 2 == 1)
    {
        const std::int64_t myTaken{myMemoryRules.back() - '0'};
        std::ranges::fill_n(std::back_insert_iterator(myVirtualMemory),
            myTaken, Space{myFinalId, Availablity::TAKEN});
    }

    auto myFreeIt{myVirtualMemory.begin()};
    auto myTakenIt{std::prev(myVirtualMemory.end())};

    while (std::distance(myFreeIt, myVirtualMemory.end()) > std::distance(myTakenIt, myVirtualMemory.end()))
    {
        while (myFreeIt->theAvailability != Availablity::FREE && ++myFreeIt != myVirtualMemory.cend());
        while (myTakenIt->theAvailability != Availablity::TAKEN && --myTakenIt != myVirtualMemory.begin());

        if (std::distance(myFreeIt, myVirtualMemory.end()) <= std::distance(myTakenIt, myVirtualMemory.end())) break;

        if (myFreeIt == myVirtualMemory.begin() || myTakenIt == myVirtualMemory.cend()) break;


        std::iter_swap(myFreeIt, myTakenIt);

        ++myFreeIt; --myTakenIt;

    }

    const std::int64_t myChecksum{std::ranges::fold_left(myVirtualMemory | std::views::enumerate, 0ll, [](std::int64_t aCurrent, auto aMemoryInfo){
        const auto [myMemoryIdx, myMemorySpace] = aMemoryInfo;
        const std::int64_t myMemoryCost{myMemorySpace.theAvailability == Availablity::TAKEN
            ? myMemoryIdx * myMemorySpace.theMemoryId : 0};
        return aCurrent + myMemoryCost;
    })};

    std::cout << "Solution: " << myChecksum << '\n';
}