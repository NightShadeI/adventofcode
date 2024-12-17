#include "support/filereader.hpp"

#include <iostream>

int main()
{
    FileReader::processLines(NonCopyablePath{"p19/test.txt"}, [&](const std::string& aLine) {
    });
}