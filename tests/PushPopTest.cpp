//
// Created by xenon on 11/10/16.
//

#include <gtest/gtest.h>
#include <algorithm>
#include <Deque.h>
#include <deque>

class PushPopTest : public testing::TestWithParam<std::size_t> {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {

    }

    const int _A = 281, _C = 28411, _M = 134456;
    int _X = 0;
    int nextRandom() {
        _X = (_A * _X + _C) % _M;
        return _X;
    }

    Deque<int> myDeque;
    std::deque<int> stdDeque;
};

template <class T>
void debugPrintDeque(const Deque<T> &deque) {
    std::cout << "Blocks count: " << deque.getBlocksCount() << std::endl;
    auto &buffer = deque.getBlocks();
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        std::cout << buffer[i]->size() << ' ';
    }
    std::cout << std::endl;
}

TEST_P(PushPopTest, FifoOrder) {
    std::size_t n = GetParam();
    std::cout << "Deque size: " <<  n << std::endl;
    for (std::size_t i = 0; i < n; ++i) {
        int x = nextRandom();
        myDeque.push_back(x);
        stdDeque.push_back(x);
    }

    ASSERT_EQ(myDeque.size(), stdDeque.size());

    auto p = std::mismatch(myDeque.begin(), myDeque.end(), stdDeque.begin());
    ASSERT_EQ(p.first, myDeque.end());

    while (!stdDeque.empty()) {
        int s = stdDeque.front();
        int m = myDeque.front();
        ASSERT_EQ(s, m);
        stdDeque.pop_front();
        myDeque.pop_front();
    }
}

INSTANTIATE_TEST_CASE_P(PushPopTest,
                        PushPopTest,
                        testing::Values(10, 100, 1000, 4000, 10000, 100000, 1000000, 10000000));
//                        testing::Values(4000));
