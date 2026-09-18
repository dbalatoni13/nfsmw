# Ronda 30 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes de `.text` ganados: 0.** El árbol queda **byte a byte como lo encontré**:
`git status src/Speed/Indep/Src/Camera/ICE/` y `.../Movers/` vacíos,
`src/Speed/Indep/bWare/` vacío, y los md5 de la r28/r29 intactos (`ICEMover.cpp`
`edc3423a…`, `ICEMath.hpp` `ec9d9c20…`, `ICEManager.hpp` `4bf44755…`,
`TrackCop.cpp` `f7c2a733…`, `TrackCar.cpp` `2e4f72e0…`, `bMath.hpp` `4190acb3…`).
Todos los ensayos se hicieron con el directorio-sombra en el scratchpad.
`audit.py`: **446/446 ok, CERO FALLA**, dos pasadas **byte a byte idénticas**.

**Aviso de convivencia (no es mío):** durante mi sesión otro agente de la ronda
tocó **tres ficheros de `Src/Camera/`** — `CDActionDebugWatchCar.cpp`,
`CDActionDrive.cpp` y `CameraAI.cpp` (`int X;` → `int X = 0;`, frente `.data`).
Lo detecté por `git status` + mtime 18:11. **Volví a medir base, i4 y e25
seguidas después**: las cinco funciones vigiladas dan **exactamente las mismas
cifras**; sólo cambia el total de la unidad en el arnés (615/627 → 621/633).
Ninguna medida de este informe está contaminada.

Lo que traigo:

1. **UN FALLO DEL ARNÉS QUE INVALIDA PARTE DE LA R29 Y REABRE UN ESPACIO
   ENTERO**: el directorio-sombra **NUNCA veía `ICEMath.hpp`**. Un `#error`
   dentro compila sin rechistar. Los ensayos `i7`/`c1` de la r29 (y todo lo que
   tocara ese fichero) **midieron la base, no el cambio**. (§1)
2. **`ICEMover::Update` NO es un muro del asignador: la fuente de
   `ICE::SignedMod` está MAL, y se demuestra con los operandos.** El objetivo
   emite `subf r8, r0, r11` (destino ≠ fuente); nuestro `a -= c;` genera
   `(set (reg 1045) (minus (reg 1045) (reg 0)))`, **un solo pseudo**, que por
   construcción sólo puede emitir `subf rN, rM, rN`. **Ninguna barrera, ningún
   pin y ningún reparto de registros puede cerrar esa fila.** (§2.2)
3. **El `.greg` que pedía el encargo, con nombres y cifras**: `1036` (`nf`, el
   operando de la barrera) **SÍ llega a `global_alloc`** (puesto 111 de 147,
   pri 2666, 4 refs / 30 insns) y coge **r8**; `1045` (`a`+`frame`, un pseudo
   con **4 escrituras**) es el puesto 5 (pri 19090) y coge **r11**. (§2.1)
4. **`static-init` (3.604 B): la fuente de `ICEReplay.hpp`/`.cpp` es CORRECTA y
   el fallo es del CONTEXTO.** Y **la hipótesis del prefijo de la r21/r22/r25
   queda muerta también por el lado de las insns REALES**: +100 globales reales
   (+505 insns) delante del static-init dejan los **cuatro `addi` en los mismos
   cuatro desplazamientos**. (§3)
5. **El censo de registros del static-init**: los 3.604 B enteros dependen de
   **una sola decisión** — si el `this` de los elementos ≥ 2 se queda en r27.
   Todos los registros coinciden salvo r9/r11 (±2, el racimo 549-564) y
   r27/r30 (±24, exactamente los 4 `addi` + 20 tiendas). (§3.3)

---

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py --muro` + `measure.py`, al empezar y al
terminar, idénticos al brief:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28Camer faltan 1, sobran 2, 1 SUST
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1
    MURO: 3868 ICEMover (15) / 1192 TerrainVelocityNoise (14) / 992 TrackCar (15) / 948 TrackCop (2)
    measure.py zCamera   113080/125008 B  90,4582 %  446 al 100 %

El arnés heredado (`c30cam_unit.py`, de la r28/r29) reproduce la base al dígito:
ICEMover 15 diffs, TVN 14, TrackCop 2, TrackCar 15, static-init 54. Y reproduce
`i4` (10 diffs, 99,93278 %) exactamente como lo dejó la r29.

---

## 1. EL FALLO DEL ARNÉS: el directorio-sombra no veía `ICEMath.hpp`

### 1.1 La prueba

Parche `probe`: meter `#error SHADOW_ICEMATH_ACTIVA` en `ICEMath.hpp`.

    probe   19.1s  ... 615 fn 100%  ficheros:ICEMath.hpp
        Update__8ICEMoverf   3868 B  99.90693%  diffs=15     <-- COMPILA. Y da la BASE.

