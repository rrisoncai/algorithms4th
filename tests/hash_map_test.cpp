#include <algorithms4th/ChainingHashMap.hpp>
#include <algorithms4th/OpenAddressingHashMap.hpp>

#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <unordered_map>

// Release 构建也执行检查，失败时报告具体行号。
#define CHECK(expr) do { if (!(expr)) throw std::runtime_error( \
    std::string("line ") + std::to_string(__LINE__) + ": " #expr); } while (false)

// 所有 key 冲突；常见的 2 的幂容量下从最后一格开始，覆盖探测绕回。
struct CollisionHash {
    std::size_t operator()(int) const { return std::numeric_limits<std::size_t>::max(); }
};

struct CountedKey {
    int value;
    inline static std::size_t comparisons = 0;
    bool operator==(const CountedKey& other) const
    {
        ++comparisons;
        return value == other.value;
    }
};

struct CountingHash {
    std::size_t operator()(const CountedKey& key) const { return key.value; }
};

struct RebuildHash {
    inline static std::size_t calls = 0;
    std::size_t operator()(int key) const { ++calls; return key; }
};

void test_tombstones()
{
    OpenAddressingHashMap<int, int, RebuildHash> map(32);
    CHECK(!map.rehash());
    for (int i = 0; i < 12; ++i) CHECK(map.insert(i, i));
    for (int i = 0; i < 8; ++i) CHECK(map.erase(i));
    CHECK(!map.erase(0));
    RebuildHash::calls = 0;
    CHECK(!map.insert(8, 80)); // 更新不触发清理。
    CHECK(RebuildHash::calls == 1);
    RebuildHash::calls = 0;
    CHECK(map.insert(20, 200)); // 不复用墓碑，新增后触发清理。
    CHECK(RebuildHash::calls == 6); // 插入 1 次，重建 5 个有效元素。
    CHECK(map.size() == 5);
    CHECK(!map.rehash()); // 清理后墓碑计数已归零。
    for (int i = 0; i < 8; ++i) CHECK(!map.get(i));
    CHECK(map.get(8) == 80);
    for (int i = 9; i < 12; ++i) CHECK(map.get(i) == i);
    CHECK(map.get(20) == 200);

    // 清理没有扩大容量：第 16 个有效元素仍应触发扩容。
    for (int i = 21; i < 31; ++i) CHECK(map.insert(i, i));
    RebuildHash::calls = 0;
    CHECK(map.insert(31, 31));
    CHECK(RebuildHash::calls == 17);
    CHECK(map.size() == 16);

    OpenAddressingHashMap<int, int, RebuildHash> reuse(8);
    CHECK(reuse.insert(0, 0));
    CHECK(reuse.insert(1, 1));
    for (int round = 0; round < 16; ++round) {
        CHECK(reuse.erase(round * 8));
        RebuildHash::calls = 0;
        CHECK(reuse.insert((round + 1) * 8, round));
        CHECK(RebuildHash::calls == 1); // 复用墓碑不应积累虚假的墓碑计数。
        CHECK(reuse.size() == 2);
        CHECK(reuse.get((round + 1) * 8) == round);
        CHECK(reuse.get(1) == 1);
    }
}

