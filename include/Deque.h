//
// Created by xenon on 11/9/16.
//

#ifndef DEQUE_DEQUE_H
#define DEQUE_DEQUE_H

#include <iterator>
#include <memory>
#include <exception>

#include "RingBuffer.h"

template <class T, class Allocator = std::allocator<T>>
class Deque {
public:
    typedef T value_type;
    typedef Allocator allocator_type;
    typedef typename allocator_type::reference reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef std::size_t size_type;
private:
    template <class IterType, class DequeType>
    class DequeIterator : public std::iterator<std::random_access_iterator_tag, IterType> {
    public:
        typedef std::ptrdiff_t difference_type;
    private:
        difference_type n_;
        DequeType *deque_;
        DequeIterator(std::size_t n, DequeType *deque) :
                n_(n), deque_(deque)
        {}
    public:
        DequeIterator() : n_(0), deque_(nullptr) {}
        DequeIterator(const DequeIterator &other) : n_(other.n_), deque_(other.deque_) {}
        DequeIterator &operator =(const DequeIterator &other) {
            n_ = other.n_;
            deque_ = other.deque_;
            return *this;
        }

        bool operator ==(const DequeIterator &other) {
            return deque_ == other.deque_ && n_ == other.n_;
        }

        bool operator !=(const DequeIterator &other) {
            return !operator==(other);
        }

        reference operator *() const {
            return deque_->at(n_);
        }

        pointer operator ->() const {
            return &deque_->at(n_);
        }

        DequeIterator &operator ++() {
            ++n_;
            return *this;
        }

        const DequeIterator operator ++(int) {
            DequeIterator ans = *this;
            ++n_;
            return ans;
        }

        DequeIterator &operator --() {
            --n_;
            return *this;
        }

        const DequeIterator operator --(int) {
            DequeIterator ans = *this;
            --n_;
            return ans;
        }

        const DequeIterator operator +(difference_type diff) const {
            DequeIterator ans = *this;
            ans += diff;
            return ans;
        }

        const DequeIterator operator -(difference_type diff) const {
            DequeIterator ans = *this;
            ans -= diff;
            return ans;
        }

