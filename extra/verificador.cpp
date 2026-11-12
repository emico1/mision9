// Mision 9

#include "aem.hpp"
using namespace std;

// O(E log V)
int main(int argc, char** argv) {
    if (argc < 3) { cerr << "Uso, verificador grafo.txt salida.txt\n"; return 1; }
    try {
        auto [n, a] = aem::leerArcos(argv[1]);
        aem::Grafo g = aem::construir(n, a, false);
        ifstream in(argv[2]);
        string encabezado;
        getline(in, encabezado);
        vector<aem::Arco> arbol;
        aem::Arco e;
        while (in >> e.u >> e.v >> e.w) arbol.push_back(e);
        long long costo = 0;
        string veredicto = aem::verificar(g, arbol, &costo);
        cout << veredicto << " costo=" << costo << " arcos=" << arbol.size() << "\n";
        return veredicto == "ok" ? 0 : 3;
    } catch (exception& ex) { cerr << "Error, " << ex.what() << "\n"; return 2; }
}
