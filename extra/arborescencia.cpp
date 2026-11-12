// Mision 9

#include "aem.hpp"
using namespace std;

// O(V E)
int main(int argc, char** argv) {
    if (argc < 2) { cerr << "Uso, arborescencia dirigido.txt [raiz]\n"; return 1; }
    try {
        auto [n, a] = aem::leerArcos(argv[1]);
        aem::construir(n, a, false, true);
        int raiz = argc > 2 ? stoi(argv[2]) : 0;
        if (raiz < 0 || raiz >= n) throw runtime_error("Raiz invalida");
        auto t0 = chrono::steady_clock::now();
        long long optimo = aem::edmonds(n, a, raiz);
        auto t1 = chrono::steady_clock::now();
        long long voraz = aem::primDirigido(n, a, raiz);
        aem::Grafo s = aem::construir(n, aem::subyacente(a), false);
        long long cota = aem::kruskalUF(s).costo;
        double ms = chrono::duration<double, milli>(t1 - t0).count();
        cout << "n=" << n << " m=" << a.size() << " raiz=" << raiz
             << " edmonds=" << optimo << " prim_dirigido=" << voraz << " aem_subyacente=" << cota
             << " tiempo_edmonds_ms=" << fixed << setprecision(4) << ms << "\n";
    } catch (exception& e) { cerr << "Error, " << e.what() << "\n"; return 2; }
}
