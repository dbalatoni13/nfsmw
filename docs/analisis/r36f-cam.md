# r36f — zCamera: cerrada `LoadCameraShakes` (168 B) con TRES familias apiladas, y el presupuesto de `cse` que explica los 16 B del `__static_init`

**Cerrado: `LoadCameraShakes__10ICEManagerP6bChunk`, 168 B, `fndiff` = 100.0 % (168/168).**
Unidad: de **10.980 B pendientes a 10.812**, de 6 funciones con código distinto a 5.

Verificación (obligatoria por el brief):

```
python scripts/fndiff.py Speed/Indep/SourceLists/zCamera LoadCameraShakes__10ICEManagerP6bChunk
# LoadCameraShakes__10ICEManagerP6bChunk  target=100.0%  ours=100.0%  size=168/168

python scripts/fncmp.py Speed/Indep/SourceLists/zCamera      # antes / después
    6 de 453 funciones ... 10980 B      ->   5 de 453 funciones ... 10812 B
    (las otras cinco salen con la MISMA cifra que antes; los 178 "solo nombres
     distintos" siguen en 87.604 B)
```

Secciones del `.o`, medidas con la base limpia y con el parche en la misma
sesión: `.text` 135.968 → **135.972** (+4 B, exactamente la instrucción que nos
faltaba), `.rodata` 8.488, `.data` 2.884, `.bss` 1.912 — **idénticas**. El
parche no mueve ni un literal.

`lcfix.py --check`: **las 21 entradas de zCamera son IDÉNTICAS antes y después**
(diff vacío entre los dos volcados). Aviso honesto: `--check` **no sale limpio
en la base** de esta sesión — reporta 344 correcciones repartidas por 19
unidades (zAI, zAnim, zCamera, zEAXSound, zEcstasy, zFe, zGameplay, zLua, zMain,
zPhysics, zSim, zWorld…). No es de este parche; lo compruebo revirtiendo,
reconstruyendo y volviendo a medir.

Un solo fichero tocado: `src/Speed/Indep/Src/Camera/ICE/ICEManager.cpp`.
Sin commits.

> Nota de entorno: durante toda la ronda había otros agentes tocando el árbol
> (`EmitterSystem.cpp`, `CarLoader.cpp`, `TrackStreamer.cpp`, `PackedDecimal.h`,
> `GRaceDatabase.cpp`, `FnStatelessQ.cpp`). Ninguno de esos ficheros entra en
> zCamera (de `TrackStreamer` sólo se incluye el `.hpp`, que nadie tocó), y la
> base de zCamera dio la misma cifra al principio y al final de la ronda.

---

## 1. `LoadCameraShakes`: **tres palancas de tres familias distintas, y ninguna vale sola**

La r36e dejó la causa bien nombrada (el original duplica la variable de
inducción) pero dio por muerta la vía. La cadena que cierra es ésta:

```c
int num_tracks = *p_handle;
ICEShakeTrack *track = reinterpret_cast<ICEShakeTrack *>(p_handle + 1);
int guard = 0;
asm("" : "+r"(guard) : "r"(num_tracks));   // 2. eje «a quién»
int i = 0;
asm("" : "+r"(guard));                     // 3. retraso de RANURA
if (guard < num_tracks) {                  // 1. la guarda con su propio cero
    do {
        track->PlatEndianSwap();
        group->AddTrack(track);
        track = reinterpret_cast<ICEShakeTrack *>(
                    reinterpret_cast<char *>(track) + track->MemoryImageSize());
        i++;
    } while (i < num_tracks);
}
```

Cada palanca arregla **una cosa distinta**, y las tres son necesarias:

| palanca | familia | qué arregla | sin ella |
|---|---|---|---|
| 1. `guard` propio + `do/while` | forma de fuente | el **tamaño**: emite el `li r0,0` del objetivo y su `cmpw r0,r28` | 164 B (falta una instrucción) |
| 2. `asm("" : "+r"(guard) : "r"(num_tracks))` | eje «a quién» | el **reparto**: `num_tracks`→r28, `i`→r27 como el objetivo | 5 filas (r27/r28 al revés) |
| 3. `asm("" : "+r"(guard))` tras `int i = 0;` | ranura | el **orden**: `li r27,0` antes del `cmpw`, no después | 2 filas |

### 1.1 La palanca 3 es NUEVA: **una segunda barrera sobre la MISMA variable no es redundante**

