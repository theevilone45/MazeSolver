#ifndef INCLUDE_MAZE_DEBUG_HPP
#define INCLUDE_MAZE_DEBUG_HPP

#include <ctime>
#include <chrono>
#include <print>
#include <format>

#define LOG(msg)                                                               \
    do {                                                                       \
        auto now = std::chrono::system_clock::now();                           \
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);      \
        std::tm local_time = *std::localtime(&now_time);                       \
        std::print("{} [{}:{} {}] {}\n",                                       \
                   std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}",          \
                               local_time.tm_year + 1900,                      \
                               local_time.tm_mon + 1, local_time.tm_mday,      \
                               local_time.tm_hour, local_time.tm_min,          \
                               local_time.tm_sec),                             \
                   __FILE__, __LINE__, __func__, msg);                         \
    } while (0)

#endif
