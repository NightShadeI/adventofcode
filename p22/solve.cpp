#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>

std::int64_t applySecretSequence(std::int64_t aSecretNumber)
{
    static constexpr std::int64_t MOD_VAL{16777216};
    aSecretNumber = ((aSecretNumber * 64) ^ aSecretNumber) % MOD_VAL;
    aSecretNumber = ((aSecretNumber / 32) ^ aSecretNumber) % MOD_VAL;
    aSecretNumber = ((aSecretNumber * 2048) ^ aSecretNumber) % MOD_VAL;
    return aSecretNumber;
}

int main()
{
    std::vector<std::int64_t> mySecrets;
    FileReader::processLines(NonCopyablePath{"p22/input.txt"}, [&](const std::string& aLine) {
        mySecrets.emplace_back(toInt(aLine));
    });

    const std::int64_t myTotal{std::ranges::fold_left(mySecrets, 0ull, [](std::int64_t aCurrent, std::int64_t aSeedValue){
        for (std::int64_t myIdx{0}; myIdx < 2000; ++myIdx)
        {
            aSeedValue = applySecretSequence(aSeedValue);
        }
        return aCurrent + aSeedValue;
    })};

    std::cout << "Solution: " << myTotal << '\n';
}