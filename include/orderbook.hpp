#ifndef BOOK_HPP
#define BOOK_HPP

#include <map>
#include <cstdint>
#include <iostream>
#include <memory>
#include <fstream>

enum class Output { Logging, NoLogging };

using price = uint64_t;
using size = uint64_t;

template<Output T>
class OrderBook {
    public:
        OrderBook() = default;
        OrderBook(uint64_t ticker, std::size_t len);
        void updateBookAdd(uint8_t side, uint32_t shares, uint32_t price);
        void updateBookRemove(uint8_t side, uint32_t shares, uint32_t price);
        friend std::ostream& operator<<(std::ostream& lhs, const OrderBook<T>& rhs) {
            lhs << " [BIDS]: " << std::endl;
            for (auto itr = rhs.bids_.begin(); itr != rhs.bids_.end(); ++itr)
                lhs << itr->second << std::endl;
            lhs << " [ASKS]: " << std::endl;
            for (auto itr = rhs.asks_.begin(); itr != rhs.asks_.end(); ++itr)
                lhs << itr->second << std::endl;
            return lhs;
        }
    private:
        void addToBook(uint8_t& side, uint32_t& shares, uint32_t& price) {
            if (side == 'B') bids_[price] += shares;
            else asks_[price] += shares;
        }
        void removeFromBook(uint8_t& side, uint32_t& shares, uint32_t& price) {
            auto& book_side = (side == 'B' ? bids_ : asks_);
            auto itr = book_side.find(price);
            if (itr == book_side.end())
                return;
            itr->second -= shares;
        }
        void output(uint8_t side, int32_t shares, uint32_t price) {
            outputstream_ << side << ' ' << shares << ' ' << price << '\n';
        }
        std::map<price, size> asks_, bids_;
        std::fstream outputstream_;
};

template<>
inline OrderBook<Output::Logging>::OrderBook(uint64_t ticker, std::size_t len) :
    outputstream_(std::fstream(std::string((char*)&ticker, len), std::ios_base::out)) 
{}

template<>
inline void OrderBook<Output::Logging>::updateBookAdd(
uint8_t side, uint32_t shares, uint32_t price) {
    addToBook(side, shares, price);
    output(side, shares, price);
}

template<>
inline void OrderBook<Output::NoLogging>::updateBookAdd(
uint8_t side, uint32_t shares, uint32_t price) {
    addToBook(side, shares, price);
}

template<>
inline void OrderBook<Output::Logging>::updateBookRemove(
uint8_t side, uint32_t shares, uint32_t price) {
    removeFromBook(side, shares, price);
    output(side, -shares, price);
}

template<>
inline void OrderBook<Output::NoLogging>::updateBookRemove(
uint8_t side, uint32_t shares, uint32_t price) {
    removeFromBook(side, shares, price);
}

#endif
