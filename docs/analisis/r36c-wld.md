# r36c — zWorld / zWorld2 (5 funciones, 8.416 B)

Base verificada al empezar con `build_direct` + `fndiff`, y la misma al terminar
salvo la línea marcada:

```
                                           BASE                AHORA
 2980 zWorld2  HolePunchAvoidables         98,133 %  55 difs   igual
 2908 zWorld   RenderFlaresOnCar           98,618 %  18 difs   igual   (2908/2912)
  876 zWorld   UpdateWheelYRenderOffset    99,384 %   7 difs   igual   (876/872)
  836 zWorld   CullParts                   99,426 %   5 difs   igual   (836/832)
  816 zWorld2  InitAtSegment               99,152 %  42 difs   99,412 %  21 difs
```

`pctsnap --cmp` sobre las dos unidades (939 funciones): **1 mejora, 0 empeoran**.
`lcfix.py --check` limpio. Un solo fichero tocado:
`src/Speed/Indep/Src/World/Common/WRoadNetwork.cpp`. Sin commits.

---

## 0. Lo que se lleva la ronda

1. **La regla nueva, y no es de esta función: los bytes de pila muertos NO son un
   problema aparte del reparto de registros, son EL MISMO problema.** Las rondas
   r27–r29 dejaron escrito en `InitAtSegment` que hacían falta «las dos cosas a la
   vez» —un registro y 12 B de marco— y que eran independientes. Es falso: con
   siete registros preservados al asignador le falta uno y **reserva 8 B de marco
   que luego no usa**; en cuanto entra el octavo el hueco desaparece solo. §1.
2. **Cómo sacarle a este compilador los volcados de RTL**, que hasta hoy no se
   podía: el driver `ngccc.exe` se come los `-d*`. Hay que llamar a `cc1plus.exe`
   a mano. Con eso se leen las decisiones del asignador (`;; Register
   dispositions`, `;; N regs to allocate`). §2.
3. **El pin de registro tiene DOS trampas de tipo** que costaron cinco medidas
   cada una y que valen para cualquier pin de un `char`/`short`. §3.
4. `InitAtSegment` pasa de 42 a **21 diferencias, todas de reparto puro**, con el
   **tamaño ya exacto (816/816)**, el marco exacto (0x90), `stmw r24,0x58`, las
   `psq` en 0x78/0x80/0x88 y la conversión int→double en 0x50. Lo que queda son
   tres ciclos de registro sin nada que leer en la fuente. §4.
5. Dieciocho vedas nuevas medidas (§6) y el diagnóstico de las otras cuatro
   funciones puesto al día (§5).
6. **Aviso de contabilidad para quien reparta**: `zWorld2` está a **dos funciones
   y ~650 B de datos** de poder promocionar. §7.

---

## 1. El hallazgo: 8 B de pila muertos == un valor que no cupo en registro

`InitAtSegment` llevaba desde la r27 con este diagnóstico escrito en la fuente:

> «Y hay una SEGUNDA diferencia independiente, que es la del marco: nos sobran
> 12 B de pila JAMÁS REFERENCIADA. […] Hacen falta LAS DOS.»

**No son dos cosas.** Medido paso a paso:

```
objetivo   marco 0x90   stmw r24,0x58 (OCHO preservados)   conversión en 0x50
nuestro    marco 0x98   stmw r25,0x64 (SIETE)              conversión en 0x58, 0x50 MUERTO
```

Descomponiendo el marco con la fórmula de `rs6000_stack_info`
(`total = 8 + vars + fpmem + align(fp+gp,8)`):

| | vars | fpmem | gp | total |
|---|---|---|---|---|
| objetivo | 72 | 8 | 32 (r24-r31) | 144 = 0x90 |
| nuestro | 72 | **16** | 28 (r25-r31) | 152 = 0x98 |

