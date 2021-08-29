#include <iostream>
#include <string>
#include <vector>

#include "feedhandler.hpp"
#include "parser.hpp"

int main(int argc, char** argv) {
    FeedHandler<Parser> feed_handler("../20200130.PSX_ITCH_50");
    if (argc > 2) {
        std::vector<std::string> tickers;
        tickers.assign(argv + 2, argv + argc);
        feed_handler.setupLoggingBooks(tickers);
    }
    feed_handler.start();
    return 0;
}
