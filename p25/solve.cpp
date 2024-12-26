#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

static constexpr std::int64_t MAX_HEIGHT{5};
using GridT = std::vector<std::string>;

struct SecurityDetail
{
    std::vector<std::int64_t> theHeights;
};

[[nodiscard]] bool checkMatch(const SecurityDetail& aKey, const SecurityDetail& aLock)
{
    for (const auto [myIdx, myKeyHeight] : aKey.theHeights | std::views::enumerate)
    {
        const std::int64_t myLockHeight{aLock.theHeights[myIdx]};
        if (myKeyHeight + myLockHeight > MAX_HEIGHT)
        {
            return false;
        }
    }
    return true;
}

int main()
{
    std::vector<SecurityDetail> myKeys;
    std::vector<SecurityDetail> myLocks;

    FileReader::processLines(NonCopyablePath{"p25/input.txt"}, [&, myGrid = GridT{}](const std::string& aLine) mutable {
        if (aLine.empty())
        {
            return;
        }

        myGrid.emplace_back(aLine);
        if (myGrid.size() == MAX_HEIGHT+2)
        {
            const auto& myTopRow{myGrid[0]};
            const char myQueryChar{myTopRow[0]};
            const bool myIsLock{myQueryChar == '#'};

            SecurityDetail mySecurityDetail;
            for (std::int64_t myX{0}; myX < std::ssize(myTopRow); ++myX)
            {
                std::int64_t myHeight{0};
                for (; myHeight < std::ssize(myGrid) && myGrid[myHeight][myX] == myQueryChar; ++myHeight);
                mySecurityDetail.theHeights.emplace_back(myIsLock ? myHeight - 1 : MAX_HEIGHT - (myHeight-1));
            }

            auto& mySecurityDetails{myIsLock ? myLocks : myKeys};
            mySecurityDetails.emplace_back(std::move(mySecurityDetail));
            myGrid.clear();
        }
    });

    std::int64_t myTotalMatches{0};
    for (const auto& myKey : myKeys)
    {
        for (const auto& myLock : myLocks)
        {
            if (checkMatch(myKey, myLock))
            {
                ++myTotalMatches;
            }
        }
    }

    std::cout << "Solution: " << myTotalMatches << '\n';
}
