# La serie de cierres, reconstruida — y mi pronóstico de la r53 medía en una unidad que no existe

Ventana de la r54, mientras los siete agentes trabajan. Cero contacto con el árbol: esto
sale entero de `docs/analisis/` y de `git log`. Herramienta nueva: `scripts/serie.py`.

---

## 1. Lo que dije en la r53, y por qué estaba mal

> «El rango de 80-160 **rondas** no se puede estrechar con honestidad hoy, y la razón es
> aritmética: el riesgo por función-ronda de las últimas cinco rondas sale de **cuatro
> cierres**, y el intervalo de Poisson al 95 % sobre cuatro eventos va de ~1 a ~10.»

Dos fallos, y el segundo es el grave.

**Usé cinco rondas porque era lo que tenía a mano.** El historial entero estaba escrito y
sin leer: cada `rNN-cierre.md` trae una tabla `antes | después` con las funciones exactas y
las unidades completas, y cada `brief-rNN.md` publica el estado con el que **arranca** la
ronda, que es el cierre de la anterior. Entre las dos fuentes salen **diecisiete
transiciones**, de la r37 a la r53, no cinco. El intervalo de Poisson sobre 37 eventos no se
parece al de cuatro.

**Y el denominador —«ronda»— no es una unidad de trabajo.** La r42 fueron 2 informes de
agente; la r52 fueron 10. Peor: la r46 llevaba dentro **seis iteraciones**, y se demuestra
con git — entre el commit que añade `r46-cierre.md` y el que añade `brief-r47.md` hay cinco
commits de trabajo real, uno titulado *«El SDK entero al 100 %: 98 de 98 unidades»* y otro
*«Vedas y avances menores de las iteraciones 2 a 6»*.

Pronosticar «en N rondas» es medir con una regla que cambia de largo entre 2 y 10. **La
unidad honesta es el encargo de agente**, que se cuenta y que es lo que de verdad cuesta.

---

## 2. La serie

`python scripts/serie.py`. **El control**: la última fila tiene que coincidir con
`report.json`, y coincide — 18.403 funciones, 518 unidades. Un historial que no reproduce el
presente no sirve para pronosticar nada, así que la herramienta se niega a hablar si falla.

| ronda | funciones | Δ | unidades | Δ |
|---|---:|---:|---:|---:|
| r37 | 18.366 | | ? | |
| r38 | 18.368 | +2 | 476/617 | |
| r39 | 18.371 | +3 | 478/618 | +2 · censo |
| r40 | 18.372 | +1 | 478/618 | 0 |
| r41 | 18.373 | +1 | 478/618 | 0 |
| r42 | 18.373 | 0 | ? | |
| r43 | 18.373 | 0 | 478/618 | 0 |
| r44 | 18.373 | 0 | 478/618 | 0 |
| r45 | 18.373 | 0 | 480/617 | +2 · censo |
| **r46** | 18.389 | **+16** | 492/619 | **+12** · censo |
| r47 | 18.394 | +5 | 493/619 | +1 |
| r48 | 18.399 | +5 | 494/618 | +1 · censo |
| r49 | 18.399 | 0 | 497/618 | +3 |
| **r50** | 18.400 | +1 | 506/616 | **+9** · censo |
| **r51** | 18.400 | 0 | 518/616 | **+12** |
| r52 | 18.401 | +1 | 518/616 | **0** |
| r53 | 18.403 | +2 | 518/616 | **0** |

`· censo` marca las transiciones en las que **el denominador se movió** (616↔619: el
troceador añade y quita entradas). Un delta de `linked` medido a través de un cambio de
censo no es comparable con los demás, y la herramienta lo señala en vez de callarlo.

---

## 3. Tres regímenes, no una tasa

| tramo | rondas | encargos | fn/encargo | uds/encargo | |
|---|---:|---:|---:|---:|---|
| r37-r45 perseguir `matched` | 8 | 34 | 0,21 | 0,12 | +7 fn, +4 uds |
| r46-r52 drenar el atasco | 7 | 57 | **0,49** | **0,67** | +28 fn, +38 uds |
| r53- sólo queda el bloque A | 1 | 8 | 0,25 | **0,00** | +2 fn, 0 uds |

