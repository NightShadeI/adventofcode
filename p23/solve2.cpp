#include "support/filereader.hpp"

#include <algorithm>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

struct ConnectionPair
{
    std::string theFirstComputer;
    std::string theSecondComputer;
};

void addIfNotExist(std::vector<std::vector<std::string_view>>& aCliqueGroups, std::vector<std::string_view> aGroup)
{
    std::ranges::sort(aGroup);
    if (!std::ranges::contains(aCliqueGroups, aGroup))
    {
        aCliqueGroups.emplace_back(std::move(aGroup));
    }
}

int main()
{
    std::vector<ConnectionPair> myLines;
    FileReader::processLines(NonCopyablePath{"p23/input.txt"}, [&](const std::string& aLine) {
        myLines.emplace_back(aLine.substr(0, 2), aLine.substr(3));
    });

    std::vector<std::vector<std::string_view>> myCliqueGroups;
    std::unordered_map<std::string_view, std::vector<std::string_view>> myAdjGraph;
    for (const auto& [myFirstComp, mySecondComp] : myLines)
    {
        myAdjGraph[myFirstComp].emplace_back(mySecondComp);
        myAdjGraph[mySecondComp].emplace_back(myFirstComp);
        addIfNotExist(myCliqueGroups, {myFirstComp, mySecondComp});
    }

    while (true)
    {
        std::cout << myCliqueGroups.front().size() << '\n';
        std::cout << myCliqueGroups.size() << '\n';
        std::vector<std::vector<std::string_view>> myLargerGroup;
        for (const auto& myGroup : myCliqueGroups)
        {
            for (const auto& [myComputer, myAdjList] : myAdjGraph)
            {
                if (!std::ranges::contains(myGroup, myComputer) && std::ranges::all_of(myGroup, [&](std::string_view aComputerName){
                    return std::ranges::contains(myAdjList, aComputerName);
                }))
                {
                    std::vector<std::string_view> myNewGroup{myGroup};
                    myNewGroup.emplace_back(myComputer);
                    addIfNotExist(myLargerGroup, myNewGroup);
                }
            }
        }
        if (myLargerGroup.empty())
        {
            break;
        }
        myCliqueGroups = std::move(myLargerGroup);
    }

    std::cout << "Done\n";
    std::cout << "Solution: ";
    const auto& myBestGroup{myCliqueGroups.front()};
    for (std::size_t myIdx{0}; myIdx < myBestGroup.size() - 1; ++myIdx)
    {
        std::cout << myBestGroup[myIdx] << ',';
    }
    std::cout << myBestGroup[myBestGroup.size()- 1];
}