template <template <class, class, class> class Map>
void run(const std::string& stage)
{
    if (stage == "scaling") {
        bool within_budget = true;
        for (int n : {1024, 4096}) {
            Map<CountedKey, int, CountingHash> map(8);
            auto start = std::chrono::steady_clock::now();
            for (int i = 0; i < n; ++i) CHECK(map.insert(CountedKey{i}, i));
            auto built = std::chrono::steady_clock::now();
            CHECK(map.size() == static_cast<std::size_t>(n));
            CountedKey::comparisons = 0;
            for (int i = 0; i < n; ++i) CHECK(map.get(CountedKey{i}) == i);
            for (int i = n; i < 2 * n; ++i) CHECK(!map.get(CountedKey{i}));
            auto finished = std::chrono::steady_clock::now();
            auto comparisons = CountedKey::comparisons;
            // 在分布良好的哈希下，每次查找平均最多 16 次 key 比较。
            // 时间只作观察；断言不依赖机器速度或系统负载。
            auto budget = static_cast<std::size_t>(2 * n) * 16;
            std::cout << "n=" << n << ", insert_us="
                      << std::chrono::duration_cast<std::chrono::microseconds>(built - start).count()
                      << ", lookup_us="
                      << std::chrono::duration_cast<std::chrono::microseconds>(finished - built).count()
                      << ", comparisons=" << comparisons << ", budget=" << budget << '\n';
            within_budget = within_budget && comparisons <= budget;
        }
        CHECK(within_budget);
        return;
    }

    if (stage == "rehash") {
        for (std::size_t capacity : {1, 3, 8}) {
            Map<int, int, std::hash<int>> map(capacity);
            std::unordered_map<int, int> expected;
            for (int round = 0; round < 4; ++round) {
                int limit = (round + 1) * 128;
                // 包含旧键更新、已删键重插、新键插入，再删除部分键。
                for (int key = 0; key < limit; ++key) {
                    int value = round * 1000 + key;
                    bool added = expected.insert_or_assign(key, value).second;
                    CHECK(map.insert(key, value) == added);
                    CHECK(map.size() == expected.size());
                    // 每次插入后检查所有旧值，捕获扩容丢元素和错误分桶。
                    for (const auto& entry : expected) CHECK(map.get(entry.first) == entry.second);
                }
                for (int key = round; key < limit; key += 3) {
                    CHECK(map.erase(key) == (expected.erase(key) != 0));
                    CHECK(!map.erase(key));
                }
                CHECK(map.size() == expected.size());
                for (int key = 0; key <= limit; ++key) {
                    auto it = expected.find(key);
                    if (it == expected.end()) CHECK(!map.get(key));
                    else CHECK(map.get(key) == it->second);
                }
            }
        }
        return;
    }

    if (stage == "generic") {
        Map<std::string, std::string, std::hash<std::string>> map;
        CHECK(map.insert("", ""));
        CHECK(map.insert("键", "值"));
        CHECK(!map.insert("键", "新值"));
        const auto& view = map;
        CHECK(view.get("") == std::optional<std::string>(""));
        CHECK(view.get("键") == std::optional<std::string>("新值"));
        CHECK(!view.get("missing"));
        CHECK(view.size() == 2);
        CHECK(map.erase(""));
        CHECK(!map.get(""));
        CHECK(map.size() == 1);
        return;
    }

    Map<int, int, CollisionHash> map(8);
    const auto& view = map;
    if (stage == "empty") {
        CHECK(view.size() == 0);
        CHECK(!view.get(42));
        CHECK(!map.erase(42));
        CHECK(view.size() == 0);
        Map<int, int, CollisionHash> zero(0);
        CHECK(zero.size() == 0);
        CHECK(zero.insert(1, 2));
        CHECK(zero.get(1) == 2);
    } else if (stage == "basic") {
        for (int key : {0, -1, std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max()}) {
            auto before = map.size();
            CHECK(map.insert(key, 0));
            CHECK(view.get(key) == 0);
            CHECK(map.size() == before + 1);
            CHECK(!map.insert(key, -7));
            CHECK(view.get(key) == -7);
            CHECK(map.size() == before + 1);
        }
        CHECK(!view.get(42));
        CHECK(map.size() == 4);
        CHECK(map.erase(-1));
        CHECK(!map.erase(-1));
        CHECK(!view.get(-1));
        CHECK(map.size() == 3);
    } else if (stage == "collisions") {
        for (int key : {10, 20, 30, 40}) CHECK(map.insert(key, key));
        CHECK(map.erase(20));
        CHECK(view.get(10) == 10);
        CHECK(view.get(30) == 30);
        CHECK(view.get(40) == 40);
        CHECK(!view.get(20));
        // 删除留下的空位不能让更新误判为新 key。
        CHECK(!map.insert(30, 300));
        CHECK(map.size() == 3);
        CHECK(map.insert(50, 500));
        CHECK(map.size() == 4);
        CHECK(view.get(30) == 300);
        CHECK(view.get(50) == 500);
        for (int key : {10, 40, 30, 50}) CHECK(map.erase(key));
        CHECK(map.size() == 0);
        CHECK(!view.get(50));
        CHECK(map.insert(20, 200));
        CHECK(view.get(20) == 200);
    } else if (stage == "growth") {
        for (int i = 0; i < 512; ++i) CHECK(map.insert(i, i * 3));
        CHECK(map.size() == 512);
        for (int i = 0; i < 512; ++i) CHECK(view.get(i) == i * 3);
        for (int i = 0; i < 512; i += 2) CHECK(map.erase(i));
        for (int i = 512; i < 1024; ++i) CHECK(map.insert(i, i * 3));
        CHECK(map.size() == 768);
        for (int i = 0; i < 1024; ++i) {
            if (i < 512 && i % 2 == 0) CHECK(!view.get(i));
            else CHECK(view.get(i) == i * 3);
        }
    } else if (stage == "churn") {
        for (int round = 0; round < 100; ++round) {
            for (int i = 0; i < 16; ++i) CHECK(map.insert(round * 16 + i, i));
            CHECK(map.size() == 16);
            for (int i = 0; i < 16; ++i) {
                CHECK(view.get(round * 16 + i) == i);
                CHECK(map.erase(round * 16 + i));
            }
            CHECK(map.size() == 0);
            CHECK(!view.get(-1));
            CHECK(!map.erase(-1));
        }
    } else if (stage == "random") {
        std::unordered_map<int, int> expected;
        std::mt19937 rng(20260929);
        for (int step = 0; step < 3000; ++step) {
            int key = static_cast<int>(rng() % 101) - 50;
            int value = static_cast<int>(rng() % 10000);
            switch (rng() % 3) {
            case 0: {
                bool added = expected.insert_or_assign(key, value).second;
                CHECK(map.insert(key, value) == added);
                break;
            }
            case 1:
                CHECK(map.erase(key) == (expected.erase(key) != 0));
                break;
            default:
                CHECK(view.get(key).has_value() == (expected.count(key) != 0));
            }
            CHECK(view.size() == expected.size());
            for (int k = -50; k <= 50; ++k) {
                auto it = expected.find(k);
                if (it == expected.end()) CHECK(!view.get(k));
                else CHECK(view.get(k) == it->second);
            }
        }
    } else {
        throw std::runtime_error("unknown stage: " + stage);
    }
}

int main(int argc, char** argv)
{
    try {
        if (argc != 3) throw std::runtime_error(
            "usage: hash_map_test <chaining|open_addressing> "
            "<empty|basic|collisions|growth|churn|random|generic|rehash|scaling|tombstones>");
        std::string implementation = argv[1];
        if (implementation == "open_addressing" && std::string(argv[2]) == "tombstones")
            test_tombstones();
        else if (implementation == "chaining") run<ChainingHashMap>(argv[2]);
        else if (implementation == "open_addressing") run<OpenAddressingHashMap>(argv[2]);
        else throw std::runtime_error("unknown implementation: " + implementation);
        std::cout << implementation << "/" << argv[2] << ": PASS\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
