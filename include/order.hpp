#ifndef ORDER_HPP
#define ORDER_HPP

#include <cstdint>

struct Order {
    Order(uint32_t price, uint32_t shares, uint64_t ticker)
        : price(price), shares(shares), ticker(ticker) 
    {}
        uint32_t price = 0;
        uint32_t shares = 0;
        uint64_t ticker = 0;
};

#endif
