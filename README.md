Deque
=====

This is my realization of double-ended queue (aka deque). It supports
following operations:

+ `push_back(const T &x)`
+ `push_front(const T &x)` 
+ `pop_back()` 
+ `pop_front()`
+ `operator [](std::size_t n)`

All these operations use *O(1)* time (not amortized). This structure uses
*O(*`size()`*)* memory.

Also this structure provides random access iterators.

This project uses Google Test. Compile your copy of this library and
put it in `lib/`.