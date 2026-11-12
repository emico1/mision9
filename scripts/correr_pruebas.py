# Mision 9

import csv
import os
import platform
import statistics
import subprocess
import sys

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = os.path.join(RAIZ, "bin")
DATOS = os.path.join(RAIZ, "datos")
RES = os.path.join(RAIZ, "resultados")
EXE = ".exe" if platform.system() == "Windows" else ""

PROFESOR = [
    ("V1 Prim arreglos", "prim_matriz", "codigo"),
    ("V2 Kruskal componentes", "kruskal_componentes", "codigo"),
    ("V3 Prim heap", "prim_heap", "codigo"),
    ("V4 Kruskal Union-Find", "kruskal_union_find", "codigo"),
]
HERRAMIENTAS = ["generador", "subyacente", "verificador", "arborescencia", "laboratorio"]

ESCENARIOS = [
    ("E1", "escenario1_100v_300a.txt", "disperso", "100 vertices, 300 arcos"),
    ("E2", "escenario2_100v_2500a.txt", "denso", "100 vertices, 2,500 arcos"),
    ("E3", "escenario3_subyacente.txt", "denso", "100 vertices, 4,500 arcos dirigidos (3,467 pares no dirigidos)"),
    ("E3b", "escenario3b_100v_4500a_no_dirigido.txt", "denso", "100 vertices, 4,500 arcos no dirigidos"),
    ("E4", "escenario4_500v_1500a.txt", "disperso", "500 vertices, 1,500 arcos"),
    ("E5", "escenario5_500v_62000a.txt", "denso", "500 vertices, 62,000 arcos"),
]


def binario(nombre):
    return os.path.join(BIN, nombre + EXE)


def compilar():
    os.makedirs(BIN, exist_ok=True)
    for _, prog, carpeta in PROFESOR:
        subprocess.run(["g++", "-std=c++17", "-O2", os.path.join(RAIZ, carpeta, prog + ".cpp"), "-o", binario(prog)], check=True)
    for prog in HERRAMIENTAS:
        subprocess.run(["g++", "-std=c++17", "-O2", os.path.join(RAIZ, "extra", prog + ".cpp"), "-o", binario(prog)], check=True)


def ejecutar(prog, archivo, inicio="", sin_comp="0", mostrar="0"):
    args = [binario(prog), archivo]
    if inicio != "" or sin_comp != "0" or mostrar != "0":
        args += [inicio if inicio != "" else "0", sin_comp, mostrar]
    proc = subprocess.run(args, capture_output=True, text=True)
    if proc.returncode != 0:
        print("Fallo al correr " + " ".join(args))
        print("Mensaje del programa " + (proc.stderr.strip() or "(sin mensaje)"))
        sys.exit(1)
    salida = proc.stdout
    lineas = salida.strip().splitlines()
    datos = dict(par.split("=") for par in lineas[0].split())
    return datos, lineas


def canonico(lineas):
    arcos = []
    for l in lineas[1:]:
        u, v, w = map(int, l.split())
        arcos.append((min(u, v), max(u, v), w))
    return tuple(sorted(arcos))


MODOS_UF = [("0", "tamano y compresion"), ("1", "solo tamano"), ("3", "solo compresion"), ("2", "sin optimizaciones")]
ADVERSARIOS = [
    ("cadena 2000", "adversario_cadena_2000.txt", "adversario", "cadena de 2,000 vertices"),
    ("cadena 20000", "adversario_cadena_20000.txt", "adversario", "cadena de 20,000 vertices"),
]


