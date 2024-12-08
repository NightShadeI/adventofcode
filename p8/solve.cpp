#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

struct AntennaInfo
{
    std::int32_t theXPos{};
    std::int32_t theYPos{};
    char theType{};

    [[nodiscard]] bool operator==(const AntennaInfo& aOther) const
    {
        return theType == aOther.theType;
    }
};

[[nodiscard]] bool inBoundsAndNotTaken(const std::vector<std::string>& aMap, const AntennaInfo& anAntennaInfo)
{
    return anAntennaInfo.theYPos >= 0 && anAntennaInfo.theYPos < std::ssize(aMap)
        && anAntennaInfo.theXPos >= 0 && anAntennaInfo.theXPos < std::ssize(aMap.front())
        && aMap[anAntennaInfo.theYPos][anAntennaInfo.theXPos] != '#';
}

int main()
{
    std::vector<std::string> myMap;
    FileReader::processLines(NonCopyablePath{"p8/input.txt"}, [&](const std::string& aLine){
        myMap.push_back(aLine);
    });

    std::vector<AntennaInfo> myAntennas;
    for (const auto& [myYPos, myRow] : myMap | std::views::enumerate)
    {
        for (const auto [myXPos, myCell] : myRow | std::views::enumerate)
        {
            if (myCell != '.')
            {
                myAntennas.emplace_back(myXPos, myYPos, myCell);
            }
        }
    }

    std::ranges::sort(myAntennas, [](const AntennaInfo& aLhs, const AntennaInfo& aRhs){
        return aLhs.theType < aRhs.theType;
    });

    std::int32_t myUniqueAntiNodes{0};

    for (const auto myChunk : myAntennas | std::views::chunk_by(std::ranges::equal_to{}))
    {
        const std::span<AntennaInfo> mySameTypeAntennas{myChunk};
        for (std::uint32_t myOuterIdx{1}; myOuterIdx < mySameTypeAntennas.size(); ++myOuterIdx)
        {
            for (std::uint32_t myInnerIdx{0}; myInnerIdx < myOuterIdx; ++myInnerIdx)
            {
                const std::int32_t myRun{mySameTypeAntennas[myOuterIdx].theXPos
                    - mySameTypeAntennas[myInnerIdx].theXPos};
                const std::int32_t myRise{mySameTypeAntennas[myOuterIdx].theYPos
                    - mySameTypeAntennas[myInnerIdx].theYPos};

                const AntennaInfo myFirstAntiNode{
                    mySameTypeAntennas[myOuterIdx].theXPos + myRun, mySameTypeAntennas[myOuterIdx].theYPos + myRise, '#'};
                const AntennaInfo mySecondAntiNode{
                    mySameTypeAntennas[myOuterIdx].theXPos - 2 * myRun, mySameTypeAntennas[myOuterIdx].theYPos - 2 * myRise, '#'};

                if (inBoundsAndNotTaken(myMap, myFirstAntiNode))
                {
                    myMap[myFirstAntiNode.theYPos][myFirstAntiNode.theXPos] = '#';
                    ++myUniqueAntiNodes;
                }

                if (inBoundsAndNotTaken(myMap, mySecondAntiNode))
                {
                    myMap[mySecondAntiNode.theYPos][mySecondAntiNode.theXPos] = '#';
                    ++myUniqueAntiNodes;
                }
            }
        }
    }

    for (const std::string& myRow : myMap)
    {
        std::cout << myRow << std::endl;
    }
    std::cout << "Solution: " << myUniqueAntiNodes << '\n';
}