Es el hallazgo transferible de esta ronda. Con la palanca 2 puesta quedaban dos
filas y eran un empate puro de `rank_for_schedule`: el `cmpw r0,r28` de la
guarda tiene más prioridad que el `li r27,0` (está en el camino crítico al
`bge`), así que `sched2` lo adelanta. No hay forma de subirle la prioridad al
`li`… pero **sí de retrasar el operando del `cmpw`**: un segundo
`asm("" : "+r"(guard))` colocado *después* de `int i = 0;` mete un eslabón de
dependencia de cero bytes entre `guard` y su uso, y con eso el `li r27,0` se
emite antes.

Esto matiza dos avisos del brief:

* «Un encadenado de `asm` de cero bytes NO propaga `INSN_PRIORITY`» — cierto, y
  aquí **no se usa para propagar prioridad sino para retrasar la disponibilidad
  de un operando**. Son dos efectos distintos del mismo idioma.
* «LAS FAMILIAS SE COMBINAN» (r36e, dos familias en `TrackCop`) — aquí hacen
  falta **tres**, y la tercera es una segunda copia de la misma familia sobre la
  misma variable. Regla operativa: si al apilar dos familias te quedan 2 filas y
  son un empate de planificador, prueba a **repetir la barrera de la variable
  que alimenta la instrucción que se adelanta**, detrás de la instrucción que
  quieres ver primero.

### 1.2 La cadena de medidas (46 variantes; base = 164 B, 6 filas)

La primera columna es lo que da `fncmp`: «164 B» quiere decir que el tamaño
sigue corto, y un número quiere decir filas con el tamaño ya exacto (168 B).

| # | forma | resultado |
|---|---|---|
| n1 | `while` con `int i` fuera | 164 B |
| n2 | `if (i<n)` + `do/while`, un solo `i` | 164 B |
| n3 | `++i` | 164 B |
| n4 / n5 | `asm("" : "+r"(i))` antes del bucle / dentro | 164 B |
| n6 | `int guard = 0;` **sin** barrera | 164 B (cse funde los dos pseudos) |
| n8 | barrera sobre `track` al final del cuerpo | 164 B |
| o5 | una sola variable con barrera | 164 B |
| u1 | `while` dentro del `if` | 176 B |
| w6 | `volatile int vi = 0; int i = vi;` | 176 B |
| **n7** | **guard + barrera, `i` dentro del `if`** | **168 B**, 6 filas |
| o1 / o2 | `i` antes de `guard` / entre `guard` y el `asm` | 7 |
| o3 | `i` tras la barrera, fuera del `if` | 5 |
| o4 / x7 | `int guard = i;` | 7 |
| o6 | `int i = guard;` | 18 |
| **p1** | **o3 + eje «a quién» (`num_tracks` de ENTRADA)** | **2** |
| p2 / u2 | `num_tracks > guard` | 6 / 3 |
| p3 / q2 / q3 / q6 | p1 + barrera sobre `i` (4 formas) | 6 |
| p7 | `asm("" : "+r"(guard) : "r"(i))` | 7 |
| p8 | `asm("" : "+r"(i) : "r"(num_tracks))` | 6 |
| q1 / w1 | p1 + `asm volatile("")` | 13 / 15 |
| q5 / r1 / r2 / r4 / r6 | `i` antes del `asm`, o dos salidas, o `i` de entrada | 7 |
| r3 / r5 / w7 | tercera barrera con `i` o `track` de entrada | 6 |
| t1 / t2 | `i` declarada arriba del bloque | 7 |
| t5 | `i` dentro del `if` (p1 + n7) | 3 |
| u3 | `if (guard >= n) {} else {…}` | 2 |
| w2 | p1 con clobber `"cc"` | 2 |
| w3 / w4 | atar `track` con `i` | 17 |
| w5 | p1 con la entrada repetida dos veces | 2 |
| x4 | entrada `track` en la barrera | 14 |
| x5 / x6 | `track` declarada después del `asm` | 2 |
| y2 | **sólo** la segunda barrera | 7 |
| y3 | las dos barreras **sin** el eje | 6 |
| y4 | las dos barreras juntas antes de `int i = 0;` | 7 |
| **x1** | **p1 + `asm("" : "+r"(guard))` tras `int i = 0;`** | **0 — CIERRA** |

y2/y3/y4 son la contraprueba de que las tres palancas son necesarias y de que
**el sitio de la segunda barrera importa**: las mismas dos barreras juntas antes
de `int i = 0;` dan 7 filas; separadas por él, cero.

