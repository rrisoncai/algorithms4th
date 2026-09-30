#include <algorithms4th/Sort.hpp>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

template <typename Fn>
void benchmark(const char* name, Fn fn)
{
    auto start = std::chrono::steady_clock::now();
    fn();
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << name << ": " << elapsed.count() << " us\n";
}

std::vector<int> random_ints(std::size_t count)
{
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-100000, 100000);

    std::vector<int> values;
    values.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        values.push_back(dist(rng));
    }

    return values;
}

} // namespace

int main()
{
    constexpr std::size_t kInputSize = 10000;

    std::vector<int> input = random_ints(kInputSize);
    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());

    std::vector<int> values = input;
    assert(!Sort<int>::is_sorted(values));

    benchmark("selection_sort", [&values]() {
        Sort<int>::selection_sort(values);
    });
    assert(values == expected);
    assert(Sort<int>::is_sorted(values));

    values = input;
    benchmark("quick_sort", [&values]() {
        Sort<int>::quick_sort(values);
    });
    assert(values == expected);

    // Exercise both new algorithms on empty, singleton, ordered and duplicate input.
    for (const auto& edge : std::vector<std::vector<int>>{
             {}, {1}, {2, 1}, {1, 2, 3, 4}, {4, 3, 2, 1},
             std::vector<int>(256, 7), {3, -1, 3, 0, -1}}) {
        auto sorted = edge;
        std::sort(sorted.begin(), sorted.end());
        auto merged = edge;
        auto quick = edge;
        Sort<int>::merge_sort(merged);
        Sort<int>::quick_sort(quick);
        assert(merged == sorted);
        assert(quick == sorted);
    }

    values = input;
    benchmark("insertion_sort", [&values]() {
        Sort<int>::insertion_sort(values);
    });
    assert(values == expected);
    assert(Sort<int>::is_sorted(values));

    values = input;
    benchmark("merge_sort", [&values]() {
        Sort<int>::merge_sort(values);
    });
    assert(values == expected);
    assert(Sort<int>::is_sorted(values));

    std::vector<std::string> words = {"pear", "apple", "apple", "orange"};
    Sort<std::string>::merge_sort(words);
    assert((words == std::vector<std::string>{"apple", "apple", "orange", "pear"}));

    Sort<int> sorter({3, 2, 1});
    sorter.merge_sort();
    assert((sorter.data() == std::vector<int>{1, 2, 3}));
    assert(sorter.is_sorted());

    Sort<int> quick_sorter({3, 2, 1});
    quick_sorter.quick_sort();
    assert((quick_sorter.data() == std::vector<int>{1, 2, 3}));

    // Preserve the existing insertion-sort coverage as well.
    Sort<int> insertion_sorter({3, 2, 1});
    insertion_sorter.insertion_sort();
    assert((insertion_sorter.data() == std::vector<int>{1, 2, 3}));
    words = {"pear", "apple", "apple", "orange"};
    Sort<std::string>::insertion_sort(words);
    assert((words == std::vector<std::string>{"apple", "apple", "orange", "pear"}));
    words = {"pear", "apple", "apple", "orange"};
    Sort<std::string>::quick_sort(words);
    assert((words == std::vector<std::string>{"apple", "apple", "orange", "pear"}));

    return 0;
}
