#include "support/conversions.hpp"
#include "support/filereader.hpp"
#include "support/strprocessing.hpp"

#include <algorithm>
#include <iostream>

struct CartesianVector
{
    std::int64_t theX;
    std::int64_t theY;
};

struct RobotSpecs
{
    CartesianVector theStartLoc;
    CartesianVector theSpeed;
};

int main()
{
    std::vector<RobotSpecs> myRobotSpecs;
    FileReader::processLines(NonCopyablePath{"p14/input.txt"}, [&](const std::string& aLine) {
        const auto myParts{splitBy(aLine, " ")};
        const auto myPositionNums{splitBy(myParts[0].substr(2), ",")};
        const auto mySpeed{splitBy(myParts[1].substr(2), ",")};
        myRobotSpecs.emplace_back(
            CartesianVector{toInt(myPositionNums[0]), toInt(myPositionNums[1])},
            CartesianVector{toInt(mySpeed[0]), toInt(mySpeed[1])});
    });

    std::ofstream myOutputStream{"robots.txt"};

    static constexpr std::int64_t STEPS{10'000};
    static constexpr std::int64_t WIDTH{101};
    static constexpr std::int64_t HEIGHT{103};
    
    for (std::int64_t myStep{0}; myStep < STEPS; ++myStep)
    {
        std::vector<std::vector<char>> myMap(HEIGHT, std::vector<char>(WIDTH, '.'));
        for (const RobotSpecs& myRobot : myRobotSpecs)
        {
            const std::int64_t myFinalX{(((WIDTH * myStep * 2) + myRobot.theStartLoc.theX
                + myRobot.theSpeed.theX * myStep) % WIDTH)
            };
            const std::int64_t myFinalY{(((HEIGHT * myStep * 2) + myRobot.theStartLoc.theY
                + myRobot.theSpeed.theY * myStep) % HEIGHT)
            };
            myMap[myFinalY][myFinalX] = 'R';
        }
        myOutputStream << "Steps: " << myStep << '\n';
        for (const auto& myRow : myMap)
        {
            for (char myCell : myRow)
            {
                myOutputStream << myCell;
            }
            myOutputStream << '\n';
        }
        myOutputStream << '\n';
    }
}