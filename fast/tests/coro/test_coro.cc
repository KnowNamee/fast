#include <gtest/gtest.h>

#include <coro/coroutine.h>

#include <vector>

using Coro = fast::coro::Coroutine;

TEST(Coro, Simple) {
    std::vector<uint32_t> result;
    Coro c([&result](){
        result.push_back(2);
        Coro::suspend();
        result.push_back(4);
    });
    result.push_back(1);
    c.resume();
    result.push_back(3);
    c.resume();
    EXPECT_TRUE(c.completed());
    EXPECT_EQ(result.size(), 4ull);
    EXPECT_EQ(result[0], 1ull);
    EXPECT_EQ(result[1], 2ull);
    EXPECT_EQ(result[2], 3ull);
    EXPECT_EQ(result[3], 4ull);
}