O sea: 8 B de más entre la última local (`nodePtr`, r1+0x48 en los dos) y la
ranura de la conversión int→double. **Y en cuanto se mete `laneInd` en r24 —el
octavo preservado— esos 8 B desaparecen y el marco cae a 0x90 solo, sin tocar
nada más.** Un pin arregló las dos cosas de una vez: 16 de las 42 filas.

### Que NO es (todo medido sobre una mini-TU, ciclo de 6 s)

Antes de dar con la causa hice el barrido entero, y la tabla vale porque estos 8 B
muertos salen en muchas funciones del proyecto:

| variante sobre `InitAtSegment` | vars+fpmem |
|---|---|
| base (2 conversiones int→float) | 88 |
| **1** conversión | 88 |
| **3** conversiones | 88 |
| bloque anónimo vacío (0 conversiones) | 64 |
| `nodePtr[2]` + conversión de un **campo** | **80** ← el reparto del objetivo, conversión en 0x50 |
| `nodePtr[2]` + conversión desde una **llamada** (`GetRawLaneOffset`) | **88** |
| `nodePtr[índice VARIABLE]` + cualquier conversión | **88** |
| `nodePtr[índice CONSTANTE]` + cualquier conversión | **80** |
| función suelta con la misma forma (`np[i]`, índice variable, conversión) | 8 B de fpmem, **sin hueco** |

Las dos últimas filas son la clave: **el índice variable no cuesta 8 B por sí
mismo** (en una función suelta no cuesta nada); cuesta cuando además sube la
presión de registros lo bastante como para que al asignador le falte uno. La
correlación con el índice variable era una pista falsa; la causa es la presión.

### La receta de diagnóstico, reutilizable

`stwu 1,-N(1)` y el offset del `lfd` de la conversión, sobre una mini-TU, en 6 s:

```sh
echo '#include "Speed/Indep/Src/World/Common/WRoadNetwork.cpp"' > mini.cpp
SN_NGC_PATH=build/compilers/ProDG/3.9.3 build/compilers/ProDG/3.9.3/ngccc.exe \
    <cflags de la unidad> -S mini.cpp -o mini.s
```

y luego `total - 8 - align(fp_size+gp_size,8)`. Si el resultado es 8 B mayor que
la suma de las locales del DWARF, **falta un registro preservado**, no sobra una
local.

---

## 2. Herramienta nueva: los volcados de RTL de GCC 2.9

`ngccc.exe -dg` / `-dR` **no produce nada**: el driver no pasa los `-d*` a cc1.
Hay que preprocesar y llamar al compilador propiamente dicho:

```sh
export SN_NGC_PATH=.../build/compilers/ProDG/3.9.3
ngccc.exe <cflags> -E fichero.cpp -o fichero.ii
cc1plus.exe -quiet -O1 -gdwarf+ -mps-nodf -G0 -fno-static-dtors -ffast-math \
    -fforce-addr -fcse-follow-jumps -fcse-skip-blocks -fforce-mem -fgcse \
    -frerun-cse-after-loop -fschedule-insns -fschedule-insns2 \
    -fexpensive-optimizations -frerun-loop-opt -fmove-all-movables \
    -dg fichero.ii -o /dev/null        # -> fichero.ii.greg   (asignador global)
    -dR ...                            # -> fichero.ii.sched2 (RTL final)
```

`cc1plus` **no acepta los `-I`/`-D`**: hay que darle el `.ii` ya preprocesado.
Tarda lo mismo que un compilado normal (7 s para WRoadNetwork.cpp); los volcados
pesan 6–60 MB, así que hay que trabajar en el scratchpad.

Lo que se lee ahí y no se ve de ninguna otra forma:

```
;; 7 regs to allocate: 104 109 82 84 90 108 85     <- los allocnos GLOBALES, en orden de prioridad
;; 84 conflicts: 82 83 84 85 90 92 ...             <- el grafo de interferencias
;; Register dispositions:
82 in 31  84 in 26  90 in 30  101 in 25  222 in 29  236 in 28  254 in 27
```

