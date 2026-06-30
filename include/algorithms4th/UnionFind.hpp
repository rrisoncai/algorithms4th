#pragma once

#include <vector>

class UnionFind {
public:
    UnionFind(int N): id(N), size(N, 1), num_root(N)
    {
        for (int i = 0; i < N; ++i) {
            id[i] = i;
        }
    }

    int find(int p)
    {
        while (id[p] != p) {
            id[p] = id[id[p]]; // path compression
            p = id[p];
        }
        return p;
    }

    void unite(int p, int q)
    {
        int root_p = find(p);
        int root_q = find(q);

        if (root_p == root_q) {
            return;
        }

        if (size[root_p] < size[root_q]) {
            id[root_p] = root_q;
            size[root_q] += size[root_p];
        } else {
            id[root_q] = root_p;
            size[root_p] += size[root_q];
        }
        num_root--;
    }

    bool connected(int p, int q)
    {
        return find(p) == find(q);
    }

    int count()
    {
        return num_root;
    }

private:
    std::vector<int> id;
    std::vector<int> size;
    int num_root;
};
