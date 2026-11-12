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

// Kruskal con etiquetas. Toma los arcos del mas barato al mas caro y descarta el arco si sus extremos tienen la misma etiqueta.
// O(E log E + V^2)
Result solve(const Graph& g, int) {
    vector<Edge> a = g.edges;
    sort(a.begin(), a.end(), [](auto x, auto y) { return tie(x.w, x.u, x.v) < tie(y.w, y.u, y.v); });
    vector<int> comp(g.n);
    iota(comp.begin(), comp.end(), 0);
    Result r;
    for (auto e : a) {
        r.revisadas++;
        int c1 = comp[e.u], c2 = comp[e.v];
        if (c1 == c2) { r.descartadas++; continue; }
        r.cost += e.w; r.count++; r.edges.push_back(e);
        for (int& x : comp) if (x == c2) x = c1;
        if (r.count == g.n - 1) break;
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
        bool show = argc > 4 && stoi(argv[4]) == 1;
        auto t0 = chrono::steady_clock::now();
        Result r = solve(g, start);
        auto t1 = chrono::steady_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        cout << "n=" << g.n << " m=" << g.edges.size() << " costo=" << r.cost << " aristas=" << r.count
             << " revisadas=" << r.revisadas << " descartadas=" << r.descartadas
             << " tiempo_ms=" << fixed << setprecision(4) << ms << "\n";
        if (show) for (auto e : r.edges) cout << e.u << " " << e.v << " " << e.w << "\n";
    } catch (exception& e) { cerr << "Error: " << e.what() << "\n"; return 2; }
}
