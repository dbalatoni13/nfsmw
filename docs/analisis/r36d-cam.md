# r36d — zCamera: una CORRECCIÓN de mi ronda anterior y dos diagnósticos nuevos

**Estado del árbol al terminar: CERO ficheros tocados en mi territorio.**
`git status` sobre `src/Speed/Indep/Src/Camera/**` y `UTLVector.h`: vacío.
`build_direct.py zCamera` reconstruye y `triaje.py` reproduce las 7 filas exactas
del encargo. `lcfix.py --check zCamera`: *todas las entradas @lc están al día*.

**No aplico nada porque nada de lo medido gana un solo byte** (`matched_code` es
todo-o-nada y las cuatro mejores variantes se quedan entre 99,15 % y 99,48 %).
Lo que sí traigo son tres cosas que cambian el mapa:

1. **El parche de ICEMover que dejé en la r36c es fuente INCORRECTA. No lo apliquéis.**
2. `__static_initialization_and_destruction_0` **no** es lo que dije en la r36c:
   son **4 instrucciones de más** con causa nombrada, no 6 diferencias de registro.
3. `LoadCameraShakes` (que la r36b barrió y el brief descarta) tiene la causa
   localizada y pasa de 96,667 % a **99,476 % con el tamaño exacto**.

---

## 0. Verificación del encargo

Reproduce fila por fila. Siguen siendo **7**, no 6 (el brief se deja
`LoadCameraShakes`, y esta ronda resulta que era la única con una causa
estructural cerrable).

| B | función | % | difs |
|---|---|---|---|
| 3868 | `Update__8ICEMoverf` | 99,907 | 15 |
| 3604 | `__static_initialization_and_destruction_0` | 97,112 | 54 |
| 1192 | `TerrainVelocityNoise` | 99,077 | 14 |
| 1156 | `_Storage<CameraAI::Director*,2>` | 98,699 | 22 |
| 992 | `Update__19TrackCarCameraMoverf` | 99,629 | 15 |
| 948 | `Update__19TrackCopCameraMoverf` | 99,156 | 2 |
| 168 | `LoadCameraShakes` | 96,667 | 6 |

---

## 1. RETIRO el parche de `ICEMover::Update` de la r36c: el DWARF dice que NO hay `t`

La r36c cerraba el racimo 596-617 metiendo una copia de trabajo explícita
(`int t = a;`) en `ICE::SignedMod`. **El volcado DWARF trae el cuerpo del
original y ahí no existe esa local.** `symbols/mw_dwarfdump.nothpp:342171`, la
expansión inline dentro de `ICEMover::Update`:

```
// Range: 0x8007CCA8 -> 0x8007CD04
inline int SignedMod(int a, int b) {
    /* anonymous block */ {
        // Range: 0x8007CCE0 -> 0x8007CD00
        int c; // r0
    }
}
```

**Una sola local, `c`, en r0, dentro de un bloque anónimo.** Es decir: el
original es *exactamente* nuestra fuente base —`while (a < 0) a += b;` con
`int c` declarado dentro del `if (b > 0)`— y el `t` de la r36c es una local
inventada. La regla del proyecto («la local que roba el registro») vale también
al revés: **una local de más es un error de fuente aunque el porcentaje suba.**

Lo bueno: el parche tampoco ganaba nada (99,881 % y 3.872 B contra 99,907 % y
3.868 B). Lo malo: la r36c lo dejaba como «la pieza a aplicar en la r36d, está a
UNA instrucción». Habría costado una ronda entera perseguir esa instrucción.

### La ley que sí queda, medida con 14 formas

Barrí el espacio entero de formas de `SignedMod` (ensayos e1-e11, f1-f5, g1-g5,
h1-h5). **Todas caen en exactamente dos familias, y lo que decide es una sola
cosa: si el parámetro `a` se asigna en el cuerpo.**

| familia | condición | tamaño | difs | ejemplos |
|---|---|---|---|---|
| **A** | `a` **se asigna** | **3.872 B** (+4) | **3** | e1, e4, e7, f1, h5 |
| **B** | `a` **no se asigna** | **3.868 B** (exacto) | 18-19 | e5, f2, f3, g3, h1, h2, h4 |

