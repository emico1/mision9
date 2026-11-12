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

struct Result { long long cost = 0; int count = 0; vector<Edge> edges; long long revisadas = 0, descartadas = 0, saltos = 0, profMax = 0; };

// Modos de la optimizacion, 0 tamano y compresion, 1 solo tamano, 2 sin optimizaciones, 3 solo compresion
const char* NOMBRE_MODO[] = {"tamano_y_compresion", "solo_tamano", "sin_optimizaciones", "solo_compresion"};

// Union-Find, cada conjunto es un arbol y su raiz lo representa
struct DSU {
    vector<int> parent, sz;
    bool porTamano, compress;
    long long saltos = 0, profMax = 0;
    // O(V)
    DSU(int n, bool t, bool c) : parent(n), sz(n, 1), porTamano(t), compress(c) { iota(parent.begin(), parent.end(), 0); }
    // Sube hasta la raiz contando los saltos y con compresion cuelga cada nodo del camino directo de ella
    // O(profundidad), log V con union por tamano, O(alfa(V)) amortizado con las dos
    int find(int x) {
        int r = x;
        long long prof = 0;
        while (parent[r] != r) { r = parent[r]; prof++; }
        saltos += prof;
        profMax = max(profMax, prof);
        if (compress) while (parent[x] != r) { int sig = parent[x]; parent[x] = r; x = sig; }
        return r;
    }
    // Con union por tamano la raiz del conjunto chico se cuelga de la del grande, sin ella la raiz de a se cuelga de la de b. Regresa false si ya estaban unidos.
    // O(costo de find)
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (porTamano) {
            if (sz[a] < sz[b]) swap(a, b);
            parent[b] = a; sz[a] += sz[b];
        } else {
            parent[a] = b; sz[b] += sz[a];
        }
        return true;
    }
};

// Kruskal con Union-Find. Toma los arcos del mas barato al mas caro y descarta el arco si unite dice que ya estaban unidos.
// O(E log E + E alfa(V)) con las dos optimizaciones, O(E log E + E V) sin ninguna
Result solve(const Graph& g, int, int modo = 0) {
    vector<Edge> a = g.edges;
    sort(a.begin(), a.end(), [](auto x, auto y) { return tie(x.w, x.u, x.v) < tie(y.w, y.u, y.v); });
    DSU d(g.n, modo == 0 || modo == 1, modo == 0 || modo == 3);
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
    r.saltos = d.saltos;
    r.profMax = d.profMax;
    return r;
}

// O(V^2 + E log E)
int main(int argc, char** argv) {
    if (argc < 2) { cerr << "Uso: programa archivo.txt [inicio_prim] [modo_union_find_0_a_3] [mostrar_0_o_1]\n"; return 1; }
    try {
        Graph g = load(argv[1]);
        int start = argc > 2 ? stoi(argv[2]) : 0;
        if (start < 0 || start >= g.n) throw runtime_error("Inicio invalido");
        int modo = argc > 3 ? stoi(argv[3]) : 0;
        if (modo < 0 || modo > 3) throw runtime_error("Modo invalido, debe ser 0, 1, 2 o 3");
        bool show = argc > 4 && stoi(argv[4]) == 1;
        auto t0 = chrono::steady_clock::now();
        Result r = solve(g, start, modo);
        auto t1 = chrono::steady_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        cout << "n=" << g.n << " m=" << g.edges.size() << " costo=" << r.cost << " aristas=" << r.count
             << " revisadas=" << r.revisadas << " descartadas=" << r.descartadas
             << " modo=" << NOMBRE_MODO[modo] << " saltos=" << r.saltos << " prof_max=" << r.profMax
             << " tiempo_ms=" << fixed << setprecision(4) << ms << "\n";
        if (show) for (auto e : r.edges) cout << e.u << " " << e.v << " " << e.w << "\n";
    } catch (exception& e) { cerr << "Error: " << e.what() << "\n"; return 2; }
}
