#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <map>
#include <cstdint>
#include <iostream>
#include <string>
#include <memory>
#include <fstream>

using price = uint64_t;
using size = int64_t;

class OrderBook {
    public:
    OrderBook(std::string ticker) : 
        outputstream_(std::make_unique<std::fstream>(std::fstream(ticker, std::ios_base::out))),
        log(true)
    {}
    OrderBook() {}
    void addToBook(uint8_t side, int32_t shares, uint32_t price, uint64_t timestamp) {
        if (side == 'B') bids_[price] += shares;
        else asks_[price] += shares;
        if (log) output(side, shares, price, timestamp);
    }
    void removeFromBook(uint8_t side, int32_t shares, uint32_t price, uint64_t timestamp) {
        auto& book_side = (side == 'B' ? bids_ : asks_);
        auto itr = book_side.find(price);
        if (itr == book_side.end())
            return;
        itr->second -= shares;
        if (itr->second <= 0)
            book_side.erase(itr);
        if (log) output(side, -shares, price, timestamp);
    }
    private:
    void output(uint8_t side, int32_t shares, uint32_t price, uint64_t timestamp) {
        *(outputstream_.get()) << timestamp << ',' << side << ',' << shares << ',' << price << '\n';
    }
    std::map<price, size> asks_, bids_;
    std::unique_ptr<std::fstream> outputstream_ = nullptr;
    bool log = false;
};

#endif
