#include "support/filereader.hpp"

#include <iostream>

int main()
{
    FileReader::processLines(NonCopyablePath{"p20/test.txt"}, [&](const std::string& aLine) {
    });
}