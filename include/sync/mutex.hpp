#pragma once

#include <mutex>
#include <utility>

namespace cppsync {

template <typename Mutex, typename T> class unique_lock_guard {
  public:
    template <typename... LockArgs>
    unique_lock_guard(T* value_ptr, Mutex& mutex, LockArgs&&... lock_args)
        : value_ptr_(value_ptr), lock_(mutex, std::forward<LockArgs>(lock_args)...) {}
    unique_lock_guard(const unique_lock_guard&) = delete;
    unique_lock_guard(unique_lock_guard&&) = default;

    unique_lock_guard& operator=(const unique_lock_guard&) = delete;
    unique_lock_guard& operator=(unique_lock_guard&&) = default;

    bool owns_lock() const { return lock_.owns_lock(); }
    operator bool() const { return lock_.owns_lock(); }

    T* operator->() { return value_ptr_; }
    T& operator*() { return *value_ptr_; }

  private:
    T* value_ptr_;
    std::unique_lock<Mutex> lock_;
};

template <typename T> class mutex {
  public:
    mutex(T value) : value_(value) {}

    unique_lock_guard<std::mutex, T> lock() { return unique_lock_guard<std::mutex, T>(&value_, mutex_); }
    unique_lock_guard<std::mutex, T> try_lock() { return unique_lock_guard<std::mutex, T>(&value_, mutex_, std::try_to_lock); }

  private:
    T value_;
    std::mutex mutex_;
};

template <typename T> class timed_mutex {
  public:
    timed_mutex(T value) : value_(value) {}

    unique_lock_guard<std::timed_mutex, T> lock() { return unique_lock_guard<std::timed_mutex, T>(&value_, mutex_); }
    unique_lock_guard<std::timed_mutex, T> try_lock() { return unique_lock_guard<std::timed_mutex, T>(&value_, mutex_, std::try_to_lock); }

    template <typename Rep, typename Period>
    unique_lock_guard<std::timed_mutex, T> try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration) {
        return unique_lock_guard<std::timed_mutex, T>(&value_, mutex_, timeout_duration);
    }

    template <typename Clock, typename Duration>
    unique_lock_guard<std::timed_mutex, T> try_lock_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
        return unique_lock_guard<std::timed_mutex, T>(&value_, mutex_, timeout_time);
    }

  private:
    T value_;
    std::timed_mutex mutex_;
};

} // namespace sync
