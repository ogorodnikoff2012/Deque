//
// Created by xenon on 11/13/16.
//

#include <gtest/gtest.h>
#include <stack>
#include <queue>
#include <Deque.h>

TEST(AdaptorTest, StackTest) {
    std::stack<int, Deque<int>> s;
    for (int i = 0; i < 100; ++i) {
        s.push(i);
    }

    for (int i = 99; i >= 0; --i) {
        ASSERT_FALSE(s.empty());
        ASSERT_EQ(s.top(), i);
        s.pop();
    }
}

TEST(AdaptorTest, QueueTest) {
    std::queue<int, Deque<int>> q;
    for (int i = 0; i < 100; ++i) {
        q.push(i);
    }

    for (int i = 0; i < 100; ++i) {
        ASSERT_FALSE(q.empty());
        ASSERT_EQ(q.front(), i);
        q.pop();
    }
}
