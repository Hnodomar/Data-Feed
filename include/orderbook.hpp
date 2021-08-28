#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <cstdint>

class OrderBook {
    using price = uint64_t;
    using size = uint64_t;
    public:
        void updateBookAdd(uint8_t side, uint32_t shares, uint32_t price) {
            if (side == 'B') bids_[price] += shares;
            else asks_[price] += shares;
        }
        void updateBookRemove(uint8_t side, uint32_t price, uint32_t shares) {
            auto& book_side = (side == 'B' ? bids_ : asks_);
            auto itr = book_side.find(price);
            if (itr == book_side.end())
                return;
            itr->second += price;
        }
    private:
        std::unordered_map<price, size> asks_, bids_;
};

#endif
