#include "support/filereader.hpp"

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <utility>

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

enum class Direction
{
    NORTH,
    EAST,
    SOUTH,
    WEST
};

struct PositionNode
{
    Position thePos;
    Direction theDir;

    [[nodiscard]] bool operator<(const PositionNode& aOther) const
    {
        if (thePos != aOther.thePos)
        {
            return thePos < aOther.thePos;
        }
        return std::to_underlying(theDir) < std::to_underlying(aOther.theDir);
    }
};

struct NodeWithStats
{
    PositionNode theNode;
    std::int64_t theCost;
};

[[nodiscard]] std::array<Direction, 2> getNextDirections(Direction aDirection)
{
    switch(aDirection)
    {
        case Direction::NORTH:
            return {Direction::EAST, Direction::WEST};
        case Direction::EAST:
            return {Direction::NORTH, Direction::SOUTH};
        case Direction::SOUTH:
            return {Direction::EAST, Direction::WEST};
        case Direction::WEST:
            return {Direction::NORTH, Direction::SOUTH};
    }
    std::unreachable();
}

[[nodiscard]] std::array<std::int64_t, 2> getOffsetsFor(Direction aDirection)
{
    switch(aDirection)
    {
        case Direction::NORTH:
            return {0, -1};
        case Direction::EAST:
            return {1, 0};
        case Direction::SOUTH:
            return {0, 1};
        case Direction::WEST:
            return {-1, 0};
    }
    std::unreachable();
}

int main()
{
    Position myStartPos;
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p16/input.txt"}, [&](const std::string& aLine) {
        myMap.push_back(aLine);
        if (const auto myStartIdx = aLine.find('S'); myStartIdx != std::string::npos)
        {
            myStartPos = Position{static_cast<int64_t>(myStartIdx), std::ssize(myMap)-1};
        }
    });

    std::set<PositionNode> myVisited;

    const auto myComp = [](const NodeWithStats& aLhs, const NodeWithStats& aRhs){ return aLhs.theCost > aRhs.theCost; };
    std::priority_queue<NodeWithStats, std::vector<NodeWithStats>, decltype(myComp)> myExploredSections;
    myExploredSections.emplace(PositionNode{myStartPos, Direction::EAST}, 0ll);

    while (!myExploredSections.empty())
    {
        const NodeWithStats myNodeStats{myExploredSections.top()};
        myExploredSections.pop();

        const auto& [myNode, myCurrentCost]{myNodeStats};

        char myCell{myMap[myNode.thePos.theY][myNode.thePos.theX]};

        if (myCell == 'E')
        {
            std::cout << "Solution: " << myCurrentCost << '\n';
            break;
        }

        if (myCell == '#' || myVisited.contains(myNode))
        {
            continue;
        }

        myVisited.emplace(myNode);

        for (const Direction myDirection : getNextDirections(myNode.theDir))
        {
            myExploredSections.emplace(PositionNode{myNode.thePos, myDirection}, myCurrentCost + 1'000);
        }

        const auto [myXOffset, myYOffset]{getOffsetsFor(myNode.theDir)};
        const Position myNextPosition{myNode.thePos.theX + myXOffset, myNode.thePos.theY + myYOffset};
        myExploredSections.emplace(PositionNode{myNextPosition, myNode.theDir}, myCurrentCost + 1);
    }
}