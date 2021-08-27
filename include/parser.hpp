#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <boost/iostreams/device/mapped_file.hpp>
#include <iostream>
#include <arpa/inet.h>

class Parser {
    public:
    Parser(const std::string& filename);
    private:
    bool parseFile();
    bool parseMessage();
    inline uint8_t parseMsgType();
    inline uint16_t parseMsgLength();
    inline void copyMsgFromMemory();
    std::size_t itch_itr_;
    boost::iostreams::mapped_file_source itch_data_;
    std::size_t itch_data_size_;
};

#endif
