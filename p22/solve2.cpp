#include "support/conversions.hpp"
#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>
#include <map>

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

    using DiffKeyT = std::tuple<std::int64_t, std::int64_t, std::int64_t, std::int64_t>;
    std::map<DiffKeyT, std::int64_t> myBananasAvailable;

    for (std::int64_t mySeedValue : mySecrets)
    {
        std::map<DiffKeyT, std::int64_t> myBananaGains;
        std::vector<std::int64_t> myGainedBananas;
        std::vector<std::int64_t> myDiffs;
        myDiffs.reserve(5);
        for (std::int64_t myIdx{0}; myIdx < 2'000; ++myIdx)
        {
            const std::int64_t myBananaGain{mySeedValue % 10};
            myGainedBananas.emplace_back(myBananaGain);
            if (myGainedBananas.size() > 1)
            {
                myDiffs.emplace_back(myBananaGain - myGainedBananas[myGainedBananas.size()-2]);
                if (myDiffs.size() > 4)
                {
                    myDiffs.erase(myDiffs.begin());
                }
            }
            if (myDiffs.size() >= 4)
            {
                const auto myKey{std::make_tuple(myDiffs[0], myDiffs[1], myDiffs[2], myDiffs[3])};
                myBananaGains.try_emplace(myKey, myBananaGain);
            }
            mySeedValue = applySecretSequence(mySeedValue);
        }

        for (const auto& [myDiffSeq, myBananaGain] : myBananaGains)
        {
            myBananasAvailable[myDiffSeq] += myBananaGain;
        }
    };

    DiffKeyT myBestSequence{};
    std::int64_t myBestBananas{};
    for (const auto& [myDiffSeq, myCnt] : myBananasAvailable)
    {
        if (myCnt > myBestBananas)
        {
            myBestBananas = myCnt;
            myBestSequence = myDiffSeq;
        }
    }

    std::cout << "Solution: " << myBestBananas << '\n';
}