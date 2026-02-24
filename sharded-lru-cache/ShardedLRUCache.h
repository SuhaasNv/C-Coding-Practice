#ifndef SHARDED_LRU_CACHE_SHARDEDLRUCACHE_H
#define SHARDED_LRU_CACHE_SHARDEDLRUCACHE_H

#include "LRUCache.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <vector>

// Thread-safe sharded LRU cache.
// Locking strategy:
// - Each shard owns an independent mutex.
// - Public methods lock only the shard(s) they touch.
// - No global mutex is used, reducing contention across disjoint key sets.
template <typename Key, typename Value>
class ShardedLRUCache final {
public:
    explicit ShardedLRUCache(std::size_t capacity_per_shard, std::size_t shard_count);
    ~ShardedLRUCache() = default;

    ShardedLRUCache(const ShardedLRUCache&) = delete;
    ShardedLRUCache& operator=(const ShardedLRUCache&) = delete;
    ShardedLRUCache(ShardedLRUCache&&) = delete;
    ShardedLRUCache& operator=(ShardedLRUCache&&) = delete;

    [[nodiscard]] std::optional<Value> Get(const Key& key);
    void Put(const Key& key, const Value& value);
    [[nodiscard]] std::size_t Size() const;
    void Clear();

private:
    struct Shard {
        explicit Shard(std::size_t capacity) : cache(capacity) {}

        LRUCache<Key, Value> cache;
        mutable std::mutex mutex;
    };

    [[nodiscard]] std::size_t ShardIndexForKey(const Key& key) const noexcept;

    std::size_t capacity_per_shard_;
    std::size_t shard_count_;
    std::hash<Key> hasher_;
    std::vector<std::unique_ptr<Shard>> shards_;
};

#include "ShardedLRUCache.cpp"

#endif  // SHARDED_LRU_CACHE_SHARDEDLRUCACHE_H
