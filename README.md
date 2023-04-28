# Sync
This header-only library is a small wrapper around [`std::mutex`](https://en.cppreference.com/w/cpp/thread/mutex) and [`std::shared_timed_mutex`](https://en.cppreference.com/w/cpp/thread/shared_timed_mutex) in order to better express which data they protect.
The design of the library is heavily influenced by the Rust [`std::sync`](https://doc.rust-lang.org/std/sync) in particular [`std::sync::Mutex`](https://doc.rust-lang.org/std/sync/struct.Mutex.html) and [`std::sync::RwLock`](https://doc.rust-lang.org/std/sync/struct.RwLock.html).

## Requirements
* C++14
* CMake 3.14 (only when using CPM for installation)

## Installation
Either,

* copy the content of the `include` folder somewhere and add it to your include directories

* using CPM
```cmake
CPMAddPackage("gh:soehrl/cpp-sync@0.1.0")
target_link_libraries(your-target PRIVATE sync::sync)
```

## `sync::mutex<T>`
In contrast to `std::mutex`, `sync::mutex<T>` takes an additional type parameter `T` that represents the data that is protected by the mutex.
When constructing the mutex, you have to supply an initial value for `T`.
Accessing the value of `T` can be done by calling `mutex::lock()` or `mutex::try_lock()` which return a RAII-style lock guard.
The lock guard then provides the `*` and `->` operator to access the value protected by the mutex.
This makes it very clear which data is protected by which mutex and makes it hard to accidentally access the data without locking the corresponding mutex.
Especially, it prevents the issue where you create a `std::unique_lock` but forget to give it a name, so the lock is immediately released.

<table>
<tr>
<th>

`std::mutex`

</th>
<th>

`sync::mutex<T>`

</th>
</tr>
<tr>
<td>
  
```c++
#include <chrono>
#include <thread>
#include <mutex>
 
int g_num = 0;  // protected by g_num_mutex
std::mutex g_num_mutex;
 
using namespace std::literals::chrono_literals;
void increment(int id) 
{
  for (int i = 0; i < 1000; ++i) {
    std::unique_lock<std::mutex> lock(g_num_mutex);
    ++g_num;
    std::this_thread::sleep_for(10ms);
  }
}
 
int main()
{
    std::thread t1{increment, 0};
    std::thread t2{increment, 1};
    t1.join();
    t2.join();
}
```
  
</td>
<td>

```c++
#include <chrono>
#include <thread>
#include "sync/mutex.hpp"
 
sync::mutex<int> g_num(0);
 
using namespace std::literals::chrono_literals;
void increment(int id) 
{
  for (int i = 0; i < 1000; ++i) {
    *g_num.lock() += 1;
    std::this_thread::sleep_for(10ms);
  }
}
 
int main()
{
    std::thread t1{increment, 0};
    std::thread t2{increment, 1};
    t1.join();
    t2.join();
}
```

</td>
</tr>
</table>

## `sync::read_write_lock`
A small wrapper around an `std::shared_timed_lock`.
It works similarly to `sync::mutex` but it allows to lock the mutex either for reading (`std::shared_lock`) via `read_write_lock::read()` or for writing (`std::unique_lock`) via `read_write_lock::write()`.
The lock guard return for reading gives access to a `const T&` while the lock guard returned for writing gives access to a non-const `T&`.
At any time there can be either multiple readers or a single writer for one `read_write_lock`.
This assumes, that it is safe to access const methods from multiple threads at a time.
According to the citation of the C++ standard from the accepted answer of [this stackoverflow post](https://stackoverflow.com/questions/14127379/does-const-mean-thread-safe-in-c11) this is true for the C++ standard library, however, you should be careful when using it with third-party libraries or your own types.

Usage:
```c++
sync::read_write_lock<int> rwlock(100);

SECTION("Simultaneous reads") {
    auto value = rwlock.read();
    REQUIRE(*value == 100);

    auto value2 = rwlock.try_read();
    REQUIRE(value2.owns_lock());
    REQUIRE(*value2 == 100);
}

SECTION("Try write while read") {
    auto value = rwlock.read();
    REQUIRE(*value == 100);

    auto value2 = rwlock.try_write();
    REQUIRE(!value2.owns_lock());
}

SECTION("Try read while write") {
    auto value = rwlock.write();
    REQUIRE(value.owns_lock());
    REQUIRE(*value == 100);

    auto value2 = rwlock.try_read();
    REQUIRE(!value2.owns_lock());
}
```
