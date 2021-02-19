//
// Created by itsuy on 2021/2/16.
//

#ifndef FASTFLOW_TIMESTAMP_H
#define FASTFLOW_TIMESTAMP_H

#include <chrono>
#include <ctime>
#include <string>

namespace fastflow {
    class timestamp {
    public:
        static std::string to_fmt_string(const std::chrono::time_point<std::chrono::system_clock> &t) {
            auto time_t = std::chrono::system_clock::to_time_t(t);
            auto tm = std::localtime(&time_t);
            char buffer[128];
            // 2020-1-1 20:00:00
            snprintf(buffer, sizeof(buffer), "[%d-%02d-%02d %02d:%02d:%02d]", tm->tm_year + 1900, tm->tm_mon,
                     tm->tm_mday, tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
            return std::string{buffer};
        };

        static std::string get_now_fmt_string() {
            return to_fmt_string(std::chrono::system_clock::now());
        }

        static std::time_t get_now_time_t() {
            return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        }
    };
}
#endif //FASTFLOW_TIMESTAMP_H
