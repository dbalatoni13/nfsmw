# Ronda 18 — zCamera + zFEng

Base `base_r18_cam.json`: 183.224/196.468 B, **93,2590%**, 787 funciones.
Final `despues_r18_cam.json`: 184.000/196.468 B, **93,6539%**, 788 funciones.
**+776 B, +1 función.** `pctsnap --cmp`: **EMPEORAN: ninguna**, mejora 1
(`Update__7FEnginelUi` 99,4588 → 100,0000).

El «antes» del `pctsnap` se reconstruyó revirtiendo a mano mis cuatro ficheros
(otro agente ya los había commiteado en `85c36e14`); la reconstrucción mide
183.224 B exactos, o sea es fiel.

## 1. `audit.py` — los 16+2 fallos

**zCamera: 16 de 446** al 100% de objdiff fallaban. **zFEng: 2 de 341.**
Clasificados y resueltos:

| | |
|---|---|
| **2 bugs reales, zCamera** | `RightToLeftMatrix4` **cruzada** entre `CDActionDebug::GetTrafficBasis` y `CDActionDebugWatchCar::GetTrafficBasis` |
| **1 bug real más, zCamera** | destapado al arreglar el anterior: `RightToLeftVector3` también cruzada en las dos |
| **2 bugs reales, zFEng** | `__builtin_vec_new` donde el objetivo llama a `__builtin_new` |
| **13 falsos positivos** | reubicaciones a `_.tmp_N` / `name.N` / `k.N`: guardas **en .bss** de estáticas locales de función; el índice lo pone el compilador |
| **1 falso positivo** | `__13CDActionDrive...`: `$LC615` **lleva una reubicación dentro**, en +4, a `MessageJumpCut__13CDActionDrive` (0x8006D74C). `audit.py` compara los bytes crudos del literal y no resuelve las reubicaciones internas |

Tras arreglar los reales: **zFEng 342/342 limpio**, zCamera 432 ok + 14 falsos
positivos (los 13 `_.tmp` + `$LC615`).

### Las plantillas cruzadas (zCamera)

El objetivo instancia `RightToLeftMatrix4<UMath::Matrix4,UMath::Matrix4>` en
`CDActionDebug` y `<bMatrix4,UMath::Matrix4>` en `CDActionDebugWatchCar`;
nosotros al revés. Las dos daban **100%** en objdiff porque
`function_reloc_diffs=none` compara la grafía y no el destino.

- `CDActionDebugWatchCar`: el objetivo **no lleva ningún cast** (`GetMatrix()`
  ya devuelve `const bMatrix4*` y `GetVelocity()` `const bVector3*`).
- `CDActionDebug`: `world` **sigue siendo `bMatrix4`** y el cast va en la
  llamada. Ensayos:
  - c1 `UMath::Matrix4 world` + cast en `eInvertTransformationMatrix` → **84,58%**:
    `&world` deja de ser una subexpresión común, se pierde el `addi r30,r1,8`
    único y sobran dos `addi r3,r1,8`.
  - c2 `bMatrix4 world` + `*reinterpret_cast<const UMath::Matrix4*>(&world)` →
    **100%**, instrucción por instrucción.  ← se queda

### El `operator new` global (zFEng)

`bWare.hpp` define `inline void *operator new(size_t) { return ::operator new[](size); }`
para que los 623 sitios del juego salgan como `__builtin_vec_new`. Medido con
el compilador: **con ese en línea en ámbito NINGUNA forma de fuente da
`__builtin_new`** — ni `::new T`, ni `::operator new(sizeof(T))`, ni `new T[n]`
(las cuatro dan `vec_new`). Y el `::operator new(size_t)` global **se llama
`__builtin_new` en el ensamblador** cuando no se expande.

Los **únicos tres** sitios de `new` de la unidad están en `FEPackageReader.cpp`,
y el objetivo emite `__builtin_new` en dos (`ReadReferencedPackagesChunk`,
`ReadObjectTags`) y `__builtin_vec_new` en el tercero (`ReadScriptTags`, que es
un `new FEKeyTrack[n]` de verdad). O sea: **la unidad del objetivo no tiene ese
en línea en ámbito**. `#define BWARE_NO_INLINE_GLOBAL_NEW` en la SourceList
(la guarda ya existía en la cabecera) deja los tres sitios exactos, y no puede
tocar nada más porque no hay más `new` en la unidad.

## 2. `Update__7FEnginelUi` — CERRADO (+776 B)

