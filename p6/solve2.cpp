#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

struct Position
{
    std::int64_t theXPos{};
    std::int64_t theYPos{};
};

[[nodiscard]] Position getPlayerPosition(const std::vector<std::string>& aMap)
{
    for (const auto [myYPos, myRow] : aMap | std::views::enumerate)
    {
        for (const auto [myXPos, myCell] : myRow | std::views::enumerate)
        {
            if (myCell == '^')
            {
                return Position{myXPos, myYPos};
            }
        }
    }
    return Position{};
}

[[nodiscard]] bool inBounds(const std::vector<std::string>& aMap, Position aPosition)
{
    const bool myXBounded{aPosition.theXPos >= 0 && aPosition.theXPos < std::ssize(aMap.front())};
    const bool myYBounded{aPosition.theYPos >= 0 && aPosition.theYPos < std::ssize(aMap)};
    const bool myPosBounded{myXBounded && myYBounded};
    return myPosBounded;
}

int main()
{
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p6/input.txt"}, [&](const std::string& aLine){
        myMap.push_back(aLine);
    });

    static constexpr std::array myDirections{
        std::make_pair(0, -1),
        std::make_pair(1, 0),
        std::make_pair(0, 1),
        std::make_pair(-1, 0)};

    std::int32_t myTotalStuckSolutions{0};
    Position myInitialPosition{getPlayerPosition(myMap)};

    for (std::int32_t myYPos{0}; myYPos < std::ssize(myMap); ++myYPos)
    {
        for (std::int32_t myXPos{0}; myXPos < std::ssize(myMap[myYPos]); ++myXPos)
        {
            const char myPotentialCell{myMap[myYPos][myXPos]};
            if (myPotentialCell != '.')
            {
                continue;
            }

            std::vector<std::string> myCurrentMap{myMap};
            std::vector<std::vector<std::array<bool, sizeof(myDirections)>>> mySeenPositions(
                myMap.size(), std::vector<std::array<bool, sizeof(myDirections)>>(
                    myMap.front().size(), std::array<bool, sizeof(myDirections)>{}));

            std::int32_t myCurrentDirection{0};
            Position myCurrentPosition{myInitialPosition};
            myCurrentMap[myYPos][myXPos] = '#';

            while (true)
            {
                bool& myPositionSeen{mySeenPositions[myCurrentPosition.theYPos][myCurrentPosition.theXPos][myCurrentDirection]};
                if (myPositionSeen)
                {
                    ++myTotalStuckSolutions;
                    break;
                }

                myCurrentMap[myCurrentPosition.theYPos][myCurrentPosition.theXPos] = 'X';

                const auto [myXDirection, myYDirection]{myDirections[myCurrentDirection]};
                const Position myProposedPosition{myCurrentPosition.theXPos + myXDirection,
                                                  myCurrentPosition.theYPos + myYDirection};

                if (!inBounds(myCurrentMap, myProposedPosition))
                {
                    break;
                }

                const char myCell{myCurrentMap[myProposedPosition.theYPos][myProposedPosition.theXPos]};
                if (myCell == '#')
                {
                    myCurrentDirection = ((myCurrentDirection + 1) % 4);
                    continue;
                }

                myPositionSeen = true;
                myCurrentPosition = myProposedPosition;
            }
        }
    }

    std::cout << "Solution: " << myTotalStuckSolutions << '\n';
}