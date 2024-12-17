#include "support/conversions.hpp"
#include "support/filereader.hpp"
#include "support/strprocessing.hpp"

#include <iostream>

enum class State
{
    REGISTER_READ,
    PROGRAM_READ
};

[[nodiscard]] std::int64_t getComboOperand(const std::array<std::int64_t, 3>& aRegisters, std::int64_t anOperand)
{
    [[assume (anOperand <= 6)]];

    if (anOperand <= 3)
    {
        return anOperand;
    }
    return aRegisters[anOperand - 4];
}

[[nodiscard]] std::vector<std::int64_t> runProgram(const std::array<std::int64_t, 3>& aRegisters, const std::vector<std::int64_t>& anInstructions)
{
    std::array<std::int64_t, 3> myRegisters{aRegisters};
    std::vector<std::int64_t> myOutput{};
    std::int64_t myInstructionPtr{0};

    while (true)
    {
        if (myInstructionPtr >= std::ssize(anInstructions) - 1)
        {
            break;
        }

        const std::int64_t myInstruction{anInstructions[myInstructionPtr]};
        const std::int64_t myOperand{anInstructions[myInstructionPtr+1]};

        if (myInstruction == 0)
        {
            const std::int64_t myNumerator{myRegisters[0]};
            const std::int64_t myComboOperand{getComboOperand(myRegisters, myOperand)};
            const std::int64_t myDenominator{1 << myComboOperand};
            myRegisters[0] = myNumerator/myDenominator;
        }
        else if (myInstruction == 1)
        {
            myRegisters[1] ^= myOperand;
        }
        else if (myInstruction == 2)
        {
            const std::int64_t myComboOperand{getComboOperand(myRegisters, myOperand)};
            myRegisters[1] = myComboOperand % 8;
        }
        else if (myInstruction == 3)
        {
            if (myRegisters[0] != 0)
            {
                myInstructionPtr = myOperand;
                continue;
            }
        }
        else if (myInstruction == 4)
        {
            myRegisters[1] = myRegisters[1] ^ myRegisters[2];
        }
        else if (myInstruction == 5)
        {
            const std::int64_t myComboOperand{getComboOperand(myRegisters, myOperand)};
            myOutput.emplace_back(myComboOperand % 8);
        }
        else if (myInstruction == 6)
        {
            const std::int64_t myNumerator{myRegisters[0]};
            const std::int64_t myComboOperand{getComboOperand(myRegisters, myOperand)};
            const std::int64_t myDenominator{1 << myComboOperand};
            myRegisters[1] = myNumerator/myDenominator;
        }
        else if (myInstruction == 7)
        {
            const std::int64_t myNumerator{myRegisters[0]};
            const std::int64_t myComboOperand{getComboOperand(myRegisters, myOperand)};
            const std::int64_t myDenominator{1 << myComboOperand};
            myRegisters[2] = myNumerator/myDenominator;
        }

        myInstructionPtr += 2;
    }

    return myOutput;
}

void solve(const std::vector<std::int64_t>& anInstructions, std::int64_t anInputA, std::int64_t aDepth)
{
    if (aDepth >= 16)
    {
        std::cout << "Solution: " << (anInputA >> 3) << '\n';
        return;
    }

    for (std::int64_t myCnt{0}; myCnt < 8; ++myCnt)
    {
        const std::int64_t myTryInput{anInputA + myCnt};
        const auto myProgramOutput{runProgram({myTryInput, 0, 0}, anInstructions)};
        if (myProgramOutput[myProgramOutput.size() - aDepth - 1] == anInstructions[anInstructions.size() - aDepth - 1])
        {
            solve(anInstructions, myTryInput << 3, aDepth + 1);
        }
    }
}

int main()
{
    std::vector<std::int64_t> myInstructions{};
    FileReader::processLines(NonCopyablePath{"p17/input.txt"}, [&, myState = State::REGISTER_READ](const std::string& aLine) mutable {
        if (aLine.empty())
        {
            myState = State::PROGRAM_READ;
            return;
        }

        if (myState == State::PROGRAM_READ)
        {
            const auto myProgramParts{splitBy(aLine, " ")};
            const auto myProgramCode{splitBy(myProgramParts[1], ",")};
            for (const std::string_view myInstruction : myProgramCode)
            {
                myInstructions.emplace_back(toInt(myInstruction));
            }
        }
    });

    solve(myInstructions, 0, 0);
}