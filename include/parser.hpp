#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include "feedhandler.hpp"

enum class SkipLogging { Skip, NoSkip };

template <class derived>
struct BaseParser {
    public:
    bool parseMessage(uint8_t*& msg) {
        uint8_t type = msg[0];
        switch(type) {
            case 'A':
            case 'F':
                static_cast<derived*>(this)->addOrder(msg);
                break;
            case 'E':
                static_cast<derived*>(this)->executeOrder(msg);
                break;
            case 'C':
                static_cast<derived*>(this)->executeOrderPrice(msg);
                break;
            case 'X':
                static_cast<derived*>(this)->cancelOrder(msg);
                break;
            case 'D':
                static_cast<derived*>(this)->deleteOrder(msg);
                break;
            case 'U':
                static_cast<derived*>(this)->replaceOrder(msg);
                break;
        }
        return true;
    }
    protected:
    uint16_t parseTwoBytesSwap(const uint8_t* buffer) {
        return __builtin_bswap16(
            *reinterpret_cast<const uint16_t*>(buffer)
        );
    }
    uint32_t parseFourBytesSwap(const uint8_t* buffer) {
        return __builtin_bswap32(
            *reinterpret_cast<const uint32_t*>(buffer)
        );
    }
    uint64_t parseSixBytesSwap(const uint8_t* buffer) {
        uint64_t ret = 
            ((uint64_t)buffer[0] << 40) |
            ((uint64_t)buffer[1] << 32) |
            ((uint64_t)buffer[2] << 24) |
            ((uint64_t)buffer[3] << 16) |
            ((uint64_t)buffer[4] << 8)  |
            ((uint64_t)buffer[5] << 0);
        return ret;
    }
    uint64_t parseEightBytesSwap(const uint8_t* buffer) {
        return __builtin_bswap64(
            *reinterpret_cast<const uint64_t*>(buffer)
        );
    }
    uint16_t parseTwoBytes(const uint8_t* buffer) {
        return *reinterpret_cast<const uint16_t*>(buffer);
    }
    uint32_t parseFourBytes(const uint8_t* buffer) {
        return *reinterpret_cast<const uint32_t*>(buffer);
    }
    uint64_t parseEightBytes(const uint8_t* buffer) {
        return *reinterpret_cast<const uint64_t*>(buffer);
    }
};

template <SkipLogging T> class Parser;

template <>
class Parser<SkipLogging::NoSkip> : public BaseParser<Parser<SkipLogging::NoSkip>> {
    public:
    Parser(FeedHandler<Parser<SkipLogging::NoSkip>>& feedhandler)
        : feedhandler_(feedhandler)
    {}
    void addOrder(uint8_t*& msg) {
        uint64_t ticker = parseEightBytes(msg + 24);
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint8_t side = msg[19];
        int32_t num_shares = parseFourBytesSwap(msg + 20);
        uint32_t price = parseFourBytesSwap(msg + 32);
        feedhandler_.addOrder(
            reference, side, num_shares, ticker, price
        );
    }
    void executeOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        feedhandler_.executeOrder(reference, num_shares);
    }
    void executeOrderPrice(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        //uint32_t price = parseFourBytesSwap(buffer_ + 32);
        feedhandler_.executeOrder(reference, num_shares);
    }
    void cancelOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        feedhandler_.cancelOrder(reference, num_shares);
    }
    void deleteOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        feedhandler_.deleteOrder(reference);
    }
    void replaceOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint64_t new_reference = parseEightBytesSwap(msg + 19);
        int32_t num_shares = parseFourBytesSwap(msg + 27);
        uint32_t price = parseFourBytesSwap(msg + 31);
        feedhandler_.replaceOrder(reference, new_reference, num_shares, price);
    }    
    void addTickerToWatchlist(uint64_t tkr) {}
    private:
    FeedHandler<Parser<SkipLogging::NoSkip>>& feedhandler_;
};

template<>
class Parser<SkipLogging::Skip> : public BaseParser<Parser<SkipLogging::Skip>> {
    public:
    Parser(FeedHandler<Parser>& feedhandler)
        : feedhandler_(feedhandler)
    {}
    void addTickerToWatchlist(uint64_t tkr) {
        tickers_.insert(tkr);
    }
    void addOrder(uint8_t*& msg) {
        uint64_t ticker = parseEightBytes(msg + 24);
        uint64_t reference = parseEightBytesSwap(msg + 11);
        if (tickers_.find(ticker) == tickers_.end())
            return;
        references_.insert(reference);
        uint8_t side = msg[19];
        int32_t num_shares = parseFourBytesSwap(msg + 20);
        uint32_t price = parseFourBytesSwap(msg + 32);
        feedhandler_.addOrder(
            reference, side, num_shares, ticker, price
        );
    }
    void executeOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        if (references_.find(reference) == references_.end())
            return;
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        feedhandler_.executeOrder(reference, num_shares);
    }
    void executeOrderPrice(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        if (references_.find(reference) == references_.end())
            return;
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        //uint32_t price = parseFourBytesSwap(buffer_ + 32);
        feedhandler_.executeOrder(reference, num_shares);
    }
    void cancelOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        if (references_.find(reference) == references_.end())
            return;
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        feedhandler_.cancelOrder(reference, num_shares);
    }
    void deleteOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        if (references_.find(reference) == references_.end())
            return;
        feedhandler_.deleteOrder(reference);
    }
    void replaceOrder(uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        if (references_.find(reference) == references_.end())
            return;
        references_.erase(reference);
        uint64_t new_reference = parseEightBytesSwap(msg + 19);
        references_.insert(new_reference);
        int32_t num_shares = parseFourBytesSwap(msg + 27);
        uint32_t price = parseFourBytesSwap(msg + 31);
        feedhandler_.replaceOrder(reference, new_reference, num_shares, price);
    }
    private:
    using tickers = uint64_t;
    using references = uint64_t;
    std::unordered_set<tickers> tickers_;
    std::unordered_set<references> references_;
    FeedHandler<Parser<SkipLogging::Skip>>& feedhandler_;
};

#endif
