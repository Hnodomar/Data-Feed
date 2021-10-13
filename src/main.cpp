#include <iostream>
#include <string>
#include <chrono>
#include <vector>

#include "feedhandler.hpp"
#include "parser.hpp"
#include "order.hpp"
#include <boost/iostreams/device/mapped_file.hpp>

struct FileHandler {
    FileHandler(const std::string& filename) : itch_data_(filename), 
        itch_data_ptr_(reinterpret_cast<const uint8_t*>(itch_data_.data())), 
        itch_data_end_ptr_(itch_data_ptr_ + itch_data_.size())
    {}
    boost::iostreams::mapped_file_source itch_data_;
    const uint8_t* itch_data_ptr_;
    const uint8_t* itch_data_end_ptr_;
};

int main(int argc, char* argv[]) {
    FileHandler itchfilemap("../20200130.PSX_ITCH_50");
    FeedHandler<Parser<SkipLogging::NoSkip>> feed_handler;
    if (argc > 1) {
        feed_handler.setupLoggingBooks(argv + 1, argc - 1);
    }
    uint8_t temp_buffer[512] = {0};
    int num_msgs = 73086167.0;
    const auto t1 = std::chrono::high_resolution_clock::now();
    while (itchfilemap.itch_data_ptr_ < itchfilemap.itch_data_end_ptr_) {
        uint16_t len = __builtin_bswap16(
            *reinterpret_cast<const uint16_t*>(itchfilemap.itch_data_ptr_)
        );
        memcpy(temp_buffer, itchfilemap.itch_data_ptr_ + 2, len);
        feed_handler.parseMessage(temp_buffer);
        itchfilemap.itch_data_ptr_ += len + 2;
    }
    const auto t2 = std::chrono::high_resolution_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() / 1000;
    const auto perf = num_msgs / time;
    std::cout << "Number of messages: " << std::fixed << num_msgs << std::endl;
    std::cout << "Time elapsed: " << time << "s" << std::endl;
    std::cout << "Performance: " << perf << "/s" << std::endl;
    return 0;
}
