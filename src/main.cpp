#include <iostream>
#include <string>
#include <vector>

#include "feedhandler.hpp"
#include "parser.hpp"
#include "order.hpp"

int main(int argc, char* argv[]) {
    FeedHandler<Parser> feed_handler("../20200130.PSX_ITCH_50");
    if (argc > 1) {
        feed_handler.setupLoggingBooks(argv + 1, argc - 1);
    }
    feed_handler.start();
    return 0;
}
