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
    [[nodiscard]] static std::vector<std::vector<std::string>> readColumns(NonCopyablePath aPath, std::string_view aDelim = "   ")
    {
        std::vector<std::vector<std::string>> myRows;

        processLines(std::move(aPath), [&](const std::string& aLine){

            auto& myRow = myRows.emplace_back();
            for (const auto myValue : std::views::split(aLine, aDelim))
            {
               myRow.emplace_back(std::string_view{myValue});
            }
        });

        return myRows;
    }

    template<std::invocable<const std::string&> LineHandlerT>
    static void processLines(NonCopyablePath aPath, LineHandlerT&& aLineHandler)
    {
        std::ifstream myFileStream{aPath.thePath};

        if (!myFileStream)
        {
            throw std::runtime_error{std::string{"Could not open file: "} + aPath.thePath.string()};
        }

        std::string myLine{};
        while (std::getline(myFileStream, myLine))
        {
            aLineHandler(myLine);
        }

    }

};