#ifndef ORDER_HPP
#define ORDER_HPP

#include <cstdint>
struct Order {
    Order(uint32_t price, uint32_t shares, uint16_t book_id, uint8_t side)
        : price(price), shares(shares), book_id(book_id), side(side)
    {}
    Order() {}
    uint32_t price = 0;
    int32_t shares = 0;
    uint16_t book_id = 0;
    uint8_t side = 0;
};
#endif
