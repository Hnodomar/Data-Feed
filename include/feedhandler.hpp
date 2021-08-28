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
                order_books_.emplace(ticker, OrderBook());
            }
            auto itrtwo = order_books_.find(ticker);
            itrtwo->second.updateBookAdd(side, shares, price);
            orders_.emplace(reference, Order(price, shares, ticker, side));
        }
        void executeOrder(uint64_t reference, uint32_t num_shares) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end()) return;
            auto books_itr = order_books_.find(orders_itr->second.ticker);
            if (books_itr == order_books_.end()) return;
            books_itr->second.updateBookRemove(orders_itr->second.side, orders_itr->second.price, num_shares);
            orders_itr->second.shares -= num_shares;
            if (orders_itr->second.shares <= 0) 
                orders_.erase(orders_itr);
        }
        void cancelOrder(uint64_t reference, uint32_t num_shares) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end()) return;
            auto books_itr = order_books_.find(orders_itr->second.ticker);
            orders_itr->second.shares -= num_shares;
            if (orders_itr->second.shares <= 0) orders_.erase(orders_itr);
            if (books_itr == order_books_.end()) return;
            books_itr->second.updateBookRemove(orders_itr->second.side, orders_itr->second.price, num_shares);
        }
        void deleteOrder(uint64_t reference) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end()) return;
            auto books_itr = order_books_.find(orders_itr->second.ticker);
            orders_.erase(orders_itr);
            if (books_itr == order_books_.end()) return;
            books_itr->second.updateBookRemove(
                orders_itr->second.side, 
                orders_itr->second.price, 
                orders_itr->second.shares
            );
        }
        void replaceOrder(uint64_t reference, uint64_t new_reference,
        uint32_t num_shares, uint32_t price) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end())
                return;
            orders_.emplace(new_reference, 
                Order(price, num_shares, orders_itr->second.ticker, orders_itr->second.side)
            );
            auto book_itr = order_books_.find(orders_itr->second.ticker);
            if (book_itr == order_books_.end()) {
                orders_.erase(orders_itr);
                return;
            }
            book_itr->second.updateBookRemove(
                orders_itr->second.side,
                orders_itr->second.price,
                orders_itr->second.shares
            );
            book_itr->second.updateBookAdd(
                orders_itr->second.side,
                num_shares,
                price
            );
            orders_.erase(orders_itr);
        }
    private:
        OutboundParser parser_;

        std::unordered_map<ticker, OrderBook> order_books_; 
        std::unordered_map<ref, Order> orders_;
};

#endif
