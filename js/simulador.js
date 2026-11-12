// Mision 9

const GRAFOS = {
  nl: {
    nota: "Costos ilustrativos, no son costos reales de tendido.",
    nodos: [
      { c: "MTY", n: "Monterrey", x: 300, y: 200 },
      { c: "SPG", n: "San Pedro", x: 215, y: 262 },
      { c: "STA", n: "Santa Catarina", x: 115, y: 225 },
      { c: "GAR", n: "García", x: 60, y: 110 },
      { c: "ESC", n: "Escobedo", x: 235, y: 60, ly: -27 },
      { c: "SNI", n: "San Nicolás", x: 320, y: 112, lx: 62, ly: 4 },
      { c: "APO", n: "Apodaca", x: 440, y: 70, ly: -27 },
      { c: "GPE", n: "Guadalupe", x: 410, y: 190, lx: 52, ly: 4 },
      { c: "JUA", n: "Juárez", x: 470, y: 280 },
      { c: "CAD", n: "Cadereyta", x: 565, y: 330 }
    ],
    arcos: [[0, 1, 4], [0, 5, 3], [0, 7, 3], [0, 2, 6], [1, 2, 4], [2, 3, 7], [3, 4, 8], [4, 5, 3], [5, 6, 5],
            [6, 7, 4], [5, 7, 4], [7, 8, 5], [8, 9, 6], [6, 9, 12], [0, 4, 5], [2, 4, 9], [7, 9, 9], [0, 8, 8]]
  },
  ej6: {
    nota: "Grafo ejemplo6.txt del paquete del profesor, costo esperado 13.",
    nodos: [
      { c: "0", n: "vértice 0", x: 80, y: 190 },
      { c: "1", n: "vértice 1", x: 230, y: 80 },
      { c: "2", n: "vértice 2", x: 230, y: 300 },
      { c: "3", n: "vértice 3", x: 390, y: 80 },
      { c: "4", n: "vértice 4", x: 390, y: 300 },
      { c: "5", n: "vértice 5", x: 530, y: 190 }
    ],
    arcos: [[0, 1, 4], [0, 2, 3], [1, 2, 2], [1, 3, 5], [2, 3, 3], [2, 4, 6], [3, 4, 2], [3, 5, 4], [4, 5, 3]]
  }
};

const sim = { grafo: "nl", version: "v1", inicio: 9, pasos: [], i: 0, timer: null };

// O(E)
function idxArco(g, u, v) {
  return g.arcos.findIndex(([a, b]) => (a === u && b === v) || (a === v && b === u));
}

// O(V + E)
function foto(est, extra) {
  const p = Object.assign({
    arbol: new Set(est.arbol), desc: new Set(est.desc), cand: [],
    dentro: est.dentro ? est.dentro.slice() : null, actual: -1, tono: "info", costo: est.costo
  }, extra);
  p.cand = new Set(p.cand);
  return p;
}

