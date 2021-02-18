//
// Created by itsuy on 2021/2/16.
//

#ifndef FASTFLOW_LOGGING_H
#define FASTFLOW_LOGGING_H


#include <iostream>
#include <string>
#include <mutex>
#include <vector>

#include "fastflow/base/timestamp.h"

namespace fastflow {
    namespace logging {
        enum class level_enum {
            trace = 0,
            debug,
            info,
            warn,
            error,
        };

        class logger {
        private:
            level_enum current_level_;
            mutable std::mutex mutex_;
        public:
            logger() : mutex_(),
                       current_level_(level_enum::trace) {

            };

            std::string lvl_to_string(level_enum level) {
                std::string l_string;
                switch (level) {
                    case level_enum::trace:
                        l_string = "[trace]";
                        break;
                    case level_enum::debug:
                        l_string = "[debug]";
                        break;
                    case level_enum::info:
                        l_string = "[info]";
                        break;
                    case level_enum::warn:
                        l_string = "[warn]";
                        break;
                    case level_enum::error:
                        l_string = "[error]";
                        break;
                }
                return l_string;
            }

            level_enum get_level() const {
                std::lock_guard<std::mutex> lock(mutex_);
                return current_level_;
            };

            void set_level(level_enum lvl) {
                std::lock_guard<std::mutex> lock(mutex_);
                current_level_ = lvl;
            };

            template<typename FormatString, typename ...Args>
            void log(level_enum lvl, const FormatString &fmt, Args &&...args) {
                if (lvl >= current_level_) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    char buffer_time[64]{0};
                    char buffer[256]{0};
                    auto sub_str_time = fastflow::timestamp::get_now_fmt_string();

                    snprintf(buffer_time, sizeof(buffer_time), "%s ", sub_str_time.c_str());
                    snprintf(buffer, sizeof(buffer), fmt, args...);
                    std::cout << buffer_time << lvl_to_string(lvl) << " " << buffer << std::endl;;

//                    std::vector<char> v;
//                    v.reserve(256);
//                    std::string filename{__FILE__};
//                    auto sub1 = filename.substr(filename.rfind('/')+1,filename.length());
//                    auto sub_str_time = fastflow::timestamp::get_now_fmt_string();
//                    std::string prefix;
//                    prefix+=sub_str_time;
//                    prefix+=" ";
//                    prefix+=sub1;
//                    prefix+=":";
//                    prefix+= std::to_string(__LINE__);
//                    prefix+= " >>> ";
//
//                    v.insert(v.begin(),prefix.data(),prefix.data()+prefix.length());
//                    sprintf(v.data()+prefix.length(), fmt, args...);
//                    std::cout << v.data() << std::endl;
                }
            };


            template<typename FormatString, typename... Args>
            void trace(const FormatString &fmt, Args &&...args) {
                log(level_enum::trace, fmt, std::forward<Args>(args)...);
            }

            template<typename FormatString, typename... Args>
            void debug(const FormatString &fmt, Args &&...args) {
                log(level_enum::debug, fmt, std::forward<Args>(args)...);
            }

            template<typename FormatString, typename... Args>
            void info(const FormatString &fmt, Args &&...args) {
                log(level_enum::info, fmt, std::forward<Args>(args)...);
            }

            template<typename FormatString, typename... Args>
            void warn(const FormatString &fmt, Args &&...args) {
                log(level_enum::warn, fmt, std::forward<Args>(args)...);
            }

            template<typename FormatString, typename... Args>
            void error(const FormatString &fmt, Args &&...args) {
                log(level_enum::error, fmt, std::forward<Args>(args)...);
            }

            //c++ >11 线程安全
            static logger *get_logger_instance() {
                static logger default_logger;
                return &default_logger;
            }

        };

        extern logger *default_logger;

        inline void set_level(level_enum lvl) {
            default_logger->set_level(lvl);
        }

        inline level_enum get_level() {
            return default_logger->get_level();
        }

//        auto default_logger = std::make_unique<fastflow::logging::logger>();
        template<typename FormatString, typename ...Args>
        inline void trace(const FormatString fmt, Args &&...args) {
            default_logger->trace(fmt, std::forward<Args>(args)...);
        };

        template<typename FormatString, typename ...Args>
        inline void debug(const FormatString fmt, Args &&...args) {
            default_logger->debug(fmt, std::forward<Args>(args)...);
        };

        template<typename FormatString, typename ...Args>
        inline void info(const FormatString fmt, Args &&...args) {
            default_logger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename FormatString, typename ...Args>
        inline void warn(const FormatString fmt, Args &&...args) {
            default_logger->warn(fmt, std::forward<Args>(args)...);
        };

        template<typename FormatString, typename ...Args>
        inline void error(const FormatString fmt, Args &&...args) {
            default_logger->error(fmt, std::forward<Args>(args)...);
        };


    }
}


#endif //FASTFLOW_LOGGING_H
