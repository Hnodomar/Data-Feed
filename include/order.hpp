#ifndef ORDER_HPP
#define ORDER_HPP

#include <cstdint>
struct Order {
    Order(uint32_t price, uint32_t shares, uint64_t ticker, uint8_t side)
        : price(price), shares(shares), ticker(ticker), side(side)
    {}
    uint32_t price = 0;
    int32_t shares = 0;
    uint64_t ticker = 0;
    uint8_t side = 0;
};
#endif
