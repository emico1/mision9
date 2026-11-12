// Mision 9

#include "aem.hpp"
using namespace std;
using Reloj = chrono::steady_clock;

volatile long long sumidero = 0;

// O(reps * costo de f)
template <class F>
double medir(F f, int reps) {
    vector<double> t;
    for (int k = 0; k < reps; k++) {
        auto a = Reloj::now();
        aem::Resultado r = f();
        auto b = Reloj::now();
        sumidero = sumidero + r.costo;
        t.push_back(chrono::duration<double, milli>(b - a).count());
    }
    sort(t.begin(), t.end());
    return t[t.size() / 2];
}

struct Escenario { string nombre, archivo; };

const vector<string> NOMBRES = {"V1 Prim arreglos", "V2 Kruskal componentes", "V3 Prim heap", "V4 Kruskal Union-Find"};

// O(costo del algoritmo)
template <bool C>
aem::Resultado correr(int k, const aem::Grafo& g, int inicio) {
    switch (k) {
        case 0: return aem::primMatriz<C>(g, inicio);
        case 1: return aem::kruskalComponentes<C>(g);
        case 2: return aem::primHeap<C>(g, inicio);
        default: return aem::kruskalUF<C>(g, true);
    }
}

// Cuenta las operaciones de cada version en los escenarios, sirve para revisar las complejidades
// O(suma de costos)
void contadores(const vector<Escenario>& esc, const string& dir) {
    ofstream c(dir + "/contadores.csv");
    c << "escenario,version,costo,comparaciones,inserciones,extracciones,obsoletas,max_heap,revisadas,descartadas,finds,saltos,prof_max,reetiquetas,barridos\n";
    for (auto& e : esc) {
        auto [n, a] = aem::leerArcos(e.archivo);
        aem::Grafo g = aem::construir(n, a, true);
        for (int k = 0; k < (int)NOMBRES.size(); k++) {
            aem::Resultado r = correr<true>(k, g, n - 1);
            auto& q = r.c;
            c << e.nombre << "," << NOMBRES[k] << "," << r.costo << "," << q.comparaciones << "," << q.inserciones << ","
              << q.extracciones << "," << q.obsoletas << "," << q.maxHeap << "," << q.revisadas << "," << q.descartadas << ","
              << q.finds << "," << q.saltos << "," << q.profMax << "," << q.reetiquetas << "," << q.barridos << "\n";
        }
    }
}

// Barrido de densidad para ver donde Prim con arreglos alcanza a Prim con heap
// O(suma de V^2 + E log E por punto)
void barridoDensidad(const string& dir) {
    ofstream o(dir + "/densidad.csv");
    o << "V,E,densidad_pct,version,mediana_ms\n";
    vector<pair<int, vector<long long>>> planes = {
        {1000, {3000, 6000, 12000, 25000, 50000, 100000, 200000, 350000, 499500}},
        {2000, {6000, 12000, 25000, 50000, 100000, 200000, 400000, 800000, 1400000, 1999000}}};
    uint32_t semilla = 500;
    for (auto& [V, lista] : planes) {
        for (long long E : lista) {
            aem::Grafo g = aem::construir(V, aem::generarNoDirigido(V, E, 1000000, semilla++, false), true);
            double dens = 100.0 * E / (1.0 * V * (V - 1) / 2);
            long long ref = -1;
            for (int k = 0; k < (int)NOMBRES.size(); k++) {
                long long costo = 0;
                double med = medir([&] { aem::Resultado r = correr<false>(k, g, 0); costo = r.costo; return r; }, 7);
                if (ref == -1) ref = costo;
                if (costo != ref) throw runtime_error("costos distintos en el barrido");
                o << V << "," << E << "," << fixed << setprecision(2) << dens << "," << NOMBRES[k] << "," << setprecision(4) << med << "\n";
            }
            cerr << "densidad V=" << V << " E=" << E << " lista\n";
        }
    }
}

