#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <cstdint>

class OrderBook {
    using price = uint64_t;
    using size = uint64_t;
    public:
        void updateBook(uint8_t side, uint32_t shares, uint32_t price) {
            if (side == 'B')
                bids_[price] += shares;
            else
                asks_[price] += shares;
        }
    private:
        std::unordered_map<price, size> asks_, bids_;
};

#endif
