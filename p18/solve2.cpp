#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <iostream>
#include <deque>

struct Position
{
    std::int64_t theX;
    std::int64_t theY;

    [[nodiscard]] bool operator==(const Position&) const = default;
};

struct PositionNode
{
    Position thePos;
    std::int64_t theSteps;
};

[[nodiscard]] bool inBounds(const std::vector<std::vector<bool>>& aMap, Position aPosition)
{
    return aPosition.theY >= 0 && aPosition.theY < std::ssize(aMap)
        && aPosition.theX >= 0 && aPosition.theX < std::ssize(aMap.front());
}

int main()
{
    const auto myRows{FileReader::readColumns(NonCopyablePath{"p18/input.txt"}, ",")};
    const auto myBytes{rowsToInts(myRows)};

    static constexpr std::size_t MAP_SIZE{71};
    static constexpr std::size_t TAKE_CNT{1'024};
    std::vector<std::vector<bool>> myInitialMap(MAP_SIZE, std::vector<bool>(MAP_SIZE, false));
    for (auto& myBytePos : myBytes | std::views::take(TAKE_CNT))
    {
        const std::int64_t myXPos{myBytePos[0]};
        const std::int64_t myYPos{myBytePos[1]};
        myInitialMap[myYPos][myXPos] = true;
    }


    for (auto& myBytePos : myBytes | std::views::drop(TAKE_CNT))
    {
        const std::int64_t myXPos{myBytePos[0]};
        const std::int64_t myYPos{myBytePos[1]};
        myInitialMap[myYPos][myXPos] = true;

        std::deque<PositionNode> myBfsQueue;

        auto myMap{myInitialMap};
        bool mySolved{false};

        myBfsQueue.emplace_back(Position{0, 0}, 0);
        while (!myBfsQueue.empty())
        {
            const PositionNode myNode{myBfsQueue.front()};
            const auto& [myCurrentPos, myCurrentSteps]{myNode};
            myBfsQueue.pop_front();

            if (myCurrentPos.theX == MAP_SIZE-1 && myCurrentPos.theY == MAP_SIZE-1)
            {
                mySolved = true;
                break;
            }

            if (myMap[myCurrentPos.theY][myCurrentPos.theX])
            {
                continue;
            }

            myMap[myCurrentPos.theY][myCurrentPos.theX] = true;

            static constexpr std::array DIRECTIONS{
                std::make_pair(0, -1),
                std::make_pair(1, 0),
                std::make_pair(0, 1),
                std::make_pair(-1, 0)};

            for (const auto& [myXDir, myYDir] : DIRECTIONS)
            {
                const Position myProposedPosition{
                    myCurrentPos.theX + myXDir, myCurrentPos.theY + myYDir};
                if (inBounds(myMap, myProposedPosition) && !myMap[myProposedPosition.theY][myProposedPosition.theX])
                {
                    myBfsQueue.emplace_back(myProposedPosition, myCurrentSteps + 1);
                }
            }
        }

        if (!mySolved)
        {
            std::cout << "Solution: " << myXPos << ',' << myYPos << '\n';
            break;
        }
    }

}