// V1, Prim con arreglos
// O(V^2)
function pasosPrimArreglos(g, s) {
  const n = g.nodos.length, INF = Infinity;
  const mat = Array.from({ length: n }, () => Array(n).fill(-1));
  g.arcos.forEach(([u, v, w]) => { mat[u][v] = w; mat[v][u] = w; });
  const best = Array(n).fill(INF), padre = Array(n).fill(-1), usado = Array(n).fill(false);
  best[s] = 0;
  const est = { arbol: [], desc: [], dentro: usado, costo: 0 };
  const pasos = [];
  let comp = 0;
  const panel = () => ({ tipo: "arreglo", titulo: "best[v] y padre[v]", celdas: g.nodos.map((d, v) => ({ k: d.c, v: best[v] === INF ? "∞" : best[v], sub: padre[v] === -1 ? "" : g.nodos[padre[v]].c, on: usado[v] })) });
  const frontera = () => g.nodos.map((d, v) => (!usado[v] && padre[v] !== -1 ? idxArco(g, padre[v], v) : -1)).filter((x) => x >= 0);
  pasos.push(foto(est, { narr: `Empieza en ${g.nodos[s].n}. best vale 0 en el inicio e infinito en los demás.`, panel: panel(), cont: { comparaciones: comp } }));
  for (let paso = 0; paso < n; paso++) {
    let u = -1;
    for (let v = 0; v < n; v++) if (!usado[v] && (u === -1 || best[v] < best[u])) u = v;
    comp += n;
    usado[u] = true;
    let narr;
    let actual = -1;
    if (padre[u] !== -1) {
      actual = idxArco(g, padre[u], u);
      est.arbol.push(actual);
      est.costo += best[u];
      narr = `Elección voraz. Recorre los ${n} valores de best y el menor fuera del árbol es ${g.nodos[u].n} con ${best[u]}. Entra el arco ${g.nodos[padre[u]].c}–${g.nodos[u].c}.`;
    } else {
      narr = `Elección voraz. El menor best es el del inicio, ${g.nodos[u].n}, que entra sin arco.`;
    }
    pasos.push(foto(est, { narr, tono: "ok", actual, cand: frontera(), panel: panel(), cont: { comparaciones: comp } }));
    const mejoras = [];
    for (let v = 0; v < n; v++) {
      if (!usado[v] && mat[u][v] >= 0 && mat[u][v] < best[v]) { best[v] = mat[u][v]; padre[v] = u; mejoras.push(g.nodos[v].c + " a " + best[v]); }
    }
    comp += n;
    if (paso < n - 1) {
      pasos.push(foto(est, {
        narr: mejoras.length ? `Revisa toda la fila de ${g.nodos[u].c} en la matriz (${n} celdas) y mejora best de ${mejoras.join(", ")}.` : `Revisa toda la fila de ${g.nodos[u].c} (${n} celdas) y no mejora nada.`,
        cand: frontera(), panel: panel(), cont: { comparaciones: comp }
      }));
    }
  }
  pasos.push(foto(est, { narr: `Listo. ${n - 1} arcos, costo ${est.costo}. Hizo ${comp} comparaciones, 2V² sin importar cuántos arcos tenga el grafo.`, tono: "ok", panel: panel(), cont: { comparaciones: comp }, fin: true }));
  return pasos;
}

// V3, Prim con heap de entradas repetidas
// O(E log V)
function pasosPrimHeap(g, s) {
  const n = g.nodos.length;
  const ady = Array.from({ length: n }, () => []);
  g.arcos.forEach(([u, v, w]) => { ady[u].push([v, w]); ady[v].push([u, w]); });
  const usado = Array(n).fill(false);
  let heap = [[0, s, -1]];
  const est = { arbol: [], desc: [], dentro: usado, costo: 0 };
  const pasos = [];
  const c = { inserciones: 1, extracciones: 0, viejas: 0 };
  const orden = (a, b) => a[0] - b[0] || a[1] - b[1] || a[2] - b[2];
  const panel = () => ({ tipo: "heap", titulo: `Heap, ${heap.length} entradas (costo, origen → destino)`, items: heap.slice().sort(orden).map(([w, u, p]) => ({ w, t: p === -1 ? `inicio ${g.nodos[u].c}` : `${g.nodos[p].c} → ${g.nodos[u].c}`, vieja: usado[u] })) });
  pasos.push(foto(est, { narr: `Mete al heap la entrada (0, ${g.nodos[s].c}) del inicio.`, panel: panel(), cont: { ...c } }));
  let cuenta = 0;
  while (heap.length && cuenta < n - 1) {
    heap.sort(orden);
    const [w, u, p] = heap.shift();
    c.extracciones++;
    const arco = p === -1 ? -1 : idxArco(g, p, u);
    if (usado[u]) {
      c.viejas++;
      est.desc.push(arco);
      pasos.push(foto(est, { narr: `Sale el tope (${w}, ${g.nodos[p].c} → ${g.nodos[u].c}), pero ${g.nodos[u].c} ya está en el árbol. Es una entrada vieja y se tira, así evita el ciclo.`, tono: "no", actual: arco, panel: panel(), cont: { ...c } }));
      continue;
    }
    usado[u] = true;
    if (p !== -1) { est.arbol.push(arco); est.costo += w; cuenta++; }
    pasos.push(foto(est, { narr: p === -1 ? `Sale el tope, el inicio ${g.nodos[u].n}.` : `Elección voraz. Sale el tope (${w}, ${g.nodos[p].c} → ${g.nodos[u].c}), el arco más barato que sale del árbol. Entra.`, tono: "ok", actual: arco, panel: panel(), cont: { ...c } }));
    const metidos = [];
    for (const [v, cw] of ady[u]) if (!usado[v]) { heap.push([cw, v, u]); c.inserciones++; metidos.push(`${g.nodos[v].c} (${cw})`); }
    if (metidos.length && cuenta < n - 1) {
      pasos.push(foto(est, { narr: `Mete al heap los arcos de ${g.nodos[u].c} hacia ${metidos.join(", ")}. Solo revisa vecinos reales, no una fila completa.`, cand: ady[u].filter(([v]) => !usado[v]).map(([v]) => idxArco(g, u, v)), panel: panel(), cont: { ...c } }));
    }
  }
  pasos.push(foto(est, { narr: `Listo. ${n - 1} arcos, costo ${est.costo}. Quedaron ${heap.length} entradas en el heap sin sacar.`, tono: "ok", panel: panel(), cont: { ...c }, fin: true }));
  return pasos;
}