        difference_type operator -(const DequeIterator &other) const {
            if (deque_ != other.deque_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ - other.n_;
        }

        bool operator <(const DequeIterator &other) const {
            if (deque_ != other.deque_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ < other.n_;
        }

        bool operator >(const DequeIterator &other) const {
            if (deque_ != other.deque_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ > other.n_;
        }

        bool operator <=(const DequeIterator &other) const {
            if (deque_ != other.deque_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ <= other.n_;
        }

        bool operator >=(const DequeIterator &other) const {
            if (deque_ != other.deque_) {
                throw std::runtime_error("Container mismatch");
            }
            return n_ >= other.n_;
        }
        DequeIterator &operator +=(difference_type diff) {
            n_ += diff;
            return *this;
        }

        DequeIterator &operator -=(difference_type diff) {
            n_ -= diff;
            return *this;
        }

        reference operator [](difference_type diff) const {
            return deque_->at(n_ + diff);
        }
        friend DequeType;
    };

    RingBuffer<pointer> current_;
    mutable RingBuffer<pointer> small_, big_;
    Allocator allocator_;

    bool smallUpToDate() const {
        return small_.full() || small_.size() == current_.size();
    }

    void smallOvertake() const {
        if (!smallUpToDate()) {
            small_.push_back(current_[small_.size()]);
        }
    }

    bool bigUpToDate() const {
        return big_.size() == current_.size();
    }

    void bigOvertake() const {
        if (!bigUpToDate()) {
            big_.push_back(current_[big_.size()]);
        }
    }

    void overtake() const {
        smallOvertake();
        smallOvertake();
        bigOvertake();
        bigOvertake();
        bigOvertake();
    }

    void levelUp() {
        current_.swap(small_);
        current_.swap(big_);
        big_.resetAndResize(2 * current_.maxSize());
    }

    void levelDown() {
        if (small_.maxSize() < 2 * MIN_BUFFER_SIZE) {
            return;
        }
        current_.swap(big_);
        current_.swap(small_);
        small_.resetAndResize(current_.maxSize() / 2);
    }

public:
    typedef DequeIterator<value_type, Deque<T, Allocator>> iterator;
    typedef DequeIterator<const value_type, const Deque<T, Allocator>> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename std::iterator_traits<iterator>::difference_type difference_type;

    const static std::size_t MIN_BUFFER_SIZE = 4;

    Deque(const Allocator &allocator = Allocator()) :
            small_(MIN_BUFFER_SIZE),
            current_(MIN_BUFFER_SIZE * 2),
            big_(MIN_BUFFER_SIZE * 4),
            allocator_(allocator)
    {}

    Deque(const Deque &other) :
            small_(other.small_),
            current_(other.current_),
            big_(other.big_),
            allocator_(other.allocator_)
    {}

    ~Deque() {
        for (size_type i = 0; i < current_.size(); ++i) {
            allocator_.destroy(current_[i]);
            allocator_.deallocate(current_[i], 1);
        }
    }

    void push_back(const value_type &val) {
        if (current_.full()) {
            levelUp();
        }
        pointer ptr = allocator_.allocate(1);
        allocator_.construct(ptr, val);
        current_.push_back(ptr);
        if (!small_.full()) {
            small_.push_back(ptr);
        }
        overtake();
    }

    void pop_back() {
        if (empty()) {
            throw std::runtime_error("Deque is already empty");
        }
        pointer ptr = current_.back();
        overtake();
        if (bigUpToDate()) {
            big_.pop_back();
        }
        if (smallUpToDate() && current_.size() <= small_.maxSize()) {
            small_.pop_back();
        }
        current_.pop_back();
        if (current_.size() <= small_.maxSize()) {
            levelDown();
        }
        allocator_.destroy(ptr);
        allocator_.deallocate(ptr, 1);
    }

    void push_front(const value_type &val) {
        if (current_.full()) {
            levelUp();
        }
        pointer ptr = allocator_.allocate(1);
        allocator_.construct(ptr, val);
        current_.push_front(ptr);
        big_.push_front(ptr);
        if (small_.full()) {
            small_.pop_back();
        }
        small_.push_front(ptr);
        overtake();
    }

    void pop_front() {
        if (empty()) {
            throw std::runtime_error("Deque is already empty");
        }
        pointer ptr = current_.front();
        if (!big_.empty()) {
            big_.pop_front();
        }
        if (!small_.empty()) {
            small_.pop_front();
        }
        current_.pop_front();
        overtake();
        if (current_.size() <= small_.maxSize()) {
            levelDown();
        }
        allocator_.destroy(ptr);
        allocator_.deallocate(ptr, 1);
    }

    reference operator [](size_type n) {
        return *current_[n];
    }

    reference at(size_type n) {
        return *current_[n];
    }
    const_reference operator [](size_type n) const {
        return *current_[n];
    }

    reference back() {
        return *current_.back();
    }

    const_reference back() const {
        return *current_.back();
    }

    reference front() {
        return *current_.front();
    }

    const_reference front() const {
        return *current_.front();
    }

    bool empty() const {
        return current_.empty();
    }

    size_type size() const {
        return current_.size();
    }

    iterator begin() {
        return iterator(0, this);
    }

    const_iterator begin() const {
        return const_iterator(0, this);
    }

    const_iterator cbegin() const {
        return const_iterator(0, this);
    }

    iterator end() {
        return iterator(current_.size(), this);
    }

    const_iterator end() const {
        return const_iterator(current_.size(), this);
    }

    const_iterator cend() const {
        return const_iterator(current_.size(), this);
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


#endif //DEQUE_DEQUE_H

