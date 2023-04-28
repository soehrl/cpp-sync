#include "sync/rwlock.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Read Write Lock") {
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
}
