#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <iostream>
#include <regex>

int main()
{
    const auto myRows = FileReader::readColumns(NonCopyablePath{"p3/input.txt"}, "\n");

    std::regex myPattern{"mul\\((\\d+),(\\d+)\\)"};

    std::smatch myMatchResult;

    std::int64_t myTotal{0};
    for (const auto& myRow : myRows)
    {
        if (myRow.empty()) continue;
        const std::string& myLine{myRow.front()};
        auto mySearchIt{myLine.cbegin()};
        while (std::regex_search(mySearchIt, myLine.cend(), myMatchResult, myPattern))
        {
            const auto myPossibleLhsInt{toInt(myMatchResult[1].str())};
            const auto myPossibleRhsInt{toInt(myMatchResult[2].str())};
            if (!myPossibleLhsInt.has_value() || !myPossibleRhsInt.has_value())
            {
                break;
            }

            myTotal += (myPossibleLhsInt.value() * myPossibleRhsInt.value());
            mySearchIt = myMatchResult.suffix().first;
        }
    }

    std::cout << "Solution: " << myTotal << '\n';
}