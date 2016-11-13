//
// Created by xenon on 11/13/16.
//

#include <gtest/gtest.h>
#include <Deque.h>
#include <deque>
#include <algorithm>

TEST(IteratorTest, ConstIterator) {
    Deque<int> myDeque;
    std::deque<int> stdDeque;
    for (int i = 0; i < 100; ++i) {
        myDeque.push_back(i);
        stdDeque.push_back(i);
    }
    auto p = std::mismatch(myDeque.cbegin(), myDeque.cend(), stdDeque.cbegin());
    ASSERT_EQ(p.first, myDeque.cend());
}