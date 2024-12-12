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

[[nodiscard]] std::pair<std::int32_t, std::int32_t> findFencedDimensions(std::vector<std::string>& aMap, Position aSource, std::vector<std::vector<bool>>& aVisited)
{
    char myCurrentCell{aMap[aSource.theYPos][aSource.theXPos]};
    aVisited[aSource.theYPos][aSource.theXPos] = true;

    static constexpr std::array DIRECTIONS{
        std::make_pair( 1,  0),
        std::make_pair( 0, -1),
        std::make_pair(-1,  0),
        std::make_pair( 0,  1)
    };

    std::pair<int32_t, int32_t> myDimensions{};
    auto& [myTotalArea, myTotalPerimeter] = myDimensions;
    
    for (const auto& [myXDir, myYDir] : DIRECTIONS)
    {
        const Position myProposedPosition{aSource.theXPos + myXDir, aSource.theYPos + myYDir};
        if (!inBounds(aMap, myProposedPosition))
        {
            ++myTotalPerimeter;
            continue;
        }
        
        const char myProposedCell{aMap[myProposedPosition.theYPos][myProposedPosition.theXPos]};
        if (myProposedCell != myCurrentCell)
        {
            ++myTotalPerimeter;
            continue;
        }

        if (aVisited[myProposedPosition.theYPos][myProposedPosition.theXPos])
        {
            continue;
        }

        const auto [myProceedingArea, myProceedingPerimeter] = findFencedDimensions(aMap, myProposedPosition, aVisited);
        myTotalArea += myProceedingArea;
        myTotalPerimeter += myProceedingPerimeter; 
    }

    ++myTotalArea;

    return myDimensions;
}

int main()
{
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p12/input.txt"}, [&](const std::string& aLine){
        myMap.push_back(aLine);
    });

    std::vector<std::vector<bool>> myVisited(myMap.size(), std::vector<bool>(myMap.front().size(), false));

    std::int64_t myTotalCost{0};
    for (const auto& [myYPosition, myRow] : myMap | std::views::enumerate)
    {
        for (const auto [myXPosition,  myCell] : myRow | std::views::enumerate)
        {
            if (!myVisited[myYPosition][myXPosition])
            {
                const auto [mySectionArea, mySectionPerimeter] = findFencedDimensions(
                    myMap, Position{myXPosition, myYPosition}, myVisited);
                myTotalCost += (mySectionArea * mySectionPerimeter);
            }
        }
    }


    std::cout << "Solution: " << myTotalCost << '\n';
}