### 1.3 Y una corrección a la r36e

La r36e cerró la función con dos vedas: «`warned_overflow` en cualquier forma» y
«la barrera de sólo lectura pegada a la asignación». Las dos siguen en pie: mi
parche no toca `warned_overflow` (que sigue muerto, como decía). Lo que estaba
mal era el veredicto de la sección: la duplicación de la variable de inducción
**sí** era reproducible desde la fuente, sólo que la forma que la reproduce
(guarda con variable propia + barrera para que `cse` no las funda) no está en el
catálogo del permutador ni en la lista de formas que se probaron.

---

## 2. `__static_initialization_and_destruction_0` (3.604 B): **los 4 `addi` son el presupuesto de `cse`, y está medido**

Éste es el resultado más transferible de la ronda aunque no cierre.

### 2.1 La reproducción aislada sale EXACTA

Compilé la tabla sola, con los cflags reales, en un fichero de 27 líneas: seis
elementos, el mismo constructor, `bStringHash` externo. Sale **sin un solo
`addi` de base** y, además, con el orden de stores por elemento del objetivo
(`pScore(+16), pMirror(+20), pName(+0), pReplayName(+8), nReplayHash(+12),
nNameHash(+4)`). O sea: **la fuente del constructor y de la tabla es correcta**;
lo que rompe es el contexto.

### 2.2 La ley: el número de `addi` depende del CÓDIGO QUE VA DELANTE, con frontera absoluta

`scripts` nuevo de diagnóstico: compilar zCamera con `-S` y contar dentro de
`__static_init` los `addi rN,r30,off` con `off` múltiplo de 24 (el `sizeof` de
`ReplayCategory`). Moviendo inicializadores dinámicos delante de la tabla:

| variante | líneas del `.s` de `__static_init` | `addi` de base | elementos sin plegar |
|---|---|---|---|
| +2 inicializadores dinámicos delante | 950 | 5 | 1..5 |
| +1 | 933 | 5 | 1..5 |
| **base** | **915** | **4** | **2..5** |
| −1 `bVector3` global | 904 | 4 | 2..5 |
| −2 | 897 | 3 | 3..5 |
| −4 | 876 | 2 | 4,5 |
| −5 | 869 | 2 | 4,5 |
| −6 | 860 | 2 | 4,5 |
| −8 | 842 | 1 | 5 |
| **−10** | **830** | **0** | **ninguno — como el objetivo** |
| −12 | 826 | 0 | ninguno |

(«−N» = N globales `bVector3`/`ICE::Vector3` de `Cubic.cpp`, `CopView.cpp`,
`Still.cpp` e `ICEMover.cpp` pasados a construcción por defecto. Son
diagnósticos: revertidos.)

La frontera **no se mueve**: está siempre en la línea ~825 del propio
`__static_init`, y cada elemento cuyo desarrollo empieza pasada esa línea pierde
el plegado. Es la firma de `cse.c` de GCC 2.95:

```c
      /* If we have processed 1000 insns, flush the hash table to
         avoid extreme quadratic behavior. ... */
      if (INSN_P (insn) && num_insns++ > 1000)
        { flush_hash_table (); num_insns = 0; }
```

`__static_init` es UN bloque básico gigante (las llamadas no lo cortan). Pasado
el vaciado, el pseudo `this` que `expand_inline_function` crea para
`&ReplayCategoryTable[i]` deja de tener valor constante conocido, `find_best_addr`
ya no puede plegar `(plus this k)` en `(plus r30 off+k)`, y el `addi` sobrevive.

### 2.3 Lo que eso implica, y por qué es raro

El objetivo pliega los seis. Pero **su `__static_init` tiene el MISMO número de
instrucciones finales que el nuestro antes de la tabla**: el `fndiff` alinea
1 a 1 los índices 0..795 y sólo hay 6 filas de intercambio de registro
(549-566, r9/r10/r11 entre `HydraulicsLookAngle`, `NOSFovWidening` y un literal).
Los 16 B de diferencia SON los cuatro `addi`.

Conclusión forzosa: **el excedente está en RTL intermedio que GCC borra
después**, no en código final. En nuestras fuentes de los globales que preceden
a la tabla (todos en `Camera/**`, o sea territorio) hay del orden de 70-85
insns de RTL de más que no se ven en el `.s`. Ése es el frente concreto para la
próxima ronda, y ahora tiene una **medida barata**: `dg.py` da el número de
`addi` en 15 s, y hay que llevarlo de 4 a 0.

