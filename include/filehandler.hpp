#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <boost/iostreams/device/mapped_file.hpp>

using biost = boost::iostreams;

class FileHandler {
    public:
        FileHandler();
        ~FileHandler();
    private:
        biost::mapped_file_source itch_data_;
};

#endif
