//
// Created by Li on 2021/2/8.
//

#ifndef FASTFLOW_NONCOPYABLE_H
#define FASTFLOW_NONCOPYABLE_H

namespace FastFlow {
    class copyable {
    protected:
        copyable() = default;

        ~copyable() = default;
    };

}
#endif //FASTFLOW_NONCOPYABLE_H
