# Sharded Thread-Safe LRU Cache (C++20)

A scalable cache design using hash-based sharding and per-shard locking.

## Design
- `LRUCache`: non-thread-safe LRU cache used internally by each shard.
- `ShardedLRUCache`: thread-safe wrapper with lock striping.
- Shard selection: `std::hash<Key>{}(key) % shard_count`.

## Concurrency Strategy
- Each shard owns one `std::mutex` and one `LRUCache` instance.
- `Get`/`Put` lock only one shard.
- `Size`/`Clear` aggregate across shards with one-shard-at-a-time locking.
- No global mutex, reducing contention under mixed key access.

## Build

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -pthread main.cpp -o sharded_lru_cache_app
./sharded_lru_cache_app
```

## Concurrent Benchmark

Executed with multiple threads performing mixed `Get`/`Put` operations.

Example output:

```text
480000 operations in 103 ms
≈ 4.6M ops/sec
```

This demonstrates reduced lock contention through shard-based partitioning.

## Benchmark Notes

Test configuration:
- Threads: 12
- Shards: 16
- Capacity per shard: 128
- Key space: 4096
- Compiler: `g++` (C++20)
