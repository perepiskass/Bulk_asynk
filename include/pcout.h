#pragma once
#include <mutex>
#include <iostream>
#include <sstream>


struct pcout: public std::stringstream
{
    static inline std::mutex cout_mutex;
    ~pcout()
    {
        // std::lock_guard<std::mutex> lcout {cout_mutex};
        std::cout << std::stringstream::rdbuf();
        std::cout.flush();
    }
};