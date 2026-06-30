#pragma once
#include <vector>
class UnionFind {
    public:
    UnionFind(int N): data(N), component_count(N)
    {
        for (int i = 0; i < N; ++i) {
            data[i] = i;
        }
    }
    int find(int p) {
        while (data[p] != p) {
            data[p] = data[data[p]];
            p = data[p];
        }
        return p;
    }
    void unite(int p, int q) {
        int root_p = find(p);
        int root_q = find(q);

        if (root_p == root_q) {
            return;
        }
        data[root_q] = root_p;
        component_count--;
    }

    bool connected(int p, int q) {
        return find(p) == find(q);
    }

    int count() {
        return component_count;
    }

    private:
    std::vector<int> data;
    int component_count;
};
