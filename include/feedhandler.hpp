#ifndef FEED_HANDLER_HPP
#define FEED_HANDLER_HPP

#include <unordered_map>
#include <string>

#include "orderbook.hpp"
#include "order.hpp"

template<typename OutboundParser>
class FeedHandler {
    using ticker = uint64_t;
    using ref = uint64_t;
    public:
        FeedHandler(const std::string& filename): 
            parser_(filename, *this) 
        {}
        void start() {
            parser_.parseFile();
        }
        void addOrder(uint64_t reference, uint8_t side, 
        uint32_t shares, uint64_t ticker, uint32_t price) {
            auto itr = order_books_.find(ticker);
            if (itr == order_books_.end()) {
                itr = order_books_.emplace(ticker, OrderBook());
            }
            itr->second.updateBook(side, shares, price);
            orders_.emplace(reference, Order(price, shares, ticker));
            
        }
        void executeOrder(uint64_t reference, uint32_t num_shares) {

        }
        void executeOrderPrice(uint64_t reference, uint32_t num_shares, 
        uint32_t price) {

        }
        void cancelOrder(uint64_t reference, uint32_t num_shares) {

        }
        void deleteOrder(uint64_t referenece) {

        }
        void replaceOrder(uint64_t reference, uint64_t new_reference,
        uint32_t num_shares, uint32_t price) {

        }
    private:
        OutboundParser parser_;

        std::unordered_map<ticker, OrderBook> order_books_; 
        std::unordered_map<ref, Order> orders_;
};

#endif
