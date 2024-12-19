#include "support/filereader.hpp"
#include "support/strprocessing.hpp"

#include <algorithm>
#include <iostream>

enum class ReadState
{
    TOWELS,
    PATTERNS
};

int main()
{
    std::vector<std::string> myTowels;
    std::vector<std::string> myPatterns;
    FileReader::processLines(NonCopyablePath{"p19/input.txt"}, [&, myState = ReadState::TOWELS](const std::string& aLine) mutable {
        if (aLine.empty())
        {
            myState = ReadState::PATTERNS;
            return;
        }

        if (myState == ReadState::TOWELS)
        {
            static constexpr std::string_view DELIM{", "};
            for (const auto myTowel : std::views::split(aLine, DELIM))
            {
                myTowels.emplace_back(std::string_view{myTowel});
            }
        }
        else
        {
            myPatterns.emplace_back(aLine);
        }
    });

    const std::int64_t myLongestLine{
        std::ssize(*std::ranges::max_element(myTowels, [](const auto& aLhs, const auto& aRhs){ return aLhs.size() < aRhs.size(); }))};

    std::vector<std::vector<std::string>> myTowelsByLength(myLongestLine+1, std::vector<std::string>{});

    for (const auto& myTowel : myTowels)
    {
        myTowelsByLength[myTowel.size()].emplace_back(myTowel);
    }

    std::int64_t myTotalSolvable{0};

    for (const auto& myPattern : myPatterns)
    {
        std::vector<bool> mySolvable(myPattern.size(), false);
        for (std::int64_t myIdx{std::ssize(myPattern)-1}; myIdx >= 0; --myIdx)
        {
            const std::int64_t myMaxLength{std::min(myLongestLine, std::ssize(mySolvable) - myIdx)};
            for (std::int64_t myCurLength{1}; myCurLength <= myMaxLength; ++myCurLength)
            {
                if (const std::int64_t myEndIdx{myIdx + myCurLength}; myEndIdx < std::ssize(myPattern) && !mySolvable[myEndIdx])
                {
                    continue;
                }
                const auto& myPossibleTowels{myTowelsByLength[myCurLength]};
                for (const std::string& myPossibleTowel : myPossibleTowels)
                {
                    if (std::string_view{myPattern}.substr(myIdx, myCurLength) == std::string_view{myPossibleTowel})
                    {
                        mySolvable[myIdx] = true;
                        break;
                    }
                }
                if (mySolvable[myIdx])
                {
                    break;
                }
            }
        }
        if (mySolvable[0])
        {
            ++myTotalSolvable;
        }
    }

    std::cout << "Solution: " << myTotalSolvable << '\n';
}