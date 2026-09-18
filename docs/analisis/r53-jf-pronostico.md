# Cuánto falta, medido — y la métrica que llevábamos dando estaba mal

Análisis con cinco medidas independientes sobre el historial completo (r36-r53) y tres
escépticos atacando el pronóstico. Más trece medidas que sólo yo podía hacer.

---

## 1. La cifra honesta es 23,02 %, no 99,15 %

`report.json` trae un campo que nunca habíamos mirado:

```
complete_code         908.452 / 3.946.048 B  =  23,02 %
matched_code        3.912.360 / 3.946.048 B  =  99,15 %
complete_units            518 / 616
```

**`complete_code` es el código que sale de nuestra fuente.** Las otras dos son ciertas y
las dos engañan: `matched` cuenta funciones que casan aunque su unidad no enlace —y por
tanto no aporten un byte al binario—, y `linked` cuenta unidades sin pesarlas, así que
`GXAttr` vale lo mismo que `zAI`.

Ese 23 % es lo que explica por qué las últimas catorce asignaciones de agente devolvieron
tan poco: llevábamos tres rondas leyendo un 99 % que no medía lo que creíamos.

Corregido en `estado.py`, que ahora la publica como cabecera. Y de paso el techo, que
salía uno de más por no restar `prodg_fixes`: son **544**, y quedan **26** unidades
alcanzables.

---

## 2. El frente, partido por lo que bloquea

| bloque | uds | código que guarda | funciones que faltan | acumulado si cae |
|---|---:|---:|---:|---:|
| ya enlazado | 518 | 908.452 B | — | 23,0 % |
| **A — código YA al 100 %** | 13 | **1.652.576 B** | **ninguna** | **64,9 %** |
| **B — a UNA función** | 6 | 690.484 B | 6 fn, 5.728 B | 82,4 % |
| **C — a 2-6 funciones** | 7 | 694.516 B | 22 fn, 27.940 B | 99,99949 % |

El bloque A guarda **el 42 % del código del juego, y no necesita cerrar ni una función**.
Es 1,8 veces todo lo enlazado hasta hoy.

---

## 3. Los trece veredictos que faltaban

El análisis señaló que **nunca se había corrido `trypromo` sobre ninguna del bloque A** — y
sus agentes no pudieron: el clasificador de permisos se lo bloqueó. Lo he corrido yo, las
trece:

```
zAI  zFe2  zFe  zMain  zEAXSound  zPhysics  zGameplay
zFeOverlay  zLua  zMisc  zAnim  zGameModes  inittmr
```

**Trece de trece: `DOL ROTO`.** Ninguna estaba ya lista.

Es un negativo, y de los que valen: colapsa una incertidumbre de 10× en la dirección mala.
El bloque A no es «algunas ya pasan y no lo sabíamos» sino «las trece necesitan trabajo
real, y ninguna medida que tenemos hoy dice cuál».

---

## 4. El cuello de botella, nombrado

**No sabemos qué separa a una unidad con el código al 100 % y las nueve secciones a delta 0
de un `DOL OK`.**

`zMain` es el espécimen perfecto: `linkdelta` IGUAL en las nueve secciones, `fncmp` 0 de
1.380, `permorden` 11 de 1.380 — **todas las medidas que tenemos leen cero** y `dolwhere`
sigue dando **17.269 B** de contenido distinto.

Mientras ese residuo no tenga nombre, cada agente mandado a una SourceList es una papeleta
de lotería. Eso es exactamente lo que pasó en la r52 y la r53: 14 agentes al bloque A, cero
promociones.

---

## 5. El pronóstico

| métrica | hoy | techo | rondas |
|---|---|---|---|
| DOL byte a byte (con `.o` extraídos) | cumplido | — | 0 — es el juez, no la meta |
| `matched` | 99,1463 % | **99,99949 %** | 60-130 |
| bloque A → 64,9 % del código | 23,02 % | — | **3-8 si el mecanismo generaliza; 15-40 si no** |
| `linked` 544 (la meta real) | 518 | 544 | **80-160** |