**El `#error` no salta.** El objeto de `c1` (mi primer ensayo, `return a - c;`)
salió **md5 idéntico al de la base, debug incluido**: no es que el cambio fuera
neutro, es que **el cambio no se compiló**.

### 1.2 La causa, y es general

    src/Speed/Indep/Src/Camera/ICE/ICEData.hpp:8:  #include "ICEMath.hpp"

Un `#include` **entrecomillado y relativo** se resuelve **primero contra el
directorio del fichero que incluye**, antes que contra los `-I`. `ICEData.hpp`
vive en el árbol real, así que abre el `ICEMath.hpp` **real**, fija su guarda de
inclusión, y la copia parcheada del `-I` no se abre nunca. Todos los demás
ficheros lo incluyen por ruta completa (`Speed/Indep/Src/Camera/ICE/ICEMath.hpp`)
y ésos sí habrían funcionado — pero `ICEData.hpp` entra antes en el orden del
SourceList.

`ICEManager.hpp` **no** está afectado (siempre por ruta completa): por eso el
`i19` de la r29 sí movió el resultado.

### 1.3 El arreglo, y la regla para todos

En `c30cam_unit.py` añadí una tabla de **compañeros obligatorios**: si el parche
toca `ICEMath.hpp`, se copia **también** `ICEData.hpp` (sin tocar) dentro de la
sombra, y así su `#include "ICEMath.hpp"` resuelve dentro de la sombra. Con eso
el `#error` salta.

> **Regla para `docs/HERRAMIENTAS.md`:** un arnés de directorio-sombra con `-I`
> **no garantiza** que el fichero parcheado se use. Cualquier `#include "X.hpp"`
> **relativo** en el árbol real gana. **Antes de fiarte de un barrido sobre una
> cabecera, mete un `#error` y comprueba que la compilación FALLA.** Cuesta 20 s
> y aquí ha costado una ronda entera de ensayos falsos.

### 1.4 Qué queda invalidado

- **r29 `i7`** («i4 + SignedMod `return a - c;` / `return 0;` → = i4»): **falso,
  no se compiló**. Con el arnés arreglado, ese cambio **no** da i4: da 17 diffs
  y una estructura distinta (§2.3).
- Cualquier ensayo anterior de cualquier ronda que tocara `ICEMath.hpp` con este
  arnés. `ICEManager.hpp` y `ICEMover.cpp` no están afectados.

---

## 2. `ICEMover::Update` (3.868 B): no es el asignador, es `SignedMod`

### 2.1 El `.greg` del racimo 602-617 — lo que pedía el encargo

Volcado con `cc1plus -dl -dg` sobre `ICEMover.cpp` suelto (reproduce el objeto
de la unidad: mismo `subf 11,0,11`, y con el parche `i4` las mismas 5 filas que
cambian). `scripts/alloc.py` sobre `void ICEMover::Update(float)`:

    ;; pseudos en .lreg: 480 | asignados por LOCAL-alloc: 333 | llegan a GLOBAL-alloc: 147

     #    pseudo  n_refs  live_len  pri     confl  reg
     5    1045    14      22        19090   27     r11   <- `a`+`frame`  (user var, 4 SETs)
     6    1044    8       13        18461   26     r10   <- temporal del Clamp
    111   1036    4       30         2666   31     r8    <- `nf`, el operando de la barrera

**Sí llega a `global_alloc`** (el aviso de la r28 queda contestado: no lo asigna
`local_alloc`). Y el RTL post-reload lo confirma: la insn del `asm` es

    (insn 2800 ... (set (reg:SI 8 r8) (asm_operands/v ("") ("=r") 0[ (reg:SI 8 r8) ] ...)))

colocada **después** de `add r30, r3, r9` (la fila 619). O sea: `nf` ocupa r8
desde la fila 593 hasta el final del racimo, y por eso `frame` no puede
reciclarlo en la 602. El diagnóstico de la r29 era exacto.