Con eso se ve, por ejemplo, que en `InitAtSegment` `fProfiles@ha` y la constante
`0x4330` **no las reparte `global_alloc` sino `local_alloc`** (no salen en la
lista de allocnos globales), lo que explica por qué el permutador —que sólo mueve
sentencias— no las toca.

También sale de aquí que el temporal de la conversión no es una ranura normal:
es el **registro duro `reg:DF 76 fpmem`** con sus cuatro patrones
`*floatsidf2_loadaddr / _store1 / _store2 / _load` y un `unspec 11` para la
dirección. Por eso su ranura no aparece en ningún volcado de RTL con offset
resuelto.

---

## 3. El pin de un `char`: dos trampas de tipo

La palanca de la r36b («pin de registro contra el reparto») funciona aquí, pero
**no se puede pinchar un parámetro** (parse error, ya estaba anotado) y la copia
local trae dos problemas de tipo que no son evidentes:

### 3.1 `register int`, nunca `register char`

Con `register char lane asm("r24") = laneInd;` el registro es **QImode** y GCC
pierde el saber que el valor ya viene extendido de la ABI: mete un
`extsb r5,r24` donde el objetivo tiene `mr r5,r24`, y un **`extsb r24,r24` de
más** antes del `slwi`. Resultado: 820 B (4 de más) y 95,93 %.

Con `register int` esas dos desaparecen (el parámetro `char` se promociona a
SImode en la llamada y `GetSegmentTrafficLaneRightSide` **ya declara `int`**).

### 3.2 Con `int` hay que esquivar los inline de parámetro `char`

`SetLaneInd(char ind)` reintroduce la extensión por el otro lado: `PROMOTE_MODE`
promociona el parámetro `ind` del inline y GCC emite `extsb r7,r24; stb r7,…`
donde el objetivo tiene `stb r24,…`. Escribiendo los dos campos a pelo
(`fToLaneInd = (char)lane; fLaneInd = (char)lane;`) salen los `stb r24` del
objetivo. Sin ese paso el pin `int` da **39 diffs** en vez de 21.

**La regla general**: al pinchar un valor de tipo estrecho, mira los dos lados —
el ancho (`extsb` al usarlo como `int`) y el estrecho (`extsb` al pasarlo a un
inline con parámetro `char`). El pin bueno es el que no paga ninguno de los dos.

### La receta que quedó aplicada

```c
register int lane asm("r24") = laneInd;   // el registro del OBJETIVO para el parámetro
...
GetSegmentTrafficLaneRightSide(*segment, lane);   // toma int: sale `mr r5,r24`
fToLaneInd = (char)lane;  fLaneInd = (char)lane;  // en vez de SetLaneInd(): `stb r24`
profile->GetRawLaneOffset(lane);                  // toma int: sale `slwi r30,r24,2`
```

Coste: una local de más en el DWARF (`regmap` lo canta), cero bytes en `.text`.

---

## 4. `InitAtSegment` (816 B): 42 → 21 diffs, tamaño exacto

Lo que ya casa y antes no: `stwu -0x90`, `stmw r24,0x58`, `lmw`, las tres
`psq_st`/`psq_l`, `stw r0,0x94`, `addi r1,r1,0x90`, las seis filas de la
conversión int→double (0x50/0x54) y las cinco de `laneInd` en r24.

Lo que queda son **tres ciclos de registro y una decisión de coalescing**:

| filas | objetivo | nuestro |
|---|---|---|
| 12, 17, 20 | `mulli r8` + `lwz r10, fSegments@l` + `add r25,r10,r8` | `mulli r10` + `lwz r25` + `add r25,r25,r10` |
| 87, 88, 91, 100, 102, 106, 110, 116-120 (12 filas) | `fNodes`→r10, `@ha` del 0.0f→r11 | al revés |
| 103, 131, 157 | `fProfiles@ha`→r27 | r28 |
| 107, 152, 172 | `0x4330`→r26 | r27 |

