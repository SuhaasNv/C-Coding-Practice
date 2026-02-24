#ifndef LRU_CACHE_LRUCACHE_H
#define LRU_CACHE_LRUCACHE_H

#include <cstddef>
#include <list>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>

// Thread-safe LRU cache using list+hash-map:
// - list keeps recency order (front = most recently used)
// - map stores key -> list iterator for O(1) lookup and promotion
template <typename Key, typename Value>
class LRUCache final {
public:
    explicit LRUCache(std::size_t capacity);
    ~LRUCache() = default;

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
    LRUCache(LRUCache&& other) noexcept;
    LRUCache& operator=(LRUCache&& other) noexcept;

    [[nodiscard]] std::optional<Value> Get(const Key& key);
    void Put(const Key& key, const Value& value);
    [[nodiscard]] std::size_t Size() const;
    void Clear();

private:
    using Node = std::pair<Key, Value>;
    using NodeList = std::list<Node>;
    using NodeIterator = typename NodeList::iterator;

    void MoveToFront(NodeIterator iterator) noexcept;

    std::size_t capacity_;
    NodeList entries_;
    std::unordered_map<Key, NodeIterator> index_;
    mutable std::mutex mutex_;
};

#include "LRUCache.cpp"

#endif  // LRU_CACHE_LRUCACHE_H
