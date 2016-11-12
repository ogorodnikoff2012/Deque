//
// Created by xenon on 11/9/16.
//

#ifndef DEQUE_DEQUE_H
#define DEQUE_DEQUE_H

#define MAX(X, Y) ((X) < (Y)) ? (Y) : (X)

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
    template <class IterType, class RefType, class PtrType, class DequeType>
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

        bool operator ==(const DequeIterator &other) const {
            return deque_ == other.deque_ && n_ == other.n_;
        }

        bool operator !=(const DequeIterator &other) const {
            return !operator==(other);
        }

        RefType operator *() const {
            return deque_->at(n_);
        }

        PtrType operator ->() const {
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

        RefType operator [](difference_type diff) const {
            return deque_->at(n_ + diff);
        }
        friend DequeType;
    };

    struct DataBlock {
        static const std::size_t SIZE = MAX(4ul, 1024 / sizeof(T));
        pointer buffer, begin, end;
        explicit DataBlock(pointer buf, bool fillFromEnd = false) :
                buffer(buf),
                begin(buf + (fillFromEnd ? SIZE : 0)),
                end(buf + (fillFromEnd ? SIZE : 0)) {}
        DataBlock() :
                buffer(nullptr),
                begin(nullptr),
                end(nullptr) {}
        DataBlock(const DataBlock &other) :
                buffer(other.buffer),
                begin(other.begin),
                end(other.end) {}
        bool canPushBack() const {
            return end - buffer < SIZE;
        }
        bool canPushFront() const {
            return buffer < begin;
        }
        bool empty() const {
            return begin == end;
        }
        size_type size() const {
            return end - begin;
        }
    };

    RingBuffer<DataBlock *> current_;
    mutable RingBuffer<DataBlock *> small_, big_;
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

    void reset() {
        for (size_type i = 0; i < current_.size(); ++i) {
            for (pointer it = current_[i]->begin; it != current_[i]->end; ++it) {
                allocator_.destroy(it);
            }
            allocator_.deallocate(current_[i]->buffer, DataBlock::SIZE);
            delete current_[i];
        }
    }

    template <class Alloc2>
    void copy(const Deque<T, Alloc2> &other) {
        for (size_type i = 0; i != other.current_.size(); ++i) {
            DataBlock *block = new DataBlock(*other.current_[i]);
            auto beginOffset = block->begin - block->buffer;
            auto endOffset = block->end - block->buffer;

            pointer ptr = allocator_.allocate(DataBlock::SIZE);
            std::copy(block->begin, block->end, ptr + beginOffset);
            block->begin = ptr + beginOffset;
            block->end = ptr + endOffset;
            block->buffer = ptr;

            small_.push_back(block);
            current_.push_back(block);
            big_.push_back(block);
        }
    }

public:
    typedef DequeIterator<value_type, reference, pointer, Deque<T, Allocator>> iterator;
    typedef DequeIterator<const value_type, reference, pointer, const Deque<T, Allocator>> const_iterator;
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

//    template <class Alloc2>
    Deque(const Deque &other) :
            small_(other.small_.maxSize()),
            current_(other.current_.maxSize()),
            big_(other.big_.maxSize()),
            allocator_() {
        copy(other);
    }

    template <class Alloc2>
    Deque &operator =(const Deque<T, Alloc2> &other) {
        if (&other != this) {
            reset();
            small_.resetAndResize(other.small_.maxSize());
            current_.resetAndResize(other.current_.maxSize());
            big_.resetAndResize(other.big_.maxSize());
            copy(other);
        }
        return *this;
    }

    ~Deque() {
        reset();
    }

    void push_back(const value_type &val) {
        if (current_.empty() || !current_.back()->canPushBack()) {
            if (current_.full()) {
                levelUp();
            }
            pointer ptr = allocator_.allocate(DataBlock::SIZE);
            DataBlock *block = new DataBlock(ptr);
            current_.push_back(block);
            if (!small_.full()) {
                small_.push_back(block);
            }
        }
        overtake();
        allocator_.construct(current_.back()->end++, val);
    }

    void pop_back() {
        if (empty()) {
            throw std::runtime_error("Deque is already empty");
        }
        overtake();
        allocator_.destroy(--current_.back()->end);
        if (current_.back()->empty()) {
            DataBlock *block = current_.back();
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
            allocator_.deallocate(block->buffer, DataBlock::SIZE);
            delete block;
        }
    }

    void push_front(const value_type &val) {
        if (current_.empty() || !current_.front()->canPushFront()) {
            if (current_.full()) {
                levelUp();
            }
            pointer ptr = allocator_.allocate(DataBlock::SIZE);
            DataBlock *block = new DataBlock(ptr, true);
            current_.push_front(block);
            big_.push_front(block);
            if (small_.full()) {
                small_.pop_back();
            }
            small_.push_front(block);
        }
        overtake();
        allocator_.construct(--current_.front()->begin, val);
    }

    void pop_front() {
        if (empty()) {
            throw std::runtime_error("Deque is already empty");
        }
        allocator_.destroy(current_.front()->begin++);
        if (current_.front()->empty()) {
            DataBlock *block = current_.front();
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
            allocator_.deallocate(block->buffer, DataBlock::SIZE);
            delete block;
        }
    }

    reference operator [](size_type n) {
        size_type offset = current_.front()->begin - current_.front()->buffer;
        size_type index = n + offset;
        return current_[index / DataBlock::SIZE]->buffer[index % DataBlock::SIZE];
    }

    reference at(size_type n) {
        size_type offset = current_.front()->begin - current_.front()->buffer;
        size_type index = n + offset;
        return current_[index / DataBlock::SIZE]->buffer[index % DataBlock::SIZE];
    }
    const_reference operator [](size_type n) const {
        size_type offset = current_.front()->begin - current_.front()->buffer;
        size_type index = n + offset;
        return current_[index / DataBlock::SIZE]->buffer[index % DataBlock::SIZE];
    }

    reference back() {
        return *(current_.back()->end - 1);
    }

    const_reference back() const {
        return *(current_.back()->end - 1);
    }

    reference front() {
        return *current_.front()->begin;
    }

    const_reference front() const {
        return *current_.front()->begin;
    }

    bool empty() const {
        return current_.empty();
    }

    size_type size() const {
        size_type blocksCount = current_.size();
        return blocksCount == 0 ? 0 :
               blocksCount == 1 ? current_.front()->size() :
               (blocksCount - 2) * DataBlock::SIZE + current_.front()->size() + current_.back()->size();
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
        return iterator(size(), this);
    }

    const_iterator end() const {
        return const_iterator(size(), this);
    }

    const_iterator cend() const {
        return const_iterator(size(), this);
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

    size_t getBlocksCount() const {
        return current_.size();
    }

    const RingBuffer<DataBlock *> &getBlocks() const {
        return current_;
    }
};


#endif //DEQUE_DEQUE_H