### 2.2 Pero el racimo NO se puede cerrar así — y la prueba no es un porcentaje

El RTL nuestro de la fila 602 es, literalmente:

    (insn 2709 ... (set (reg/v:SI 11 r11)
            (minus:SI (reg/v:SI 11 r11) (reg/v:SI 0 r0))))

**El mismo pseudo (1045) como destino y como operando B.** Un pseudo = un
registro duro. Nuestro `a -= c;` **sólo puede emitir `subf rN, rM, rN`**.

    objetivo   8007CCF8   subf r8, r0, r11     <- destino r8, operando r11: DOS pseudos
    nuestro               subf r11, r0, r11    <- UN pseudo

**El objetivo tiene ahí dos pseudos y nosotros uno.** Ninguna barrera, ningún
`register asm`, ningún cambio de prioridad y ningún reparto de registros puede
hacer que un `(set X (minus X c))` emita destino ≠ operando. **`ICEMover::Update`
no es «15 registros»: le falta una diferencia de FUENTE en `ICE::SignedMod`, y
está en el cuerpo del `if`, no en el llamante.**

`1045` está descrito en el `.lreg` como `used 14 times ... set 4 times; user var`
— las cuatro escrituras son `a = nf`, `a += b`, `a -= c` y `a = 0`: el
parámetro **es** el valor de retorno. En el objetivo, `a -= c` y `a = 0` escriben
un pseudo **distinto** (r8) del que lee el `divw`/`mullw` (r11).

### 2.3 Qué forma de fuente parte el pseudo: **dos `return`**

Con el arnés arreglado:

| ensayo | forma de `SignedMod` | ICEMover |
|---|---|---|
| `e1`  | `… return a - c; } else { return 0; }`      | **19** diffs, 99,78284 % |
| `e12` | `… return a - c; } return 0;` (sin `else`)  | 19 |
| `e13` | `… int c = a/b; return a - c * b;`          | 19 |
| `e14` | `… a -= c; return a; } else { return 0; }`  | 19 |
| `e15` | `if (b <= 0) return 0;` (salida temprana)   | 23, 99,36401 % PEOR |

Los cuatro primeros dan **exactamente el mismo resultado**: lo que parte el
pseudo **no es `return a - c` sino tener DOS sentencias `return`**. Y el `subf`
pasa a ser el del objetivo en forma:

    e1/e2/e3/e17/e25   subf r0, r0, r9   /  subf r10, r0, r9   <- destino ≠ operando  OK

`e1` además **pierde 4 B** (3.864 B): sin barrera, `a` y `nf` acaban en el mismo
registro duro y el `mr r11, r8` de la fila 594 desaparece — el objetivo **sí**
lo tiene, o sea que hace falta que `nf` siga vivo en la guarda.

### 2.4 Los ensayos numerados

Todos sobre la unidad entera con `c30cam_unit.py` (15-35 s), contra
`ICEMover 3868 B 99,90693 % 15 diffs`. Los `i*` son los de la r29, reproducidos.

    i4    barrera volatil "+r"(nf) DETRAS de GetKey            99,93278 %  10   (r29, reproducido)
    i6    la misma NO volatil (insn fantasma)                  99,90693 %  15   (r29, reproducido)
    probe #error en ICEMath.hpp                                99,90693 %  15   NO COMPILA MAL -> BUG
    c1/c2/c3  = e1/e2/e3 con el arnes ROTO                     NO-OP (md5 == base)
    e1    SignedMod con dos `return` (parte el pseudo)         99,78284 %  19   3864 B
    e2    e1 + i6 (fantasma detras de GetKey)                  99,87591 %  19
    e3    e1 + i4 (barrera volatil detras de GetKey)           99,89142 %  17
    e12   e1 sin `else`                                        99,78284 %  19   = e1
    e13   e1 con `return a - c * b;`                           99,78284 %  19   = e1
    e14   `a -= c; return a;` + `return 0;`                    99,78284 %  19   = e1
    e15   salida temprana `if (b <= 0) return 0;`              99,36401 %  23   PEOR
    e17   e1 + barrera volatil ANTES de GetKey                 99,90176 %  14   <-- el salto
    e18   e1 + fantasma ANTES de GetKey                        99,87591 %  19
    e19   e1 + `asm("" : : "m"(nf))` detras de GetKey          98,86763 %  65   CATASTROFICO
    e25   e17 + GetKey `(Clamp(n,0,N-1) == n)`                 99,91210 %  13   <-- MEJOR de la ronda
    e28   e17 con "+b" en vez de "+r"                          99,87591 %  19   PEOR
    e30   e17 + `int nkeys = pShake->GetNumKeys();` izado      99,90176 %  14   = e17