**Los 20 B «sin fuente» están encontrados, y no son código.** El `.init` del ELF original
mide **908 B** (`0x80003100..0x8000348C`); el `text0` del DOL mide **928**
(`0x80003100..0x800034A0`). Los 20 B de diferencia son **ceros**, y son exactamente el
relleno de `0x38C` a `0x3A0`: la alineación a 32 B del contenedor DOL, la misma que
`cerca1` midió en la r53 sin conectarla con esto.

No existen en el ELF —por eso no hay fuente— y **nuestro DOL ya los tiene idénticos**,
porque el constructor del DOL emite el relleno él mismo. `objdiff` los cuenta como código
porque viven en una sección de texto.

O sea que **`matched` al 99,99949 % ES el 100 % real**: los 20 B que faltan son un artefacto
del denominador, no trabajo pendiente. Y de las 29 «funciones» abiertas, **28 son funciones
de verdad**.

**El rango de 80-160 no se puede estrechar con honestidad hoy**, y la razón es aritmética:
el riesgo por función-ronda de las últimas cinco rondas sale de **cuatro cierres**, y el
intervalo de Poisson al 95 % sobre cuatro eventos va de ~1 a ~10. Los datos no distinguen
«40 rondas» de «400» para la cola del bloque C. Dar un punto medio sería la novena
extrapolación de una serie que la memoria del proyecto ya tiene marcada como falsa ocho
veces.

Lo que sí se puede cifrar: **el 65 % del código está a una medida de distancia, no a una
ronda de agentes.**

---

## 6. Qué estrecharía el rango, por coste

1. ~~Los trece `trypromo`~~ — **hecho hoy: trece ROTO.** Ya no es incertidumbre.
2. **El forense de `zMain`**: nombrar el residuo de 17.269 B. Decide si el bloque A son 5
   rondas o 40. Una ronda, dos agentes.
3. Reconstruir la serie de cierres desde la r36 para bajar el intervalo de Poisson.
4. Un intento real de conseguir SN BUILD v1.72 — saca `madidct` (1.148 B de `matched`,
   1.504 de `linked`) del terreno de la adivinanza. **Ojo**: de las 28 unidades con
   compilador ausente, 27 enlazan con el v1.76 que sí tenemos, así que «imposible por
   compilador» no está demostrado; lo correcto es «la única con una causa fuera de la
   fuente».

---

## 7. El plan

**r54 — ronda de diagnóstico, no de producción.** El entregable es un mecanismo.

*Antes de repartir (mío, cero agentes):* aplicar la ventana pendiente —las dos fronteras de
`splits.txt` y los tres rangos de `steering`, re-extraer y verificar el `sha1`—; revertir la
regresión viva de `GetLoadingPriority` (97,18 % → 94,72 % desde la r45, nunca corregida);
limpiar las 14 huérfanas de `keep.lst` de `zEAXSound2` antes de que nadie intente su
promoción.

*Reparto:* 2 agentes al forense de `zMain` · 1 al control del mismo residuo en
`zLua`/`zAnim`/`zMisc` (¿es el mismo mecanismo?) · 3 a los **tres experimentos ya escritos y
nunca ejecutados** (`RenderFlaresOnCar`+`UpdateWheelY` con el volcado `.cse`; `epCalculate`
con `-da` y las líneas «Reloads for insn»; `UpdateLoaded` con la anticipabilidad de PRE) ·
1 a `Setup` de zSpeech.

**r55 — propagación o repliegue.** Si la r54 nombra el mecanismo: 5 agentes a propagarlo,
una unidad grande por agente. Si no: **el bloque A deja de recibir agentes** hasta que haya
mecanismo, y los siete van a las seis unidades a-una-función, que es el único camino
medible.

**r56 — cerrar el bloque B** (6 unidades, 5.728 B, 690.484 B de `linked`) y las dos entradas
más baratas del C: `zWorld2` y `zEagl4Anim`.
