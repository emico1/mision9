// Mision 9

#pragma once
#include <bits/stdc++.h>

namespace aem {

using std::vector;
using std::pair;

struct Arco { int u, v; long long w; };

struct Grafo {
    int n = 0;
    bool dirigido = false;
    vector<Arco> arcos;
    vector<vector<pair<int, long long>>> ady;
    vector<vector<long long>> mat;   // solo se llena si se pide, -1 es sin arco
};

struct Contadores {
    long long comparaciones = 0;   // Prim matriz, celdas revisadas
    long long inserciones = 0;     // heap
    long long extracciones = 0;    // heap
    long long obsoletas = 0;       // Prim heap, entradas viejas que se tiran
    long long maxHeap = 0;         // tamano maximo que alcanzo el heap
    long long revisadas = 0;       // Kruskal, arcos que se sacaron del orden
    long long descartadas = 0;     // Kruskal, arcos que cerraban ciclo
    long long finds = 0;           // Union-Find, llamadas a find
    long long saltos = 0;          // Union-Find, pasos de un nodo a su padre
    long long profMax = 0;         // Union-Find, camino mas largo que recorrio un find
    long long reetiquetas = 0;     // Kruskal componentes, celdas reescritas al fusionar
    long long barridos = 0;        // Kruskal componentes, celdas leidas al fusionar
};

struct Resultado {
    long long costo = 0;
    vector<Arco> arcos;
    Contadores c;
};

// Entero uniforme por rechazo, no usa uniform_int_distribution porque cambia entre compiladores
// O(1) esperado
inline uint32_t acotado(std::mt19937& gen, uint32_t n) {
    uint32_t limite = UINT32_MAX - UINT32_MAX % n;
    uint32_t x;
    do { x = gen(); } while (x >= limite);
    return x % n;
}

// O(n)
template <class T>
void mezclar(vector<T>& a, std::mt19937& gen) {
    for (size_t i = a.size(); i > 1; i--) std::swap(a[i - 1], a[acotado(gen, (uint32_t)i)]);
}

// Primero arma un arbol aleatorio para que el grafo sea conexo y luego completa con pares al azar sin repetir.
// O(V^2)
inline vector<Arco> generarNoDirigido(int V, long long E, int pesoMax, uint32_t semilla, bool distintos) {
    long long maximo = 1LL * V * (V - 1) / 2;
    if (V < 2 || E < V - 1 || E > maximo) throw std::runtime_error("E fuera de rango para un grafo simple conexo");
    std::mt19937 gen(semilla);
    vector<char> usado(1LL * V * V, 0);
    vector<Arco> a;
    a.reserve(E);
    vector<int> p(V);
    std::iota(p.begin(), p.end(), 0);
    mezclar(p, gen);
    for (int i = 1; i < V; i++) {
        int u = p[i], v = p[acotado(gen, i)];
        if (u > v) std::swap(u, v);
        usado[1LL * u * V + v] = 1;
        a.push_back({u, v, 0});
    }
    long long faltan = E - (V - 1);
    if (faltan * 3 < maximo) {
        while (faltan > 0) {
            int u = acotado(gen, V), v = acotado(gen, V);
            if (u == v) continue;
            if (u > v) std::swap(u, v);
            if (usado[1LL * u * V + v]) continue;
            usado[1LL * u * V + v] = 1;
            a.push_back({u, v, 0});
            faltan--;
        }
    } else {
        vector<pair<int, int>> libres;
        libres.reserve(maximo - (V - 1));
        for (int u = 0; u < V; u++)
            for (int v = u + 1; v < V; v++)
                if (!usado[1LL * u * V + v]) libres.push_back({u, v});
        for (long long i = 0; i < faltan; i++) {
            long long j = i + acotado(gen, (uint32_t)(libres.size() - i));
            std::swap(libres[i], libres[j]);
            a.push_back({libres[i].first, libres[i].second, 0});
        }
    }
    if (distintos) {
        vector<long long> pesos(E);
        std::iota(pesos.begin(), pesos.end(), 1);
        mezclar(pesos, gen);
        for (long long i = 0; i < E; i++) a[i].w = pesos[i];
    } else {
        for (auto& e : a) e.w = 1 + acotado(gen, pesoMax);
    }
    mezclar(a, gen);
    return a;
}

// Grafo dirigido fuertemente conexo, un ciclo que pasa por todos los vertices mas arcos al azar
// O(V^2)
inline vector<Arco> generarDirigido(int V, long long E, int pesoMax, uint32_t semilla) {
    long long maximo = 1LL * V * (V - 1);
    if (V < 2 || E < V || E > maximo) throw std::runtime_error("E fuera de rango para un digrafo fuertemente conexo");
    std::mt19937 gen(semilla);
    vector<char> usado(1LL * V * V, 0);
    vector<Arco> a;
    a.reserve(E);
    vector<int> p(V);
    std::iota(p.begin(), p.end(), 0);
    mezclar(p, gen);
    for (int i = 0; i < V; i++) {
        int u = p[i], v = p[(i + 1) % V];
        usado[1LL * u * V + v] = 1;
        a.push_back({u, v, 0});
    }
    vector<pair<int, int>> libres;
    for (int u = 0; u < V; u++)
        for (int v = 0; v < V; v++)
            if (u != v && !usado[1LL * u * V + v]) libres.push_back({u, v});
    long long faltan = E - V;
    for (long long i = 0; i < faltan; i++) {
        long long j = i + acotado(gen, (uint32_t)(libres.size() - i));
        std::swap(libres[i], libres[j]);
        a.push_back({libres[i].first, libres[i].second, 0});
    }
    for (auto& e : a) e.w = 1 + acotado(gen, pesoMax);
    mezclar(a, gen);
    return a;
}

// Grafo no dirigido subyacente, cada par conserva el arco mas barato de los dos sentidos
// O(E log E)
inline vector<Arco> subyacente(const vector<Arco>& dirigidos) {
    std::map<pair<int, int>, long long> mejor;
    for (auto e : dirigidos) {
        pair<int, int> k = {std::min(e.u, e.v), std::max(e.u, e.v)};
        auto it = mejor.find(k);
        if (it == mejor.end() || e.w < it->second) mejor[k] = e.w;
    }
    vector<Arco> a;
    for (auto& [k, w] : mejor) a.push_back({k.first, k.second, w});
    return a;
}

// O(E log E), mas O(V^2) si se pide la matriz
inline Grafo construir(int n, const vector<Arco>& arcos, bool conMatriz, bool dirigido = false) {
    Grafo g;
    g.n = n;
    g.dirigido = dirigido;
    g.ady.resize(n);
    if (conMatriz) g.mat.assign(n, vector<long long>(n, -1));
    std::set<pair<int, int>> vistos;
    for (auto e : arcos) {
        if (e.u < 0 || e.v < 0 || e.u >= n || e.v >= n || e.u == e.v || e.w <= 0) throw std::runtime_error("Arista invalida");
        if (!dirigido && e.u > e.v) std::swap(e.u, e.v);
        if (!vistos.insert({e.u, e.v}).second) throw std::runtime_error("Arista repetida");
        g.arcos.push_back(e);
        g.ady[e.u].push_back({e.v, e.w});
        if (!dirigido) g.ady[e.v].push_back({e.u, e.w});
        if (conMatriz) {
            g.mat[e.u][e.v] = e.w;
            if (!dirigido) g.mat[e.v][e.u] = e.w;
        }
    }
    return g;
}

// O(E)
inline pair<int, vector<Arco>> leerArcos(const std::string& ruta) {
    std::ifstream in(ruta);
    if (!in) throw std::runtime_error("No se pudo abrir " + ruta);
    int n; long long m;
    if (!(in >> n >> m) || n < 1 || m < 0) throw std::runtime_error("Encabezado invalido");
    vector<Arco> a(m);
    for (auto& e : a) if (!(in >> e.u >> e.v >> e.w)) throw std::runtime_error("Arista incompleta");
    return {n, a};
}

// O(E)
inline void escribir(const std::string& ruta, int n, const vector<Arco>& a) {
    std::ofstream out(ruta);
    out << n << " " << a.size() << "\n";
    for (auto e : a) out << e.u << " " << e.v << " " << e.w << "\n";
}

// Version 1, Prim con arreglos, igual al programa del profesor pero con contadores
// O(V^2)
template <bool C = false>
Resultado primMatriz(const Grafo& g, int inicio) {
    int n = g.n;
    vector<bool> usado(n, false);
    vector<long long> best(n, LLONG_MAX);
    vector<int> padre(n, -1);
    best[inicio] = 0;
    Resultado r;
    for (int paso = 0; paso < n; paso++) {
        int u = -1;
        for (int v = 0; v < n; v++)
            if (!usado[v] && (u == -1 || best[v] < best[u])) u = v;
        if constexpr (C) r.c.comparaciones += n;
        if (u == -1 || best[u] == LLONG_MAX) throw std::runtime_error("Grafo desconectado");
        usado[u] = true;
        if (padre[u] != -1) { r.costo += best[u]; r.arcos.push_back({padre[u], u, best[u]}); }
        for (int v = 0; v < n; v++)
            if (!usado[v] && g.mat[u][v] >= 0 && g.mat[u][v] < best[v]) { best[v] = g.mat[u][v]; padre[v] = u; }
        if constexpr (C) r.c.comparaciones += n;
    }
    return r;
}

// Version 3, Prim con heap de entradas repetidas, con contadores
// O(E log V)
template <bool C = false>
Resultado primHeap(const Grafo& g, int inicio) {
    using Entrada = std::tuple<long long, int, int>;
    std::priority_queue<Entrada, vector<Entrada>, std::greater<Entrada>> pq;
    vector<bool> usado(g.n, false);
    pq.push({0, inicio, -1});
    Resultado r;
    int cuenta = 0;
    if constexpr (C) { r.c.inserciones = 1; r.c.maxHeap = 1; }
    while (!pq.empty() && cuenta < g.n - 1) {
        auto [w, u, p] = pq.top(); pq.pop();
        if constexpr (C) r.c.extracciones++;
        if (usado[u]) { if constexpr (C) r.c.obsoletas++; continue; }
        usado[u] = true;
        if (p != -1) { r.costo += w; cuenta++; r.arcos.push_back({p, u, w}); }
        for (auto [v, c] : g.ady[u]) if (!usado[v]) {
            pq.push({c, v, u});
            if constexpr (C) { r.c.inserciones++; r.c.maxHeap = std::max<long long>(r.c.maxHeap, pq.size()); }
        }
    }
    if (cuenta != g.n - 1) throw std::runtime_error("Grafo desconectado");
    return r;
}

// Version 2, Kruskal con arreglo de componentes, con contadores
// O(E log E + V^2)
template <bool C = false>
Resultado kruskalComponentes(const Grafo& g) {
    vector<Arco> a = g.arcos;
    std::sort(a.begin(), a.end(), [](auto x, auto y) { return std::tie(x.w, x.u, x.v) < std::tie(y.w, y.u, y.v); });
    vector<int> comp(g.n);
    std::iota(comp.begin(), comp.end(), 0);
    Resultado r;
    int cuenta = 0;
    for (auto e : a) {
        if constexpr (C) r.c.revisadas++;
        int c1 = comp[e.u], c2 = comp[e.v];
        if (c1 == c2) { if constexpr (C) r.c.descartadas++; continue; }
        r.costo += e.w; cuenta++; r.arcos.push_back(e);
        for (int& x : comp) if (x == c2) { x = c1; if constexpr (C) r.c.reetiquetas++; }
        if constexpr (C) r.c.barridos += g.n;
        if (cuenta == g.n - 1) break;
    }
    if (cuenta != g.n - 1) throw std::runtime_error("Grafo desconectado");
    return r;
}

// Union-Find con union por tamano, la compresion es opcional
template <bool C>
struct DSU {
    vector<int> parent, sz;
    bool compress;
    Contadores* c;
    // O(V)
    DSU(int n, bool comp, Contadores* cc) : parent(n), sz(n, 1), compress(comp), c(cc) { std::iota(parent.begin(), parent.end(), 0); }
    // O(log V) sin compresion, O(alfa(V)) amortizado con compresion
    int find(int x, int prof = 0) {
        if (parent[x] == x) {
            if constexpr (C) c->profMax = std::max<long long>(c->profMax, prof);
            return x;
        }
        if constexpr (C) c->saltos++;
        if (compress) return parent[x] = find(parent[x], prof + 1);
        return find(parent[x], prof + 1);
    }
    // O(costo de find)
    bool unite(int a, int b) {
        if constexpr (C) c->finds += 2;
        a = find(a); b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) std::swap(a, b);
        parent[b] = a; sz[a] += sz[b];
        return true;
    }
};