// O(E log E)
function ordenados(g) {
  return g.arcos.map(([u, v, w], i) => ({ u: Math.min(u, v), v: Math.max(u, v), w, i })).sort((a, b) => a.w - b.w || a.u - b.u || a.v - b.v);
}

// V2, Kruskal con arreglo de componentes
// O(E log E + V^2)
function pasosKruskalComp(g) {
  const n = g.nodos.length, lista = ordenados(g);
  const comp = g.nodos.map((d, v) => v), estado = lista.map(() => "");
  const est = { arbol: [], desc: [], costo: 0 };
  const pasos = [];
  const c = { revisados: 0, descartados: 0, "celdas leídas al unir": 0 };
  const panel = () => ({ tipo: "kruskal", titulo: "comp[v]", celdas: g.nodos.map((d, v) => ({ k: d.c, v: comp[v] })), lista: lista.map((e, k) => ({ t: `${g.nodos[e.u].c}–${g.nodos[e.v].c} ${e.w}`, s: estado[k] })) });
  pasos.push(foto(est, { narr: `Ordena los ${lista.length} arcos de menor a mayor costo. Cada vértice empieza en su propia componente.`, panel: panel(), cont: { ...c } }));
  let cuenta = 0;
  for (let k = 0; k < lista.length; k++) {
    const e = lista[k];
    c.revisados++;
    const c1 = comp[e.u], c2 = comp[e.v];
    if (c1 === c2) {
      c.descartados++; estado[k] = "no"; est.desc.push(e.i);
      pasos.push(foto(est, { narr: `Revisa ${g.nodos[e.u].c}–${g.nodos[e.v].c} (${e.w}). Las dos etiquetas valen ${c1}, ya están conectados y cerraría un ciclo. Se descarta.`, tono: "no", actual: e.i, panel: panel(), cont: { ...c } }));
      continue;
    }
    let cambian = 0;
    for (let v = 0; v < n; v++) if (comp[v] === c2) { comp[v] = c1; cambian++; }
    c["celdas leídas al unir"] += n;
    estado[k] = "ok"; est.arbol.push(e.i); est.costo += e.w; cuenta++;
    pasos.push(foto(est, { narr: `Elección voraz. ${g.nodos[e.u].c}–${g.nodos[e.v].c} (${e.w}) tiene etiquetas ${c1} y ${c2}, distintas, entra. Para fusionar lee las ${n} etiquetas y cambia ${cambian} de ${c2} a ${c1}.`, tono: "ok", actual: e.i, panel: panel(), cont: { ...c } }));
    if (cuenta === n - 1) {
      pasos.push(foto(est, { narr: `Listo. Ya hay ${n - 1} arcos, costo ${est.costo}. Los ${lista.length - k - 1} arcos restantes ni se revisan.`, tono: "ok", panel: panel(), cont: { ...c }, fin: true }));
      break;
    }
  }
  return pasos;
}

