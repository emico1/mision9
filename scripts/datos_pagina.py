# Mision 9

import csv
import json
import os

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RES = os.path.join(RAIZ, "resultados")


def leer(nombre):
    with open(os.path.join(RES, nombre), newline="") as f:
        return list(csv.DictReader(f))


datos = {
    "oficiales": [[r["escenario"], r["tipo"], r["version"], float(r["mediana_ms"])] for r in leer("tiempos_oficiales.csv")],
    "densidad": [[int(r["V"]), int(r["E"]), float(r["densidad_pct"]), r["version"], float(r["mediana_ms"])] for r in leer("densidad.csv")],
    "descartes": [[r["escenario"], r["version"], int(r["arcos_totales"]), int(r["revisadas"]), int(r["descartadas"])] for r in leer("kruskal_descartes.csv")],
    "inicio": [[r["grafo"], r["pesos"], r["version"], int(r["inicios"]), int(r["costos_distintos"]), int(r["arboles_distintos"])] for r in leer("vertice_inicial.csv")],
    "dirigido": [[r["grafo"], int(r["edmonds"]), int(r["prim_dirigido"]), int(r["aem_subyacente"]), float(r["brecha_prim_pct"])] for r in leer("dirigido.csv")],
    "consistencia": [[r["escenario"], r["version"], int(r["costo"]), r["verificador"], r["arbol"]] for r in leer("consistencia.csv")],
}

with open(os.path.join(RAIZ, "js", "datos.js"), "w") as f:
    f.write("// Mision 9\n")
    f.write("const DATOS = " + json.dumps(datos, ensure_ascii=False) + ";\n")
print("js/datos.js listo")
