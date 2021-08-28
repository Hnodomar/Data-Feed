#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <boost/iostreams/device/mapped_file.hpp>
#include <iostream>
#include "feedhandler.hpp"

class Parser {
    public:
    Parser(const std::string& filename, FeedHandler<Parser>& feedhandler);
    bool parseFile();
    private:
    bool parseMessage();
    inline void copyMsgFromMap(uint16_t len);
    inline void addOrder();
    inline void cancelOrder();
    inline void replaceOrder();
    inline void executeOrder();
    inline void executeOrderPrice();
    inline void deleteOrder();

    uint16_t parseTwoBytes(const uint8_t* buffer) {
        return __builtin_bswap16(
            *reinterpret_cast<const uint16_t*>(buffer)
        );
    }

    uint32_t parseFourBytes(const uint8_t* buffer) {
        return __builtin_bswap32(
            *reinterpret_cast<const uint32_t*>(buffer)
        );
    }

    uint64_t parseSixBytes(const uint8_t* buffer) {
        uint64_t ret = 
            ((uint64_t)buffer[0] << 40) |
            ((uint64_t)buffer[1] << 32) |
            ((uint64_t)buffer[2] << 24) |
            ((uint64_t)buffer[3] << 16) |
            ((uint64_t)buffer[4] << 8)  |
            ((uint64_t)buffer[5] << 0);
        return ret;
    }

    uint64_t parseEightBytes(const uint8_t* buffer) {
        return __builtin_bswap64(
            *reinterpret_cast<const uint64_t*>(buffer)
        );
    }
    
    FeedHandler<Parser>& feedhandler_;
    boost::iostreams::mapped_file_source itch_data_;
    const uint8_t* itch_data_ptr_;
    const uint8_t* itch_data_end_ptr_;
    std::size_t itch_data_size_;
    uint8_t buffer_[512] = {0}; //factor of 16 - cacheline friendly
};

#endif
