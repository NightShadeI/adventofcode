#include "support/filereader.hpp"

#include <iostream>
#include <utility>

struct Position
{
    std::int64_t theX;
    std::int64_t theY;
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
            myMap.push_back(aLine);
            if (const auto myRobotPos = aLine.find('@'); myRobotPos != std::string::npos)
            {
                myCurrentPos = Position{static_cast<std::int64_t>(myRobotPos), std::ssize(myMap) - 1};
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
        const Position myProposedPosition{applyOffsetTo(myCurrentPos, myOffsets)};

        char& myProposedCell{myMap[myProposedPosition.theY][myProposedPosition.theX]};
        
        if (myProposedCell == '.')
        {
            std::swap(myMap[myCurrentPos.theY][myCurrentPos.theX], myProposedCell);
            myCurrentPos = myProposedPosition;
            continue;
        }

        // Get first non 'O' position
        Position myPositionIt{myProposedPosition};
        for (; myMap[myPositionIt.theY][myPositionIt.theX] == 'O'; myPositionIt = applyOffsetTo(myPositionIt, myOffsets));

        char& myReachedCell{myMap[myPositionIt.theY][myPositionIt.theX]};
        if (myReachedCell == '#')
        {
            continue;
        }

        std::swap(myProposedCell, myReachedCell);
        std::swap(myMap[myCurrentPos.theY][myCurrentPos.theX], myProposedCell);
        myCurrentPos = myProposedPosition;
    }

    std::int64_t myGpsScore{0};
    for (const auto& [myY, myRow] : myMap | std::views::enumerate)
    {
        for (const auto [myX, myCell] : myRow | std::views::enumerate)
        {
            if (myCell == 'O')
            {
                myGpsScore += (100 * myY + myX);
            }
        }
    }

    std::cout << "Solution: " << myGpsScore << '\n';
}