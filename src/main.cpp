#include <iostream>
#include <string>

#include "feedhandler.hpp"
#include "parser.hpp"

int main(int argc, char** argv) {
    FeedHandler<Parser> feed_handler("../20200130.PSX_ITCH_50");
    feed_handler.start();
    return 0;
}
