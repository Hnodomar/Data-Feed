#ifndef FEED_HANDLER_HPP
#define FEED_HANDLER_HPP

#include <unordered_map>
#include <string>
#include <iostream>
#include <utility>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/functional/hash.hpp>

#include "oalphashmap.hpp"
#include "orderbook.hpp"
#include "order.hpp"

template<typename OutboundParser>
class FeedHandler {
    using ticker = uint64_t;
    using ref = uint64_t;
    public:
        FeedHandler(): 
            parser_(*this), tickers_(16384, 0),
            orders_(16000000, std::numeric_limits<uint64_t>::max())
        {}
        void parseMessage(uint8_t* msg) {
            parser_.parseMessage(msg);
        }
        void setupLoggingBooks(char* tickers[], int num_tickers) {
            for (int i = 0; i < num_tickers; ++i) {
                std::size_t len = strlen(tickers[i]);
                if (len > 8) continue;
                char arr[9] = "        ";
                strncpy(arr, *tickers, len);
                uint64_t tkr = *reinterpret_cast<uint64_t*>(arr);
                parser_.addTickerToWatchlist(tkr);
                order_books_.push_back(OrderBook(std::string((char*)&tkr, strlen(tickers[i]))));
                tickers_.emplace(tkr, order_books_.size() - 1);
            }
        }
        void addOrder(uint64_t reference, uint8_t side, 
        int32_t shares, uint64_t ticker, uint32_t price) {
            auto itr = tickers_.find(ticker);
            if (itr == tickers_.end()) {
                order_books_.emplace_back(OrderBook());
                itr = tickers_.emplace(ticker, order_books_.size() - 1).first;
            }
            uint16_t book_id = itr->second;
            auto& bookv = order_books_[book_id];
            bookv.addToBook(side, shares, price);
            orders_.emplace(
                reference, Order(price, shares, book_id, side)
            );
        }
        void executeOrder(uint64_t reference, int32_t num_shares) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end()) return;
            auto& bookv = order_books_[orders_itr->second.book_id];
            bookv.removeFromBook(orders_itr->second.side, num_shares, orders_itr->second.price);
            orders_itr->second.shares -= num_shares;
            if (orders_itr->second.shares <= 0) 
                orders_.erase(orders_itr);
        }
        void cancelOrder(uint64_t reference, int32_t num_shares) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end()) return;
            auto& bookv = order_books_[orders_itr->second.book_id];
            bookv.removeFromBook(orders_itr->second.side, num_shares, orders_itr->second.price);
            orders_itr->second.shares -= num_shares;
            if (orders_itr->second.shares <= 0) orders_.erase(orders_itr);
        }
        void deleteOrder(uint64_t reference) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end()) return;
            auto& bookv = order_books_[orders_itr->second.book_id];
            bookv.removeFromBook(
                orders_itr->second.side,
                orders_itr->second.shares,
                orders_itr->second.price
            );
            orders_.erase(orders_itr);
        }
        void replaceOrder(uint64_t reference, uint64_t new_reference,
        int32_t num_shares, uint32_t price) {
            auto orders_itr = orders_.find(reference);
            if (orders_itr == orders_.end())
                return;
            orders_.emplace(new_reference, 
                Order(price, num_shares, orders_itr->second.book_id, orders_itr->second.side)
            );
            auto& bookv = order_books_[orders_itr->second.book_id];
            bookv.removeFromBook(
                orders_itr->second.side,
                orders_itr->second.shares,
                orders_itr->second.price
            );
            bookv.addToBook(
                orders_itr->second.side,
                num_shares,
                price
            );
            orders_.erase(orders_itr);
        }
        private:
        OutboundParser parser_;
        using bookindex = uint16_t;
        std::vector<OrderBook> order_books_;
        OALPHashMap<ticker, bookindex> tickers_;
        OALPHashMap<ref, Order> orders_;
};

#endif
