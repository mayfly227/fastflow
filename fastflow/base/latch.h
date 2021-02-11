//
// Created by itsuy on 2021/2/11.
//

#ifndef FASTFLOW_LATCH_H
#define FASTFLOW_LATCH_H

#include <mutex>
#include <condition_variable>

namespace FastFlow {
    class latch {
    private:
        unsigned int count_;
        mutable std::mutex mutex_;
        std::condition_variable cv_;
    public:
        explicit latch(int i);

        void count_down();

        void wait();

        unsigned int get_count() const;
    };
}

#endif //FASTFLOW_LATCH_H
