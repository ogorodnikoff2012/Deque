//
// Created by xenon on 11/12/16.
//

#include <gtest/gtest.h>
#include <Deque.h>
#include <algorithm>

TEST(DummyTest, Empty) {
    Deque<int> d;
    ASSERT_TRUE(d.empty());
    d.push_back(1);
    ASSERT_FALSE(d.empty());
    d.pop_back();
    ASSERT_TRUE(d.empty());
}

TEST(DummyTest, Size) {
    Deque<int> d;
    for (int i = 0; i < 100; ++i) {
        d.push_back(i);
        ASSERT_EQ(d.size(), i + 1);
    }
}

TEST(DummyTest, Copy) {
    Deque<int> d;
    for (int i = 0; i < 100; ++i) {
        d.push_back(i);
    }

    Deque<int> d2(d);
    auto p = std::mismatch(d.begin(), d.end(), d2.begin());
    ASSERT_EQ(p.first, d.end());

    d[3] = 5;
    ASSERT_NE(d[3], d2[3]);
}