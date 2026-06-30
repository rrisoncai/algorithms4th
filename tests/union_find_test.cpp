#include <algorithms4th/UnionFind.hpp>

#include <cassert>

int main()
{
    UnionFind uf(8);

    assert(uf.count() == 8);
    assert(!uf.connected(0, 1));

    uf.unite(0, 1);
    assert(uf.connected(0, 1));
    assert(uf.count() == 7);

    uf.unite(2, 3);
    uf.unite(4, 5);
    uf.unite(0, 2);
    uf.unite(0, 4);

    assert(uf.connected(1, 5));
    assert(!uf.connected(1, 7));
    assert(uf.count() == 3);

    uf.unite(1, 5);
    assert(uf.count() == 3);

    return 0;
}
