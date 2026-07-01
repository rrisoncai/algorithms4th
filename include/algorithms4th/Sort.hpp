#pragma once

#include <cstddef>
#include <utility>
#include <vector>

template <typename T>
class Sort {
public:
    explicit Sort(std::vector<T> data): data_(std::move(data)) {}

    void selection_sort()
    {
        selection_sort(data_);
    }

    void insertion_sort()
    {
        insertion_sort(data_);
    }

    bool is_sorted() const
    {
        return is_sorted(data_);
    }

    const std::vector<T>& data() const
    {
        return data_;
    }

    static void selection_sort(std::vector<T>& values)
    {
        const std::size_t n = values.size();
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t min_index = i;
            for (std::size_t j = i + 1; j < n; ++j) {
                if (values[j] < values[min_index]) {
                    min_index = j;
                }
            }
            swap(values[i], values[min_index]);
        }
    }

    static void insertion_sort(std::vector<T>& values)
    {
        const std::size_t n = values.size();
        for (std::size_t i = 1; i < n; ++i) {
            for (std::size_t j = i; j > 0 && values[j] < values[j - 1]; --j) {
                swap(values[j], values[j - 1]);
            }
        }
    }

    static bool is_sorted(const std::vector<T>& values)
    {
        for (std::size_t i = 1; i < values.size(); ++i) {
            if (values[i] < values[i - 1]) {
                return false;
            }
        }
        return true;
    }

private:
    static void swap(T& lhs, T& rhs)
    {
        using std::swap;
        swap(lhs, rhs);
    }

    std::vector<T> data_;
};