// V4, Kruskal con Union-Find (union por tamano y compresion)
// O(E log E + E alfa(V))
function pasosKruskalUF(g) {
  const n = g.nodos.length, lista = ordenados(g);
  const padre = g.nodos.map((d, v) => v), tam = Array(n).fill(1), estado = lista.map(() => "");
  const est = { arbol: [], desc: [], costo: 0 };
  const pasos = [];
  const c = { revisados: 0, descartados: 0, saltos: 0 };
  const find = (x) => {
    const camino = [x];
    let r = x;
    while (padre[r] !== r) { r = padre[r]; camino.push(r); c.saltos++; }
    for (const y of camino) padre[y] = r;
    return { r, camino };
  };
  const nom = (v) => g.nodos[v].c;
  const panel = () => ({ tipo: "kruskal", titulo: "padre[v], las raíces muestran su tamaño", celdas: g.nodos.map((d, v) => ({ k: d.c, v: padre[v] === v ? "raíz" : nom(padre[v]), sub: padre[v] === v ? "tam " + tam[v] : "", on: padre[v] === v })), lista: lista.map((e, k) => ({ t: `${nom(e.u)}–${nom(e.v)} ${e.w}`, s: estado[k] })) });
  pasos.push(foto(est, { narr: "Ordena los arcos. Cada vértice es la raíz de su propio árbol de tamaño 1.", panel: panel(), cont: { ...c } }));
  let cuenta = 0;
  for (let k = 0; k < lista.length; k++) {
    const e = lista[k];
    c.revisados++;
    const a = find(e.u), b = find(e.v);
    const ca = a.camino.map(nom).join(" → "), cb = b.camino.map(nom).join(" → ");
    if (a.r === b.r) {
      c.descartados++; estado[k] = "no"; est.desc.push(e.i);
      pasos.push(foto(est, { narr: `Revisa ${nom(e.u)}–${nom(e.v)} (${e.w}). find sube ${ca} y ${cb}, las dos llegan a ${nom(a.r)}. Misma raíz, cerraría un ciclo. Se descarta.`, tono: "no", actual: e.i, panel: panel(), cont: { ...c } }));
      continue;
    }
    let r1 = a.r, r2 = b.r;
    if (tam[r1] < tam[r2]) [r1, r2] = [r2, r1];
    padre[r2] = r1; tam[r1] += tam[r2];
    estado[k] = "ok"; est.arbol.push(e.i); est.costo += e.w; cuenta++;
    pasos.push(foto(est, { narr: `Elección voraz. ${nom(e.u)}–${nom(e.v)} (${e.w}) tiene raíces ${nom(a.r)} y ${nom(b.r)}, distintas, entra. Une colgando ${nom(r2)} de ${nom(r1)}, el árbol más grande queda arriba.`, tono: "ok", actual: e.i, panel: panel(), cont: { ...c } }));
    if (cuenta === n - 1) {
      pasos.push(foto(est, { narr: `Listo. ${n - 1} arcos, costo ${est.costo}. Mismos arcos revisados y descartados que la versión con componentes, cambia solo cuánto cuesta unir.`, tono: "ok", panel: panel(), cont: { ...c }, fin: true }));
      break;
    }
  }
  return pasos;
}

// O(costo de la version)
function generar() {
  const g = GRAFOS[sim.grafo];
  if (sim.inicio >= g.nodos.length) sim.inicio = g.nodos.length - 1;
  sim.pasos = sim.version === "v1" ? pasosPrimArreglos(g, sim.inicio)
    : sim.version === "v3" ? pasosPrimHeap(g, sim.inicio)
    : sim.version === "v2" ? pasosKruskalComp(g) : pasosKruskalUF(g);
  sim.i = 0;
  dibujar();
}

// Dibuja el grafo, el panel y la narracion del paso actual
// O(V + E)
function dibujar() {
  const g = GRAFOS[sim.grafo], p = sim.pasos[sim.i];
  const svg = document.getElementById("simGrafo");
  const esPrim = sim.version === "v1" || sim.version === "v3";
  let h = "";
  g.arcos.forEach(([u, v, w], i) => {
    const a = g.nodos[u], b = g.nodos[v];
    let cls = "e";
    if (p.arbol.has(i)) cls += " tree";
    else if (p.desc.has(i)) cls += " desc";
    else if (p.cand.has(i)) cls += " cand";
    if (p.actual === i) cls += " cur";
    const mx = (a.x + b.x) / 2, my = (a.y + b.y) / 2;
    h += `<g class="${cls}"><line x1="${a.x}" y1="${a.y}" x2="${b.x}" y2="${b.y}"/><rect x="${mx - 10}" y="${my - 9}" width="20" height="17" rx="3"/><text x="${mx}" y="${my + 4}">${w}</text></g>`;
  });
  g.nodos.forEach((d, v) => {
    const on = esPrim ? p.dentro && p.dentro[v] : false;
    const ini = esPrim && v === sim.inicio;
    h += `<g class="n${on ? " on" : ""}${ini ? " ini" : ""}"><circle cx="${d.x}" cy="${d.y}" r="19"/><text x="${d.x}" y="${d.y + 4}">${d.c}</text>${d.c !== d.n.replace("vértice ", "") ? `<text class="nm" x="${d.x + (d.lx || 0)}" y="${d.y + (d.ly === undefined ? 33 : d.ly)}">${d.n}</text>` : ""}</g>`;
  });
  svg.innerHTML = h;
  const nar = document.getElementById("simNarr");
  nar.textContent = p.narr;
  nar.className = "narration" + (p.tono === "ok" ? " match" : p.tono === "no" ? " bad" : "");
  document.getElementById("simPanel").innerHTML = panelHTML(p.panel);
  const cont = Object.entries(p.cont).map(([k, v]) => `${k} <b>${v}</b>`).join(" · ");
  document.getElementById("simStats").innerHTML = `paso <b>${sim.i + 1}</b> de ${sim.pasos.length} · costo <b>${p.costo}</b> · ${cont}`;
  document.getElementById("simNota").textContent = g.nota;
  document.getElementById("simStep").disabled = sim.i >= sim.pasos.length - 1;
}

