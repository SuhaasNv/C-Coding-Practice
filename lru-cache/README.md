# Thread-Safe LRU Cache (C++20)

A production-style implementation of a thread-safe least-recently-used cache using `std::unordered_map` and `std::list`.

## Features
- Fixed-capacity cache with LRU eviction
- `Get`, `Put`, `Size`, and `Clear` operations
- O(1) average lookup and update
- `std::optional` return for cache misses
- Thread-safe public API using `std::mutex`

## Design Notes
- `std::list` maintains recency order with stable iterators.
- `std::unordered_map` stores key-to-list-iterator mappings for O(1) average access.
- A single mutex protects both data structures to keep state transitions consistent.

## Build

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread main.cpp -o lru_cache_app
./lru_cache_app
```