// Version 4, Kruskal con Union-Find, con contadores
// O(E log E + E alfa(V))
template <bool C = false>
Resultado kruskalUF(const Grafo& g, bool compress = true) {
    vector<Arco> a = g.arcos;
    std::sort(a.begin(), a.end(), [](auto x, auto y) { return std::tie(x.w, x.u, x.v) < std::tie(y.w, y.u, y.v); });
    Resultado r;
    DSU<C> d(g.n, compress, &r.c);
    int cuenta = 0;
    for (auto e : a) {
        if constexpr (C) r.c.revisadas++;
        if (d.unite(e.u, e.v)) {
            r.costo += e.w; cuenta++; r.arcos.push_back(e);
            if (cuenta == g.n - 1) break;
        } else if constexpr (C) r.c.descartadas++;
    }
    if (cuenta != g.n - 1) throw std::runtime_error("Grafo desconectado");
    return r;
}

// Edmonds para grafos dirigidos. Cada vertice toma su arco de entrada mas barato y si se forma un ciclo lo contrae y repite.
// O(V E)
inline long long edmonds(int n, vector<Arco> a, int raiz) {
    const long long INF = LLONG_MAX / 4;
    long long total = 0;
    while (true) {
        vector<long long> entrada(n, INF);
        vector<int> pre(n, -1);
        for (auto& e : a)
            if (e.u != e.v && e.w < entrada[e.v]) { entrada[e.v] = e.w; pre[e.v] = e.u; }
        for (int v = 0; v < n; v++)
            if (v != raiz && entrada[v] == INF) return -1;
        int ciclos = 0;
        vector<int> id(n, -1), visto(n, -1);
        entrada[raiz] = 0;
        for (int v = 0; v < n; v++) {
            total += entrada[v];
            int x = v;
            while (visto[x] != v && id[x] == -1 && x != raiz) { visto[x] = v; x = pre[x]; }
            if (x != raiz && id[x] == -1) {
                for (int y = pre[x]; y != x; y = pre[y]) id[y] = ciclos;
                id[x] = ciclos++;
            }
        }
        if (ciclos == 0) break;
        for (int v = 0; v < n; v++) if (id[v] == -1) id[v] = ciclos++;
        for (auto& e : a) {
            int v = e.v;
            e.u = id[e.u]; e.v = id[e.v];
            if (e.u != e.v) e.w -= entrada[v];
        }
        n = ciclos;
        raiz = id[raiz];
    }
    return total;
}

