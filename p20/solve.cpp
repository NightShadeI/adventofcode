#include "support/filereader.hpp"

#include <iostream>
#include <queue>
#include <set>

struct Position
{
    std::int64_t theX;
    std::int64_t theY;

    [[nodiscard]] bool operator==(const Position&) const = default;

    [[nodiscard]] bool operator<(const Position& aOther) const
    {
        if (theX != aOther.theX)
        {
            return theX < aOther.theX;
        }
        return theY < aOther.theY;
    }
};

struct PositionNode
{
    Position theNode;
    std::int64_t theCost;
};

struct DistanceMetrics
{
    std::int64_t theStartDist;
    std::int64_t theEndDist;
};

[[nodiscard]] bool inBounds(const std::vector<std::string>& aMap, Position aPosition)
{
    return aPosition.theY >= 0 && aPosition.theY < std::ssize(aMap)
        && aPosition.theX >= 0 && aPosition.theX < std::ssize(aMap.front());
}

template<typename HandlerT>
void runBfs(const std::vector<std::string>& aMap, Position aSeedPosition, HandlerT aVisitHandler)
{
    std::set<Position> myVisited;

    std::deque<PositionNode> myBfsQueue;
    myBfsQueue.emplace_back(aSeedPosition, 0ll);
    myVisited.emplace(aSeedPosition);
    aVisitHandler(aSeedPosition, 0);

    while (!myBfsQueue.empty())
    {
        const PositionNode myNodeStats{myBfsQueue.front()};
        myBfsQueue.pop_front();

        const auto& [myNode, myCurrentCost]{myNodeStats};

        static constexpr std::array DIRECTIONS{
            std::make_pair(0, -1),
            std::make_pair(1, 0),
            std::make_pair(0, 1),
            std::make_pair(-1, 0)};
        for (const auto& [myXDir, myYDir] : DIRECTIONS)
        {
            const Position myProposedPosition{myNode.theX + myXDir, myNode.theY + myYDir};
            if (inBounds(aMap, myProposedPosition))
            {
                const char myProposedCell{aMap[myProposedPosition.theY][myProposedPosition.theX]};
                if (myProposedCell != '#' && !myVisited.contains(myProposedPosition))
                {
                    myBfsQueue.emplace_back(myProposedPosition, myCurrentCost + 1);
                    myVisited.emplace(myProposedPosition);
                    aVisitHandler(myProposedPosition, myCurrentCost + 1);
                }
            }

        }
    }
}

int main()
{
    Position myStartPos{};
    Position myEndPos{};
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p20/input.txt"}, [&](const std::string& aLine) {
        myMap.push_back(aLine);
        if (const auto myStartIdx = aLine.find('S'); myStartIdx != std::string::npos)
        {
            myStartPos = Position{static_cast<int64_t>(myStartIdx), std::ssize(myMap)-1};
        }
        if (const auto myEndIdx = aLine.find('E'); myEndIdx != std::string::npos)
        {
            myEndPos = Position{static_cast<int64_t>(myEndIdx), std::ssize(myMap)-1};
        }
    });

    static constexpr auto LIMIT{std::numeric_limits<std::int64_t>::max()};
    std::vector<std::vector<DistanceMetrics>> myCostToFinish(
        myMap.size(), std::vector<DistanceMetrics>(myMap.front().size(), DistanceMetrics{LIMIT, LIMIT}));

    runBfs(myMap, myStartPos, [&](const Position& aNewPosition, std::int64_t aCost){
        myCostToFinish[aNewPosition.theY][aNewPosition.theX].theStartDist = aCost;
    });

    runBfs(myMap, myEndPos, [&](const Position& aNewPosition, std::int64_t aCost){
        myCostToFinish[aNewPosition.theY][aNewPosition.theX].theEndDist = aCost;
    });

    std::int64_t myPossibleCheats{0};
    const std::int64_t myFastestCost{myCostToFinish[myStartPos.theY][myStartPos.theX].theEndDist};

    for (const auto& [myY, myRow] : myCostToFinish | std::views::enumerate)
    {
        for (const auto [myX, myCosts] : myRow | std::views::enumerate)
        {
            if (myCosts.theStartDist != LIMIT && myCosts.theEndDist != LIMIT)
            {
                static constexpr std::array CHEAT_DIRECTIONS{
                    std::make_pair(0, -2),
                    std::make_pair(2, 0),
                    std::make_pair(0, 2),
                    std::make_pair(-2, 0)};
                for (const auto& [myXDir, myYDir] : CHEAT_DIRECTIONS)
                {
                    const Position myProposedPosition{myX + myXDir, myY + myYDir};
                    if (inBounds(myMap, myProposedPosition))
                    {
                        const DistanceMetrics myCheatMetrics{myCostToFinish[myProposedPosition.theY][myProposedPosition.theX]};
                        if (myCheatMetrics.theStartDist == LIMIT || myCheatMetrics.theEndDist == LIMIT)
                        {
                            continue;
                        }
                        const std::int64_t myNewDistance{myCosts.theStartDist + myCheatMetrics.theEndDist + 2};
                        if (myFastestCost - myNewDistance >= 100)
                        {
                            ++myPossibleCheats;
                        }
                    }
                }
            }
        }
    }

    std::cout << "Solution: " << myPossibleCheats << '\n';
}