//
// Created by itsuy on 2021/2/16.
//

#include "logging.h"


// 简单日志库实现,使用iostream
using namespace fastflow;

logging::logger *logging::default_logger = logging::logger::get_logger_instance();