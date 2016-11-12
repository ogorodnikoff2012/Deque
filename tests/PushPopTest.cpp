//
// Created by xenon on 11/10/16.
//

#include <gtest/gtest.h>
#include <algorithm>
#include <Deque.h>
#include <deque>
#include <chrono>

class PushPopTest : public testing::TestWithParam<std::size_t> {
protected:
    virtual void SetUp() {
        std::cout << "Deque size: " << GetParam() << std::endl;
    }

    virtual void TearDown() {

    }

    const int _A = 281, _C = 28411, _M = 134456;
    int _X = 0;
    int nextRandom() {
        _X = (_A * _X + _C) % _M;
        return _X;
    }

    void resetRandom() {
        _X = 0;
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

#define MEASURE_TIME_BEGIN(NAME) std::chrono::steady_clock::time_point __##NAME##_begin = std::chrono::steady_clock::now()
#define MEASURE_TIME_END(NAME)  std::chrono::steady_clock::time_point __##NAME##_end = std::chrono::steady_clock::now(); \
                                double NAME = std::chrono::duration_cast<std::chrono::nanoseconds>(__##NAME##_end - __##NAME##_begin).count() / 1e6;


TEST_P(PushPopTest, FifoOrder) {
    std::size_t n = GetParam();
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

TEST_P(PushPopTest, LifoOrder) {
    std::size_t n = GetParam();

    for (std::size_t i = 0; i < n; ++i) {
        int x = nextRandom();
        myDeque.push_back(x);
        stdDeque.push_back(x);
    }

    ASSERT_EQ(myDeque.size(), stdDeque.size());

    while (!stdDeque.empty()) {
        int s = stdDeque.back();
        int m = myDeque.back();
        ASSERT_EQ(s, m);
        stdDeque.pop_back();
        myDeque.pop_back();
    }
}

TEST_P(PushPopTest, RandomOrder) {
    std::size_t n = GetParam();

    for (std::size_t i = 0; i < n; ++i) {
        int x = nextRandom();
        myDeque.push_back(x);
        stdDeque.push_back(x);
        int y = nextRandom();
        myDeque.push_front(y);
        stdDeque.push_front(y);
    }

    ASSERT_EQ(myDeque.size(), stdDeque.size());

    while (!stdDeque.empty()) {
        int s = stdDeque.back();
        int m = myDeque.back();
        ASSERT_EQ(s, m);
        stdDeque.pop_back();
        myDeque.pop_back();
    }
}

TEST_P(PushPopTest, TimeMeasurement) {
    std::size_t n = GetParam();

    resetRandom();
    MEASURE_TIME_BEGIN(stdDequeMs);
    for (std::size_t i = 0; i < n; ++i) {
        int x = nextRandom();
        stdDeque.push_back(x);
        int y = nextRandom();
        stdDeque.push_front(y);
    }

    while (!stdDeque.empty()) {
        stdDeque.pop_back();
    }
    MEASURE_TIME_END(stdDequeMs);

    resetRandom();
    MEASURE_TIME_BEGIN(myDequeMs);
    for (std::size_t i = 0; i < n; ++i) {
        int x = nextRandom();
        myDeque.push_back(x);
        int y = nextRandom();
        myDeque.push_front(y);
    }

    while (!myDeque.empty()) {
        myDeque.pop_back();
    }
    MEASURE_TIME_END(myDequeMs);

    std::cout   << "std::deque time: " << stdDequeMs << " ms." << std::endl
                << "My Deque time: " << myDequeMs << " ms." << std::endl;
}

TEST_P(PushPopTest, SortTimeMeasurement) {
    std::size_t n = GetParam();

    resetRandom();
    MEASURE_TIME_BEGIN(stdDequeMs);
    for (std::size_t i = 0; i < n; ++i) {
        int x = nextRandom();
        stdDeque.push_back(x);
        int y = nextRandom();
        stdDeque.push_front(y);
    }

    std::sort(stdDeque.begin(), stdDeque.end());
    MEASURE_TIME_END(stdDequeMs);

    resetRandom();
    MEASURE_TIME_BEGIN(myDequeMs);
    for (std::size_t i = 0; i < n; ++i) {
        int x = nextRandom();
        myDeque.push_back(x);
        int y = nextRandom();
        myDeque.push_front(y);
    }

    std::sort(myDeque.begin(), myDeque.end());
    MEASURE_TIME_END(myDequeMs);

    auto p = std::mismatch(myDeque.begin(), myDeque.end(), stdDeque.begin());
    ASSERT_EQ(p.first, myDeque.end());

    std::cout   << "std::deque time: " << stdDequeMs << " ms." << std::endl
                << "My Deque time: " << myDequeMs << " ms." << std::endl;

}

INSTANTIATE_TEST_CASE_P(PushPopTest,
                        PushPopTest,
                        testing::Values(10, 100, 1000, 4000, 10000, 100000, 1000000, 10000000));
