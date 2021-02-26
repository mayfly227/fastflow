// copied from muduo/net/tests/TimerQueue_unittest.cc

#include "fastflow/net/eventloop.h"
#include "fastflow/base/timestamp.h"
#include "fastflow/base/logging.h"

#include <thread>
#include <functional>

#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace fastflow;
int cnt = 0;
event_loop *g_loop;

void printTid() {
    printf("pid = %d, tid = %ld\n", getpid(), this_thread::get_id());
    printf("now %s\n", timestamp::get_now_fmt_string().c_str());
}

void print(const char *msg) {
    printf("%s %s\n", timestamp::get_now_fmt_string().c_str(), msg);
    if (++cnt == 20) {
        g_loop->quit();
    }
}

int main() {
    printTid();
    logging::set_level(logging::level_enum::error);
    event_loop loop;
    g_loop = &loop;
    print("main");
    loop.call_later(1, [] { return print("once1\n"); });
//    loop.call_later(1.5, [] { return print("once1.5\n"); });
//    loop.call_later(2.5, [] { return print("once2.5\n"); });
//    loop.call_later(3.5, [] { return print("once3.5\n"); });
//    loop.call_every(2, [] { return print("every2\n"); });
    loop.call_every(3, [] { return print("every3\n"); });

    loop.loop();
    print("main loop exits");
    sleep(1);
}
