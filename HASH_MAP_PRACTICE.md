# Hash map 手写练习

先完成 `include/algorithms4th/ChainingHashMap.hpp`，再完成
`include/algorithms4th/OpenAddressingHashMap.hpp`。两个文件只有接口和 TODO，
没有算法实现；数据布局、冲突处理、删除和扩容由你自己设计。
每个版本约 100 行是参考目标，不必为了行数牺牲清晰度。

接口约定：

- `insert(key, value)`：新增返回 true；覆盖返回 false，不增加 size。
- `get(key) const`：返回 `std::optional<Value>` 副本；查不到返回 nullopt。
- `erase(key)`：实际删除返回 true；不存在返回 false。
- `size() const`：当前键值对数量。
- 初始容量默认为 8；传 0 也应可用；插入数量不受初始容量限制。
- Key 支持相等比较，Hash 可默认构造，Value 可复制。

可以用标准库的 vector/list/optional 等作为底层组件，
但不要用 map/unordered_map 实现练习；测试中的 unordered_map 仅作正确性对照。
暂不要求迭代器、复制/移动语义、operator[] 或缩容。

在仓库根目录运行：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target hash_map_test

# 先跑第一阶段
ctest --test-dir build -R '^hash_map_chaining_empty$' --output-on-failure
# chaining 全部阶段
ctest --test-dir build -R '^hash_map_chaining_' --output-on-failure
# 再练 open addressing
ctest --test-dir build -R '^hash_map_open_addressing_' --output-on-failure
```

每次修改头文件后重新 build。建议依次完成：
empty → basic → collisions → growth → churn → random → generic → rehash → scaling。
测试覆盖空表、极端整数键、更新、强制冲突、删除后查找与更新、大量插入、
反复清空重插、固定种子的随机对照，以及字符串键值。
两种实现执行同一套测试，各阶段独立，CTest 设置了 10 秒超时以捕获死循环。

两个实现均为 TODO 的初始状态应当编译成功、18 项练习测试失败，并打印 TODO。
实现之前的失败是预期结果；不要通过修改测试或标记预期失败来消除它。
新增进阶阶段：

- `rehash`：从 1、3、8 的初始容量开始，反复插入、更新、删除和重插，
  每次插入后对照标准库检查所有已有元素，检测重新分桶时的数据丢失。
- `scaling`：分别插入 1024、4096 个键，输出插入和查询耗时（微秒），
  再检查命中及未命中查询平均不超过 16 次 key 相等比较。
  使用分布良好的哈希；固定 8 个桶的链表实现会超出这个预算。
  这是进阶性能要求，失败不代表基础增删查逻辑错误。

查看成功测试的计时输出要加 `-V`：

```bash
ctest --test-dir build -R '^hash_map_chaining_(rehash|scaling)$' -V
```

耗时包含检查和计数开销，仅供观察，不用于断言；比较次数与机器速度无关。
这些测试能暴露固定少量桶导致的长链，但不能证明实际扩容、渐进复杂度或具体结构。
例如预分配很大的表也可能通过，仍需结合自己的代码检查。