Único defecto: el objetivo funde `mr. r28,r27`; nosotros dábamos
`mr` + `cmpwi`. Ensayos (compilador directo sobre `FEngine.cpp` suelto, 10 s
cada uno; reproduce el objeto de la SourceList al bit):

    d1 base (barrera al final, asignación dentro del if)  195 insns, sin mr.
    d2 negación `if (!(...))`                             195, sin mr.
    d3 dos sentencias, barrera al final                   195, sin mr.
    d4 dos sentencias, barrera tras `iIterationTicks = 0` 194, mr. 27,28 (registros AL REVÉS)
    d5/d6 sin barrera al final, barrera arriba            195, sin mr.
    d7 dos sentencias + `if (!iTicksRemaining)`           195, sin mr.
    p1..p4 las cuatro posiciones de la barrera dentro del for -> todas como d4
    sin barrera ninguna                                   191, registros BIEN, salida plegada
    f5 barrera con DOS restricciones                      194, mr. 28,27 + registros bien  <- se queda

El DWARF confirma `iTicksRemaining` → **r28**. La barrera sobre
`iIterationTicks` sola le sube el peso de reparto por encima de
`iTicksRemaining` y los intercambia; la segunda restricción (vacía, no emite
ninguna instrucción) hace de sustituta de las referencias a `iTicksRemaining`
que tienen las sentencias 745-748 del original, que **siguen sin encontrarse**.

## 3. `__static_initialization_and_destruction_0` (3.604 B, 97,112%) — NO cerrada

**Lo que queda es UNA sola cosa**: en el original los seis elementos de
`ICE::ReplayCategoryTable` se escriben con `0xNN(r30)`; nosotros materializamos
`addi r27,r30,0x30/0x48/0x60/0x78` para los elementos **2..5** (0 y 1 sí
pliegan). Son los 4 `addi` que sobran, y **no hay ninguna otra diferencia real
en las 901 instrucciones**: lo demás del diff normalizado es `lbl_` contra
`$LC`, el formato de las ramas y la permutación de registros que causan esos
cuatro `addi`.

### La pista de «expandimos cabeceras distintas» está MUERTA — medida

Escribí un lector de la tabla de líneas DWARF1 (`.line` + `.debug_srcinfo` +
`.debug_sfnames`; cabecera de `.debug_srcinfo` de **20 bytes**, no 8) para
sacar nuestro censo, y lo crucé con `debug_lines.txt`:

| fichero | original | nuestro |
|---|---|---|
| bMath.hpp | 247 | 309 |
| UCOM.h | 49 | 31 |
| Table.hpp | 40 | 57 |
| **ICEMath.hpp** | **15** | **0** |
| **CubicTweaks.hpp** | **13** | **0** |
| **Still.cpp** | **6** | **0** |
| **CameraNoise.hpp** | **2** | **0** |
| Cubic.cpp | 5 | 18 |

Parece código que falta y **no lo es**: `lmap` enseña que `ICEMath.hpp:35` son
`vIceAccelLagScale/Min/Max`, `Still.cpp:49-51` son `StillEyeTweak/LookTweak/UpTweak`,
`CameraNoise.hpp:61` es `CameraNoiseSpeedData` y `CubicTweaks.hpp:48/54/333/390/430`
son `aDriftData`, `gDriftSpeed`, `aCubicPovTables`, `CameraSpeedHugData` y
`vCubicBirdsEyeOffset` — **todas están en nuestro asm también**. Es sólo que EA
las tenía en cabeceras que nosotros hemos fundido en los `.cpp`. Y como
nuestras instrucciones son las mismas menos los 4 `addi`, el censo **no puede**
significar «expandimos más código»: sólo mide densidad de anotación. **Frente
cerrado.**

### Lo que sí se probó

- La forma del constructor `ICE::ReplayCategory` es **correcta**: un repro
  mínimo (la clase + la tabla de 6 + funciones tontas) compilado con los cflags
  de zCamera reproduce el original **instrucción por instrucción**, incluido el
  orden `nombre, score, mirror, escena, hash(escena), hash(nombre)`. O sea, la
  divergencia es de **contexto**, no de fuente.
- Variante con las seis asignaciones en el cuerpo en vez de lista de
  inicialización: el repro mínimo también casa, y en la unidad completa deja
  los cuatro `addi` **igual** (97,112%). Descartada.
- El árbol de inlines del original (`plan.py --dwarf-only`) dice que el ctor se
  llama `ICEReplayCategory(char*, char*, float(*)(ICEAnchor*), bool(*)(ICEAnchor*))`
  — **`char*`, no `const char*`**. No probado si eso mueve algo.

### Qué NO he probado

- Cambiar los parámetros del ctor a `char *` (lo que dice el DWARF).
- Leer nuestras expansiones inline del `.debug` (DWARF1) y compararlas con las
  del volcado del original una a una. Es la única medida que queda que puede
  encontrar de dónde salen los pseudo-registros de más; el censo de líneas no
  sirve para eso.

