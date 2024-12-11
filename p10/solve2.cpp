#include "support/filereader.hpp"

#include <iostream>

struct Position
{
    std::int64_t theXPos{};
    std::int64_t theYPos{};
};

[[nodiscard]] bool inBounds(const std::vector<std::string>& aMap, Position aPosition)
{
    return aPosition.theYPos >= 0 && aPosition.theYPos < std::ssize(aMap)
        && aPosition.theXPos >= 0 && aPosition.theXPos < std::ssize(aMap.front());
}

[[nodiscard]] std::int32_t findMapLimit(const std::vector<std::string>& aMap, Position aSource)
{   
    const char myCell{aMap[aSource.theYPos][aSource.theXPos]};
    if (myCell == '9')
    {
        return 1;
    }

    static constexpr std::array DIRECTIONS{
        std::make_pair( 1,  0),
        std::make_pair( 0, -1),
        std::make_pair(-1,  0),
        std::make_pair( 0,  1)
    };

    std::int32_t myPossiblePaths{0};
    for (const auto& [myXDir, myYDir] : DIRECTIONS)
    {   
        if (const Position myProposedPosition{aSource.theXPos + myXDir, aSource.theYPos + myYDir}; inBounds(aMap, myProposedPosition))
        {
            if (const char myProposedCell{aMap[myProposedPosition.theYPos][myProposedPosition.theXPos]}; myProposedCell - myCell == 1)
            {
                myPossiblePaths += findMapLimit(aMap, myProposedPosition);
            }
        }
    }

    return myPossiblePaths;
}

int main()
{
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p10/input.txt"}, [&](const std::string& aLine){
        myMap.push_back(aLine);
    });

    std::int32_t myTotalPaths{0};
    for (const auto& [myYPosition, myRow] : myMap | std::views::enumerate)
    {
        for (const auto [myXPosition,  myCell] : myRow | std::views::enumerate)
        {
            if (myCell == '0')
            {
                myTotalPaths += findMapLimit(myMap, Position{myXPosition, myYPosition});
            }
        }
    }

    std::cout << "Solution: " << myTotalPaths << '\n';
}