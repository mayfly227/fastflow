#include "fastflow/base/blockingqueue.hpp"
#include <thread>
#include <queue>
#include <iostream>
#include <thread>
#include <functional>

using namespace fastflow;
using namespace std;

blockingqueue<int> bq;

void add_value() {
    this_thread::sleep_for(chrono::seconds(3));
    for (int i = 10; i < 20; ++i) {
        bq.push(i);
    }
}

int main() {

    for (int i = 1; i < 2; ++i) {
        bq.push(i);
    }
//     应该会阻塞在这里
    printf("-----\n");

    auto t = thread(add_value);
    while (true) {
        cout << bq.take() << endl;
    }
// 为什么会调用无数次wait?
//    this_thread::sleep_for(chrono::seconds(15));
    return 0;
}

