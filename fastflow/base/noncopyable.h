//
// Created by Li on 2021/2/8.
//

#ifndef FASTFLOW_NONCOPYABLE_H
#define FASTFLOW_NONCOPYABLE_H

namespace FastFlow {
    class noncopyable {
    public:
        noncopyable(const noncopyable &) = delete;

        noncopyable &operator=(const noncopyable &) = delete;

    protected:
        noncopyable() = default;

        ~noncopyable() = default;
    };

}
#endif //FASTFLOW_NONCOPYABLE_H
