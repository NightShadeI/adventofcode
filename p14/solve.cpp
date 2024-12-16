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

    std::array<std::int64_t, 4> myQuadrantCounts{};

    static constexpr std::int64_t WIDTH{101};
    static constexpr std::int64_t HEIGHT{103};
    static constexpr std::int64_t STEPS{100};
    for (const RobotSpecs& myRobot : myRobotSpecs)
    {
        const std::int64_t myFinalX{(((WIDTH * STEPS) + myRobot.theStartLoc.theX
            + myRobot.theSpeed.theX * STEPS) % WIDTH)
        };
        const std::int64_t myFinalY{(((HEIGHT * STEPS) + myRobot.theStartLoc.theY
            + myRobot.theSpeed.theY * STEPS) % HEIGHT)
        };

        if (myFinalX > WIDTH/2)
        {
            if (myFinalY > HEIGHT/2)
            {
                ++myQuadrantCounts[0];
            }
            else if (myFinalY < HEIGHT/2)
            {
                ++myQuadrantCounts[1];
            }
        }
        else if (myFinalX < WIDTH/2)
        {
            if (myFinalY > HEIGHT/2)
            {
                ++myQuadrantCounts[2];
            }
            else if (myFinalY < HEIGHT/2)
            {
                ++myQuadrantCounts[3];
            }
        }
    }

    std::int64_t mySafetyScore{
        std::ranges::fold_left(myQuadrantCounts, 1ll, std::multiplies<std::int64_t>{})};
    std::cout << "Solution: " << mySafetyScore << '\n';
}