// Prim aplicado tal cual a un dirigido, da una arborescencia valida pero no siempre la optima.
// O(E log V)
inline long long primDirigido(int n, const vector<Arco>& a, int raiz) {
    vector<vector<pair<int, long long>>> sal(n);
    for (auto e : a) sal[e.u].push_back({e.v, e.w});
    using Entrada = std::tuple<long long, int>;
    std::priority_queue<Entrada, vector<Entrada>, std::greater<Entrada>> pq;
    vector<bool> usado(n, false);
    pq.push({0, raiz});
    long long total = 0;
    int cuenta = 0;
    while (!pq.empty()) {
        auto [w, u] = pq.top(); pq.pop();
        if (usado[u]) continue;
        usado[u] = true; total += w; cuenta++;
        for (auto [v, c] : sal[u]) if (!usado[v]) pq.push({c, v});
    }
    return cuenta == n ? total : -1;
}

// Verificador. Revisa que sea arbol de expansion y que ningun arco de fuera sea mas barato que el mas caro del camino del arbol entre sus extremos.
// O(E log V)
inline std::string verificar(const Grafo& g, const vector<Arco>& arbol, long long* costoOut = nullptr) {
    int n = g.n;
    if ((int)arbol.size() != n - 1) return "no tiene n-1 arcos";
    std::map<pair<int, int>, long long> peso;
    for (auto e : g.arcos) peso[{std::min(e.u, e.v), std::max(e.u, e.v)}] = e.w;
    vector<int> rep(n);
    std::iota(rep.begin(), rep.end(), 0);
    std::function<int(int)> raiz = [&](int x) { return rep[x] == x ? x : rep[x] = raiz(rep[x]); };
    vector<vector<pair<int, long long>>> t(n);
    long long costo = 0;
    for (auto e : arbol) {
        auto it = peso.find({std::min(e.u, e.v), std::max(e.u, e.v)});
        if (it == peso.end() || it->second != e.w) return "usa un arco que no existe o con otro peso";
        int a = raiz(e.u), b = raiz(e.v);
        if (a == b) return "forma un ciclo";
        rep[a] = b;
        t[e.u].push_back({e.v, e.w}); t[e.v].push_back({e.u, e.w});
        costo += e.w;
    }
    if (costoOut) *costoOut = costo;
    int LOG = 1;
    while ((1 << LOG) < n) LOG++;
    vector<vector<int>> up(LOG, vector<int>(n, 0));
    vector<vector<long long>> mx(LOG, vector<long long>(n, 0));
    vector<int> prof(n, -1);
    vector<int> pila = {0};
    prof[0] = 0;
    while (!pila.empty()) {
        int u = pila.back(); pila.pop_back();
        for (auto [v, w] : t[u]) if (prof[v] == -1) { prof[v] = prof[u] + 1; up[0][v] = u; mx[0][v] = w; pila.push_back(v); }
    }
    for (int k = 1; k < LOG; k++)
        for (int v = 0; v < n; v++) { up[k][v] = up[k - 1][up[k - 1][v]]; mx[k][v] = std::max(mx[k - 1][v], mx[k - 1][up[k - 1][v]]); }
    auto maxCamino = [&](int a, int b) {
        long long m = 0;
        if (prof[a] < prof[b]) std::swap(a, b);
        for (int k = LOG - 1; k >= 0; k--) if (prof[a] - (1 << k) >= prof[b]) { m = std::max(m, mx[k][a]); a = up[k][a]; }
        if (a == b) return m;
        for (int k = LOG - 1; k >= 0; k--) if (up[k][a] != up[k][b]) { m = std::max({m, mx[k][a], mx[k][b]}); a = up[k][a]; b = up[k][b]; }
        return std::max({m, mx[0][a], mx[0][b]});
    };
    for (auto e : g.arcos) if (e.w < maxCamino(e.u, e.v)) return "no es minimo, existe un arco que mejora el arbol";
    return "ok";
}

// O(V log V)
inline vector<pair<int, int>> canonico(const vector<Arco>& a) {
    vector<pair<int, int>> k;
    for (auto e : a) k.push_back({std::min(e.u, e.v), std::max(e.u, e.v)});
    std::sort(k.begin(), k.end());
    return k;
}

}  // namespace aem
