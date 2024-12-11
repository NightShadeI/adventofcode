#include "support/filereader.hpp"
#include "support/conversions.hpp"

#include <iostream>
#include <unordered_map>

int main()
{
    std::unordered_map<std::int64_t, std::int64_t> myNumberBuckets{};
    const auto myRows{FileReader::readColumns(NonCopyablePath{"p11/input.txt"}, " ")};
    std::vector<std::int64_t> myIntegralRows{rowsToInts(myRows).front()};
    for (std::int64_t myIntegral : myIntegralRows)
    {
        ++myNumberBuckets[myIntegral];
    }

    for (std::int64_t myNum{0}; myNum < 25; ++myNum)
    {
        std::unordered_map<std::int64_t, std::int64_t> myNewBuckets{};
        for (const auto& [myKey, myVal] : myNumberBuckets)
        {
            if (myKey == 0)
            {
                myNewBuckets[1] += myVal;
                continue;
            }
            const std::string myNumStr{std::to_string(myKey)};
            
            if (myNumStr.size() % 2 == 0)
            {

                myNewBuckets[toInt(std::string_view{myNumStr}.substr(0, myNumStr.size()/2))] += myVal;
                myNewBuckets[toInt(std::string_view{myNumStr}.substr(myNumStr.size()/2))] += myVal;
            }
            else
            {
                myNewBuckets[2024 * myKey] += myVal;
            }
        }
        myNumberBuckets = std::move(myNewBuckets);
    }

    std::int64_t myResult{};
    for (const auto& [myKey, myVal] : myNumberBuckets)
    {
        myResult += myVal;        
    }
    std::cout << "Solution: " << myResult << '\n';
}