#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>

#include <vector>
#include <list>
#include <utility>
#include <algorithm>

// 练习：separate chaining（拉链法）。
// Key 支持 ==，Hash 支持 Hash{}(key)；Value 可复制。
// 所有方法由你实现；底层成员、辅助函数和扩容策略也由你设计。
template <class Key, class Value, class Hash = std::hash<Key>>
class ChainingHashMap {
public:
    // initial_capacity == 0 时也要可用；它是初始容量，不是元素数量上限。
    explicit ChainingHashMap(std::size_t initial_capacity = 8)
    {
        capacity_ = std::max<std::size_t>(initial_capacity, 8);
        size_ = 0;
        hashmap_.resize(capacity_);
    }

    // 新 key 返回 true；已有 key 覆盖 value，返回 false，size 不变。
    bool insert(const Key& key, const Value& value)
    {
        std::size_t h = hash_(key);
        std::size_t index = h % capacity_;

        auto& bucket = hashmap_[index];

        for (auto& x : bucket) {
            if (x.first == key) {
                x.second = value;
                return false;
            }
        }

        bucket.push_back(std::make_pair(key, value));
        size_++;

        if (size_ > capacity_) {
            rehash();
        }
        return true;
    }

    // 返回值的副本；不存在时返回 std::nullopt，不修改 map。
    std::optional<Value> get(const Key& key) const
    {
        std::size_t h = hash_(key);
        std::size_t index = h % capacity_;

        auto& bucket = hashmap_[index];

        for (auto& x : bucket) {
            if (x.first == key) {
                return x.second;
            }
        }
        return std::nullopt;
    }

    // 删除成功返回 true；不存在返回 false。
    bool erase(const Key& key)
    {
        std::size_t h = hash_(key);
        std::size_t index = h % capacity_;
        auto& bucket = hashmap_[index];

        for (auto it = bucket.begin(); it != bucket.end();) {
            if (it->first == key) {
                bucket.erase(it);
                --size_;
                return true;
            } else {
                ++it;
            }
        }
        return false;
    }

    std::size_t size() const
    {
        return size_;
    }

    bool rehash(void)
    {
        std::size_t new_cap = capacity_ * 2;
        std::vector<std::list<std::pair<Key, Value>>> new_hashmap;
        new_hashmap.resize(new_cap);

        for (auto& b : hashmap_) {
            for (auto& x : b) {
                auto h = hash_(x.first);
                auto index = h % new_cap;
                auto& bucket = new_hashmap[index];
                bucket.push_back(x);
            }
        }
        hashmap_ = std::move(new_hashmap);
        capacity_ = new_cap;
        return true;
    }

private:
    // TODO: 自己选择存储结构和必要成员。
    std::vector<std::list<std::pair<Key, Value> > > hashmap_;
    Hash hash_;
    std::size_t size_;
    std::size_t capacity_;

};
