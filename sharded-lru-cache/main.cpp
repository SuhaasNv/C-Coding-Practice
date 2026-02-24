#include "ShardedLRUCache.h"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

void PrintResult(const std::string& name, const bool passed) {
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << name << '\n';
}

bool TestConstructorValidation() {
    bool zero_capacity_thrown = false;
    bool zero_shards_thrown = false;

    try {
        ShardedLRUCache<int, int> invalid_cache(0, 2);
        (void)invalid_cache;
    } catch (const std::invalid_argument&) {
        zero_capacity_thrown = true;
    }

    try {
        ShardedLRUCache<int, int> invalid_cache(2, 0);
        (void)invalid_cache;
    } catch (const std::invalid_argument&) {
        zero_shards_thrown = true;
    }

    return zero_capacity_thrown && zero_shards_thrown;
}

bool TestBasicBehavior() {
    ShardedLRUCache<int, std::string> cache(2, 2);

    cache.Put(1, "one");
    cache.Put(2, "two");

    const auto one = cache.Get(1);
    const auto two = cache.Get(2);
    if (!one.has_value() || one.value() != "one" || !two.has_value() || two.value() != "two") {
        return false;
    }

    cache.Put(1, "one-updated");
    const auto updated = cache.Get(1);
    return updated.has_value() && updated.value() == "one-updated";
}

bool TestEvictionPerShard() {
    // One shard gives deterministic LRU behavior while still exercising sharded API.
    ShardedLRUCache<int, int> cache(2, 1);
    cache.Put(1, 10);
    cache.Put(2, 20);
    (void)cache.Get(1);  // Promote key 1 to MRU.
    cache.Put(3, 30);  // Should evict key 2.

    const auto one = cache.Get(1);
    const auto two = cache.Get(2);
    const auto three = cache.Get(3);
    return one.has_value() && !two.has_value() && three.has_value();
}

bool TestClearAndSize() {
    ShardedLRUCache<int, int> cache(3, 4);
    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);

    if (cache.Size() == 0) {
        return false;
    }

    cache.Clear();
    return cache.Size() == 0 && !cache.Get(1).has_value();
}

bool TestConcurrentStress() {
    constexpr int kThreads = 12;
    constexpr int kOpsPerThread = 4000;
    constexpr int kShards = 16;
    constexpr int kCapacityPerShard = 64;

    ShardedLRUCache<int, int> cache(kCapacityPerShard, kShards);
    std::atomic<bool> failed{false};
    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int thread_id = 0; thread_id < kThreads; ++thread_id) {
        threads.emplace_back([thread_id, &cache, &failed]() {
            for (int i = 0; i < kOpsPerThread; ++i) {
                // Spread accesses across shard space while maintaining overlap.
                const int key = ((thread_id * 131) + i) % 1024;
                cache.Put(key, i);
                const auto value = cache.Get(key);
                if (!value.has_value()) {
                    failed.store(true);
                    return;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    const std::size_t max_possible = static_cast<std::size_t>(kShards * kCapacityPerShard);
    return !failed.load() && cache.Size() <= max_possible;
}

void RunConcurrentBenchmark() {
    constexpr int kThreads = 12;
    constexpr int kOpsPerThread = 20000;
    constexpr int kShards = 16;
    constexpr int kCapacityPerShard = 128;
    constexpr int kKeySpace = 4096;

    ShardedLRUCache<int, int> cache(kCapacityPerShard, kShards);
    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    const auto start = std::chrono::steady_clock::now();
    for (int thread_id = 0; thread_id < kThreads; ++thread_id) {
        threads.emplace_back([thread_id, &cache]() {
            for (int i = 0; i < kOpsPerThread; ++i) {
                const int key = ((thread_id * 257) + i) % kKeySpace;
                cache.Put(key, i);
                (void)cache.Get(key);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
    const auto end = std::chrono::steady_clock::now();

    const auto elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    const double elapsed_s = static_cast<double>(elapsed_ms) / 1000.0;
    const double total_operations = static_cast<double>(kThreads) * kOpsPerThread * 2.0;
    const double ops_per_second = elapsed_s > 0.0 ? total_operations / elapsed_s : 0.0;

    std::cout << "[INFO] Benchmark: " << static_cast<long long>(total_operations)
              << " operations in " << elapsed_ms << " ms ("
              << std::fixed << std::setprecision(2) << ops_per_second << " ops/s)\n";
}

}  // namespace

int main() {
    PrintResult("Constructor validation", TestConstructorValidation());
    PrintResult("Basic put/get/update", TestBasicBehavior());
    PrintResult("Per-shard LRU eviction", TestEvictionPerShard());
    PrintResult("Clear and size", TestClearAndSize());
    PrintResult("Concurrent stress", TestConcurrentStress());
    RunConcurrentBenchmark();
    return 0;
}
