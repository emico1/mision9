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

struct Result { long long cost = 0; int count = 0; vector<Edge> edges; };

// Prim con arreglos. La eleccion voraz es buscar el menor best fuera del arbol y used evita los ciclos.
// O(V^2)
Result solve(const Graph& g, int start) {
    int n = g.n;
    vector<bool> used(n, false);
    vector<long long> best(n, LLONG_MAX);
    vector<int> parent(n, -1);
    best[start] = 0;
    Result r;
    for (int step = 0; step < n; step++) {
        int u = -1;
        for (int v = 0; v < n; v++)
            if (!used[v] && (u == -1 || best[v] < best[u])) u = v;
        if (u == -1 || best[u] == LLONG_MAX) throw runtime_error("Grafo desconectado");
        used[u] = true;
        if (parent[u] != -1) { r.cost += best[u]; r.count++; r.edges.push_back({parent[u], u, best[u]}); }
        for (int v = 0; v < n; v++)
            if (!used[v] && g.mat[u][v] >= 0 && g.mat[u][v] < best[v]) {
                best[v] = g.mat[u][v];
                parent[v] = u;
            }
    }
    return r;
}

// O(V^2 + E log E)
int main(int argc, char** argv) {
    if (argc < 2) { cerr << "Uso: programa archivo.txt [inicio_prim] [sin_compresion_0_o_1] [mostrar_0_o_1]\n"; return 1; }
    try {
        Graph g = load(argv[1]);
        int start = argc > 2 ? stoi(argv[2]) : g.n - 1;
        if (start < 0 || start >= g.n) throw runtime_error("Inicio invalido");
        bool show = argc > 4 && stoi(argv[4]) == 1;
        auto t0 = chrono::steady_clock::now();
        Result r = solve(g, start);
        auto t1 = chrono::steady_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        cout << "n=" << g.n << " m=" << g.edges.size() << " inicio=" << start << " costo=" << r.cost
             << " aristas=" << r.count << " tiempo_ms=" << fixed << setprecision(4) << ms << "\n";
        if (show) for (auto e : r.edges) cout << e.u << " " << e.v << " " << e.w << "\n";
    } catch (exception& e) { cerr << "Error: " << e.what() << "\n"; return 2; }
}