- En la familia A sobra siempre el mismo `mr`: la copia de ligadura que
  `expand_inline_function` inserta porque el formal se modifica.
- En la familia B el tamaño es exacto y **todas las instrucciones están**: es una
  permutación pura, pero los papeles están cambiados (el bucle corre sobre el
  registro cargado y el resultado va a la copia; el objetivo hace lo contrario).

Ninguna de las dos es la fuente del original, que es la base (15 difs, 3.868 B).

### Y lo que sí es la base: `frame` r8 contra r9, y `regmap` mintiendo

Con la fuente correcta, `regmap` dice que la única local con registro distinto en
`ICEMover::Update` es **`frame`: original r8, nuestro r9**. Las otras tres que
marca son **falsos positivos del emparejamiento posicional**:

```
fn   bLerpLag / nState    r27  r26   <-- DISTINTO
fn   n_state / bGeneric   r26  r27   <-- DISTINTO
```

El DWARF del original (`:341565`) lista `bool bLerpLag; // r27` y
`int n_state; // r26`. Nosotros tenemos `bGeneric` en r27 y `nState` en r26. O
sea: `bGeneric↔bLerpLag` (r27=r27) y `nState↔n_state` (r26=r26), **los dos
coinciden**. `regmap` los cruzó porque empareja por POSICIÓN cuando los nombres
difieren, y aquí el orden de los DIE no coincide con el de la fuente.

> **Regla nueva para todo el proyecto:** cuando `regmap` marca dos filas
> «DISTINTO» con nombres distintos a ambos lados y los registros forman un
> intercambio limpio, **comprueba a mano contra el volcado DWARF antes de
> creértelo**. Aquí me habría mandado a pinchar dos locales que ya estaban bien.

El desajuste real es de todo el bloque: el objetivo usa r7 siete veces y r8
dieciséis; nosotros r7 cinco y r8 nueve, y empujamos a r9/r10/r11. Es orden de
`allocno_compare`, no una local. El pin sobre `frame` está medido negativo tres
veces (vedas 1, 3 y 4 de la r36c) y no lo repito.

---

## 2. `__static_init` (3.604 B): NO son 6 diferencias de registro. Son **4 instrucciones de más**, y valen los 16 B enteros

La r36c describió esta función como «racimo A de 6 filas de registro + racimo B
de `cse2`, sin palanca de fuente». **Eso es falso, y lo delata el tamaño:
3.604 B el objetivo, 3.620 B el nuestro. Sobran 16 B.**

Los 16 B son exactamente cuatro `addi`, y salen todos del mismo sitio:
`ICE::ReplayCategoryTable[6]` en `Camera/ICE/ICEReplay.cpp:171`.

```
objetivo:  stw r9, 0x40(r30) / stw r11, 0x44(r30) / stw r29, 0x30(r30) ...
nuestro:   addi r27, r30, 0x30
           stw r28, 0x8(r27) / stw r11, 0x10(r27) / stw r9, 0x14(r27) ...
```

El objetivo direcciona los seis campos de cada elemento con el desplazamiento
constante metido en el `stw`; nosotros materializamos `&tabla[i]` en r27 y
almacenamos relativo a él. Pasa con los elementos **2, 3, 4 y 5** (`addi r27,
r30, 0x30 / 0x48 / 0x60 / 0x78`); los elementos **0 y 1 son idénticos byte a
byte**. Cuatro `addi` × 4 B = **los 16 B, exactos**.

Y no es un detalle menor: **40 de las 54 filas de diff son este racimo**. El
«racimo A» de la r36c (filas 549-564, `HydraulicsLookAngle`) son las otras 6.

### Lo que sé del mecanismo

- El flag está: los cflags de zCamera llevan **`-fforce-addr`** (y `-ffast-math`).
  `-fforce-addr` es lo que fuerza la dirección constante `&tabla[i]` a un
  registro.
