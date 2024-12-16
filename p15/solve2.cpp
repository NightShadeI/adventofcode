#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>
#include <deque>
#include <utility>

struct Position
{
    std::int64_t theX;
    std::int64_t theY;

    [[nodiscard]] bool operator==(const Position&) const = default;
};

enum class ReadState
{
    MAP,
    MOVES
};

[[nodiscard]] std::array<std::int64_t, 2> getOffsetOf(char aChar)
{
    if (aChar == '<')
    {
        return {-1, 0};
    }
    if (aChar == '>')
    {
        return {1, 0};
    }
    if (aChar == 'v')
    {
        return {0, 1};
    }
    if (aChar == '^')
    {
        return {0, -1};
    }
    std::unreachable();
}

[[nodiscard]] Position applyOffsetTo(
    const Position& aPosition, const std::array<std::int64_t, 2>& anOffsets)
{
    const auto [myXOffset, myYOffset]{anOffsets};
    return Position{aPosition.theX + myXOffset, aPosition.theY + myYOffset};
}

std::vector<std::pair<Position, Position>> tryPush(std::vector<std::string>& aMap, Position aSourcePush, const std::array<std::int64_t, 2>& anOffsets)
{
    const auto [myXOffset, myYOffset]{anOffsets};

    std::vector<std::pair<Position, Position>> myPushes{};

    std::deque<Position> myPushStack{};
    myPushStack.emplace_back(aSourcePush);

    while (!myPushStack.empty())
    {
        const Position myCurrentPosition{myPushStack.front()};
        myPushStack.pop_front();

        const Position myProposedPosition{applyOffsetTo(myCurrentPosition, anOffsets)};
        char myProposedCell{aMap[myProposedPosition.theY][myProposedPosition.theX]};

        if (myProposedCell == '#')
        {
            return {};
        }

        myPushes.emplace_back(myCurrentPosition, myProposedPosition);
        
        if (myProposedCell == '.')
        {
            continue;
        }

        if (!std::ranges::contains(myPushStack, myProposedPosition))
        {
            myPushStack.emplace_back(myProposedPosition);
        }

        if (myYOffset == 0)
        {
            continue;
        }

        const auto myBoxOffset{myProposedCell == '[' ? std::array{1ll, 0ll} : std::array{-1ll, 0ll}};
        const auto mySecondaryPos{applyOffsetTo(myProposedPosition, myBoxOffset)};
        if (!std::ranges::contains(myPushStack, mySecondaryPos))
        {
            myPushStack.emplace_back(mySecondaryPos);
        }
    }
    
    return myPushes;
}

int main()
{
    Position myCurrentPos{};
    std::string myMoves;
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p15/input.txt"}, [&, myReadState = ReadState::MAP](const std::string& aLine) mutable {
        if (aLine.empty())
        {
            myReadState = ReadState::MOVES;
            return;
        }

        if (myReadState == ReadState::MAP)
        {
            auto& myMapRow = myMap.emplace_back();
            for (const auto [myX, myObstacle] : aLine | std::views::enumerate)
            {
                if (myObstacle == '@')
                {
                    myCurrentPos = Position{2*myX, std::ssize(myMap) - 1};
                    myMapRow += "@.";
                }
                else if (myObstacle == '#')
                {
                    myMapRow += "##";
                }
                else if (myObstacle == '.')
                {
                    myMapRow += "..";
                }
                else if (myObstacle == 'O')
                {
                    myMapRow += "[]";
                }
            }
        }
        else
        {
            myMoves += aLine;
        }
    });

    for (const char myMove : myMoves)
    {
        const auto myOffsets{getOffsetOf(myMove)};
        const auto myDesiredSwaps{tryPush(myMap, myCurrentPos, myOffsets)};

        for (const auto& [mySourcePos, myDestPos] : myDesiredSwaps | std::views::reverse)
        {
            char& mySourceCell{myMap[mySourcePos.theY][mySourcePos.theX]};
            char& myDestCell{myMap[myDestPos.theY][myDestPos.theX]};
            std::swap(mySourceCell, myDestCell);
        }

        if (!myDesiredSwaps.empty())
        {
            myCurrentPos = applyOffsetTo(myCurrentPos, myOffsets);
        }
    }

    std::int64_t myGpsScore{0};
    for (const auto& [myY, myRow] : myMap | std::views::enumerate)
    {
        for (const auto [myX, myCell] : myRow | std::views::enumerate)
        {
            if (myCell == '[')
            {
                myGpsScore += (100 * myY + myX);
            }
        }
    }

    std::cout << "Solution: " << myGpsScore << '\n';
}