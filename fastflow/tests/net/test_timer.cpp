//
// Created by itsuy on 2021/2/17.
//

//#include "fastflow/net/channel.h"
#include "fastflow/net/eventloop.h"


#include <thread>
#include <iostream>

#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#elif __linux__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/timerfd.h>
#include "unistd.h"

#endif

using namespace std;

event_loop *g_loop;

void run1(int delay) {
    printf("run delay [%d]\n", delay);
}

int main() {
    event_loop loop;
    loop.call_later(5, [] { run1(5); });
    this_thread::sleep_for(10s);
    loop.call_later(3, [] { run1(3); });
    loop.call_later(8, [] { run1(8); });
    loop.call_later(12, [] { run1(12); });

    loop.call_every(3, [] { printf("every\n"); });
    loop.loop();
}
