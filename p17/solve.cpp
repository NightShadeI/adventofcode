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

int main()
{
    std::array<std::int64_t, 3> myRegisters{};
    std::vector<std::int64_t> myInstructions{};
    FileReader::processLines(NonCopyablePath{"p17/input.txt"}, [&, myState = State::REGISTER_READ](const std::string& aLine) mutable {
        if (aLine.empty())
        {
            myState = State::PROGRAM_READ;
            return;
        }

        if (myState == State::REGISTER_READ)
        {
            const auto myParts{splitBy(aLine, " ")};
            const std::int64_t myRegisterIdx{myParts[1][0] - 'A'};
            myRegisters[myRegisterIdx] = toInt(myParts[2]);
        }
        else
        {
            const auto myProgramParts{splitBy(aLine, " ")};
            const auto myProgramCode{splitBy(myProgramParts[1], ",")};
            for (const std::string_view myInstruction : myProgramCode)
            {
                myInstructions.emplace_back(toInt(myInstruction));
            }
        }
    });

    std::vector<std::int64_t> myOutput{};
    std::int64_t myInstructionPtr{0};

    while (true)
    {
        if (myInstructionPtr >= std::ssize(myInstructions) - 1)
        {
            std::cout << "Program complete\n";
            break;
        }

        const std::int64_t myInstruction{myInstructions[myInstructionPtr]};
        const std::int64_t myOperand{myInstructions[myInstructionPtr+1]};

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

    for (std::size_t myIdx{0}; myIdx < myOutput.size() - 1; ++myIdx)
    {
        std::cout << myOutput[myIdx] << ',';
    }
    std::cout << myOutput.back() << '\n';
}