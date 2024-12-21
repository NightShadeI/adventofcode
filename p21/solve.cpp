#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <iostream>
#include <unordered_map>

[[nodiscard]] std::int64_t getGridIndex(char aChar)
{
    return aChar == 'A' ? 10 : (aChar - '0');
}

[[nodiscard]] std::int64_t getArrowIndex(char aChar)
{
    if (aChar == '<')
    {
        return 0;
    }
    if (aChar == '>')
    {
        return 1;
    }
    if (aChar == '^')
    {
        return 2;
    }
    if (aChar == 'v')
    {
        return 3;
    }
    return 4;
}

int main()
{
    std::vector<std::string> myInputs;
    FileReader::processLines(NonCopyablePath{"p21/input.txt"}, [&](const std::string& aLine) {
        myInputs.emplace_back(aLine);
    });

    // 7 8 9
    // 4 5 6
    // 1 2 3
    //   0 A

        // 0, 1, ... , A
    static constexpr std::array<std::array<std::string_view, 11>, 11> OPTIMAL_GRID_MOVES{
        std::array<std::string_view, 11>{"", "^<", "^", "^>", "^^<", "^^", "^^>", "^^^<", "^^^", "^^^>", ">"}, // 0
                                        {">v", "", ">", ">>", "^", "^>", "^>>", "^^", "^^>", "^^>>", ">>v"}, // 1
                                        {"v", "<", "", ">", "<^", "^", "^>", "<^^", "^^", "^^>", "v>"}, // 2
                                        {"<v", "<<", "<", "", "<<^", "<^", "^", "<<^^", "<^^", "^^", "v"}, // 3
                                        {">vv", "v", "v>", "v>>", "", ">", ">>", "^", "^>", "^>>", ">>vv"}, // 4
                                        {"vv", "<v", "v", "v>", "<", "", ">", "<^", "^", "^>", "vv>"}, // 5
                                        {"<vv", "<<v", "<v", "v", "<<", "<", "", "<<^", "<^", "^", "vv"}, // 6
                                        {">vvv", "vv", "vv>", "vv>>", "v", "v>", "v>>", "", ">", ">>", ">>vvv"}, // 7
                                        {"vvv", "<vv", "vv", "vv>", "<v", "v", "v>", "<", "", ">", "vvv>"}, // 8
                                        {"<vvv", "<<vv", "<vv", "vv", "<<v", "<v", "v", "<<", "<", "", "vvv"}, // 9
                                        {"<", "^<<", "<^", "^", "^^<<", "<^^", "^^", "^^^<<", "<^^^", "^^^", ""} // A
    };
        
        // <>^vA
    static constexpr std::array<std::array<std::string_view, 5>, 5> OPTIMAL_ARROW_MOVES{
        std::array<std::string_view, 5>{"", ">>", ">^", ">", ">>^"}, // <
                                       {"<<", "", "<^", "<", "^"}, // >
                                       {"v<", "v>", "", "v", ">"}, // ^
                                       {"<", ">", "^", "", "^>"}, // v
                                       {"v<<", "v", "<", "<v", ""}  // A
    };

    std::int64_t myTotal{0};
    for (const auto& myLine : myInputs)
    {
        std::unordered_map<std::string, std::int64_t> myPrevCounts{};
        std::int64_t myRestingIdx{std::ssize(OPTIMAL_GRID_MOVES) - 1};
        for (const char myChar : myLine)
        {
            const std::int64_t myNextIdx{getGridIndex(myChar)};
            const std::string myMovement{std::string{OPTIMAL_GRID_MOVES[myRestingIdx][myNextIdx]} + "A"};
            ++myPrevCounts[myMovement];
            myRestingIdx = myNextIdx;
        }

        for (std::int64_t myIdx{0}; myIdx < 2; ++myIdx)
        {
            std::unordered_map<std::string, std::int64_t> myCurrentCounts{};
            for (const auto& [myKey, myCnt] : myPrevCounts)
            {
                myRestingIdx = std::ssize(OPTIMAL_ARROW_MOVES) - 1;
                for (const char myChar : myKey)
                {
                    const std::int64_t myNextIdx{getArrowIndex(myChar)};
                    const std::string myMovement{std::string{OPTIMAL_ARROW_MOVES[myRestingIdx][myNextIdx]} + "A"};
                    myCurrentCounts[myMovement] += myCnt;     
                    myRestingIdx = myNextIdx;
                }
            }
            myPrevCounts = std::move(myCurrentCounts);
        }

        const std::int64_t myLineEncoding{toInt(myLine.substr(0, myLine.size()-1))};
        std::int64_t myLineLen{0};
        for (const auto& [myKey, myCnt] : myPrevCounts)
        {
            myLineLen += myKey.size() * myCnt;
        }
        myTotal += myLineLen * myLineEncoding;
    }

    std::cout << "Solution: " << myTotal << '\n';
}