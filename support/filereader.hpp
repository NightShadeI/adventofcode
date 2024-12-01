#pragma once

#include "support/nocopy.hpp"

#include <cstdint>
#include <fstream>
#include <filesystem>
#include <ranges>
#include <stdexcept>
#include <vector>

struct NonCopyablePath : public NonCopyable
{
    std::filesystem::path thePath;

    explicit NonCopyablePath(std::string_view aPath) noexcept
        : thePath{aPath}
    {}
};

class FileReader
{
public:
    template<size_t NumColumns>
    [[nodiscard]] static std::vector<std::array<std::string, NumColumns>> readColumns(NonCopyablePath aPath, std::string_view aDelim = "   ")
    {
        std::vector<std::array<std::string, NumColumns>> myRows;
        std::ifstream myFileStream{aPath.thePath};

        if (!myFileStream)
        {
            throw std::runtime_error{std::string{"Could not open file: "} + aPath.thePath.string()};
        }

        std::string myLine{};
        while (std::getline(myFileStream, myLine))
        {
            auto& myRow = myRows.emplace_back();
            for (const auto [myIdx, myValue] : std::views::split(myLine, aDelim) 
                                                | std::views::enumerate
                                                | std::views::take(NumColumns))
            {
                myRow[myIdx] = std::string_view{myValue};
            }
        }

        return myRows;
    }

};