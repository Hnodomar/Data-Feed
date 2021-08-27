#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <unordered_map>
#include <string>
#include <arpa/inet.h>

int main(int argc, char** argv) {
    boost::iostreams::mapped_file_source itch_data("../20200130.PSX_ITCH_50");
    std::size_t itr = 0;
    std::cout << "Itch Data size: " << itch_data.size() << std::endl;
    char type;
    std::unordered_map<char, uint> type_counts;
    std::vector<uint16_t> lens;
    while (itr < 10000) {
        int len = ntohs(
            *reinterpret_cast<const uint16_t*>((itch_data.data() + itr))
        );
        //__builtin_bswap16(*reinterpret_cast<const uint16_t *>(itch_data.data() + itr));
        std::cout << len << std::endl;
        itr += 2;
        type = *(itch_data.data() + itr);
        ++itr;
        type_counts[type]++;
        switch(type) {
            case 'S':
                itr += 11;
                break;
            case 'R': {
                char ticker[9] = {0};
                int i = itr + 10;
                strncpy(ticker, (itch_data.data() + i), 8);
                //std::cout << ticker << std::endl;
                itr += 38;
                break;
            }
            case 'H':
                itr += 24;
                break;
            case 'Y':
                itr += 19;
                break;
            case 'L':
                itr += 25;
                break;
            case 'V':
                itr += 34;
                break;
            case 'W':
                itr += 11;
                break;
            case 'K':
                itr += 27;
                break;
            case 'J':
                itr += 34;
                break;
            case 'h':
                itr += 20;
                break;
            case 'A':
                itr += 35;
                break;
            case 'F':
                itr += 39;
                break;
            case 'E':
                itr += 30;
                break;
            case 'C':
                itr += 35;
                break;
            case 'X':
                itr += 22;
                break;
            case 'D':
                itr += 18;
                break;
            case 'U':
                itr += 34;
                break;
            case 'P':
                itr += 43;
                break;
            case 'Q':
                itr += 39;
                break;
            case 'B':
                itr += 18;
                break;
            case 'I':
                itr += 49;
                break;
            case 'N':
                itr += 19;
                break;
            default:
                std::cout << "Wrong type!!\n";
                break;
        }
    }
    for (const auto& ele : type_counts) {
        std::cout << ele.first << ": " << ele.second << "\n";
    }
    std::cout << std::endl;
    return 0;
}