El primero es **coste del pin `asm("r25")` de `segment`**: con el pin, el
asignador funde el destino del `lwz` con r25; sin el pin las filas 12 y 17 casan
pero `segment` se va a r26 y arrastra **nueve** filas más (99,265 %, 28 difs). El
pin de r25 sigue siendo el mejor de los dos.

Los dos últimos son el mismo desplazamiento de uno: **el objetivo deja r28 libre
y nosotros dejamos r26.** El objetivo reparte r31, r30, r29, `(r28 vacío)`, r27,
r26, r25, r24 — o sea que su `local_alloc` tenía **una cantidad más** que la
nuestra, una que se lleva r28 y no emite ni una instrucción. No he encontrado con
qué fabricarla: los seis intentos de bloquear r28 o r26 con `asm` clobber están
en §6 y todos empeoran (el clobber es una barrera de planificación completa y en
esta función cuesta entre 15 y 35 filas).

Vía para la siguiente ronda: **permutador CIEGO** (el guiado no vale: `regmap`
señala nuestra local `lane`, que es el pin a propósito, así que el plan guiado
iría justo en contra). Es un `PERMUTADOR` de manual: 21 diferencias, todas de
registro, tamaño idéntico y estructura idéntica.

---

## 5. Las otras cuatro, al día

### `HolePunchAvoidables` (2.980 B, 98,133 %, 55 difs) — sin tocar

**El marco YA casa (0x2f0 en los dos), así que la palanca de §1 no aplica aquí.**
Los cuatro racimos de la r36b siguen igual y el diagnóstico no cambia. Un dato
nuevo sobre el racimo cr2/cr3 (filas 176, 178, 241, 650): **no se puede separar
con un clobber**, porque `is_racer` se usa dentro del bucle y por tanto está viva
en el arco de retroceso igual que `is_drag`; no hay ningún punto del cuerpo donde
una esté viva y la otra no.

### `RenderFlaresOnCar` (2.908 B, 98,618 %, 18 difs) — sin tocar

Nos SOBRA un `lis` (2.912 contra 2.908): el objetivo iza **un** pseudo `@ha` de
`lbl_8040AD04` en la fila 123 y lo mantiene 330 instrucciones para los tres usos;
nosotros materializamos dos en el preencabezado (fila 452 → r9 y fila 461 → r30).

**Veda nueva, y es la misma que ya estaba por otro camino**: adelantar el primer
uso del símbolo al bloque de `GetCarTypeInfo` (`float base_headlight_intensity =
lbl_8040AD04;`, que es exactamente donde el objetivo crea el pseudo) da el
**tamaño EXACTO 2908/2908** pero 98,054 % y **62 diffs** — el mismo 98,054 % que
ya daba pasar los `lbl_` a literales `0.0f`. Es decir: las dos formas producen el
mismo objeto y el izado sí aparece, pero se lleva por delante todo el reparto.
Con `base_brakelight_intensity` sale idéntico.

### `UpdateWheelYRenderOffset` (876 B, 99,384 %, 7 difs) — sin tocar

Falta un `lis`. Diagnóstico de la r36b confirmado y sin palanca nueva. Nota: la
variante «`extern const float lbl_8040AA84` en el width» que la r36b apuntó como
la única que da los 876 B **exige declarar y definir un símbolo `.rodata` nuevo**,
y eso mueve el mapa de literales (`lcfix`). No la he tocado por eso; si alguien la
retoma, que mida `lcfix.py --check` antes y después.

### `CullParts` (836 B, 99,426 %, 5 difs) — sin tocar

Falta un `mr r9, r0`. Causa raíz (r36b): el objetivo tiene `Polarity` en **r0**,
que se pisa acto seguido con `NumPlanes`, y por eso necesita el cero de
`debug_print` (r9) para la guarda y luego rehacer r9; nosotros tenemos `Polarity`
en **r11**, que después del `bne` vale 0, y GCC lo reusa como cero.

