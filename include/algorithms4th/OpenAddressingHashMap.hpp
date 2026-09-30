#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>
#include <utility>
#include <algorithm>
// 练习：open addressing（开放寻址，建议先用线性探测）。
// Key 支持 ==，Hash 支持 Hash{}(key)；Value 可复制。
// 所有方法由你实现；底层成员、辅助函数和扩容策略也由你设计。
template <class Key, class Value, class Hash = std::hash<Key>>
class OpenAddressingHashMap {
public:
    // initial_capacity == 0 时也要可用；它是初始容量，不是元素数量上限。
    explicit OpenAddressingHashMap(std::size_t initial_capacity = 8)
    {
        capacity_ = std::max<std::size_t>(initial_capacity, 8);
        hashmap_.resize(capacity_);
        states_.resize(capacity_, State::Empty);
        occupied_count_ = 0;
        deleted_count_ = 0;
    }

    // 新 key 返回 true；已有 key 覆盖 value，返回 false，size 不变。
    bool insert(const Key& key, const Value& value)
    {
        std::size_t h = hash_(key);
        std::size_t index = h % capacity_;
        std::size_t num_probe = capacity_;
        bool tomb_flag = false;
        std::size_t tomb_index = 0;
        while (num_probe--) {
            auto& s = states_[index];
            switch (s) {
            case State::Empty:
                if (tomb_flag) {
                    index = tomb_index;
                }
                hashmap_[index] = std::make_pair(key, value);
                states_[index] = State::Occupied;
                if (tomb_flag) {
                    --deleted_count_;
                }
                occupied_count_++;
                rehash();
                return true;
                break;
            case State::Occupied:
                if (hashmap_[index].first == key) {
                    hashmap_[index].second = value;
                    return false;
                } else {
                    index = (index + 1) % capacity_;
                }
                break;
            case State::Deleted:
                if (!tomb_flag) {
                    tomb_flag = true;
                    tomb_index = index;
                }
                index = (index + 1) % capacity_;
                break;
            }
        }

        if (tomb_flag) {
            hashmap_[tomb_index] = std::make_pair(key, value);
            states_[tomb_index] = State::Occupied;
            occupied_count_++;
            deleted_count_--;
            rehash();
        }
        return true;
    }

    // 返回值的副本；不存在时返回 std::nullopt，不修改 map。
    std::optional<Value> get(const Key& key) const
    {
        std::size_t h = hash_(key);
        std::size_t index = h % capacity_;
        std::size_t num_prob = capacity_;

        while (num_prob--) {
            auto& s = states_[index];
            if (s == State::Empty) {
                return std::nullopt;
            } else if (s == State::Occupied) {
                if (hashmap_[index].first == key) {
                    return hashmap_[index].second;
                } else {
                    index = (index + 1) % capacity_;
                }
            } else if (s == State::Deleted) {
                index = (index + 1) % capacity_;
            }
        }
        return std::nullopt;
    }

    // 删除成功返回 true；不存在返回 false。
    bool erase(const Key& key)
    {
        std::size_t h = hash_(key);
        std::size_t index = h % capacity_;
        std::size_t num_prob = capacity_;

        while (num_prob--) {
            auto& s = states_[index];
            if (s == State::Empty) {
                return false;
            } else if (s == State::Occupied) {
                if (hashmap_[index].first == key) {
                    hashmap_[index] = {};
                    s = State::Deleted;
                    occupied_count_--;
                    deleted_count_++;
                    return true;
                } else {
                    index = (index + 1) % capacity_;
                }
            } else if (s == State::Deleted) {
                index = (index + 1) % capacity_;
            }
        }
        return false;
    }

    std::size_t size() const
    {
        return occupied_count_;
    }

    bool rehash()
    {
        std::size_t new_cap = capacity_;
        if (occupied_count_ >= capacity_ / 2) {
            new_cap = capacity_ * 2;
        } else if (deleted_count_ >= capacity_ / 4) {
            new_cap = capacity_;
        } else {
            return false;
        }

        std::vector<std::pair<Key, Value>> new_hashmap(new_cap);
        std::vector<State> new_state(new_cap, State::Empty);
        for (std::size_t i = 0; i < capacity_; ++i) {
            auto& x = hashmap_[i];
            auto& s = states_[i];

            if (s != State::Occupied) {
                continue;
            }
            std::size_t h = hash_(x.first);
            std::size_t index = h % new_cap;

            std::size_t num_probe = new_cap;
            while (num_probe--) {
                auto& s = new_state[index];
                if (s == State::Empty) {
                    new_hashmap[index] = x;
                    s = State::Occupied;
                    break;
                } else if (s == State::Occupied) {
                    index = (index + 1) % new_cap;
                }
            }
        }

        hashmap_ = std::move(new_hashmap);
        states_ = std::move(new_state);
        capacity_ = new_cap;
        deleted_count_ = 0;
        return true;
    }

private:
    // TODO: 自己选择存储结构和必要成员。
    enum class State {
        Empty,
        Occupied,
        Deleted
    };
    std::vector<std::pair<Key, Value>> hashmap_;
    std::vector<State> states_;
    std::size_t capacity_;
    std::size_t occupied_count_;
    std::size_t deleted_count_ = 0;
    Hash hash_;
};
