#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <map>
#include <cstdint>
#include <iostream>

enum class Output { Logging, NoLogging };

using price = uint64_t;
using size = uint64_t;

template<Output T>
class OrderBook {
    public:
        void updateBookAdd(uint8_t side, uint32_t shares, uint32_t price);
        void updateBookRemove(uint8_t side, uint32_t shares, uint32_t price);
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
        friend std::ostream& operator<<(std::ostream& lhs, const OrderBook& rhs) {
            return lhs;
        }
    private:
        std::map<price, size> asks_, bids_;
};

template<>
inline void OrderBook<Output::Logging>::updateBookAdd(
uint8_t side, uint32_t shares, uint32_t price) {
    addToBook(side, shares, price);
    //log
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
   //log
}

template<>
inline void OrderBook<Output::NoLogging>::updateBookRemove(
uint8_t side, uint32_t shares, uint32_t price) {
    removeFromBook(side, shares, price);
}

#endif
