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
    benchmark("insertion_sort", [&values]() {
        Sort<int>::insertion_sort(values);
    });
    assert(values == expected);
    assert(Sort<int>::is_sorted(values));

    std::vector<std::string> words = {"pear", "apple", "apple", "orange"};
    Sort<std::string>::insertion_sort(words);
    assert((words == std::vector<std::string>{"apple", "apple", "orange", "pear"}));

    Sort<int> sorter({3, 2, 1});
    sorter.insertion_sort();
    assert((sorter.data() == std::vector<int>{1, 2, 3}));
    assert(sorter.is_sorted());

    return 0;
}
