//
// Created by xenon on 11/9/16.
//

#ifndef DEQUE_RINGBUFFER_H
#define DEQUE_RINGBUFFER_H

#include <cstdint>
#include <algorithm>
#include <memory>

template <class T, class Allocator = std::allocator<T>>
class RingBuffer {
private:
    Allocator allocator_;
    T *arr_, *begin_, *end_;
    std::size_t size_, allocSize_;

    void reset() {
        if (arr_ != nullptr) {
            allocator_.deallocate(arr_, allocSize_);
            arr_ = nullptr;
        }
    }

    template <class Alloc2>
    void copy(const RingBuffer<T, Alloc2> &other) {
        reset();
        size_ = other.size_;
        allocSize_ = other.allocSize_;
        arr_ = allocator_.allocate(allocSize_);
        begin_ = end_ = arr_;
        for (std::size_t i = 0; i < size_; ++i) {
            *end_++ = other[i];
        }
    }
public:
    RingBuffer(std::size_t size, const Allocator allocator = Allocator()) :
            allocator_(allocator),
            arr_(allocator_.allocate(size)),
            begin_(arr_),
            end_(arr_),
            size_(0),
            allocSize_(size) {}
    ~RingBuffer() {
        reset();
    }

    template <class Alloc2>
    RingBuffer(const RingBuffer<T, Alloc2> &other) : allocator_(Allocator()), arr_(nullptr) {
        copy(other);
    }

    template <class Alloc2>
    RingBuffer<T, Allocator> &operator =(const RingBuffer<T, Alloc2> &other) {
        if (arr_ == other.arr_) {
            return *this;
        }
        copy(other);
        return *this;
    }

    void swap(RingBuffer<T, Allocator> &other) {
        std::swap(arr_, other.arr_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(size_, other.size_);
        std::swap(allocSize_, other.allocSize_);
        std::swap(allocator_, other.allocator_);
    }

    void resetAndResize(std::size_t n) {
        reset();
        end_ = begin_ = arr_ = allocator_.allocate(n);
        size_ = 0;
        allocSize_ = n;
    }

    bool full() const {
        return size_ == allocSize_;
    }

    bool empty() const {
        return size_ == 0;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t maxSize() const {
        return allocSize_;
    }

    void push_back(const T &val) {
        if (full()) {
            return;
        }
        *end_++ = val;
        ++size_;
        if (end_ - arr_ == allocSize_) {
            end_ = arr_;
        }
    }

    void push_front(const T &val) {
        if (full()) {
            return;
        }
        if (begin_ == arr_) {
            begin_ += allocSize_;
        }
        *--begin_ = val;
        ++size_;
    }

    void pop_back() {
        if (empty()) {
            return;
        }
        if (end_ == arr_) {
            end_ += allocSize_;
        }
        --end_;
        --size_;
    }

    void pop_front() {
        if (empty()) {
            return;
        }
        ++begin_;
        --size_;
        if (begin_ - arr_ == allocSize_) {
            begin_ = arr_;
        }
    }

    T &operator [](std::size_t n) {
        T *ptr = begin_ + n;
        if (ptr - arr_ >= allocSize_) {
            ptr -= allocSize_;
        }
        return *ptr;
    }

    const T &operator [](std::size_t n) const {
        T *ptr = begin_ + n;
        if (ptr - arr_ >= allocSize_) {
            ptr -= allocSize_;
        }
        return *ptr;
    }

    T &front() {
        return *begin_;
    }

    const T &front() const {
        return *begin_;
    }

    T &back() {
        return *(end_ == arr_ ? arr_ + allocSize_ - 1 : end_ - 1);
    }

    const T &back() const {
        return *(end_ == arr_ ? arr_ + allocSize_ - 1 : end_ - 1);
    }
};


#endif //DEQUE_RINGBUFFER_H
