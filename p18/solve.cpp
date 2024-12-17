#include "support/filereader.hpp"

#include <iostream>

int main()
{
    FileReader::processLines(NonCopyablePath{"p18/test.txt"}, [&](const std::string& aLine) {
    });
}