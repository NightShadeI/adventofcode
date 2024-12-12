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

[[nodiscard]] std::pair<std::int32_t, std::int32_t> findFencedDimensions(
    std::vector<std::string>& aMap, Position aSource, std::vector<std::vector<bool>>& aVisited, const std::vector<std::vector<std::int32_t>>& aPerimeters)
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
            continue;
        }
        
        const char myProposedCell{aMap[myProposedPosition.theYPos][myProposedPosition.theXPos]};
        if (aVisited[myProposedPosition.theYPos][myProposedPosition.theXPos] || myProposedCell != myCurrentCell)
        {
            continue;
        }

        const auto [myProceedingArea, myProceedingPerimeter] = findFencedDimensions(aMap, myProposedPosition, aVisited, aPerimeters);
        myTotalArea += myProceedingArea;
        myTotalPerimeter += myProceedingPerimeter; 
    }

    ++myTotalArea;
    myTotalPerimeter += aPerimeters[aSource.theYPos][aSource.theXPos];

    return myDimensions;
}

struct TrackedFence
{
    char thePlantType{};
    // char theBoundaryPlant{};
    Position theStartPosition{-1, -1};
};

[[nodiscard]] std::vector<std::vector<std::int32_t>> generatePerimeterMap(const std::vector<std::string>& aMap)
{
    std::vector<std::vector<std::int32_t>> myPerimeterMap(aMap.size(), std::vector<std::int32_t>(aMap.front().size(), 0));
    for (const auto& [myYPosition, myRow] : aMap | std::views::enumerate)
    {
        TrackedFence myUpperFence{};
        TrackedFence myLowerFence{};
        for (const auto [myXPosition, myCell] : myRow | std::views::enumerate)
        {
            const char myUpperCell{myYPosition - 1 >= 0 ? aMap[myYPosition-1][myXPosition] : '\0'};
            const char myLowerCell{myYPosition + 1 < std::ssize(aMap) ? aMap[myYPosition+1][myXPosition] : '\0'};

            if (myCell != myUpperFence.thePlantType || myUpperCell == myCell)
            {
                if (inBounds(aMap, myUpperFence.theStartPosition))
                {
                    ++myPerimeterMap[myUpperFence.theStartPosition.theYPos][myUpperFence.theStartPosition.theXPos];
                }
                myUpperFence = myUpperCell != myCell ? TrackedFence{myCell, {myXPosition, myYPosition}} : TrackedFence{};
            }

            if (myCell != myLowerFence.thePlantType || myLowerCell == myCell)
            {
                if (inBounds(aMap, myLowerFence.theStartPosition))
                {
                    ++myPerimeterMap[myLowerFence.theStartPosition.theYPos][myLowerFence.theStartPosition.theXPos];
                }
                myLowerFence = myLowerCell != myCell ? TrackedFence{myCell, {myXPosition, myYPosition}} : TrackedFence{};
            }
        }

        if (inBounds(aMap, myUpperFence.theStartPosition))
        {
            ++myPerimeterMap[myUpperFence.theStartPosition.theYPos][myUpperFence.theStartPosition.theXPos];
        }
        if (inBounds(aMap, myLowerFence.theStartPosition))
        {
            ++myPerimeterMap[myLowerFence.theStartPosition.theYPos][myLowerFence.theStartPosition.theXPos];
        }
    }

    for (std::int64_t myXPosition{0}; myXPosition < std::ssize(aMap.front()); ++myXPosition)
    {
        TrackedFence myLeftFence{};
        TrackedFence myRightFence{};
        for (std::int64_t myYPosition{0}; myYPosition < std::ssize(aMap); ++myYPosition)
        {
            const char myCell{aMap[myYPosition][myXPosition]};

            const char myLeftCell{myXPosition - 1 >= 0 ? aMap[myYPosition][myXPosition-1] : '\0'};
            const char myRightCell{myXPosition + 1 < std::ssize(aMap.front()) ? aMap[myYPosition][myXPosition+1] : '\0'};

            if (myCell != myLeftFence.thePlantType || myLeftCell == myCell)
            {
                if (inBounds(aMap, myLeftFence.theStartPosition))
                {
                    ++myPerimeterMap[myLeftFence.theStartPosition.theYPos][myLeftFence.theStartPosition.theXPos];
                }
                myLeftFence = myLeftCell != myCell ? TrackedFence{myCell, {myXPosition, myYPosition}} : TrackedFence{};
            }

            if (myCell != myRightFence.thePlantType || myRightCell == myCell)
            {
                if (inBounds(aMap, myRightFence.theStartPosition))
                {
                    ++myPerimeterMap[myRightFence.theStartPosition.theYPos][myRightFence.theStartPosition.theXPos];
                }
                myRightFence = myRightCell != myCell ? TrackedFence{myCell, {myXPosition, myYPosition}} : TrackedFence{};
            }
        }
        if (inBounds(aMap, myLeftFence.theStartPosition))
        {
            ++myPerimeterMap[myLeftFence.theStartPosition.theYPos][myLeftFence.theStartPosition.theXPos];
        }
        if (inBounds(aMap, myRightFence.theStartPosition))
        {
            ++myPerimeterMap[myRightFence.theStartPosition.theYPos][myRightFence.theStartPosition.theXPos];
        }
    }

    return myPerimeterMap;
}

int main()
{
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p12/input.txt"}, [&](const std::string& aLine){
        myMap.push_back(aLine);
    });

    std::vector<std::vector<bool>> myVisited(myMap.size(), std::vector<bool>(myMap.front().size(), false));

    const auto myPerimeterValues(generatePerimeterMap(myMap));

    std::int64_t myTotalCost{0};
    for (const auto& [myYPosition, myRow] : myMap | std::views::enumerate)
    {
        for (const auto [myXPosition,  myCell] : myRow | std::views::enumerate)
        {
            if (!myVisited[myYPosition][myXPosition])
            {
                const auto [mySectionArea, mySectionPerimeter] = findFencedDimensions(
                    myMap, Position{myXPosition, myYPosition}, myVisited, myPerimeterValues);
                myTotalCost += (mySectionArea * mySectionPerimeter);
            }
        }
    }


    std::cout << "Solution: " << myTotalCost << '\n';
}