El tramo del medio no es un ritmo: es **el drenaje de un atasco**. De las 38 promociones,
**34 llegaron en cuatro ventanas** de 6, 8, 8 y 12 unidades —«las seis unidades sin fuente
cierran», «ocho promociones demostradas», «la ventana de la r50», «la ventana de la r51»—.
Eran unidades cuyo bloqueo **ya estaba resuelto** y a las que sólo les faltaba una medida
que se diera cuenta. En cuanto se agotó ese stock, la r52 y la r53 dieron **cero**.

Ese es el sentido exacto en que la pendiente no se puede extrapolar, y es un sentido
distinto del que di en la r53: no es que haya pocos datos, es que **los datos que hay
pertenecen a un proceso que ya terminó**.

---

## 4. El pronóstico, ahora sí acotado

Quedan **29 entradas de función** (28 reales + los 20 B de relleno del DOL) y **26 unidades**
hasta el techo de 544.

| | funciones | unidades |
|---|---|---|
| al ritmo de **toda la serie** (0,374 fn/enc, 0,424 uds/enc sobre 99 encargos) | **78 encargos** | **61 encargos** |
| al ritmo de **las dos últimas rondas** | 174 encargos | **cero en 18 encargos** |

Con tandas de siete, eso es **entre 9 y 25 rondas para las funciones**, y para `linked` la
horquilla no se cierra por arriba: **no hay ritmo que extrapolar cuando el numerador es
cero**. Por eso la r54 es de diagnóstico y no de producción, y por eso su entregable es un
mecanismo.

Comparado con el «80-160 rondas» de la r53: el extremo optimista estaba **nueve veces** mal,
y el pesimista medía en una unidad indefinida.

**Lo que esto NO dice.** Tres avisos, porque la memoria del proyecto tiene ocho
extrapolaciones mías marcadas como falsas:

1. **Sesgo de supervivencia.** Las 28 funciones que quedan son las que han aguantado
   diecisiete rondas de intentos. Son, por construcción, las más caras de la serie: la tasa
   media de la serie las cuenta como si fueran una cualquiera, y no lo son.
2. **`linked` no está limitado por las funciones.** El bloque A —13 unidades, 1,65 MB, el
   42 % del código del juego— **no necesita cerrar ni una función**. Su cuello es el
   mecanismo que la r54 busca. Si aparece, esos 61 encargos sobran; si no, no bastan.
3. **El conteo de encargos es un proxy**: cuenta informes `rNN-*.md`. Un agente sin informe
   no se ve, y un informe partido en dos cuenta doble.

---

## 5. Lo que se corrige en el catálogo

- **`brief-r54.md` §5 del pronóstico de la r53**: «80-160 rondas» pasa a «61-174 encargos de
  agente para `linked`, y sin techo mientras no haya mecanismo».
- **La unidad de todo pronóstico futuro es el encargo**, no la ronda. `serie.py` lo imprime.
- **Cambio de definición detectado y normalizado**: hasta la r53 los documentos contaban las
  29 entradas de `report.json`; desde que se supo que `pad_00_8000348C_init` son 20 B de
  relleno, el brief de la r54 dice «28 funciones **reales**». Sin normalizar, la serie se
  apuntaba un cierre que nadie hizo. Toda la serie va en entradas de `report.json`.
- **`r46-cierre.md` está escrito a mitad de ronda** y sus cifras (18.378, 483/616) no son el
  cierre de la r46. Gana `brief-r47.md`. Queda anotado en la salida de la herramienta.

---

## 6. Dos fallos que cometí construyendo esto, por si sirven

**El regex leía el «antes».** En `r46-cierre.md` la celda del después se escribe
`**18.378**` **sin denominador**, así que un patrón que exigía `/ 18.432` sólo veía la celda
de la izquierda. Se comía los cierres de tres rondas enteras y nadie lo habría notado: las
cifras eran plausibles.

**Y al arreglarlo, lo rompí al revés.** Pasé a coger la última cifra de la línea y salieron
**deltas negativos** — porque varios cierres citan más abajo un «censo directo
independiente» que mide con **otro denominador** (3.946.204 B) y da otra cifra. El síntoma
delató el fallo: el trabajo no se deshace, así que un delta negativo sólo puede ser un error
de medida. La regla correcta es **la primera línea etiquetada, que es la tabla oficial**.
