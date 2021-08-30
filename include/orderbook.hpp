#ifndef BOOK_HPP
#define BOOK_HPP

#include <map>
#include <cstdint>
#include <iostream>
#include <memory>
#include <fstream>
#include <boost/optional/optional.hpp>

enum class Output { Logging, NoLogging };

using price = uint64_t;
using size = int64_t;

struct BaseOrderBook {
    void addToBook(uint8_t& side, int32_t& shares, uint32_t& price) {
        if (side == 'B') bids_[price] += shares;
        else asks_[price] += shares;
    }
    void removeFromBook(uint8_t& side, int32_t& shares, uint32_t& price) {
        auto& book_side = (side == 'B' ? bids_ : asks_);
        auto itr = book_side.find(price);
        if (itr == book_side.end())
            return;
        itr->second -= shares;
        if (itr->second <= 0)
            book_side.erase(itr);
    }
    std::map<price, size> asks_, bids_;
};

template<Output T>
class OrderBook : private BaseOrderBook {
    public:
    void updateBookAdd(uint8_t side, int32_t shares, uint32_t price) {
        addToBook(side, shares, price);
    }
    void updateBookRemove(uint8_t side, int32_t shares, uint32_t price) {
        removeFromBook(side, shares, price);
    }
    friend std::ostream& operator<<(std::ostream& lhs, const BaseOrderBook*& rhs);
};

template<>
class OrderBook<Output::Logging> : private BaseOrderBook {
    public:
    OrderBook(uint64_t ticker, std::size_t len) :
        outputstream_(std::fstream(std::string((char*)&ticker, len), std::ios_base::out)) 
    {}
    friend std::ostream& operator<<(std::ostream& lhs, const BaseOrderBook*& rhs);
    void updateBookAdd(uint8_t side, int32_t shares, uint32_t price) {
        addToBook(side, shares, price);
        output(side, shares, price);
    }
    void updateBookRemove(uint8_t side, int32_t shares, uint32_t price) {
        removeFromBook(side, shares, price);
        output(side, -shares, price);
    }
    private:
    void output(uint8_t side, int32_t shares, uint32_t price) {
        outputstream_ << side << ',' << shares << ',' << price << '\n';
    }
    std::fstream outputstream_;
};

inline std::ostream& operator<<(std::ostream& lhs, const BaseOrderBook*& rhs) {
    lhs << " [BIDS]: " << std::endl;
    for (auto itr = rhs->bids_.begin(); itr != rhs->bids_.end(); ++itr)
        lhs << itr->second << std::endl;
    lhs << " [ASKS]: " << std::endl;
    for (auto itr = rhs->asks_.begin(); itr != rhs->asks_.end(); ++itr)
        lhs << itr->second << std::endl;
    return lhs;
}

#endif
