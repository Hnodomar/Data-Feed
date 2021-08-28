#include "parser.hpp"

Parser::Parser(const std::string& filename, FeedHandler<Parser>& feedhandler):
    feedhandler_(feedhandler), itch_data_(filename),
    itch_data_ptr_(reinterpret_cast<const uint8_t*>(itch_data_.data())),
    itch_data_end_ptr_(reinterpret_cast<const uint8_t*>((itch_data_.data() + itch_data_.size()))) {
    if (!itch_data_.is_open()) {
        std::cout << "error\n";
    }
    itch_data_size_ = itch_data_.size();
    parseFile();
}

bool Parser::parseFile() {
    while (itch_data_ptr_ < itch_data_end_ptr_)
        parseMessage();
    return true;
}

bool Parser::parseMessage() {
    uint16_t msg_length = parseTwoBytes(itch_data_ptr_);
    itch_data_ptr_ += 2;
    copyMsgFromMap(msg_length);
    uint8_t type = buffer_[0];
    switch(type) {
        case 'A':
            addOrder();
            break;
        case 'F':
            addOrder();
            break;
        case 'E':
            executeOrder();
            break;
        case 'C':
            executeOrderPrice();
            break;
        case 'X':
            cancelOrder();
            break;
        case 'D':
            deleteOrder();
            break;
        case 'U':
            replaceOrder();
            break;
    }
    return true;
}

//ensures that we can more confidently read our
//message memory into the cache
inline void Parser::copyMsgFromMap(uint16_t len) {
    memcpy(buffer_, itch_data_ptr_, len);
    itch_data_ptr_ += len;
}

inline void Parser::addOrder() {
    uint64_t reference = parseEightBytes(buffer_ + 11);
    uint8_t side = buffer_[19];
    uint32_t num_shares = parseFourBytes(buffer_ + 20);
    uint64_t ticker = parseEightBytes(buffer_ + 24);
    uint32_t price = parseFourBytes(buffer_ + 32);
    feedhandler_.addOrder(
        reference, side, num_shares, ticker, price
    );
}

inline void Parser::executeOrder() {
    uint64_t reference = parseEightBytes(buffer_ + 11);
    uint32_t num_shares = parseFourBytes(buffer_ + 19);
    feedhandler_.executeOrder(reference, num_shares);
}

inline void Parser::executeOrderPrice() {
    uint64_t reference = parseEightBytes(buffer_ + 11);
    uint32_t num_shares = parseFourBytes(buffer_ + 19);
    //uint32_t price = parseFourBytes(buffer_ + 32);
    feedhandler_.executeOrder(reference, num_shares);
}

inline void Parser::cancelOrder() {
    uint64_t reference = parseEightBytes(buffer_ + 11);
    uint32_t num_shares = parseFourBytes(buffer_ + 19);
    feedhandler_.cancelOrder(reference, num_shares);
}

inline void Parser::deleteOrder() {
    uint64_t reference = parseEightBytes(buffer_ + 11);
    feedhandler_.deleteOrder(reference);
}

inline void Parser::replaceOrder() {
    uint64_t reference = parseEightBytes(buffer_ + 11);
    uint64_t new_reference = parseEightBytes(buffer_ + 19);
    uint32_t num_shares = parseFourBytes(buffer_ + 27);
    uint32_t price = parseFourBytes(buffer_ + 31);
    feedhandler_.replaceOrder(reference, new_reference, num_shares, price);
}