- **No es presión de registros ni forma del constructor.** Escribir el
  constructor como cuerpo de asignaciones en vez de lista de inicialización da un
  **binario idéntico** (ensayos p0/p1) — dicho sea de paso, eso invalida el
  comentario que hay en `ICEReplay.hpp:37-40`, que afirma que el original usa
  cuerpo y no lista: *no se puede distinguir*.
- **Trazado de r27**: hasta el índice 704 los dos lados tienen r27 *idéntico*
  (mismas 8 definiciones, mismo último uso en `JumpToPosition`). A partir de ahí
  el objetivo no vuelve a tocarlo y nosotros lo redefinimos cuatro veces.
- **Es una navaja, y medí el filo**: inyectando N inicializadores dinámicos
  ficticios justo antes de la tabla, los cuatro `addi` **desaparecen a partir de
  N = 4** y no antes:

  | dummies | tamaño | `addi` de base de tabla |
  |---|---|---|
  | 0 | 3.620 | **4** |
  | 1 | 3.640 | 4 |
  | 2 | 3.664 | 4 |
  | 3 | 3.684 | 4 |
  | **4** | 3.704 | **0** |
  | 8 / 16 / 32 | 3.784 / 3.924 / 4.244 | 0 |

  O sea: el plegado del desplazamiento depende del estado de `cse` en ese punto
  de la función, con un umbral duro. Es la misma familia que el «racimo B» de la
  r36c, pero ahora con un coste medido (16 B) y una diana concreta.

### Lo que NO probé y le toca al siguiente

El umbral de 4 dice que lo decide **cuánta inicialización dinámica va delante de
`ReplayCategoryTable`**, y eso lo fija el **orden de la SourceList**
(`ICEReplay.cpp` es el último del grupo ICE, línea 86 de `zCamera.cpp`).
**No lo toco: está fuera de mi territorio.** Queda como propuesta explícita, y es
comprobable en un solo build.

### El racimo A (6 filas) sigue cerrado, y ahora con el argumento que faltaba

Filas 549-564: el objetivo emite `li r9,0x38e` (el VALOR de
`HydraulicsLookAngle = bDegToAng(5.0f)`) y luego la dirección; nosotros al revés,
con el `lis` del literal flotante $LC1125 colándose en medio. Mismo número de
instrucciones: empate de `sched2` dentro de un bloque de ~900.

La r36c decía «no hay palanca de fuente». Lo confirmo con el argumento que le
faltaba: **estas sentencias son inicializadores estáticos a nivel de namespace,
así que no se les puede meter un `asm` de barrera** — un `asm` ahí es
`asm` de nivel superior y se emite literalmente. La barrera de ranura, que es la
palanca que funciona para este tipo de empate (ver §3), **no tiene dónde
colocarse**. Racimo A: cerrado de verdad.

---

## 3. `TrackCop::Update` (948 B, 2 difs): la barrera de ranura **funciona**, pero el empate no cierra

`regmap` da el veredicto más limpio que he visto: **mismo conjunto de locales,
mismo árbol de bloques y MISMO REPARTO** (15 iguales, 0 distintas). Todo el diff
es una instrucción mal colocada:

```
objetivo:  addi r3,r1,0x68 / mr r5,r3 / stfs f30,0x58(r1)
nuestro:   addi r3,r1,0x68 / stfs f30,0x58(r1) / mr r5,r3
```

**La barrera de ranura sí mueve la aguja aquí** — es la primera vez que se
demuestra sobre un empate de `sched2` en zCamera. `asm("" : "+m"(hcomp));`
colocada **antes** de `bScale(&hcomp, &hcomp, vert_comp)` (ensayo k1) coloca el
`stfs` en su sitio... y desplaza otra:

```
k1:  ... mr r5,r3 / stfs f30,0x58 / [addi r4,r4,0x48] / stfs f30,0x5c ...
     el objetivo mete el addi UNA ranura más tarde
```

Barrí 12 colocaciones (k1-k5, m1-m8, n1-n3). **Todas se quedan en 2 difs**; lo
único que cambia es cuál de las tres instrucciones de cálculo de dirección
(`stfs f30,0x58`, `addi r4,r4,0x48`, `addi r3,r1,0x68`) queda descolocada. El
presupuesto de ranuras está desfasado en una y la barrera sólo mueve la frontera.