// Corre Prim desde cada vertice y cuenta costos y arboles distintos, con los pesos originales y con pesos todos distintos
// O(V * costo de Prim)
void verticeInicial(const vector<Escenario>& esc, const string& dir) {
    ofstream o(dir + "/vertice_inicial.csv");
    o << "grafo,pesos,V,E,version,inicios,costos_distintos,arboles_distintos,inicios_igual_a_kruskal,pesos_repetidos\n";
    auto estudiar = [&](const string& nombre, const string& tipo, int n, const vector<aem::Arco>& a) {
        aem::Grafo g = aem::construir(n, a, true);
        auto kk = aem::canonico(aem::kruskalUF(g).arcos);
        map<long long, int> veces;
        for (auto e : g.arcos) veces[e.w]++;
        long long repetidos = 0;
        for (auto& [w, k] : veces) if (k > 1) repetidos += k;
        for (int ver = 0; ver < 2; ver++) {
            set<long long> costos;
            set<vector<pair<int, int>>> arboles;
            int iguales = 0;
            for (int s = 0; s < n; s++) {
                aem::Resultado r = ver == 0 ? aem::primMatriz(g, s) : aem::primHeap(g, s);
                costos.insert(r.costo);
                auto k = aem::canonico(r.arcos);
                arboles.insert(k);
                if (k == kk) iguales++;
            }
            o << nombre << "," << tipo << "," << n << "," << g.arcos.size() << "," << (ver == 0 ? "V1 Prim arreglos" : "V3 Prim heap") << ","
              << n << "," << costos.size() << "," << arboles.size() << "," << iguales << "," << repetidos << "\n";
        }
    };
    for (auto& e : esc) {
        auto [n, a] = aem::leerArcos(e.archivo);
        estudiar(e.nombre, "originales", n, a);
    }
    vector<tuple<string, int, long long, uint32_t>> gemelos = {
        {"E1", 100, 300, 9001}, {"E2", 100, 2500, 9002}, {"E3b", 100, 4500, 9033}, {"E4", 500, 1500, 9004}, {"E5", 500, 62000, 9005}};
    for (auto& [nom, V, E, s] : gemelos) estudiar(nom, "todos distintos", V, aem::generarNoDirigido(V, E, 1000, s, true));
    estudiar("todos iguales", "un solo peso", 30, aem::generarNoDirigido(30, 200, 1, 77, false));
}

// Escenario dirigido y 200 digrafos al azar del mismo tamano, Edmonds contra Prim dirigido
// O(casos * V E)
void dirigido(const string& datos, const string& dir) {
    ofstream o(dir + "/dirigido.csv");
    o << "grafo,V,arcos,edmonds,prim_dirigido,aem_subyacente,brecha_prim_pct\n";
    auto fila = [&](const string& nom, int V, const vector<aem::Arco>& arcs) {
        long long e = aem::edmonds(V, arcs, 0), p = aem::primDirigido(V, arcs, 0);
        aem::Grafo s = aem::construir(V, aem::subyacente(arcs), false);
        long long c = aem::kruskalUF(s).costo;
        o << nom << "," << V << "," << arcs.size() << "," << e << "," << p << "," << c << "," << fixed << setprecision(2)
          << 100.0 * (p - e) / e << "\n";
    };
    auto [n, a] = aem::leerArcos(datos + "/escenario3_100v_4500a_dirigido.txt");
    fila("escenario 3", n, a);
    for (uint32_t s = 1; s <= 200; s++) fila("aleatorio " + to_string(s), 100, aem::generarDirigido(100, 4500, 1000, 70000 + s));
}

// O(total de experimentos)
int main(int argc, char** argv) {
    if (argc < 3) { cerr << "Uso, laboratorio carpeta_datos carpeta_resultados [contadores|inicio|dirigido|densidad]\n"; return 1; }
    string datos = argv[1], dir = argv[2], solo = argc > 3 ? argv[3] : "todo";
    vector<Escenario> esc = {
        {"E1", datos + "/escenario1_100v_300a.txt"},
        {"E2", datos + "/escenario2_100v_2500a.txt"},
        {"E3 subyacente", datos + "/escenario3_subyacente.txt"},
        {"E3b", datos + "/escenario3b_100v_4500a_no_dirigido.txt"},
        {"E4", datos + "/escenario4_500v_1500a.txt"},
        {"E5", datos + "/escenario5_500v_62000a.txt"}};
    try {
        if (solo == "todo" || solo == "contadores") contadores(esc, dir);
        if (solo == "todo" || solo == "inicio") verticeInicial(esc, dir);
        if (solo == "todo" || solo == "dirigido") dirigido(datos, dir);
        if (solo == "todo" || solo == "densidad") barridoDensidad(dir);
    } catch (exception& e) { cerr << "Error, " << e.what() << "\n"; return 2; }
}