**Dos vedas nuevas** intentando sacar `Polarity` de r11 con la palanca del
clobber: las dos dan 99,330 % (peor que 99,426). La primera reestructura el
`else if` en `else { asm("" : : : "r11"); if (...) }` para que el clobber caiga en
el camino donde `Polarity` sigue viva; la segunda lo pone como primera sentencia
de la rama `ANY_VISIBLE` para impedir que r11 se reuse como cero. El fichero ya
lleva un `asm("" : : : "r11")` de una ronda anterior en el sitio bueno; añadir un
segundo no ayuda.

---

## 6. Vedas nuevas, medidas

Todas con `build_direct` + `fndiff` sobre la unidad completa. Base de
`InitAtSegment` para comparar: **99,152 %, 42 difs, 816/816**; con el pin bueno,
**99,412 %, 21 difs, 816/816**.

| función | ensayo | resultado |
|---|---|---|
| `InitAtSegment` | `register char lane asm("r24")` en todos los usos | 95,926 %, 19 difs, **820 B** (los dos `extsb`) |
| `InitAtSegment` | ídem con el pin como PRIMERA sentencia | idéntico (la posición del pin no importa) |
| `InitAtSegment` | `register int` con `SetLaneInd(lane)` normal | 98,407 %, 39 difs, 820 B |
| `InitAtSegment` | `register int`, `laneInd` en `GetSegmentTrafficLaneRightSide` | 95,436 %, 46 difs, 820 B |
| `InitAtSegment` | `register int` en todos los usos (sin esquivar SetLaneInd) | 95,436 %, 44 difs, 820 B |
| `InitAtSegment` | pin `char` usado SÓLO en `SetLaneInd` | 92,961 %, 41 difs, 824 B |
| `InitAtSegment` | pin `char` salvo en `GetRawLaneOffset` | 93,917 %, 40 difs, 824 B |
| `InitAtSegment` | el pin bueno **sin** el pin `asm("r25")` de `segment` | 99,265 %, 28 difs |
| `InitAtSegment` | pin `char` sin el pin de r25 | 95,804 %, 25 difs, 820 B |
| `InitAtSegment` | `asm volatile("" : : : "r28")` entre los dos `GetProfile` | 87,505 %, 46 difs |
| `InitAtSegment` | `asm("" : "+r"(lane) : : "r28")` entre los dos `GetProfile` | 95,662 %, 48 difs, 820 B |
| `InitAtSegment` | la misma antes del PRIMER `GetProfile` | 97,848 %, 36 difs |
| `InitAtSegment` (base) | `asm("" : : : "r26")` detrás de `fDeadEnd = 0` | 92,304 %, 38 difs, 812 B |
| `InitAtSegment` (base) | `asm("" : : : "r26")` delante de `SetLaneOffset(0.0f)` | 89,672 %, 63 difs |
| `InitAtSegment` | `fLaneInd` antes que `fToLaneInd` | 99,402 %, 23 difs |
| `InitAtSegment` | `SetLaneOffset(0.0f)` delante de `fStartPos` | 90,103 %, 38 difs |
| `CullParts` | `else { asm("" : : : "r11"); if (Polarity==ALL…) }` | 99,426 → **99,330 %** |
| `CullParts` | `asm("" : : : "r11")` primera sentencia de la rama `ANY_VISIBLE` | 99,426 → **99,330 %** |
| `RenderFlaresOnCar` | `base_headlight_intensity = lbl_8040AD04` | 98,618 → **98,054 %**, 62 difs, 2908/2908 |
| `RenderFlaresOnCar` | `base_brakelight_intensity = lbl_8040AD04` | idéntico al anterior |

**Neutras** (21 difs, igual que el mejor): el pin como primera sentencia,
`SetLaneOffset(0.0f)` detrás de las dos escrituras de campo, quitar la escritura
repetida de dentro del bloque, y escribir `SetLaneOffset` como tres asignaciones
de campo.

