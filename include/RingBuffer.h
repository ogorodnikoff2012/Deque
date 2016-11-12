//
// Created by xenon on 11/9/16.
//

#ifndef DEQUE_RINGBUFFER_H
#define DEQUE_RINGBUFFER_H

#include <cstdint>
#include <algorithm>
#include <memory>
#include <iterator>

template <class T, class Allocator = std::allocator<T>>
class RingBuffer {
public:
    typedef T value_type;
    typedef Allocator allocator_type;
    typedef typename allocator_type::reference reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef std::size_t size_type;
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
    template <class IterType, class RefType, class PtrType, class RingBufferType>
    class RingBufferIterator : public std::iterator<std::random_access_iterator_tag, IterType> {
    public:
        typedef std::ptrdiff_t difference_type;
    private:
        PtrType begin_, end_, cur_;
        RingBufferType *ringBuf_;
        difference_type n_;
        RingBufferIterator(PtrType begin, PtrType end, PtrType cur, RingBufferType *ringBuf, difference_type n) :
                begin_(begin),
                end_(end),
                cur_(cur),
                ringBuf_(ringBuf),
                n_(n)
        {}
    public:
        RingBufferIterator() : begin_(nullptr), end_(nullptr), cur_(nullptr), ringBuf_(nullptr), n_(0) {}
        RingBufferIterator(const RingBufferIterator &other) :
                begin_(other.begin_),
                end_(other.end_),
                cur_(other.cur_),
                ringBuf_(other.ringBuf_),
                n_(other.n_)
        {}

        RingBufferIterator &operator =(const RingBufferIterator &other) {
            begin_ = other.begin_;
            end_ = other.end_;
            cur_ = other.cur_;
            ringBuf_ = other.ringBuf_;
            n_ = other.n_;
            return *this;
        }

        bool operator ==(const RingBufferIterator &other) {
            return ringBuf_ == other.ringBuf_ && n_ == other.n_;
        }

        bool operator !=(const RingBufferIterator &other) {
            return !operator==(other);

        }

        RefType operator *() const {
            return *cur_;
        }

        PtrType operator ->() const {
            return cur_;
        }

        RingBufferIterator &operator ++() {
            if (cur_ != ringBuf_->end_) {
                ++n_;
                ++cur_;
                if (cur_ == end_) {
                    cur_ = begin_;
                }
            }
            return *this;
        }

        const RingBufferIterator operator ++(int) {
            RingBufferIterator ans = *this;
            ++(*this);
            return ans;
        }

        RingBufferIterator &operator --() {
            if (cur_ != ringBuf_->begin_) {
                --n_;
                if (cur_ == begin_) {
                    cur_ = end_;
                }
                --cur_;
            }
            return *this;
        }

        const RingBufferIterator operator --(int) {
            RingBufferIterator ans = *this;
            --(*this);
            return ans;
        }

        RingBufferIterator &operator +=(difference_type diff) {
            n_ += diff;
            cur_ += diff;
            if (cur_ >= end_) {
                cur_ -= (end_ - begin_);
            }
            return *this;
        }

        RingBufferIterator &operator -=(difference_type diff) {
            n_ -= diff;
            cur_ -= diff;
            if (cur_ < begin_) {
                cur_ += (end_ - begin_);
            }
            return *this;
        }

        const RingBufferIterator operator +(difference_type diff) const {
            RingBufferType ans = *this;
            ans += diff;
            return ans;
        }

        const RingBufferIterator operator -(difference_type diff) const {
            RingBufferType ans = *this;
            ans -= diff;
            return ans;
        }

        difference_type operator -(const RingBufferIterator &other) const {
            if (ringBuf_ != other.ringBuf_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ - other.n_;
        }

        bool operator <(const RingBufferIterator &other) const {
            if (ringBuf_ != other.ringBuf_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ < other.n_;
        }

        bool operator >(const RingBufferIterator &other) const {
            if (ringBuf_ != other.ringBuf_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ > other.n_;
        }

        bool operator <=(const RingBufferIterator &other) const {
            if (ringBuf_ != other.ringBuf_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ <= other.n_;
        }

        bool operator >=(const RingBufferIterator &other) const {
            if (ringBuf_ != other.ringBuf_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ >= other.n_;
        }

        RefType operator [](difference_type diff) const {
            return (*ringBuf_)[n_ + diff];
        }

        friend RingBufferType;
    };
public:
    typedef RingBufferIterator<value_type, reference, pointer, RingBuffer<T, Allocator>> iterator;
    typedef RingBufferIterator<const value_type, const_reference, const_pointer, const RingBuffer<T, Allocator>> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename std::iterator_traits<iterator>::difference_type difference_type;

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

    iterator begin() {
        return iterator(arr_, arr_ + allocSize_, begin_, 0, this);
    }

    const_iterator begin() const {
        return const_iterator(arr_, arr_ + allocSize_, begin_, 0, this);
    }

    const_iterator cbegin() const {
        return const_iterator(arr_, arr_ + allocSize_, begin_, 0, this);
    }

    iterator end() {
        return iterator(arr_, arr_ + allocSize_, end_, size_, this);
    }

    const_iterator end() const {
        return const_iterator(arr_, arr_ + allocSize_, end_, size_, this);
    }

    const_iterator cend() const {
        return const_iterator(arr_, arr_ + allocSize_, end_, size_, this);
    }


    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin());
    }
};


#endif //DEQUE_RINGBUFFER_H