**`e17` es el hallazgo de método**: con el pseudo partido, el último uso de `a`
pasa de la fila 615 (dentro de `GetKey`) a la 602 (el `subf`), **y por eso la
barrera ya vale DELANTE de `GetKey`** — que es justo donde la r29 la había vedado
(`i2`). La veda 1 de la r29 **caduca en cuanto se parte el pseudo**.

Con `e25` las filas 589, 593, 596, 605, 607, 608 y 612 **casan**, y lo que queda
son **13 filas que son una permutación de cuatro registros**:

    valor            objetivo   e25
    b (extsh)        r10        r11
    a (copia param)  r11        r9
    resultado/frame  r8         r10
    temporal Clamp   r10        r8

El objetivo empaqueta esos cuatro valores en **tres** registros (r8/r10/r11,
reciclando r10 y r11); nosotros gastamos **cuatro** porque `a` se lleva r9.

### 2.5 Por qué `a` coge r9 (y no r11) — medido, no supuesto

Con el pseudo partido, `1045` (`a` a secas) pasa de **14 refs / 22 insns**
(pri 19090, puesto 5) a **9 refs / 9 insns** (pri **30000**, puesto **1**), o sea
que se asigna **el primero de todos** y `find_reg` le da r9 en vez de r11. En la
base, con `a`+`frame` fundidos, el puesto 5 le daba r11 y 1044 se llevaba r10 —
**que es el reparto del objetivo**. Para reproducirlo con el pseudo partido
haría falta `pri(a)` entre 18461 (1044) y ~20000, o sea `live_length` ≈ 13 en vez
de 9. No he encontrado ninguna forma de fuente que lo consiga.

**No he aplicado nada.** Ni `i4` (10 filas = 0 bytes) ni `e25` (13 filas = 0
bytes): las dos llevan un `asm` que el original no tiene, y `e25` además une
`ICEMath.hpp` (11 unidades) con `ICEManager.hpp`. Aplicar media función es
exactamente lo que la memoria del proyecto llama «trabajo a medias».

### 2.6 Lo que dicen el DWARF y el mapa de líneas (contradicciones declaradas)

- **`dwbody.py`**: el `SignedMod` del original tiene **una sola local, `int c`,
  en un bloque anónimo** — igual que el nuestro. Un `int r;` explícito
  contradiría el DWARF; **la forma de los dos `return` NO lo contradice** (no
  añade locales).
- **El `int nf` del llamante SÍ es deuda**: las locales de `ICEMover::Update` del
  original están una a una en el volcado y **no hay ninguna `int` en el bloque
  del shake**. La barrera, obviamente, también.
- **`lmap.py` sobre el objetivo** sitúa cada sentencia de `SignedMod` en su línea
  (`ICEMath.hpp:136` la copia del parámetro, `137` el `if`, `138`+`139` la
  guarda del `while`, `140` `int c = a/b`, `141` `c = c*b`, `142` `a -= c`,
  `145` `a = 0`). De ahí salen dos datos de disposición del fichero original,
  **cosméticos pero comprobados**: el `while` del original ocupa **una línea
  menos** que el nuestro (cuerpo sin llaves, 138-139 seguidas de 140), y
  **`FloatToInt` está DELANTE de `SignedMod`** (línea 132 contra 136) mientras
  que en el nuestro está detrás (164 contra 150). `ICE::Clamp(int,int,int)` está
  **detrás** (≈152) y en el nuestro delante (128). También: en el original
  `GetNumKeys`/`GetKey` viven en **`ICEData.hpp`** (líneas 307/309), no en
  `ICEManager.hpp`. Nada de esto cambia el código generado.

---

## 3. `static-init` (3.604 B): la fuente de `ICEReplay` es correcta

### 3.1 Los cuatro `addi`, con nombre y apellidos

    objetivo   stw r9, 0x40(r30)                       <- un solo base, r30
    nuestro    addi r27, r30, 0x30 ; stw r9, 0x10(r27) <- `this` materializado

