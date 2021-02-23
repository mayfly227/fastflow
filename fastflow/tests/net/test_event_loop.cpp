//
// Created by itsuy on 2021/2/17.
//

#include <iostream>
#include <thread>
#include <functional>
#include "fastflow/net/eventloop.h"

using namespace std;

void threadFunc() {
    event_loop loop;
    cout << "threadFunc:" << this_thread::get_id() << endl;
//    loop.loop();
    event_loop::get_current_thread_loop()->loop();
}

int main() {
    event_loop loop;
    thread t(threadFunc);


//    cout << "main:" << this_thread::get_id() << endl;
//    event_loop loop;
//    loop.loop();
    t.join();
//#ifdef WIN32
//cout << "test" << endl;
//#endif
//    ::WSADATA wsaData;
//    ::WSAStartup( MAKEWORD(2, 2), &wsaData);
//    SOCKET listen_fd = socket(AF_INET,SOCK_STREAM,0);
////    sockaddr_in server{};
////    memset(&server, 0, sizeof(server));
////    server.sin_port = htons(7799);
////    server.sin_family = AF_INET;
////    server.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
////    inet_pton(AF_INET, "0.0.0.0", &server.sin_addr);
////    bind(listen_fd, (sockaddr *) &server, sizeof(server));
////    listen(listen_fd, 10);
////    int e = ::WSAGetLastError();
////    cout << e << endl;
//    pollfd client[1024]{0};
//    client[0].fd = listen_fd;
//    client[0].events = POLLIN;
//    auto max_i = 0;
//    auto ready_i = max_i;
//
//    int s = ::WSAPoll(client,1,100000);
//    cout << s << endl;
//
//    int error = ::WSAGetLastError();
//    cout << error << endl;
//    ::WSACleanup();
    return 0;
}