### 2.4 Negativas de esta sección (todas medidas)

| # | qué | tamaño / `addi` |
|---|---|---|
| e1 | cuerpo de asignaciones en orden de DECLARACIÓN | **3.616** (el mejor visto) pero 4 `addi` |
| e1b/e1c/e1d | tres permutaciones más del cuerpo | 913-917 líneas, 4 `addi` |
| e2 | cuerpo en el orden de stores del objetivo | 3.620 |
| e3 | cuerpo en el orden lógico actual | 3.620 |
| e4 | hashes a locales primero y seis stores al final | 3.652 |
| e5 | lista sólo para `pScore`/`pMirror` | 3.620 |
| e6 | lista sólo para `pName`/`pReplayName` | 3.620 |
| f1 | `bVector3::bVector3(f,f,f)` escribiendo miembros en vez de llamar a `bFill` | **objeto IDÉNTICO** a la base |

f1 es informativo: la cadena `ctor → bFill` **no** genera RTL de más; el
inliner la colapsa. Hay que buscar el excedente en otra parte.

Esto retira además el dato de la r36d («ensayo 11: el cuerpo mata los `addi`»):
medido de nuevo, ninguna de las seis formas de cuerpo los mata.

---

## 3. `bCopy` / `bScale` de `bMath.hpp`: **el mapa de líneas MIENTE sobre los inlines** (veda cara, y vale para todo el proyecto)

`lmap` sobre `TrackCar::Update` del objetivo pone:

```
80075DD8  lfs f12, 0x20(r9)     bMath.hpp:906   <- las TRES cargas de bCopy
80075DE0  lfs f0,  0x18(r9)     bMath.hpp:906
80075DE8  lfs f13, 0x1c(r9)     bMath.hpp:906
80075DF0  stfs f12, 0x98(r31)   bMath.hpp:905   <- los TRES stores de bFill
80075DF8  stfs f0,  0x90(r31)   bMath.hpp:905
80075E00  stfs f13, 0x94(r31)   bMath.hpp:905
...
80075E08  lfs f13, 0x20(r1)     bMath.hpp:915   <- bScale entero en UNA línea
80075E20  fmuls f13, f13, f0    bMath.hpp:915
80075E28  stfs f13, 0x20(r1)    bMath.hpp:915
```

Las tres cargas en UNA línea y los tres stores en OTRA leen como «el `bCopy`
original es `bFill(dest, v->x, v->y, v->z)` de una sola sentencia, sin las
locales `x/y/z` intermedias», y el `bScale` igual. **Es falso, y lo dice la
medida**: quitar las locales intermedias es catastrófico.

| # | cambio en `bMath.hpp` | zCamera pendiente |
|---|---|---|
| base | — | **10.980 B** (6 funciones) |
| b_cp | `bCopy(bVector3*)` sin locales: `bFill(dest, v->x, v->y, v->z)` | **27.968 B** (21 funciones) |
| b_sc | `bScale(bVector3*,·,float)` sin locales | **20.548 B** (12 funciones) |
| b_both | las dos | **32.496 B** (25 funciones) |

Rompe funciones que hoy casan al 100 % (`Init__19TrackCarCameraMover` 2.708→2.736,
`Blend__t6tTable1Z12CubicPovData` 780→756, `SetDesired__8ICEMoverbT1`,
`AquireCar__13CDActionDrive`, `__20SelectCarCameraMoveri`…).

**Regla nueva**: una línea única para el cuerpo de un inline en `debug_lines`
**no** implica que el cuerpo fuese una sola sentencia. Con `-gdwarf+` GCC
etiqueta las insns de una expansión inline con la línea de la DEFINICIÓN, y esa
atribución se agrupa. No sirve para reconstruir la estructura de sentencias de
una cabecera. (Esto anula, para inlines, la lectura que sí vale para código
normal en `nfsmw-mapa-de-lineas.md`.)

---

## 4. `ICEMover::Update` (3.868 B, 15 filas): 19 variantes más, y `SignedMod` es TERRITORIO

La r36e la dejó como «agotadas las cinco palancas». Faltaba un sitio: **el
cuerpo de `ICE::SignedMod`, en `Camera/ICE/ICEMath.hpp`, que sólo tiene UN
llamante en todo el árbol** (`ICEMover.cpp:675`) y por tanto se puede tocar sin
afectar a nadie. Lo barrí entero. Todo negativo.

Base: 15 filas, 3.868/3.868.