**Y una veda cara**: `asm("" : "+f"(vert_comp))` **rompe el plegado a cero**
(`-ffast-math` convierte `hcomp * 0.0f` en tres `stfs f30`): el marco pasa de
0xf8 a 0x108 y la función de 948 a 976 B. **Nunca pongas una barrera sobre una
constante que el compilador está plegando.**

---

## 4. `LoadCameraShakes` (168 B): causa encontrada — **un store muerto que ellos NO borran**

El brief la descarta y la r36b la barrió. Es la única de las siete con una causa
estructural nombrada, y **estábamos 4 B CORTOS** (168 objetivo / 164 nuestro).

```
objetivo:  lwz r28,0x8(r29) / li r0,0x0 / addi r30,r29,0xc / li r27,0x0
           cmpw r0,r28 / ... / addi r27,r27,0x1 / cmpw r27,r28
nuestro:   lwz r27,0x8(r29) / li r28,0x0 / addi r30,r29,0xc
           cmpw r28,r27 / ... / addi r28,r28,0x1 / cmpw r28,r27
```

El original materializa **dos** ceros: el contador `i` y otro más, y la guarda de
entrada del bucle compara contra ese otro (`cmpw r0, r28`) en vez de reutilizar
el contador. Nosotros fundimos los dos.

**El DWARF dice qué es ese segundo cero** (`:344295`):

```
void ICEManager::LoadCameraShakes(struct bChunk * set_chunk /* r29 */) {
    unsigned int id;
    bool warned_overflow; // r0        <-- TIENE REGISTRO
    struct bChunk * chunk;
```

`warned_overflow` está **vivo en r0** en el original. En nuestra fuente
(`ICEManager.cpp:963`) se le asigna `false` y **no se lee nunca**, así que GCC
borra el store muerto. El del original no lo borró, y `cse` reutilizó ese r0 como
el `0` de la guarda del bucle. Ese es el `li` que nos falta, y de ahí salen las 6
filas.

### La mejor variante medida: 96,667 % → **99,476 % con el tamaño exacto**

```cpp
ICEShakeGroup *group = pShakeGroup;
if (group) {
    warned_overflow = false;
    int num_tracks = *p_handle;
    ICEShakeTrack *track = ...;
    for (int i = 0; i < num_tracks; i++) { ... }
    asm("" : : "r"(warned_overflow));      // <- barrera de SÓLO LECTURA
}
```

**168/168 B y 4 filas.** Lo único que queda es que `warned_overflow` aterriza en
**r26** (preservado) en vez de r0, y eso cambia `stmw r27,0xc` por
`stmw r26,0x8` y el `lmw` de vuelta.

### Por qué no cierra, y la regla que se lleva

Probé la barrera en las seis posiciones posibles (r1-r6, t1, t2):

| posición | resultado |
|---|---|
| tras la asignación, `"+r"` | **binario idéntico** — un `asm` NO volátil con salidas muertas se BORRA entero |
| tras el bucle, `"+r"` | 91,667 % |
| **tras el bucle, sólo lectura** | **99,476 %, 168/168, 4 difs** ← mejor |
| al inicio del cuerpo del bucle | 98,643 %, 168/168, 8 difs |
| al final del cuerpo del bucle | 89,595 % |
| justo antes del `for` | 94,643 %, 168/168, 18 difs (aparece el `li r0,0x0` bueno, pero se cruzan r29/r31) |
| tras `num_tracks` | 93,048 %, 172 B |

> **La regla:** el original tiene `warned_overflow` **muerto pero no borrado**.
> Una barrera, por definición, lo pone **vivo** — y vivo cruzando el bucle exige
> un registro preservado. **«Muerto pero no borrado» no es reproducible con una
> barrera.** Es el techo de esta palanca y conviene tenerlo escrito: aparecerá
> cada vez que al original le sobreviva un store inútil.

