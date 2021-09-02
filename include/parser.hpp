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
        BaseParser(FeedHandler<derived>& fh) : feedhandler_(fh) {}
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
    void addOrder(const uint8_t*& msg) {
        uint64_t ticker = parseEightBytes(msg + 24);
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint64_t timestamp = parseSixBytesSwap(msg + 5);
        uint8_t side = msg[19];
        int32_t num_shares = parseFourBytesSwap(msg + 20);
        uint32_t price = parseFourBytesSwap(msg + 32);
        feedhandler_.addOrder(
            reference, side, num_shares, ticker, price, timestamp
        );
    }
    void executeOrder(const uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint64_t timestamp = parseSixBytesSwap(msg + 5);
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        feedhandler_.executeOrder(reference, num_shares, timestamp);
    }
    void executeOrderPrice(const uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint64_t timestamp = parseSixBytesSwap(msg + 5);
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        //uint32_t price = parseFourBytesSwap(buffer_ + 32);
        feedhandler_.executeOrder(reference, num_shares, timestamp);
    }
    void cancelOrder(const uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint64_t timestamp = parseSixBytesSwap(msg + 5);
        int32_t num_shares = parseFourBytesSwap(msg + 19);
        feedhandler_.cancelOrder(reference, num_shares, timestamp);
    }
    void deleteOrder(const uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint64_t timestamp = parseSixBytesSwap(msg + 5);
        feedhandler_.deleteOrder(reference, timestamp);
    }
    void replaceOrder(const uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        uint64_t new_reference = parseEightBytesSwap(msg + 19);
        uint64_t timestamp = parseSixBytesSwap(msg + 5);
        int32_t num_shares = parseFourBytesSwap(msg + 27);
        uint32_t price = parseFourBytesSwap(msg + 31);
        feedhandler_.replaceOrder(reference, new_reference, num_shares, price, timestamp);
    }    
    FeedHandler<derived>& feedhandler_;
};

template <SkipLogging T> class Parser;

template <>
class Parser<SkipLogging::NoSkip> : private BaseParser<Parser<SkipLogging::NoSkip>> {
    public:
    Parser(FeedHandler<Parser<SkipLogging::NoSkip>>& feedhandler)
        : BaseParser(feedhandler)
    {}
    bool parseMessage(const uint8_t*& msg) {
        const uint8_t type = msg[0];
        switch(type) {
            case 'A':
            case 'F':
                addOrder(msg);
                break;
            case 'E':
                executeOrder(msg);
                break;
            case 'C':
                executeOrderPrice(msg);
                break;
            case 'X':
                cancelOrder(msg);
                break;
            case 'D':
                deleteOrder(msg);
                break;
            case 'U':
                replaceOrder(msg);
                break;
        }
        return true;
    }
    void addTickerToWatchlist(uint64_t tkr) {}
};

template<>
class Parser<SkipLogging::Skip> : private BaseParser<Parser<SkipLogging::Skip>> {
    public:
    Parser(FeedHandler<Parser>& feedhandler)
        : BaseParser(feedhandler)
    {}
    bool parseMessage(const uint8_t*& msg) {
        const uint8_t type = msg[0];
        switch(type) {
            case 'A':
            case 'F':
                checkTicker(msg);
                break;
            case 'E':
                if (referenceExists(parseEightBytesSwap(msg + 11)))
                    executeOrder(msg);
                break;
            case 'C':
                if (referenceExists(parseEightBytesSwap(msg + 11)))
                    executeOrderPrice(msg);
                break;
            case 'X':
                if (referenceExists(parseEightBytesSwap(msg + 11)))
                    cancelOrder(msg);
                break;
            case 'D':
                if (referenceExists(parseEightBytesSwap(msg + 11)))
                    deleteOrder(msg);
                break;
            case 'U':
                if (referenceExists(parseEightBytesSwap(msg + 11)))
                    replaceOrder(msg);
                break;
        }
        return true;
    }
    void checkTicker(const uint8_t*& msg) {    
        uint64_t ticker = parseEightBytes(msg + 24);
        uint64_t reference = parseEightBytesSwap(msg + 11);
        if (tickers_.find(ticker) == tickers_.end())
            return;
        references_.insert(reference);
        addOrder(msg);
    }
    void addTickerToWatchlist(const uint64_t tkr) {
        tickers_.insert(tkr);
    }
    bool referenceExists(const uint64_t ref) {
        return references_.find(ref) != references_.end();
    }
    void replaceOrder(const uint8_t*& msg) {
        uint64_t reference = parseEightBytesSwap(msg + 11);
        references_.erase(reference);
        uint64_t new_reference = parseEightBytesSwap(msg + 19);
        references_.insert(new_reference);
        uint64_t timestamp = parseSixBytesSwap(msg + 5);
        int32_t num_shares = parseFourBytesSwap(msg + 27);
        uint32_t price = parseFourBytesSwap(msg + 31);
        feedhandler_.replaceOrder(reference, new_reference, num_shares, price, timestamp);
    }
    private:
    using tickers = uint64_t;
    using references = uint64_t;
    std::unordered_set<tickers> tickers_;
    std::unordered_set<references> references_;

};

#endif
