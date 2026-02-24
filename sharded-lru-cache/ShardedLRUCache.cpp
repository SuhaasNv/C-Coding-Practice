#ifndef SHARDED_LRU_CACHE_SHARDEDLRUCACHE_CPP
#define SHARDED_LRU_CACHE_SHARDEDLRUCACHE_CPP

#include "ShardedLRUCache.h"

template <typename Key, typename Value>
ShardedLRUCache<Key, Value>::ShardedLRUCache(const std::size_t capacity_per_shard,
                                             const std::size_t shard_count)
    : capacity_per_shard_(capacity_per_shard), shard_count_(shard_count) {
    if (capacity_per_shard_ == 0U) {
        throw std::invalid_argument("capacity_per_shard must be greater than zero");
    }
    if (shard_count_ == 0U) {
        throw std::invalid_argument("shard_count must be greater than zero");
    }

    shards_.reserve(shard_count_);
    for (std::size_t i = 0; i < shard_count_; ++i) {
        shards_.push_back(std::make_unique<Shard>(capacity_per_shard_));
    }
}

template <typename Key, typename Value>
std::optional<Value> ShardedLRUCache<Key, Value>::Get(const Key& key) {
    const std::size_t shard_index = ShardIndexForKey(key);
    Shard& shard = *shards_[shard_index];

    std::scoped_lock lock(shard.mutex);
    return shard.cache.Get(key);
}

template <typename Key, typename Value>
void ShardedLRUCache<Key, Value>::Put(const Key& key, const Value& value) {
    const std::size_t shard_index = ShardIndexForKey(key);
    Shard& shard = *shards_[shard_index];

    std::scoped_lock lock(shard.mutex);
    shard.cache.Put(key, value);
}

template <typename Key, typename Value>
std::size_t ShardedLRUCache<Key, Value>::Size() const {
    std::size_t total_size = 0;
    for (const auto& shard_ptr : shards_) {
        std::scoped_lock lock(shard_ptr->mutex);
        total_size += shard_ptr->cache.Size();
    }

    return total_size;
}

template <typename Key, typename Value>
void ShardedLRUCache<Key, Value>::Clear() {
    for (auto& shard_ptr : shards_) {
        std::scoped_lock lock(shard_ptr->mutex);
        shard_ptr->cache.Clear();
    }
}

template <typename Key, typename Value>
std::size_t ShardedLRUCache<Key, Value>::ShardIndexForKey(const Key& key) const noexcept {
    return hasher_(key) % shard_count_;
}

#endif  // SHARDED_LRU_CACHE_SHARDEDLRUCACHE_CPP
