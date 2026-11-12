// Mision 9

#include "aem.hpp"
using namespace std;

// Peor caso de Union-Find sin optimizaciones, una cadena 0-1-2-... con arcos (0, i) que obligan a find(0) a recorrerla completa
// O(V)
vector<aem::Arco> cadenaAdversaria(int V) {
    vector<aem::Arco> a;
    for (int i = 1; i < V; i++) {
        a.push_back({i - 1, i, 2LL * i});
        if (i >= 2) a.push_back({0, i, 2LL * i + 1});
    }
    return a;
}

// O(V^2 + E)
int main(int argc, char** argv) {
    if (argc == 4 && string(argv[1]) == "cadena") {
        int V = stoi(argv[2]);
        vector<aem::Arco> a = cadenaAdversaria(V);
        aem::escribir(argv[3], V, a);
        cout << "escrito " << argv[3] << " V=" << V << " E=" << a.size() << " cadena adversaria\n";
        return 0;
    }
    if (argc < 8) {
        cerr << "Uso, generador V E dirigido(0|1) pesoMax semilla distintos(0|1) salida.txt\n";
        cerr << "     generador cadena V salida.txt\n";
        return 1;
    }
    try {
        int V = stoi(argv[1]);
        long long E = stoll(argv[2]);
        bool dirigido = stoi(argv[3]) == 1;
        int pesoMax = stoi(argv[4]);
        uint32_t semilla = (uint32_t)stoul(argv[5]);
        bool distintos = stoi(argv[6]) == 1;
        string salida = argv[7];
        vector<aem::Arco> a = dirigido ? aem::generarDirigido(V, E, pesoMax, semilla)
                                       : aem::generarNoDirigido(V, E, pesoMax, semilla, distintos);
        aem::escribir(salida, V, a);
        cout << "escrito " << salida << " V=" << V << " E=" << a.size() << (dirigido ? " dirigido" : " no dirigido") << "\n";
    } catch (exception& e) { cerr << "Error, " << e.what() << "\n"; return 2; }
}
