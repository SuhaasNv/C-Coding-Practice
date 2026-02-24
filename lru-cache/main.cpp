#include "LRUCache.h"

#include <atomic>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

void PrintResult(const std::string& name, const bool passed) {
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << name << '\n';
}

bool TestCapacityValidation() {
    try {
        LRUCache<int, int> cache(0);
        (void)cache;
    } catch (const std::invalid_argument&) {
        return true;
    }

    return false;
}

bool TestBasicPutGetAndEviction() {
    LRUCache<int, std::string> cache(2);
    cache.Put(1, "one");
    cache.Put(2, "two");

    const auto one = cache.Get(1);
    if (!one.has_value() || one.value() != "one") {
        return false;
    }

    cache.Put(3, "three");

    const auto two = cache.Get(2);
    const auto three = cache.Get(3);
    return !two.has_value() && three.has_value() && three.value() == "three";
}

bool TestDuplicatePutUpdatesValue() {
    LRUCache<std::string, int> cache(2);
    cache.Put("k", 10);
    cache.Put("k", 42);

    const auto value = cache.Get("k");
    return value.has_value() && value.value() == 42 && cache.Size() == 1;
}

bool TestClearAndSize() {
    LRUCache<int, int> cache(3);
    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);

    if (cache.Size() != 3) {
        return false;
    }

    cache.Clear();
    return cache.Size() == 0 && !cache.Get(1).has_value();
}

bool TestConcurrentAccess() {
    constexpr int kThreads = 8;
    constexpr int kOpsPerThread = 5000;
    constexpr int kKeysPerThread = 16;
    constexpr std::size_t kCapacity = 256;

    LRUCache<int, int> cache(kCapacity);
    std::atomic<bool> failed{false};
    std::vector<std::thread> workers;
    workers.reserve(kThreads);

    for (int thread_id = 0; thread_id < kThreads; ++thread_id) {
        workers.emplace_back([thread_id, &cache, &failed]() {
            for (int i = 0; i < kOpsPerThread; ++i) {
                const int key = (thread_id * kKeysPerThread) + (i % kKeysPerThread);
                cache.Put(key, i);
                const auto value = cache.Get(key);
                if (!value.has_value()) {
                    failed.store(true);
                    return;
                }
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }

    return !failed.load() && cache.Size() <= static_cast<std::size_t>(kThreads * kKeysPerThread);
}

}  // namespace

int main() {
    PrintResult("Capacity=0 throws", TestCapacityValidation());
    PrintResult("Basic put/get/eviction", TestBasicPutGetAndEviction());
    PrintResult("Duplicate put updates existing entry", TestDuplicatePutUpdatesValue());
    PrintResult("Clear resets cache state", TestClearAndSize());
    PrintResult("Concurrent access stress", TestConcurrentAccess());
    return 0;
}
