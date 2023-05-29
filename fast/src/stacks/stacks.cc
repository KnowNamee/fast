#include <sync/spinlock.h>
#include <mutex>
#include <stacks/stacks.h>

namespace fast::stacks {

Stack* Stacks::get() {
    std::lock_guard<Lock> guard(_lock);
    if (!_stacks.empty()) {
        Stack* s = _stacks.back();
        _stacks.pop_back();
        return s;
    }
    return new Stack();
}

void Stacks::ret(Stack* s) {
    std::lock_guard<Lock> guard(_lock);
    if (_stacks.size() > 1000) {
        delete s;
    } else {
        _stacks.push_front(s);
    }
}

}