| # | dónde | qué | filas / tamaño |
|---|---|---|---|
| s1 | `SignedMod` | `asm("" : "+r"(a))` como 1.ª sentencia | 15 = base |
| s2 | `SignedMod` | `asm("" : "+r"(a))` dentro del `if` | 15 = base |
| s6 | `SignedMod` | `asm volatile("")` antes del `while` | 15 = base |
| s3 | `SignedMod` | `asm("" : "+r"(b))` | 20 |
| s4 | `SignedMod` | `asm("" : "+r"(a) : "r"(b))` | 18 |
| s5 | `SignedMod` | `asm("" : "+r"(b) : "r"(a))` | 17 |
| s7 | `SignedMod` | bucle sobre copia `m` y `return m` | **3.864** (una instrucción MENOS) |
| s8 | `SignedMod` | `if (b<=0) return 0;` y el resto sin `else` | **3.864** |
| t1/t2/t4 | `SignedMod` | bucle sobre `m` y `a = m - c` al final (3 formas) | 20 |
| t3 | `SignedMod` | `int m = a;` fuera del `if` | 3.872 |
| t5 | `SignedMod` | `for(int m=a;;)` con `break` | 35 |
| q1 | llamada | `int nkeys = pShake->GetNumKeys();` | 3.864 |
| q2 | llamada | `int fi = ICE::FloatToInt(...)` | 15 = base |
| q3 | llamada | `asm("" : "+r"(frame))` detrás | 24 |
| p7/p8/p11 | llamada | pin `register int frame asm("r7"/"r8"/"r11")` | 3.872 los tres |
| c1 | llamada | `nkeys` en local + pin de `frame` a r8 | 21 |
| c2 | llamada | pin `nkeys` a r7 | 3.864 |
| c3 | llamada | `fi` en local + pin de `frame` a r8 | 3.872 |
| c4 | llamada | `asm("" : "+r"(pKey) : "r"(frame))` | 3.872 |
| c5 | llamada | `asm("" : "+r"(pShake) : "r"(frame))` | 3.860 |
| c6 | llamada | `asm("" : "+r"(frame) : "r"(nkeys))` | 23 |

Dos datos que cambian la lectura de la r36e:

1. **s7/s8 dan una instrucción MENOS que el objetivo.** La r36e decía que al
   objetivo le sobra un pseudo respecto a nosotros; el conteo dice que la forma
   ACTUAL tiene el número correcto de instrucciones y que separar el bucle en
   una copia lo pierde. Lo que queda es reparto puro, no estructura.
2. **`regmap` con el DWARF del original** (tras reconstruir el `.o` — ojo: el
   `sweep` restaura la fuente pero no reconstruye, y medir `regmap` sobre el
   `.o` de la última variante da falsos «SOLO NUESTRA») dice:
   `frame` → **r8** en el original, **sin registro** en el nuestro (coalescido
   con la copia del bucle). El pin a r8 es la palanca teóricamente correcta y
   cuesta 4 B, así que el reparto es un SÍNTOMA: hay que arreglar antes por qué
   `cse` elige la copia como representante en `cmpwi ·,0`.

La diferencia raíz, ya alineada al registro:
`mr r11,r8` + `cmpwi r8,0` (objetivo, prueba el ORIGINAL) contra
`mr r11,r9` + `cmpwi r11,0` (nuestro, prueba la COPIA). El mecanismo es
`canon_reg`/`make_regs_eqv` de `cse.c`: el representante de la clase
{arg, `a`} es el que vive MÁS (`REGNO_LAST_UID`), y en el objetivo el que vive
más es el del argumento porque su registro se reutiliza para el valor de retorno
(`subf r8,r0,r11`). Ninguna de las 19 formas mueve esa elección.

---

## 5. `TrackCar::Update` (992 B, 15 filas): el racimo C NO es orden de fuente

Ocho variantes más, todas negativas (base 15 filas):

| # | qué | filas |
|---|---|---|
| a3 | `asm("" : "+m"(displacement))` DESPUÉS del `/=` | 15 = base |
| a6 | `bCopy(&Look, …)` explícito | 15 = base |
| a8 | puntero local a la posición, sin barrera | 15 = base |
| a1 | puntero local + `asm("" : "+r"(pos))` | **996 B** |
| a7 | `bVector3 *pd = &displacement;` + barrera | **996 B** |
| a2 | `asm("" : "+f"(distance))` antes del `/=` | 29 |
| a4 | a2 + `"+m"(Look)` | 28 |
| a5 | `displacement /= distance;` antes de `Look = …` | 31 |