## 4. `_Storage<Director*,2>` (1.156 B) — 96,159 → 97,785% y **REVERTIDO**

Tres cambios en `Vector::assign` (`UTLVector.h`), todos leídos del mapa de
líneas del original, no adivinados:

- **h4**: el cuerpo del bucle de copia son **tres** sentencias, no una. El
  original tiene notas en 328 (`mr r11,r10` + `addi r10,r10,4`), 329
  (`mr r9,r27` + `addi r27,r27,4`) y 330 (`lwz`/`stw`): o sea
  `iterator dest = destIt++; const_iterator src = srcIt++; *dest = *src;`.
  Solo esto: 89,97 → **92,41%** (métrica de asm) y 96,159 → 97,3 en objdiff.
- **h5**: `if (minSize > size())` / `while (minSize > size())` en vez de
  `size() < minSize` — el original emite `cmplw r26,r4; bgt`, nosotros
  `cmplw r0,r26; bge` y encima recargamos `0x8(r31)`.
- **h6**: `while (end() != destIt)` en el bucle de `pop_back`.

**h4+h5+h6 medido de verdad con objdiff: 96,159 → 97,785%**, y el resto es
`falta 1 mr., sobran 1 lwz + 1 cmpwi`.

Descartados con cifra: h1 (voltear el `while` de `resize`) 89,97 (sin efecto),
h2 (que `assign` llame a `resize()`, que es lo que dice el mapa de líneas:
el original entra en algo en `UTLVector.h:241` cuyo bucle de crecimiento está
en 249) **85,37% y 296 instrucciones — 7 de más, el `else` con `pop_back` no
se pliega**, h3 (calcular `minSize` antes de `srcIt`, que **sí** produce el
`mr.` que falta) 96,557% real porque desplaza un registro y cuesta más de lo
que gana, i7/i8 (`minSize` local a cada rama) 90,00%.

**Revertido**, y la razón importa: `UTLVector.h` la incluye el árbol entero,
97,785% aporta **cero bytes** (`matched_code` es todo-o-nada), y en el triaje
de la ronda 17 **esta es la ÚNICA near-miss de `assign`/`_Storage` de todo el
árbol** — o sea el único efecto posible del cambio en las demás unidades es
romper instanciaciones que ya están al 100%. Queda anotado para cuando alguien
pueda pagar el A/B completo por objetos.

## 5. `LoadCameraShakes` (168 B) — no cerrada, pero el DWARF da la forma

Falta un `li r0,0` y están intercambiados `i` y `num_tracks`. El DWARF del
original dice:

    void ICEManager::LoadCameraShakes(bChunk *set_chunk /* r29 */)
      locales de FUNCION:  id (pila), warned_overflow (r0), chunk (pila)
      bloque anonimo:      p_handle (pila), group (r31)     <- una LLAVE de mas
        bloque anonimo:    num_tracks (r28), track (r30)
          bloque anonimo:  i (r27)

Nosotros damos `num_tracks`→r27 e `i`→r28, justo al revés, y no tenemos
`warned_overflow` (que es el `li r0,0` que falta), ni `id`, ni `chunk`, ni la
llave de más alrededor de `p_handle`/`group`.

**Resultado negativo nuevo y medido**: la vía de «mantener viva la local con un
uso» **no vale con una restricción de asm vacía de entrada**. Probado
`bool warned_overflow = false; asm("" ::"r"(warned_overflow));`: GCC 2.9
**borra el asm entero** y el ensamblador sale byte a byte idéntico al de
partida. Probadas también las variantes con `id`/`chunk` declaradas y con la
llave de más (j2..j5): las cuatro dan **exactamente el mismo asm** que la base.
Hace falta un uso **real** de `warned_overflow`, y el que sea tiene que
plegarse a cero instrucciones.

## Notas de herramienta

- **`scripts/frozen.py` no existe** en el árbol. No he podido congelar nada.
- `audit.py` peta con `KeyError: 'address'` en zFEng (símbolos sin `address`
  en el JSON de objdiff). Se arregla con `L.get('address') or 0`; sin eso la
  auditoría de zFEng se corta y **no se ven los dos fallos reales**.
- `audit.py` sólo audita funciones que objdiff da al **100%**, así que al
  arreglar un fallo la función puede caer del 100 y salir del radar: hay que
  volver a pasarlo tras cada arreglo (aquí destapó la segunda plantilla
  cruzada).
- `audit.py` da falso positivo cuando el literal lleva una **reubicación
  dentro** (`$LC615`), porque compara bytes crudos del `.o` contra el ELF
  enlazado.
