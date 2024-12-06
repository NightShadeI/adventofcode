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
        std::make_pair( 0, -1),
        std::make_pair( 1,  0),
        std::make_pair( 0,  1),
        std::make_pair(-1,  0)
    };

    std::int32_t myCurrentDirection{0};
    Position myCurrentPosition{getPlayerPosition(myMap)};

    while(true)
    {
        myMap[myCurrentPosition.theYPos][myCurrentPosition.theXPos] = 'X';
        
        const auto [myXDirection, myYDirection]{myDirections[myCurrentDirection]};
        const Position myProposedPosition{myCurrentPosition.theXPos + myXDirection,
            myCurrentPosition.theYPos + myYDirection};

        if (!inBounds(myMap, myProposedPosition))
        {
            break;
        }

        const char myCell{myMap[myProposedPosition.theYPos][myProposedPosition.theXPos]};
        if (myCell == '#')
        {
            myCurrentDirection = ((myCurrentDirection + 1) % 4);
            continue;
        }

        myCurrentPosition = myProposedPosition;
    }

    const std::int32_t myVisitedCells{std::ranges::fold_left(myMap, 0, [](std::int32_t aTotal, const auto& aRow){
        return aTotal + std::ranges::fold_left(aRow, 0, [](std::int32_t aRowTotal, char aCell){
            return aRowTotal + (aCell == 'X');
        });
    })};

    std::cout << "Solution: " << myVisitedCells << '\n';
}