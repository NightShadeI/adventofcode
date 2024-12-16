#include "support/filereader.hpp"
#include "support/conversions.hpp"

#include <algorithm>
#include <iostream>

struct Coordinate
{
    std::int64_t theXPos{};
    std::int64_t theYPos{};
};

struct MachineSetup
{
    Coordinate theLeftMovement;
    Coordinate theRightMovement;
    Coordinate thePrize;
};

enum class State
{
    READ_LEFT,
    READ_RIGHT,
    READ_PRIZE
};

int main()
{
    std::vector<MachineSetup> myMachineSetups;
    FileReader::processLines(NonCopyablePath{"p13/input.txt"}, [&, myState = State{}, myMachineSetup = MachineSetup{}](const std::string& aLine) mutable {
        if (aLine.empty()) return;

        std::vector<std::string> myLineParts;
        for (const auto myPart : std::views::split(aLine, ' '))
        {
            myLineParts.emplace_back(std::string_view{myPart});
        }

        if (myState == State::READ_LEFT)
        {
            const auto& myXPart{myLineParts[2]};
            const auto& myYPart{myLineParts[3]};
            myMachineSetup.theLeftMovement.theXPos = toInt(myXPart.substr(2, myXPart.size()-3));
            myMachineSetup.theLeftMovement.theYPos = toInt(myYPart.substr(2));
            myState = State::READ_RIGHT;
        }
        else if (myState == State::READ_RIGHT)
        {
            const auto& myXPart{myLineParts[2]};
            const auto& myYPart{myLineParts[3]};
            myMachineSetup.theRightMovement.theXPos = toInt(myXPart.substr(2, myXPart.size()-3));
            myMachineSetup.theRightMovement.theYPos = toInt(myYPart.substr(2));
            myState = State::READ_PRIZE;
        }
        else if (myState == State::READ_PRIZE)
        {
            const auto& myXPart{myLineParts[1]};
            const auto& myYPart{myLineParts[2]};
            myMachineSetup.thePrize.theXPos = toInt(myXPart.substr(2, myXPart.size()-3));
            myMachineSetup.thePrize.theYPos = toInt(myYPart.substr(2));
            myMachineSetups.push_back(myMachineSetup);
            myState = State::READ_LEFT;
        }
    });

    const std::int64_t myTokensNeeded{std::ranges::fold_left(myMachineSetups, 0ll, [](std::int64_t aTokensNeeded, const MachineSetup& aMachineSetup){
        const std::int64_t myNum{(
            aMachineSetup.theLeftMovement.theXPos * aMachineSetup.thePrize.theYPos - aMachineSetup.theLeftMovement.theYPos * aMachineSetup.thePrize.theXPos)};
        const std::int64_t myDem{(
            aMachineSetup.theLeftMovement.theXPos * aMachineSetup.theRightMovement.theYPos) - (aMachineSetup.theRightMovement.theXPos * aMachineSetup.theLeftMovement.theYPos)};

        if (myNum % myDem != 0)
        {
            return aTokensNeeded;
        }
        else
        {
            const std::int64_t myNumRight{myNum/myDem};
            const std::int64_t myMovementRemaining{aMachineSetup.thePrize.theXPos - myNumRight * aMachineSetup.theRightMovement.theXPos};
            const std::int64_t myNumLeft{myMovementRemaining/aMachineSetup.theLeftMovement.theXPos};
            return aTokensNeeded + (3 * myNumLeft + myNumRight);

        }
    })};

    std::cout << "Solution: " << myTokensNeeded << '\n';
}