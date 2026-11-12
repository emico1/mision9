// Mision 9

#include "aem.hpp"
using namespace std;

// O(E log E)
int main(int argc, char** argv) {
    if (argc < 3) { cerr << "Uso, subyacente dirigido.txt salida.txt\n"; return 1; }
    try {
        auto [n, a] = aem::leerArcos(argv[1]);
        vector<aem::Arco> s = aem::subyacente(a);
        aem::escribir(argv[2], n, s);
        long long pares = 0;
        set<pair<int, int>> d;
        for (auto e : a) d.insert({e.u, e.v});
        for (auto e : a) if (e.u < e.v && d.count({e.v, e.u})) pares++;
        cout << "arcos dirigidos=" << a.size() << " pares en ambos sentidos=" << pares
             << " arcos no dirigidos=" << s.size() << "\n";
    } catch (exception& e) { cerr << "Error, " << e.what() << "\n"; return 2; }
}
