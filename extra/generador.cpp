// Mision 9

#include "aem.hpp"
using namespace std;

// O(V^2 + E)
int main(int argc, char** argv) {
    if (argc < 8) {
        cerr << "Uso, generador V E dirigido(0|1) pesoMax semilla distintos(0|1) salida.txt\n";
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
