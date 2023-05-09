#include "sync/mutex.hpp"
#include <catch2/catch_test_macros.hpp>
#include <map>
#include <random>
#include <thread>

TEST_CASE("Mutex") {
    cppsync::mutex<int> mutex(100);

    {
        auto value = mutex.lock();
        REQUIRE(value.owns_lock());
        REQUIRE(*value == 100);
        *value = 200;

        auto value2 = mutex.try_lock(); // mutex.lock() would dead-lock!
        REQUIRE(!value2.owns_lock());
    }

    {
        auto value = mutex.try_lock();
        REQUIRE(value.owns_lock());
        REQUIRE(*value == 200);
    }
}

TEST_CASE("Heavy Thread Load") {
    cppsync::mutex<std::map<int, int>> mutex({});

    constexpr auto THREAD_COUNT = 100;
    constexpr auto INSERT_COUNT = 10000;

    std::vector<std::thread> threads;
    threads.reserve(THREAD_COUNT);
    for (auto i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back([&mutex]() {
            std::default_random_engine e;
            std::uniform_int_distribution<int> dist;

            for (auto i = 0; i < INSERT_COUNT; ++i) {
                mutex.lock()->insert(std::make_pair(dist(e), dist(e)));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

TEST_CASE("Timed Mutex") {
    using namespace std::chrono_literals;
    using Clock = std::chrono::steady_clock;

    cppsync::timed_mutex<int> mutex(100);

    for (int i = 0; i < 10; ++i) {
        auto value = mutex.try_lock_for(10ms);
        auto value2 = mutex.try_lock_until(Clock::now() + 10ms);

        // Exactly on value should own the lock
        REQUIRE(value.owns_lock() ^ value2.owns_lock());
    }
}