def union_find_modos(reps):
    filas = []
    for clave, archivo, tipo, _ in ESCENARIOS + ADVERSARIOS:
        ruta = os.path.join(DATOS, archivo)
        tiempos = {m: [] for m, _ in MODOS_UF}
        for _ in range(reps):
            for m, _ in MODOS_UF:
                d, _ = ejecutar("kruskal_union_find", ruta, "0", m, "0")
                tiempos[m].append(float(d["tiempo_ms"]))
        base = None
        for m, nombre in MODOS_UF:
            d, lineas = ejecutar("kruskal_union_find", ruta, "0", m, "1")
            arbol = canonico(lineas)
            base = base or arbol
            filas.append([clave, tipo, nombre, d["costo"], d["saltos"], d["prof_max"],
                          f"{statistics.median(tiempos[m]):.4f}", "mismo AEM" if arbol == base else "AEM distinto"])
        print(f"union-find {clave} listo")
    with open(os.path.join(RES, "union_find_modos.csv"), "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["escenario", "tipo", "modo", "costo", "saltos", "prof_max", "mediana_ms", "arbol"])
        w.writerows(filas)


def main():
    reps = 15
    for a in sys.argv[1:]:
        if a == "--compilar":
            compilar()
        elif a.isdigit():
            reps = max(5, int(a))
    if "--solo-union-find" in sys.argv:
        union_find_modos(reps)
        return
    os.makedirs(os.path.join(RES, "salidas"), exist_ok=True)
    todos = PROFESOR
    filas_t, filas_c, filas_p, filas_k = [], [], [], []
    for clave, archivo, tipo, desc in ESCENARIOS:
        ruta = os.path.join(DATOS, archivo)
        tiempos = {nombre: [] for nombre, _, _ in todos}
        tiempos["V4 sin compresion"] = []
        n = 0
        for _ in range(reps):
            for nombre, prog, _ in todos:
                d, _ = ejecutar(prog, ruta)
                tiempos[nombre].append(float(d["tiempo_ms"]))
                n = int(d["n"])
            d, _ = ejecutar("kruskal_union_find", ruta, "0", "1", "0")
            tiempos["V4 sin compresion"].append(float(d["tiempo_ms"]))
        arboles = {}
        for nombre, prog, _ in todos:
            d, lineas = ejecutar(prog, ruta, str(n - 1), "0", "1")
            salida = os.path.join(RES, "salidas", f"{clave}_{prog}.txt")
            with open(salida, "w") as f:
                f.write("\n".join(lineas) + "\n")
            ver = subprocess.run([binario("verificador"), ruta, salida], capture_output=True, text=True).stdout.split()[0]
            arboles[nombre] = canonico(lineas)
            t = tiempos[nombre]
            filas_t.append([clave, tipo, desc, nombre, f"{statistics.median(t):.4f}", f"{min(t):.4f}", f"{max(t):.4f}", reps])
            filas_c.append([clave, nombre, d["costo"], d["aristas"], ver])
        t = tiempos["V4 sin compresion"]
        filas_t.append([clave, tipo, desc, "V4 sin compresion", f"{statistics.median(t):.4f}", f"{min(t):.4f}", f"{max(t):.4f}", reps])
        ref = arboles["V4 Kruskal Union-Find"]
        for fila in filas_c:
            if fila[0] == clave:
                fila.append("igual a V4" if arboles[fila[1]] == ref else "otro arbol, mismo costo")
        for nombre, prog, _ in PROFESOR[0::2]:
            base = None
            for s in [0, n // 2, n - 1]:
                d, lineas = ejecutar(prog, ruta, str(s), "0", "1")
                arbol = canonico(lineas)
                base = base or arbol
                filas_p.append([clave, nombre, s, d["costo"], "igual al de inicio 0" if arbol == base else "otro arbol"])
        for nombre, prog, _ in PROFESOR[1::2]:
            d, _ = ejecutar(prog, ruta)
            filas_k.append([clave, nombre, d["m"], d["revisadas"], d["descartadas"]])
        print(f"{clave} listo")

    def guardar(nombre, encabezado, filas):
        with open(os.path.join(RES, nombre), "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(encabezado)
            w.writerows(filas)

    guardar("tiempos_oficiales.csv", ["escenario", "tipo", "descripcion", "version", "mediana_ms", "minimo_ms", "maximo_ms", "corridas"], filas_t)
    guardar("consistencia.csv", ["escenario", "version", "costo", "aristas", "verificador", "arbol"], filas_c)
    guardar("prim_inicio.csv", ["escenario", "version", "inicio", "costo", "arbol"], filas_p)
    guardar("kruskal_descartes.csv", ["escenario", "version", "arcos_totales", "revisadas", "descartadas"], filas_k)

    union_find_modos(reps)

    print("\nMediana en ms (" + str(reps) + " corridas)")
    nombres = [n for n, _, _ in todos] + ["V4 sin compresion"]
    print("| Escenario | Tipo | " + " | ".join(nombres) + " |")
    for clave, _, tipo, _ in ESCENARIOS:
        valores = {f[3]: f[4] for f in filas_t if f[0] == clave}
        print(f"| {clave} | {tipo} | " + " | ".join(valores[n] for n in nombres) + " |")


if __name__ == "__main__":
    main()
