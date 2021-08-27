#include "parser.hpp"

Parser::Parser(const std::string& filename):
    itch_itr_(0), itch_data_(filename) {
        if (!itch_data_.is_open()) {
            std::cout << "error\n";
        }
        itch_data_size_ = itch_data_.size();
        parseFile();
}

bool Parser::parseFile() {
    while (itch_itr_ < itch_data_size_)
        parseMessage();
}

bool Parser::parseMessage() {
    uint16_t msg_length = parseMsgLength();
    uint8_t type = parseMsgType();
    
    switch(type) {
        case 'A':
            break;
        case 'F':
            break;
        case 'E':
            break;
        case 'C':
            break;
        case 'X':
            break;
        case 'D':
            break;
        case 'U':
            break;
    }
}

inline uint8_t Parser::parseMsgType() {
    return *(itch_data_.data() + itch_itr_);
}

inline uint16_t Parser::parseMsgLength() {
    return ntohs(*reinterpret_cast<const uint16_t*>(
        itch_data_.data() + itch_itr_
    ));
    itch_itr_ += 2;
}

//ensures that we can more confidently read our
//message memory into the cache
inline void Parser::copyMsgFromMemory(uint16_t& len, uint8_t*& buffer) {

}
