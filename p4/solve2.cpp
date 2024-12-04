#include "support/filereader.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>

std::vector<std::string> flattenToMatrix(const std::vector<std::vector<std::string>>& anInput)
{
    std::vector<std::string> myMatrix{};
    std::ranges::transform(anInput, std::back_insert_iterator(myMatrix), [](const auto& aLine){
        return aLine.front();
    });
    return myMatrix;
}

int main()
{
    const auto myRows = FileReader::readColumns(NonCopyablePath{"p4/input.txt"}, "\n");

    const auto myMatrix{flattenToMatrix(myRows)};

    int myTotalMatches{0};

    for (int myY{0}; myY < std::ssize(myMatrix)-2; ++myY)
    {
        for (int myX{0}; myX < std::ssize(myMatrix.front())-2; ++myX)
        {
            static constexpr std::array ROTATIONS{
                std::array{'M', 'S', 'A', 'M', 'S'},
                std::array{'M', 'M', 'A', 'S', 'S'},
                std::array{'S', 'M', 'A', 'S', 'M'},
                std::array{'S', 'S', 'A', 'M', 'M'}
            };

            const bool myXmasFound{std::ranges::any_of(ROTATIONS, [&](const auto& aRotation){
                if (myMatrix[myY][myX] != aRotation[0]) return false;
                if (myMatrix[myY][myX+2] != aRotation[1]) return false;
                if (myMatrix[myY+1][myX+1] != aRotation[2]) return false;
                if (myMatrix[myY+2][myX] != aRotation[3]) return false;
                if (myMatrix[myY+2][myX+2] != aRotation[4]) return false;
                return true;
            })};
            
            if (myXmasFound)
            {
                ++myTotalMatches;
            }
        }
    }

    std::cout << "Solutions: " << myTotalMatches << '\n';
}