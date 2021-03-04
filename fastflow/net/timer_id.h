//
// Created by itsuy on 2021/2/20.
//

#ifndef FASTFLOW_TIMER_ID_H
#define FASTFLOW_TIMER_ID_H

#include <memory>
#include <utility>

#include "fastflow/base/copyable.h"

class timer;

typedef std::shared_ptr<timer> timer_ptr;

class timer_id : fastflow::noncopyable {
public:

    timer_id(timer_ptr t, int id)
            : t_(std::move(t)), id_(id) {

    };

    timer_id(timer_ptr t)
            : t_(std::move(t)) {

    };

    timer_id()
            : t_(nullptr), id_(0) {
    }


private:
    timer_ptr t_;
    int id_;
//    typedef std::q
};


#endif //FASTFLOW_TIMER_ID_H