Y una veda de pin: `register ICEShakeGroup *group asm("r31")` (ensayo s1)
**miscompila** — emite `stw r1, 0x8(r30)` y `addi r31, r1, 0x4`, o sea usa el
puntero de pila como si fuera el objeto. Confirma «el pin no FIJA el registro,
lo sugiere», ahora con un ejemplo de corrupción real y no sólo de porcentaje.

---

## 5. Vedas nuevas de esta ronda (todas medidas)

| # | función | cambio | resultado |
|---|---|---|---|
| 1 | `SignedMod` | **el parche entero de la r36c** | fuente incorrecta: el DWARF no tiene `t` |
| 2 | `SignedMod` | `asm("" : "+r"(a))` al principio | binario idéntico a e1 |
| 3 | `SignedMod` | `asm("" : "+r"(t))` tras `int t = a` | 3.864 B, 19 difs |
| 4 | `SignedMod` | `int r = a; int t = a;` (r primero) | 3.872 B / 3 difs (familia A sin mutar `a`) |
| 5 | `SignedMod` | `int t = a; int r = a;` (t primero) | 3.868 B / 18 difs (familia B) |
| 6 | `SignedMod` | `register int t asm("r11")` | 3.864 B, borra el `mr` bueno |
| 7 | `SignedMod` | `asm("" : : "r"(a))` (barrera de lectura) | 98,16 %: destroza el planificador 8 filas antes |
| 8 | `ICEMover.cpp` | `int nkeys = GetNumKeys()` izado | 3.864 B, borra el `mr` |
| 9 | `ICEMover.cpp` | `asm("" : "+r"(frame))` entre las dos sentencias | binario idéntico |
| 10 | `ReplayCategory` | constructor con cuerpo en vez de lista de init | **binario idéntico** |
| 11 | `ReplayCategory` | cuerpo con `pScore`/`pMirror` primero | mata los 4 `addi` pero 118 difs |
| 12 | `TrackCop` | `asm("" : "+f"(vert_comp))` | rompe el plegado `x*0` de `-ffast-math`: +28 B |
| 13 | `TrackCop` | 12 colocaciones de barrera de ranura | **todas 2 difs**; sólo cambia cuál se descoloca |
| 14 | `LoadCameraShakes` | `asm("" : "+r"(x))` con salida muerta | se borra el `asm` entero, binario idéntico |
| 15 | `LoadCameraShakes` | `register ... group asm("r31")` | **MISCOMPILA** (`stw r1, 0x8(r30)`) |
| 16 | `regmap` | fiarse de dos filas «DISTINTO» con nombres distintos | falso positivo: empareja por posición |

---

## 6. Lo que dejo abierto, por orden de premio

1. **`__static_init` — los 4 `addi` de `ReplayCategoryTable` (16 B, 40 de 54
   filas).** Es la única diferencia **estructural** que queda en zCamera y tiene
   umbral medido (4 inicializadores dinámicos delante lo apagan). La palanca
   probable es el **orden de la SourceList** (`ICEReplay.cpp` va el último del
   grupo ICE): fuera de mi territorio, se comprueba en un build.
2. **`LoadCameraShakes` (168 B)**: a un registro. Hace falta que
   `warned_overflow` esté en r0 y **muerto** antes del bucle. La barrera no
   sirve (pone vivo). Ideas sin probar: que el original tuviera un `#if`
   compilado a nada que lo leyera, o un flag de dead-store.
3. **`ICEMover::Update` (3.868 B)**: fuente CORRECTA y confirmada por DWARF; los
   15 difs son reparto de `frame` (r8 contra r9) arrastrando r7/r8→r8/r9. Pin
   medido negativo tres veces. Aquí sí toca el **permutador ciego**, que nunca se
   le ha pasado a esta función (la r36b gastó 393 variantes del **guiado** sobre
   un cambio que ahora sabemos que era la fuente equivocada).
4. **`TrackCop` (948 B)**: 2 difs, empate de `sched2` con 12 colocaciones
   agotadas. Techo de la barrera de ranura.
5. **`bMath.hpp` `bCopy`/`operator/=`** (orden z,x,y contra x,y,z): sigue
   señalado desde la r36b para `TrackCar`. Fuera de territorio.
