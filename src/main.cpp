#include <iostream>
#include <string>

#include "parser.hpp"

int main(int argc, char** argv) {
    const std::string input("../20200130.PSX_ITCH_50");
    Parser parseITCH50(input);
    return 0;
}