Cuatro `addi r27, r30, {0x30, 0x48, 0x60, 0x78}` = **elementos 2, 3, 4 y 5** de
`ReplayCategoryTable` = **+16 B** (3.620 contra 3.604). Los elementos 0 y 1 usan
`0x..(r30)` en los dos lados. `r27` es un registro salvado: el `this` del
constructor tiene que sobrevivir a las **dos llamadas a `bStringHash`**.

### 3.2 El constructor ya está en su forma óptima (tres ensayos)

    s1   cuerpo de asignaciones en vez de lista de inicializacion   54 diffs  = BASE
    s2   bStringHash(name) ANTES de bStringHash(replay)             86 diffs  PEOR
    s3   los dos hashes a locales y luego los seis campos          253 diffs  CATASTROFICO

`s1` da exactamente la base: **el comentario de `ICEReplay.hpp` que dice «cuerpo
de asignaciones, no lista de inicialización» es indiferente** (las dos formas dan
el mismo objeto). `s2` confirma el orden de los hashes que ya documentaba.

### 3.3 El censo de registros: los 3.604 B cuelgan de UNA decisión

Contando cada mención de registro en las 905 filas de los dos lados:

    reg   objetivo  nuestro
    r9      151       153     <- +2, el racimo 549-564
    r11     101        99     <- -2, el mismo racimo
    r27      26        50     <- +24: los 4 `addi` y las 20 tiendas
    r30     103        87     <- -16
    (los otros 28 registros: IDENTICOS)

Y `r27` **está en uso en los dos lados** (26 menciones en el objetivo), o sea que
no es que a nosotros nos sobre un registro salvado.

### 3.4 La hipótesis del prefijo de la r21/r22/r25, MUERTA por medida

La r29 cerró la vía de los `asm` vacíos. Faltaba el lado de las **insns
REALES**, que era donde la r21 decía haber llegado a 0 `addi` con N=10
(+31 insns). Sonda `c30cam_pad2.py`: **la SourceList `zCamera.cpp` entera** con
N globales `unsigned int g_c30padK = bStringHash("pK");` inyectados **delante del
primer `#include`**, o sea al principio del static-init:

    N=  0  insns= 905  addi=4  [0x30 0x48 0x60 0x78]
    N=  2  insns= 920  addi=4  [0x30 0x48 0x60 0x78]
    N=  4  insns= 929  addi=4  [ … ]
    N=  6  insns= 939  addi=4
    N=  8  insns= 949  addi=4
    N= 10  insns= 959  addi=4
    N= 30  insns=1059  addi=4
    N=100  insns=1410  addi=4   <- +505 insns REALES: los mismos cuatro addi

**La longitud del prefijo es IRRELEVANTE.** No es una ventana de
`cse_end_of_basic_block`, no es un `flush_hash_table` por número de insns, y no
es «nos faltan 26-31 insns delante». **La r21 no reprodujo lo que creyó
reproducir** (o su sonda medía otra cosa). Con la r29, esta veda queda cerrada
por los dos lados.

### 3.5 Dónde SÍ está la diferencia: el contexto, no `ICEReplay.cpp`

Compilando **`ICEReplay.cpp` suelto** (con los `cflags` de zCamera), su propio
`__static_initialization_and_destruction_0` sale con **125 insns y CERO `addi`**:

    stw 9,16(30)   stw 9,40(30)   stw 9,64(30)   stw 9,88(30)   stw 9,112(30)   stw 9,136(30)

es decir **`stw r9, 0x40(r30)` para el elemento 2 — exactamente el objetivo**,
y para los seis. (Aviso honesto: `ICEReplay.cpp` no compila limpio suelto —
faltan símbolos de otras unidades a partir de la línea 207 — pero GCC emite el
static-init entero y optimizado; el dato hay que tomarlo como indicio fuerte, no
como prueba.)

**Conclusión accionable: la fuente de `ICEReplay.cpp`/`.hpp` NO es el problema.
Deja de barrerla.** El `addi` lo provoca el resto de la unidad, y no por su
tamaño (§3.4). La lectura que me queda, sin comprobar, es la de `reload`: el
`this` de los elementos ≥ 2 **recibe** r27 en nuestra versión y en el objetivo
**no lo recibe**, con lo que `reload` pliega su `REG_EQUIV (plus r30 K)` dentro
de la dirección del `stw`. Eso lo decide la tabla de allocnos del static-init
entero.

