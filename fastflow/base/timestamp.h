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
        typedef std::chrono::time_point<std::chrono::system_clock> system_time_point;
        typedef std::chrono::time_point<std::chrono::steady_clock> steady_time_point;

        static std::string to_fmt_string(const system_time_point &t) {
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

        static steady_time_point now_steady() {
            return std::chrono::steady_clock::now();
        }

        static system_time_point now_system() {
            return std::chrono::system_clock::now();
        }

        timestamp()
                : p_system(std::chrono::system_clock::now()) {

        }

        explicit timestamp(const system_time_point &t) {
            p_system = t;
        }

        explicit timestamp(const steady_time_point &t) {
            p_steady = t;
        }

        std::string to_string() {
            return to_fmt_string(p_system);
        }

        auto &getTimePointSystem() const {
            return p_system;
        }

        auto &getTimePointSteady() const {
            return p_steady;
        }

        timestamp &operator=(const timestamp other) {
            this->p_system = other.p_system;
            return *this;
        }

        timestamp &operator=(const system_time_point other) {
            this->p_system = other;
            return *this;
        }

    private:
        system_time_point p_system;
        steady_time_point p_steady;

    };

    inline bool operator<(timestamp one, timestamp other) {
        return one.getTimePointSystem() < other.getTimePointSystem();
    }

    inline bool operator<=(timestamp one, timestamp other) {
        return one.getTimePointSystem() <= other.getTimePointSystem();
    }

    inline bool operator>(timestamp one, timestamp other) {
        return one.getTimePointSystem() > other.getTimePointSystem();
    }

    inline bool operator>=(timestamp one, timestamp other) {
        return one.getTimePointSystem() >= other.getTimePointSystem();
    }

    inline bool operator==(timestamp one, timestamp other) {
        return one.getTimePointSystem() == other.getTimePointSystem();
    }
}
#endif //FASTFLOW_TIMESTAMP_H
