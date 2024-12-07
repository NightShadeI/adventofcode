#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <iostream>
#include <set>

struct PageProcessor
{
    std::set<std::pair<int32_t, int32_t>> theRankingMatrix{};
    bool theProcessingUpdates{false};
    int theTotal{0};

    void operator()(const std::string& aLine)
    {
        if (aLine.empty())
        {
            theProcessingUpdates = true;
            return;
        }

        if (!theProcessingUpdates)
        {
            for (const auto myValue : std::views::split(aLine, '|') | std::views::adjacent<2>)
            {
                const auto [myPreceedingStr, myPostceedingStr] = myValue;
                const auto myPreceedingInt{toInt(std::string_view{myPreceedingStr})};
                const auto myPostceedingInt{toInt(std::string_view{myPostceedingStr})};

                theRankingMatrix.emplace(myPreceedingInt, myPostceedingInt);
            }
        }
        else
        {
            std::vector<int> myValues{};
            for (const auto myValueStr : std::views::split(aLine, ','))
            {
                const auto myValue{toInt(std::string_view{myValueStr})};

                myValues.emplace_back(myValue);
            }

            bool myFoundInvalidLine{false};
            for (int myOuterIdx{0}; myOuterIdx < std::ssize(myValues); ++myOuterIdx)
            {
                const auto myEndIt{myValues.begin() + myOuterIdx};
                const auto myIt{std::ranges::find_if(myValues.begin(), myEndIt, [&](int aQueryValue){
                    return !theRankingMatrix.contains(std::make_pair(aQueryValue, myValues[myOuterIdx]));
                })};
                if (myIt != myEndIt)
                {
                    myFoundInvalidLine = true;
                    break;
                }
            }
            
            if (!myFoundInvalidLine)
            {
                theTotal += myValues[myValues.size()/2];
            }
        }
    }
};

int main()
{
    PageProcessor myPageProcessor{};
    FileReader::processLines(NonCopyablePath{"p5/input.txt"}, myPageProcessor);
    std::cout << "Solution: " << myPageProcessor.theTotal << '\n';
}