**Intenté acotar por bisección de la SourceList** (`c30cam_bisect.py`, incluye
las N últimas `#include` de `zCamera.cpp`): sólo compilan k=0 (las 28, 905 insns,
4 addi) y k=7 (21 ficheros, 642 insns, 4 addi). Los demás cortes rompen por
dependencias entre unidades. **Eje agotado para mí; queda como trabajo.**

---

## 4. Vedas nuevas, con la sentencia barrida

1. **El directorio-sombra con `-I` NO ve `ICEMath.hpp`** por el
   `#include "ICEMath.hpp"` relativo de `ICEData.hpp:8`. Sentencia: un `#error`
   dentro del fichero parcheado **compila sin fallar**. Arreglado copiando
   también `ICEData.hpp` a la sombra. **Verifica siempre con `#error`.**
2. **`ICEMover::Update` no puede casar con `a -= c;` en `SignedMod`**: el RTL es
   `(set (reg 1045) (minus (reg 1045) (reg 0)))`, un pseudo, y el objetivo emite
   `subf r8, r0, r11`. **Ninguna barrera ni ningún pin lo arregla.** Barridas:
   `i4`, `i6` y las 24 formas de la r29 — todas mantienen el pseudo único.
3. **Lo que parte el pseudo son DOS `return`, no `return a - c`**: `e1`, `e12`,
   `e13` y `e14` dan el **mismo** resultado (19 diffs) con cuerpos distintos.
4. **La salida temprana (`if (b <= 0) return 0;`) es peor**: `e15`, 23 diffs.
5. **La veda 1 de la r29 («la barrera sólo vale DETRÁS de `GetKey`») CADUCA al
   partir el pseudo**: con `e1` aplicado, delante de `GetKey` (`e17`, 14) es
   **mejor** que detrás (`e3`, 17), porque el último uso de `a` se adelanta de la
   fila 615 a la 602.
6. **`asm("" : : "m"(nf))` es catastrófico aquí**: `e19`, 65 diffs (98,87 %).
   Fuerza `nf` a memoria y rompe el bloque entero.
7. **`"+b"` en vez de `"+r"` empeora con el pseudo partido**: `e28`, 19 contra
   14 de `e17`.
8. **Izar `pShake->GetNumKeys()` a una local no mueve nada**: `e30` = `e17`.
9. **El constructor de `ReplayCategory` está en su forma óptima**: lista de
   inicialización y cuerpo de asignaciones dan el **mismo objeto** (`s1`);
   invertir el orden de los dos `bStringHash` cuesta 54 → 86 diffs (`s2`); sacar
   los hashes a locales, 54 → 253 (`s3`).
10. **La longitud del prefijo del static-init NO influye**: +505 insns reales
    (`N=100`) dejan los cuatro `addi` en los mismos cuatro desplazamientos.
    **Cierra la hipótesis de la r21/r22/r25 por el lado que faltaba.**

---

## 5. Lo que NO he probado

- **`TrackCar` (992 B) y `TrackCop` (948 B): cero ensayos.** Las dos están
  vedadas por la r29 con medida (`bFill` en `bMath.hpp`, −29 pp de unidad; y el
  `mr r5,r3` izado que una barrera no puede producir). Confirmadas en 15 y 2
  diffs al empezar y al terminar.
- **`TerrainVelocityNoise` (1.192 B): cero ensayos** (cerrada por construcción en
  la r29: un `elf_high` no tiene operandos de registro).
- **`_Storage::assign` (1.156 B) y `LoadCameraShakes` (168 B): cero ensayos**
  (vetadas por el encargo / cerradas por la r26-r27).
- **`permuter.py`: sin usar** en zCamera. Con el diagnóstico de §2.2 ahora hay
  algo que permutar de verdad: el cuerpo del `if` de `SignedMod`.
- **El A/B de `ICEMath.hpp` sobre las 11 unidades que la incluyen**: no lo he
  hecho porque **no aplico el cambio** (`e1` solo empeora ICEMover, 15 → 19).
  En el arnés, `e1` deja el total de la unidad idéntico a la base.
- **La permutación de cuatro registros que le queda a `e25`**: sé que `a` sube
  al puesto 1 con pri 30000 y por eso coge r9, pero **no he encontrado ninguna
  forma de fuente que le alargue la vida a ~13 insns**. No he mirado
  `prune_preferences`/`regs_someone_prefers` en el volcado.
