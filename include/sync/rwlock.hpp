#pragma once

#include "mutex.hpp"
#include <cassert>
#include <shared_mutex>
#include <utility>

namespace cppsync {

// [17.6.5.9/1] This section specifies requirements that implementations shall meet to prevent data races (1.10). Every standard library
// function shall meet each requirement unless otherwise specified. Implementations may prevent data races in cases other than those
// specified below. [17.6.5.9/3] A C++ standard library function shall not directly or indirectly modify objects (1.10) accessible by
// threads other than the current thread unless the objects are accessed directly or indirectly via the function’s non-const arguments,
// including this.
template <typename Mutex, typename T> class shared_lock_guard {
  public:
    template <typename... LockArgs>
    shared_lock_guard(T* value_ptr, Mutex& mutex, LockArgs&&... lock_args)
        : value_ptr_(value_ptr), lock_(mutex, std::forward<LockArgs>(lock_args)...) {}
    shared_lock_guard(const shared_lock_guard&) = delete;
    shared_lock_guard(shared_lock_guard&&) = default;

    shared_lock_guard& operator=(const shared_lock_guard&) = delete;
    shared_lock_guard& operator=(shared_lock_guard&&) = default;

    bool owns_lock() const { return lock_.owns_lock(); }
    operator bool() const { return lock_.owns_lock(); }

    const T* operator->() { return value_ptr_; }
    const T& operator*() { return *value_ptr_; }

  private:
    T* value_ptr_;
    std::shared_lock<Mutex> lock_;
};

template <typename T> class read_write_lock {
    using Mutex = std::shared_timed_mutex;

  public:
    read_write_lock(T value) : value_(value) {}

    unique_lock_guard<Mutex, T> write() { return unique_lock_guard<Mutex, T>(&value_, mutex_); }
    unique_lock_guard<Mutex, T> try_write() { return unique_lock_guard<Mutex, T>(&value_, mutex_, std::try_to_lock); }

    shared_lock_guard<Mutex, T> read() { return shared_lock_guard<Mutex, T>(&value_, mutex_); }
    shared_lock_guard<Mutex, T> try_read() { return shared_lock_guard<Mutex, T>(&value_, mutex_, std::try_to_lock); }

  private:
    T value_;
    Mutex mutex_;
};

} // namespace sync
