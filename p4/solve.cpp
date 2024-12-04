#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>

std::vector<std::string> flattenToMatrix(const std::vector<std::vector<std::string>>& anInput)
{
    std::vector<std::string> myMatrix{};
    std::ranges::transform(anInput, std::back_insert_iterator(myMatrix), [](const auto& aLine){
        return aLine.front();
    });
    return myMatrix;
}

std::vector<std::string> getColumns(const std::vector<std::string>& aMatrix)
{
    std::vector<std::string> myColumns{aMatrix[0].size(), ""};
    for (const std::string& myRow : aMatrix)
    {
        for (const auto [myIdx, myChar] : myRow | std::views::enumerate)
        {
            myColumns[myIdx] += myChar;
        }
    }
    return myColumns;
}

struct Point
{
    int64_t theX;
    int64_t theY;
};

std::string getDiagonalFrom(const std::vector<std::string>& aMatrix, Point aFrom, int32_t aXDirection)
{
    std::string myDiagonal{};
    for (Point myCurrent{aFrom};
            myCurrent.theX >= 0 && myCurrent.theX < std::ssize(aMatrix.front()) && myCurrent.theY < std::ssize(aMatrix);
            myCurrent = Point{myCurrent.theX + aXDirection, myCurrent.theY + 1})
    {
        myDiagonal += aMatrix[myCurrent.theY][myCurrent.theX];
    }
    return myDiagonal;
}

std::vector<std::string> getDiagonals(const std::vector<std::string>& aMatrix)
{
    std::vector<std::string> myDiagonals{};
    for (int32_t myX{0}; myX < std::ssize(aMatrix.front()); ++myX)
    {
        // Technically adding them twice is incorrect, due to the corner's
        // However since we know we are searching for XMAS, this should be OK
        myDiagonals.emplace_back(getDiagonalFrom(aMatrix, Point{myX, 0}, 1));
        myDiagonals.emplace_back(getDiagonalFrom(aMatrix, Point{myX, 0}, -1));
    }
    for (int32_t myY{1}; myY < std::ssize(aMatrix); ++myY)
    {
        myDiagonals.emplace_back(getDiagonalFrom(aMatrix, Point{0, myY}, 1));
        myDiagonals.emplace_back(getDiagonalFrom(aMatrix, Point{std::ssize(aMatrix.front())-1, myY}, -1));
    }
    return myDiagonals;
}

int main()
{
    const auto myRows = FileReader::readColumns(NonCopyablePath{"p4/input.txt"}, "\n");

    const auto myMatrix{flattenToMatrix(myRows)};

    const auto myColumns{getColumns(myMatrix)};
    const auto myDiagonals{getDiagonals(myMatrix)};

    std::vector<std::string> myEveryStr{};
    std::ranges::copy(myMatrix, std::back_insert_iterator(myEveryStr));
    std::ranges::copy(myColumns, std::back_insert_iterator(myEveryStr));
    std::ranges::copy(myDiagonals, std::back_insert_iterator(myEveryStr));

    size_t myTotalMatches{0};
    for (const std::string& myStr : myEveryStr)
    {
        static constexpr std::string_view QUERY{"XMAS"};
        auto myView = std::views::split(myStr, QUERY);
        const size_t myUnmatchingLen{std::ranges::fold_left(myView, 0, [](int myCurrent, const auto myPortion){
            return myCurrent + std::string_view{myPortion}.size();
        })};

        std::string myReversedStr{myStr};
        std::ranges::reverse(myReversedStr);
        auto myReverseView = std::views::split(myReversedStr, QUERY);
        const size_t myReverseUnmatchingLen{std::ranges::fold_left(myReverseView, 0, [](int myCurrent, const auto myPortion){
            return myCurrent + std::string_view{myPortion}.size();
        })};

        myTotalMatches += ((myStr.size() - myUnmatchingLen)/4);
        myTotalMatches += ((myReversedStr.size() - myReverseUnmatchingLen)/4);
    }

    std::cout << "Solutions: " << myTotalMatches << '\n';
}