// O(V + E)
function panelHTML(pn) {
  let h = `<div class="group-label">${pn.titulo}</div>`;
  if (pn.tipo === "heap") {
    h += `<div class="heap">${pn.items.map((it, k) => `<span class="hchip${k === 0 ? " top" : ""}${it.vieja ? " old" : ""}"><b>${it.w}</b> ${it.t}</span>`).join("") || "<span class='hchip'>vacío</span>"}</div>`;
    return h;
  }
  h += `<div class="cells">${pn.celdas.map((c) => `<div class="vc${c.on ? " on" : ""}"><span class="k">${c.k}</span><span class="v">${c.v}</span><span class="s">${c.sub || "&nbsp;"}</span></div>`).join("")}</div>`;
  if (pn.lista) h += `<div class="group-label">Arcos en orden</div><div class="heap">${pn.lista.map((e) => `<span class="hchip ${e.s}">${e.t}</span>`).join("")}</div>`;
  return h;
}

// O(1) por evento
function iniciarSimulador() {
  const selIni = document.getElementById("simInicio");
  const llenarInicios = () => {
    const g = GRAFOS[sim.grafo];
    selIni.innerHTML = g.nodos.map((d, v) => `<option value="${v}"${v === sim.inicio ? " selected" : ""}>${d.n}</option>`).join("");
    selIni.disabled = !(sim.version === "v1" || sim.version === "v3");
  };
  const parar = () => { clearInterval(sim.timer); sim.timer = null; document.getElementById("simPlay").textContent = "Animar"; };
  document.querySelectorAll("#simVersion button").forEach((b) => b.addEventListener("click", () => {
    document.querySelectorAll("#simVersion button").forEach((x) => x.classList.toggle("active", x === b));
    sim.version = b.dataset.v; parar(); llenarInicios(); generar();
  }));
  document.querySelectorAll("#simGrafoSel button").forEach((b) => b.addEventListener("click", () => {
    document.querySelectorAll("#simGrafoSel button").forEach((x) => x.classList.toggle("active", x === b));
    sim.grafo = b.dataset.g; sim.inicio = GRAFOS[sim.grafo].nodos.length - 1; parar(); llenarInicios(); generar();
  }));
  selIni.addEventListener("change", () => { sim.inicio = Number(selIni.value); parar(); generar(); });
  document.getElementById("simStep").addEventListener("click", () => { parar(); if (sim.i < sim.pasos.length - 1) { sim.i++; dibujar(); } });
  document.getElementById("simEnd").addEventListener("click", () => { parar(); sim.i = sim.pasos.length - 1; dibujar(); });
  document.getElementById("simReset").addEventListener("click", () => { parar(); sim.i = 0; dibujar(); });
  document.getElementById("simPlay").addEventListener("click", (ev) => {
    if (sim.timer) { parar(); return; }
    if (sim.i >= sim.pasos.length - 1) sim.i = 0;
    ev.target.textContent = "Pausa";
    const tick = () => { if (sim.i < sim.pasos.length - 1) { sim.i++; dibujar(); } else parar(); };
    sim.timer = setInterval(tick, 2600 / Number(document.getElementById("simSpeed").value));
  });
  llenarInicios();
  generar();
}

iniciarSimulador();