Lo nuevo es el mapa de líneas del objetivo, que reparte los tres racimos:

| racimo | líneas del objetivo | qué |
|---|---|---|
| A (107-111) | `bMath.hpp:905` (stores) / `906` (cargas) | `Look = *GetGeometryPosition()` → `bCopy`+`bFill` |
| B (113-124) | `bMath.hpp:915` + `952` | `displacement /= distance` → `bScale` + el `1.0f/x` de `operator/=` |
| C (134-143) | `TrackCar.cpp:609/610/611` | los tres `TrackCarLookOffset[XYZ][CameraType]` |

**El racimo C queda descartado como causa**: el objetivo carga Z,X,Y y almacena
X,Y,Z **exactamente igual que nosotros**; sólo cambian los nombres de FPR
(f10/f11/f12 contra f12/f0/f13), y ésos vienen de qué registros deja libres el
racimo B. Es cola, no causa — igual que ya se sabía de C respecto a A.

A y B son el mismo problema y está **fuera de territorio**: los tres `stfs` de
`bFill` y los tres `fmuls`/`stfs` de `bScale` salen en orden Z,X,Y en el
objetivo y X,Y,Z / X,Z,Y en el nuestro, y el orden de emisión previo a `sched2`
lo decide la fuente de `bMath.hpp`. La §3 mide que la vía obvia (quitar las
locales intermedias) es catastrófica, así que la palanca tendrá que ser otra.

---

## 6. `TerrainVelocityNoise` (1.192 B, 13 filas): la causa, afinada — y la fantasma NO vale

La r36e decía «el objetivo gasta 4 registros para 5 direcciones y reusa r9».
Afinado con los registros a la vista:

| literal | objetivo | nuestro |
|---|---|---|
| 20.0f (`accel_max`) | `lis r9` en 115 → `lfs` en 126 | `lis r10` en 115 → `lfs` en 126 |
| 0.05f | `lis r8` en 117 → `lfs` en 132 | `lis r7` en 119 → `lfs` en 132 |
| 0.5f | `lis r10` en 119 → `lfs` en 133 | `lis r11` en 123 → `lfs` en 133 |
| 0.15f | `lis r11` en 123 → `lfs` en 136 | `lis r9` en 127 → `lfs` en 136 |
| **0.0f** (`MINIMUM` de `bClamp`) | **`lis r9` en 129** → `lfs` en 130 | **`lis r8` en 117** → `lfs` en 130 |

El orden de los cinco `lfs` es IDÉNTICO en los dos (20.0f, 0.0f, 0.05f, 0.5f,
0.15f — que es el orden de evaluación de `bMin(MAXIMUM, bMax(a, MINIMUM))`).
Lo único que cambia es **cuál de los cinco `lis` se queda pegado a su `lfs`**:

* nosotros repartimos CINCO registros (r7, r8, r9, r10, r11) y el `lis` del
  0.0f, con r8 libre desde el índice 117, lo iza `sched2` doce ranuras;
* el objetivo reparte CUATRO (r8, r9, r10, r11; **no usa r7**) y el `lis` del
  0.0f cae en r9, que sigue vivo hasta el `lfs` de 126: la antidependencia WAR
  le prohíbe subir de 127.

O sea que la palanca que hace falta es «**un registro menos disponible en ese
bloque**», y la cantidad fantasma —que es justo el idioma para eso— **no lo
consigue**: mete el rango de vida pero además hace de barrera de planificación.

| # | cantidad fantasma `register int guard asm("rN")`, cerrada sobre `f_speed_frequency` | filas |
|---|---|---|
| g6 / g7 / g9 / g10 / g12 | r6 / r7 / r9 / r10 / r12 | 20 |
| g8 / g11 | r8 / r11 | 21 |
| g7m | r7, cerrada sobre `f_speed_magnitude` | 21 |

Base 13. **Veda nueva**: la cantidad fantasma no sirve cuando lo que sobra es un
registro NUESTRO; el criterio del brief («sólo paga si al objetivo le sobra una
cantidad viva») queda confirmado por el negativo.

---

## 7. `_Storage<CameraAI::Director*,2>` (1.156 B): son **22** filas, no 26 ni 80, y hay tres causas

`fncmp` dice «80 insn» y la r36e decía 26; `fndiff` (que es el juez) da **22**.
Repartidas así:

