// Mision 9

const fmt = (v, d = 2) => v.toLocaleString("es-MX", { minimumFractionDigits: d, maximumFractionDigits: d });
const ent = (v) => v.toLocaleString("es-MX");
const MISION = ["V1 Prim arreglos", "V2 Kruskal componentes", "V3 Prim heap", "V4 Kruskal Union-Find"];

// O(t)
function crearTabs(contenedor, claves, etiqueta, alElegir) {
  claves.forEach((clave, idx) => {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.className = "tab" + (idx === 0 ? " active" : "");
    btn.setAttribute("role", "tab");
    btn.innerHTML = `<span class="label">${etiqueta(clave)}</span>`;
    btn.addEventListener("click", () => {
      contenedor.querySelectorAll(".tab").forEach((t) => t.classList.toggle("active", t === btn));
      alElegir(clave);
    });
    contenedor.appendChild(btn);
  });
  alElegir(claves[0]);
}

const ESC = { E1: "E1 disperso", E4: "E4 disperso", E2: "E2 denso", E3: "E3 denso", E3b: "E3b denso", E5: "E5 denso" };
const DESC = {
  E1: "100 vértices y 300 arcos, 6% de densidad.",
  E4: "500 vértices y 1,500 arcos, 1.2% de densidad.",
  E2: "100 vértices y 2,500 arcos, 50% de densidad.",
  E3: "Escenario dirigido de 100 vértices y 4,500 arcos, convertido a su grafo no dirigido de 3,467 pares.",
  E3b: "100 vértices y 4,500 arcos no dirigidos, 91% de densidad.",
  E5: "500 vértices y 62,000 arcos, 50% de densidad."
};

crearTabs(document.getElementById("tabsTiempos"), Object.keys(ESC), (c) => ESC[c], (c) => {
  const filas = DATOS.oficiales.filter((r) => r[0] === c);
  const min = Math.min(...filas.filter((r) => MISION.includes(r[2])).map((r) => r[3]));
  document.getElementById("bodyTiempos").innerHTML = filas.map(([, , v, ms]) => {
    const extra = !MISION.includes(v);
    const cls = extra ? ' class="extra"' : ms === min ? ' class="fastest"' : "";
    const best = "";
    return `<tr${cls}><td class="${best}">${v.replace("heap", "Heap").replace("compresion", "compresión")}</td><td class="num${best}">${fmt(ms, 4)}</td><td class="num">${fmt(ms / min, 1)}×</td></tr>`;
  }).join("");
  const costo = DATOS.consistencia.find((r) => r[0] === c)[2];
  const ok = DATOS.consistencia.filter((r) => r[0] === c && r[3] === "ok").length;
  document.getElementById("footTiempos").textContent = `${DESC[c]} Costo del AEM ${ent(costo)} en los 4 programas, ${ok} de 4 salidas aprobadas por el verificador. En negritas la más rápida. En gris V4 sin compresión de caminos, como referencia.`;
});

crearTabs(document.getElementById("tabsDensidad"), [1000, 2000], (v) => `V = ${ent(v)}`, (V) => {
  const filas = DATOS.densidad.filter((r) => r[0] === V);
  const es = [...new Set(filas.map((r) => r[1]))];
  const cols = MISION;
  document.getElementById("bodyDensidad").innerHTML = es.map((E) => {
    const vals = cols.map((c) => filas.find((r) => r[1] === E && r[3] === c)[4]);
    const min = Math.min(...vals);
    const dens = filas.find((r) => r[1] === E)[2];
    return `<tr><td class="num">${fmt(dens, 1)}%</td><td class="num">${ent(E)}</td>${vals.map((v) => `<td class="num${v === min ? " best" : ""}">${fmt(v, 2)}</td>`).join("")}</tr>`;
  }).join("");
});

document.getElementById("bodyDescartes").innerHTML = DATOS.descartes.filter((r) => r[1].startsWith("V4")).map(([e, , m, rev, des]) =>
  `<tr><td>${ESC[e]}</td><td class="num">${ent(m)}</td><td class="num">${ent(rev)}</td><td class="num">${ent(des)}</td><td class="num">${fmt(100 * rev / m, 1)}%</td></tr>`).join("");

const filasInicio = {};
DATOS.inicio.forEach(([g, p, v, ini, costos, arb]) => {
  const k = g + "|" + p;
  filasInicio[k] = filasInicio[k] || { g, p, ini, costos, a1: 0, a3: 0 };
  if (v.startsWith("V1")) filasInicio[k].a1 = arb; else filasInicio[k].a3 = arb;
});
document.getElementById("bodyInicio").innerHTML = Object.values(filasInicio).map((f) =>
  `<tr><td>${f.g === "todos iguales" ? "30 vértices, 200 arcos" : f.g}</td><td>${f.p}</td><td class="num">${f.ini}</td><td class="num">${f.costos}</td><td class="num${f.a1 > 1 ? " best" : ""}">${f.a1}</td><td class="num${f.a3 > 1 ? " best" : ""}">${f.a3}</td></tr>`).join("");

const dir = DATOS.dirigido;
const azar = dir.filter((r) => r[0].startsWith("aleatorio"));
const peor = azar.filter((r) => r[2] > r[1]).length;
const brecha = azar.reduce((s, r) => s + r[4], 0) / azar.length;
document.getElementById("bodyDirigido").innerHTML = dir.slice(0, 6).map(([g, e, p, s, b]) =>
  `<tr><td>${g === "escenario 3" ? "E3 dirigido" : "digrafo " + g.replace("aleatorio ", "#")}</td><td class="num best">${ent(e)}</td><td class="num">${ent(p)} (+${fmt(b, 1)}%)</td><td class="num">${ent(s)}</td></tr>`).join("");
document.getElementById("footDirigido").textContent = `En ${peor} de ${azar.length} digrafos al azar de 100 vértices y 4,500 arcos Prim dirigido salió más caro, ${fmt(brecha, 1)}% en promedio. El AEM del grafo subyacente ignora el sentido de los arcos, por eso siempre queda abajo del óptimo dirigido.`;


const MODOS_UF = { "tamano y compresion": "tamaño y compresión", "solo tamano": "solo unión por tamaño", "solo compresion": "solo compresión", "sin optimizaciones": "sin optimizaciones" };
const grafosUF = [...new Set(DATOS.uf.map((r) => r[0]))];
crearTabs(document.getElementById("tabsUF"), grafosUF, (g) => g.replace("cadena 2000", "cadena 2,000").replace("cadena 20000", "cadena 20,000"), (g) => {
  const filas = DATOS.uf.filter((r) => r[0] === g);
  const min = Math.min(...filas.map((r) => r[4]));
  document.getElementById("bodyUF").innerHTML = filas.map(([, modo, s, pm, ms]) =>
    `<tr${ms === min ? ' class="fastest"' : ""}><td>${MODOS_UF[modo]}</td><td class="num">${ent(s)}</td><td class="num">${ent(pm)}</td><td class="num">${fmt(ms, 4)}</td></tr>`).join("");
  const iguales = filas.every((r) => r[5] === "mismo AEM");
  document.getElementById("footUF").textContent = (g.startsWith("cadena")
    ? "Grafo armado para que la unión sin optimizaciones forme una lista y cada consulta empiece desde el fondo. "
    : "En grafos al azar casi todo el tiempo se va en ordenar los arcos, por eso los saltos cambian mucho más que los milisegundos. ")
    + (iguales ? "Los cuatro modos dieron el mismo AEM." : "Algún modo dio otro AEM.");
});