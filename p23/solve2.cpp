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

struct Clique
{
    std::vector<std::int64_t> theNodes;

    [[nodiscard]] bool operator<=>(const Clique&) const = default;
    struct Hash
    {
        [[nodiscard]] std::size_t operator()(const Clique& aClique) const
        {
            std::size_t myResult{17};
            for (const std::int64_t myNode : aClique.theNodes)
            {
                myResult = 31 * myResult + std::hash<std::int64_t>()(myNode);
            }
            return myResult;
        }
    };

};

int main()
{
    std::vector<ConnectionPair> myLines;
    FileReader::processLines(NonCopyablePath{"p23/input.txt"}, [&](const std::string& aLine) {
        myLines.emplace_back(aLine.substr(0, 2), aLine.substr(3));
    });

    std::unordered_set<Clique, Clique::Hash> myCliqueGroups;
    std::array<std::vector<std::int64_t>, 26*26> myAdjGraph;
    for (const auto& [myFirstComp, mySecondComp] : myLines)
    {
        const auto myGenHash = [](std::string_view aComputer){ return 26 * (aComputer[0] - 'a') + (aComputer[1] - 'a'); };
        const auto myFirstCompHash{myGenHash(myFirstComp)};
        const auto mySecondCompHash{myGenHash(mySecondComp)};
        myAdjGraph[myFirstCompHash].emplace_back(mySecondCompHash);
        myAdjGraph[mySecondCompHash].emplace_back(myFirstCompHash);
        if (myFirstComp < mySecondComp)
        {
            myCliqueGroups.emplace(Clique{{myFirstCompHash, mySecondCompHash}});
        }
        else
        {
            myCliqueGroups.emplace(Clique{{mySecondCompHash, myFirstCompHash}});
        }
    }

    while (true)
    {
        std::unordered_set<Clique, Clique::Hash> myLargerGroup;
        for (const auto& myGroup : myCliqueGroups)
        {
            const std::int64_t myQueryComputer{myGroup.theNodes.front()};
            const auto& myAdjList{myAdjGraph[myQueryComputer]};
            const auto myFilterToClique = [&](std::int64_t aCurrentComputer){
                if (aCurrentComputer <= myGroup.theNodes[myGroup.theNodes.size()-1]) return false; 
                const auto& myQueryList{myAdjGraph[aCurrentComputer]};
                return std::ranges::all_of(myGroup.theNodes, [&myQueryList](std::int64_t aGroupComputer){
                    return std::ranges::contains(myQueryList, aGroupComputer);
                });
            };
            
            for (const std::int64_t myNewCantidate : myAdjList | std::views::filter(myFilterToClique))
            {
                Clique myNewGroup{myGroup};
                myNewGroup.theNodes.emplace_back(myNewCantidate);
                myLargerGroup.emplace(std::move(myNewGroup));
            }
        }
        if (myLargerGroup.empty())
        {
            break;
        }
        myCliqueGroups = std::move(myLargerGroup);
    }

    std::cout << "Solution: ";

    const auto myHashToOut = [](std::ostream& aOutStream, std::int64_t aHash){
        aOutStream << static_cast<char>('a' + aHash/26) << static_cast<char>('a' + aHash%26); };
    const auto& myBestGroup{*myCliqueGroups.begin()};
    for (std::size_t myIdx{0}; myIdx < myBestGroup.theNodes.size() - 1; ++myIdx)
    {
        myHashToOut(std::cout, myBestGroup.theNodes[myIdx]);
        std::cout << ',';
    }
    myHashToOut(std::cout, myBestGroup.theNodes[myBestGroup.theNodes.size()- 1]);
}