* **(a) r24↔r25 recorriendo la función entera** — índices 8, 11, 12, 13, 17,
  159, 181, 207, 272, 275 (10 filas). El cero de `Vector::Init()` va a r25 en el
  objetivo y a r24 en el nuestro; `srcEnd` al revés.
* **(b) r0↔r9 en los índices 14-19** (6 filas). Aquí hay que corregir a la
  r36e: **el orden de los operandos es el MISMO** (`add rD, base, size<<2` en
  los dos, y las cargas `0x8(r4)` antes de `0x0(r4)` en los dos). Lo único que
  cambia es qué registro lleva `size` y cuál `base`.
* **(c) la carga izada, índices 66-72 y 128-132** (6 filas), y **no es coste
  cero repartido**: es una permuta exacta de una instrucción. El objetivo
  mantiene `mSize` vivo en **r4** y lo refresca dentro del bucle con
  `mr r4,r9` (índice 130); nosotros lo recargamos con
  `lwz r4,0x8(r31)` (índice 70) y comparamos directo con r9. Su `mr` = nuestro
  `lwz`, por eso el tamaño cuadra a 1.156 B.

Todo sale del `if (srcIt == 0)` de `UTL::Collections::Vector::assign()`
(`UTLVector.h` 130-136), **fuera de territorio**. La instrucción a perseguir es
el `mr r4,r9` del índice 130: el objetivo tiene un pseudo propio para `size()`
en la condición del `while` y nosotros lo coalescemos con el resultado de
`mSize++`.

---

## 8. Barrido de guardas de cabecera (palanca 6 del brief): **vacío en `Camera/**`**

Hecho y negativo, para que nadie lo repita. En todo
`src/Speed/Indep/Src/Camera/**` sólo hay **21** `#ifndef EA_PRAGMA_ONCE_SUPPORTED`,
las 20 guardas de inclusión, y dos condicionales reales:

* `CAMERA_HPP_NO_INIS` (`Camera.hpp:19`) — ya lo usa zSim, y encenderlo en
  zCamera apagaría los INIs que zCamera sí tiene.
* `ATTRIB_NO_INLINE_CLASSKEY` (`CameraAI.cpp:622`) — ya está definido en
  `zCamera.cpp`.

No hay ninguna guarda apagada que valga como la de `gc_interface`.

**Y una comprobada aparte**: `UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR`
(`UCollections.h:46`, la que vale 8 funciones en zSim) **NO se puede encender en
zCamera**: el `.o` objetivo SÍ trae
`_._Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2` (180 B). No lo probé
porque la tabla de símbolos ya lo descarta.

---

## 9. Lo que cae fuera de territorio (propuestas con medida)

### 9.1 `bMath.hpp` — el `// TODO is this order correct?` de la línea 197

La r36e lo dejó como «indecidible». Añado una lectura que lo confirma desde el
código: `bClamp(float) = bMin(MAXIMUM, bMax(a, MINIMUM))` emite el `lfs` del
MAXIMUM ANTES que el del MINIMUM en el objetivo (índices 126 y 130 de
`TerrainVelocityNoise`), que es exactamente lo que produce esta forma. El orden
actual es coherente con el objetivo; el TODO puede cerrarse.

### 9.2 `bMath.hpp` — NO quitar las locales intermedias de `bCopy`/`bScale`

Ver §3. Conviene dejarlo escrito en la propia cabecera: es una tentación
recurrente porque el mapa de líneas del objetivo la sugiere.

### 9.3 `UTLVector.h` — la relectura de `mSize` en `assign()`

Ver §7(c). La instrucción concreta: conseguir un `mr r4,r9` en vez de un
`lwz r4,0x8(r31)`, o sea que `size()` en la condición del `while` de crecimiento
tenga pseudo propio. Comparten esa cabecera zSim, zPhysics, zMain y zFe.

### 9.4 `.text` de más y `.data`/`.rodata` de menos (frente `linked`, no `matched`)

`seccdiff`: `+10.964` de `.text`, `−552` de `.rodata`, `−252` de `.data`,
`−12` de `.bss`.

Del `.text` sobrante identifiqué con la tabla de símbolos de los DOS `.o` un
bloque limpio de **1.600 B** que el objetivo no tiene y nosotros sí:

