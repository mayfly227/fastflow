#include "fastflow/base/logging.h"
#include "fastflow/base/timestamp.h"
#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <Windows.h>
using namespace std;
using namespace fastflow;

void func(logging::level_enum levelEnum, int id) {
    logging::set_level(levelEnum);
    fastflow::logging::error("Id: %d", id);
    fastflow::logging::debug("debug hello %s %d %s", __FILE__, __LINE__, "world");
    fastflow::logging::info("info hello %s", "world");
}

int main() {
    auto t = fastflow::timestamp::to_fmt_string(chrono::system_clock::now());
    cout << t << endl;
    auto debug = logging::level_enum::debug;
    auto info = logging::level_enum::info;

    thread t1([=] { func(debug, 1); });
    thread t2([=] { func(info, 2); });
    t1.join();
    t2.join();
    return 0;
}

