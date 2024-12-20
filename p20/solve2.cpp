#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>
#include <map>
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

static constexpr std::array DIRECTIONS{
    std::make_pair(0, -1),
    std::make_pair(1, 0),
    std::make_pair(0, 1),
    std::make_pair(-1, 0)};

template<typename HandlerT, typename ValidHandlerT>
void runBfs(const std::vector<std::string>& aMap, Position aSeedPosition,
    [[maybe_unused]] ValidHandlerT aValidHandler, HandlerT aVisitHandler, std::int64_t aLimit = std::numeric_limits<std::int64_t>::max())
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

        if (myCurrentCost >= aLimit)
        {
            continue;
        }

        for (const auto& [myXDir, myYDir] : DIRECTIONS)
        {
            const Position myProposedPosition{myNode.theX + myXDir, myNode.theY + myYDir};
            if (inBounds(aMap, myProposedPosition))
            {
                const char myProposedCell{aMap[myProposedPosition.theY][myProposedPosition.theX]};
                if (aValidHandler(myProposedCell) && !myVisited.contains(myProposedPosition))
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

    runBfs(myMap, myStartPos, [](char aCell){ return aCell != '#'; }, [&](const Position& aNewPosition, std::int64_t aCost){
        myCostToFinish[aNewPosition.theY][aNewPosition.theX].theStartDist = aCost;
    });

    runBfs(myMap, myEndPos, [](char aCell){ return aCell != '#'; }, [&](const Position& aNewPosition, std::int64_t aCost){
        myCostToFinish[aNewPosition.theY][aNewPosition.theX].theEndDist = aCost;
    });

    std::map<Position, std::vector<std::pair<Position, std::int64_t>>> myReachableByCheating;

    std::int64_t myPossibleCheats{0};
    const std::int64_t myFastestCost{myCostToFinish[myStartPos.theY][myStartPos.theX].theEndDist};

    for (const auto& [myY, myRow] : myCostToFinish | std::views::enumerate)
    {
        for (const auto [myX, myCosts] : myRow | std::views::enumerate)
        {
            if (myCosts.theStartDist == LIMIT || myCosts.theEndDist == LIMIT) { continue; } 
            const Position mySourcePosition{myX, myY};
            runBfs(myMap, mySourcePosition, [](char){ return true; }, [&](const Position& aNewPosition, std::int64_t aCost){
                const DistanceMetrics myCheatMetrics{myCostToFinish[aNewPosition.theY][aNewPosition.theX]};
                if (myCheatMetrics.theStartDist == LIMIT || myCheatMetrics.theEndDist == LIMIT)
                {
                    return;
                }
                const std::int64_t myNewDistance{myCosts.theStartDist + myCheatMetrics.theEndDist + aCost};
                if (myFastestCost - myNewDistance >= 100)
                {
                    ++myPossibleCheats;
                }
            }, 20);
        }
    }

    std::cout << "Solution: " << myPossibleCheats << '\n';
}