| símbolo | objetivo | nuestro |
|---|---|---|
| `_._Q33UTL11Collectionst8_Storage2ZP14IDebugWatchCari2` | — | 180 |
| `__Q33UTL...8_Storage2ZP14IDebugWatchCari2RCQ33UTL...` (ctor de copia) | — | 1.144 |
| `_vt.Q33UTL11Collectionst8_Storage2ZP14IDebugWatchCari2` | — | 64 |
| `__Q33UTL...8_Storage2ZPQ28CameraAI8Directori2` (ctor por defecto) | — | 36 |
| `find__H2ZPP14IDebugWatchCarZP14IDebugWatchCar_4_STL...` | 0 | 176 |

El objetivo SÍ instancia `FixedVector<IDebugWatchCar*,2,16>` (dtor 180 B) y
`Listable<IDebugWatchCar,2>::List` (dtor 180 B), pero **no**
`_Storage<IDebugWatchCar*,2>`; y de `_Storage<CameraAI::Director*,2>` trae el
dtor, la vtable y el ctor de copia pero **no el ctor por defecto**. La asimetría
apunta a qué `#include` de `Camera/**` arrastra `IDebugWatchCar` y a que el
original nunca construye un `_Storage` vacío de `Director`.

De los datos que faltan, `datadiff zCamera .data` señala como bloque grande y
limpio una tira de ~28 `3f800000` (1.0f) en `obj +0x0A90..0x0B00` y un
`3d4ccccd` (0.05f) en `+0x0B80`. No los perseguí.

---

## 10. Vedas nuevas de esta ronda (todas medidas)

| # | dónde | cambio | resultado |
|---|---|---|---|
| 1 | `bMath.hpp` | `bCopy(bVector3*)` sin locales intermedias | 10.980 → **27.968 B** pendientes |
| 2 | `bMath.hpp` | `bScale(bVector3*,·,float)` sin locales | 10.980 → **20.548 B** |
| 3 | `bMath.hpp` | las dos | 10.980 → **32.496 B** |
| 4 | `bMath.hpp` | `bVector3::bVector3(f,f,f)` sin pasar por `bFill` | objeto IDÉNTICO |
| 5 | `ICEMath.hpp` | 12 formas del cuerpo de `SignedMod` (barreras, eje, copia, retorno temprano) | 15 = base, 17-35, o 3.864/3.872 B |
| 6 | `ICEMover.cpp` | pin de `frame` a r7 / r8 / r11 | 3.872 B los tres |
| 7 | `ICEMover.cpp` | `nkeys` / `fi` en local, con y sin pin | 3.864 / 15 = base / 3.872 |
| 8 | `CameraMover.cpp` | cantidad fantasma en r6..r12 sobre el bloque de `bClamp` | 20-21 filas contra 13 |
| 9 | `TrackCar.cpp` | 8 colocaciones de barrera y reordenaciones | 15 = base, 28-31, o 996 B |
| 10 | `ICEReplay.hpp` | 6 formas del cuerpo del constructor `ReplayCategory` | 3.616-3.652 B, y **ninguna mata los `addi`** |
| 11 | `UCollections.h` | `UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR` en zCamera | descartado por la tabla de símbolos (el objetivo trae el dtor) |
| 12 | `Camera/**` | barrido de guardas de cabecera apagadas | **no hay ninguna** |
| 13 | `ICEManager.cpp` | 40 formas de la guarda/contador que NO cierran | ver §1.2 |

---

## 11. Lo que dejo abierto, por orden de premio

1. **`__static_init` (3.604 B)** — mecanismo NOMBRADO y MEDIDO (§2): hay que
   quitar ~70-85 insns de **RTL intermedio** de los inicializadores de
   `Camera/**` que van delante de `ReplayCategoryTable`. La medida cuesta 15 s
   (contar `addi rN,r30,off` múltiplos de 24 en el `-S`) y hay que llevarla de
   4 a 0. Ojo: NO es código final; el `.s` de delante ya casa 1 a 1.
2. **`ICEMover::Update` (3.868 B)** — 15 filas, permuta global del asignador.
   Agotado el interior de `SignedMod` (§4), que era el único sitio de territorio
   que quedaba. Lo que queda es forzar a `cse` a elegir el pseudo del argumento
   como representante en `cmpwi ·,0`.
3. **`TerrainVelocityNoise` (1.192 B)** — hace falta que el bloque tenga UN
   registro menos, y la fantasma no lo da (§6). Idea sin probar: reducir el
   número de literales vivos del bloque, no el de registros.
4. **`_Storage` (1.156 B)** — `UTLVector.h`, fuera de territorio (§7/§9.3).
5. **`TrackCar::Update` (992 B)** — racimos A y B, los dos en `bMath.hpp`;
   el racimo C queda descartado como causa (§5).