**Aviso de medida, otra vez**: el fuzzy de objdiff da crédito parcial a una fila
que sólo cambia de registro. El pin `char` **baja** el fuzzy de 99,15 a 95,93
mientras las diferencias caen de 42 a 19 — porque los `INSERT`/`DELETE` que
introduce el `extsb` puntúan cero y las 42 de la base puntuaban a medias. La
medida honesta es el recuento de diferencias **y el tamaño**; el pin bueno (`int`)
gana en las tres a la vez y por eso es el que queda.

---

## 7. Contabilidad: qué le falta a `zWorld2` para promocionar

Las dos unidades son `NonMatching` en `configure.py` (líneas 687-688), así que el
DOL sigue enlazando los objetos originales y nada de esta ronda puede romperlo.
Contando **todos** los símbolos con objdiff (no sólo los del encargo):

```
zWorld2   near-miss: HolePunchAvoidables (2980 B, 98,133) + InitAtSegment (816 B, 99,412)
          [.rodata-0]  62,1 %   4560/4168   (-392 B)
          [.data-0]    52,2 %    396/140    (-256 B)
zWorld    near-miss: RenderFlaresOnCar, UpdateWheelYRenderOffset, CullParts,
                     DefragmentPool (684 B, 99,269), SetMemoryPoolSize (304 B, 97,368)
          [.rodata-0]  79,2 %  17040/13632  (-3408 B)
          [.data-0]    82,6 %   7440/7588
          CarSlotIDNames 66,7 % (1112 B) · CarEffectParameters 66,7 % · CarMemoryInfoTable 85,7 %
```

**`zWorld2` está a dos funciones y ~650 B de datos.** Y el `.rodata` que falta
**no es contenido nuevo**: `datadiff.py zWorld2` muestra que las cadenas están
las dos veces (`AIParams`, `RBComplexParams`, `Attrib::Gen::rigidbodyspecs`,
`Attrib::Gen::pursuitlevels`…) sólo que **en otro orden**. Es un problema de
orden de emisión de cadenas, no de código que falte — la misma familia que
`SourceLists: el orden de link`. Es trabajo para otro agente, pero es el que
convierte estas dos funciones en 300 kB promocionables en vez de en dos
porcentajes.

---

## 8. Qué queda y por dónde

1. **`InitAtSegment` (816 B, 21 difs)** — el mejor premio del grupo: tamaño,
   marco y estructura ya casan; sólo quedan tres ciclos de registro. **Permutador
   CIEGO**, no guiado (`regmap` señala la local del pin, que es intencionada).
2. **`zWorld2` como unidad** — ver §7: dos funciones y el orden del `.rodata`.
3. **`HolePunchAvoidables` (2.980 B, 55 difs)** — cuatro racimos ya nombrados; el
   de la reasociación de `-ffast-math` sigue siendo una corrección real que no
   paga sola.
4. **`RenderFlaresOnCar` / `UpdateWheelYRenderOffset` / `CullParts`** — una
   instrucción cada una, con 15, 15 y 6 formas barridas. No metería más horas
   hasta que aparezca una palanca sobre `-fforce-addr` (los dos primeros son el
   MISMO mecanismo: cuántos pseudos `@ha` viven a la vez).

**Fuera de mi territorio**, dos cosas que valen para cualquiera:

- **La regla de §1**: `8 B de marco muerto == un preservado que falta`. Se
  comprueba en 6 s con una mini-TU. Cualquier near-miss cuyo `stwu` no case por 8
  o 16 bytes es candidato, y el arreglo es un pin, no quitar una local.
- **El volcado de RTL de §2**: hasta ahora el proyecto sólo veía DWARF y
  ensamblador. `cc1plus.exe` a mano da el grafo de interferencias y el reparto
  del asignador, que es exactamente lo que falta cuando `regmap` dice
  «PERMUTACIÓN» y no hay nada más que leer.
