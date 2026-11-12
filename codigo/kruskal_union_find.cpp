// Mision 9

#include <bits/stdc++.h>
using namespace std;

struct Edge { int u, v; long long w; };
struct Graph {
    int n;
    vector<Edge> edges;
    vector<vector<pair<int, long long>>> adj;
    vector<vector<long long>> mat;
};

// O(V^2 + E log E)
Graph load(const string& path) {
    ifstream in(path);
    if (!in) throw runtime_error("No se pudo abrir archivo");
    int n, m;
    if (!(in >> n >> m) || n < 1 || m < 0) throw runtime_error("Encabezado invalido");
    Graph g; g.n = n; g.adj.resize(n); g.mat.assign(n, vector<long long>(n, -1));
    set<pair<int, int>> seen;
    for (int k = 0; k < m; k++) {
        int u, v; long long w;
        if (!(in >> u >> v >> w) || u < 0 || v < 0 || u >= n || v >= n || u == v || w <= 0)
            throw runtime_error("Arista invalida");
        if (u > v) swap(u, v);
        if (!seen.insert({u, v}).second) throw runtime_error("Arista repetida");
        g.edges.push_back({u, v, w});
        g.adj[u].push_back({v, w}); g.adj[v].push_back({u, w});
        g.mat[u][v] = g.mat[v][u] = w;
    }
    return g;
}

struct Result { long long cost = 0; int count = 0; vector<Edge> edges; long long revisadas = 0, descartadas = 0; };

// Union-Find, cada conjunto es un arbol y su raiz lo representa
struct DSU {
    vector<int> parent, sz;
    bool compress;
    // O(V)
    DSU(int n, bool c) : parent(n), sz(n, 1), compress(c) { iota(parent.begin(), parent.end(), 0); }
    // Sube hasta la raiz y con compresion cuelga cada nodo del camino directo de ella
    // O(log V) sin compresion, O(alfa(V)) amortizado con compresion
    int find(int x) {
        if (parent[x] == x) return x;
        if (compress) return parent[x] = find(parent[x]);
        return find(parent[x]);
    }
    // Union por tamano, la raiz del conjunto chico se cuelga de la del grande. Regresa false si ya estaban unidos.
    // O(costo de find)
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) swap(a, b);
        parent[b] = a; sz[a] += sz[b];
        return true;
    }
};

// Kruskal con Union-Find. Toma los arcos del mas barato al mas caro y descarta el arco si unite dice que ya estaban unidos.
// O(E log E + E alfa(V))
Result solve(const Graph& g, int, bool compress = true) {
    vector<Edge> a = g.edges;
    sort(a.begin(), a.end(), [](auto x, auto y) { return tie(x.w, x.u, x.v) < tie(y.w, y.u, y.v); });
    DSU d(g.n, compress);
    Result r;
    for (auto e : a) {
        r.revisadas++;
        if (d.unite(e.u, e.v)) {
            r.cost += e.w; r.count++; r.edges.push_back(e);
            if (r.count == g.n - 1) break;
        } else {
            r.descartadas++;
        }
    }
    if (r.count != g.n - 1) throw runtime_error("Grafo desconectado");
    return r;
}

// O(V^2 + E log E)
int main(int argc, char** argv) {
    if (argc < 2) { cerr << "Uso: programa archivo.txt [inicio_prim] [sin_compresion_0_o_1] [mostrar_0_o_1]\n"; return 1; }
    try {
        Graph g = load(argv[1]);
        int start = argc > 2 ? stoi(argv[2]) : 0;
        if (start < 0 || start >= g.n) throw runtime_error("Inicio invalido");
        bool noComp = argc > 3 && stoi(argv[3]) == 1;
        bool show = argc > 4 && stoi(argv[4]) == 1;
        auto t0 = chrono::steady_clock::now();
        Result r = solve(g, start, !noComp);
        auto t1 = chrono::steady_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        cout << "n=" << g.n << " m=" << g.edges.size() << " costo=" << r.cost << " aristas=" << r.count
             << " revisadas=" << r.revisadas << " descartadas=" << r.descartadas
             << " compresion=" << (noComp ? 0 : 1)
             << " tiempo_ms=" << fixed << setprecision(4) << ms << "\n";
        if (show) for (auto e : r.edges) cout << e.u << " " << e.v << " " << e.w << "\n";
    } catch (exception& e) { cerr << "Error: " << e.what() << "\n"; return 2; }
}