- **El static-init**: no he volcado el RTL de `zCamera.cpp` entera (`-ds`,
  `-dl`, `-dg`) para ver en qué pase nace el `addi` ni si el `this` tiene
  `REG_EQUIV`. Es lo primero que haría el que siga.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): **no aplicado**; el
  encargo lo condiciona a cerrar algo con ganancia de bytes y he ganado cero.
- **`configure.py`, `config/GOWE69/*`, `splits.txt`, cflags**: no he tocado nada.

---

## 6. Herramientas (scratchpad, prefijo `c30cam_`)

    c30cam_unit.py    arnes de la r28/r29 CON EL FALLO ARREGLADO (tabla COMPANIONS:
                      al parchear ICEMath.hpp copia tambien ICEData.hpp a la sombra).
                      **Usa esta version, no la de la r29.**
    c30cam_rtl2.py    como c30cam_rtl.py pero admite un directorio-sombra por la
                      variable de entorno SHADOW (el original no lo pasaba al
                      preproceso y volcaba RTL de la fuente SIN parchear).
    c30cam_fd.py      fndiff contra el .o del ensayo. Con ALL=1 imprime todo.
    c30cam_pad2.py    inyecta N globales reales al principio del static-init de la
                      SourceList entera y cuenta los `addi rX,r30,off`.
    c30cam_bisect.py  SourceList reducida a las N ultimas #include (solo k=0 y k=7
                      compilan).
    c30cam_p_*.py     los parches de los ensayos e1..e30, s1..s3, i4, i6, probe.
    c30cam_alloc_{base,i4,e2}.txt   las tablas de allocnos de alloc.py.
    c30cam_i4_upd.greg              el .greg de ICEMover::Update con i4.
    c30cam_ice.lmap                 el mapa de lineas del objetivo.
    c30cam_si_all.txt               las 905 filas del static-init, los dos lados.
    c30cam_audit1.txt / c30cam_audit2.txt

**Trampa nueva:** `c30cam_rtl.py` (heredado) **ignora el directorio-sombra**: no
pasa `-I` al preproceso, así que volcaba el RTL del fichero SIN parchear y el
`.lreg` salía con el mismo tamaño exacto. Se detecta con
`grep <tu cambio> <tag>.ii`.

---

## 7. Verificación final

    build_direct.py zCamera                       ok
    triage.py zCamera --muro                      identico al encargo
    measure.py zCamera                            113080/125008 B  90,4582 %  446 al 100 %
    audit.py Speed/Indep/SourceLists/zCamera      446/446 ok, CERO FALLA
                                                  (dos pasadas, ficheros IDENTICOS)
    md5 ICEMover.cpp   edc3423a7989bf96b6ce5925412a7983   (= r28/r29)
    md5 ICEMath.hpp    ec9d9c201fb6b4138fd210f42f97c8d6   (= r28/r29)
    md5 ICEManager.hpp 4bf44755d305a929827e6f99f20f28d8   (= r28/r29)
    md5 ICEReplay.hpp  8bc27db0acd28256496a2a27d1dd6ad8
    md5 TrackCop.cpp   f7c2a733a30af8b7e83c25b92813a648   (= r28/r29)
    md5 TrackCar.cpp   2e4f72e08f4f47e64ce56b63b01e0410
    md5 bMath.hpp      4190acb3b919f8bd4ece9d117a654126   (= r29)
    git status src/Speed/Indep/Src/Camera/ICE/    vacio
    git status src/Speed/Indep/Src/Camera/Movers/ vacio
    git status src/Speed/Indep/bWare/             vacio
    git status src/Speed/Indep/SourceLists/zCamera.cpp   vacio

**No dejo ningún fichero modificado.** Los tres `.cpp` de `Src/Camera/Actions/`
y `CameraAI.cpp` que salen en `git status` son de **otro agente** de esta ronda
(frente `.data`, `int X;` → `int X = 0;`, mtime 18:11); no los he tocado y no
rompen el enlace.

`frozen.py` no lo he tocado: no he cambiado nada. Disco: arranqué con 15 GB
libres y termino con 15 GB tras borrar mis `.s` (25 MB cada uno), `.json` de
objdiff (17 MB cada uno), `.ii`, volcados RTL, `.o` y directorios-sombra.
