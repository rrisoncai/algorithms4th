#pragma once

#include <cstddef>
#include <utility>
#include <vector>

template <typename T>
class Sort {
public:
    explicit Sort(std::vector<T> data) : data_(std::move(data)) {}

    void selection_sort()
    {
        selection_sort(data_);
    }

    void insertion_sort()
    {
        insertion_sort(data_);
    }

    void merge_sort()
    {
        merge_sort(data_);
    }

    void quick_sort()
    {
        quick_sort(data_);
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

    static void merge_sort(std::vector<T>& values)
    {
        const std::size_t n = values.size();
        std::vector<T> aux(n);
        merge_sort(values, aux, 0, n);
    }

    static void merge_sort(std::vector<T>& values, std::vector<T>& aux, std::size_t left, std::size_t right)
    {
        if (right - left <= 1) {
            return;
        }
        std::size_t mid = left + (right - left) / 2;
        merge_sort(values, aux, left, mid);
        merge_sort(values, aux, mid, right);
        merge(values, aux, left, mid, right);
    }

    static void merge(std::vector<T>& values, std::vector<T>& aux, std::size_t left, std::size_t mid, std::size_t right)
    {
        std::size_t i = left;
        std::size_t j = mid;
        std::size_t k = left;

        while (i < mid && j < right) {
            if (values[j] < values[i]) {
                aux[k++] = values[j++];
            } else {
                aux[k++] = values[i++];
            }
        }

        while (i < mid) {
            aux[k++] = values[i++];
        }

        while (j < right) {
            aux[k++] = values[j++];
        }

        for (std::size_t p = left; p < right; ++p) {
            values[p] = aux[p];
        }
    }

    // ponytail: first-element pivot can take O(n^2) time and O(n) stack;
    // use three-way partitioning and recurse on the smaller side if needed.
    static void quick_sort(std::vector<T>& values)
    {
        quick_sort(values, 0, values.size());
    }

    static void quick_sort(std::vector<T>& values, std::size_t left, std::size_t right)
    {
        if (right - left <= 1) {
            return;
        }
        std::size_t pivot = partition(values, left, right);
        quick_sort(values, left, pivot);
        quick_sort(values, pivot + 1, right);
    }

    static std::size_t partition(std::vector<T>& values, std::size_t left, std::size_t right)
    {
        T v = values[left];
        std::size_t store = left + 1;

        for (auto i = left + 1; i < right; ++i) {
            if (values[i] < v) {
                swap(values[i], values[store]);
                ++store;
            }
        }

        std::size_t pivot_index = store - 1;
        swap(values[left], values[pivot_index]);
        return pivot_index;
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
