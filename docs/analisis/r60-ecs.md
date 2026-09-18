# r60 — zEcstasy: el PIN DE SINTESIS cierra un ciclo de 6 funciones; DOL 10.658 -> 8.207 B

Agente `ecs`. Unidad en propiedad: **zEcstasy**. **DOS ficheros tocados**, los dos de la
unidad en exclusiva. Ninguna cabecera, ningun `config/`, ningun `keep.lst`, ningun
`splits.txt`, ningun commit, ninguna sonda en `scripts/`.

**El encargo venia RANCIO**: pedia partir el bloque `asm` de `.data` en 59 fragmentos
porque «317 de 325 simbolos de .data estan fuera de sitio». Eso lo hizo la r59: hoy la
`.data` esta en **1 de 325 fuera de sitio** y vale 14 B del DOL. El frente real era otro
y esta medido abajo.

---

## 1. La medida, antes y despues

`ANTES` = HEAD `7ba4d59e` (estado con el que cerro la r59).

| | ANTES | DESPUES |
|---|---|---|
| `fncmp` | 3 de 539, **4.384 B** | 3 de 539, **4.384 B** — sin regresion |
| `linkdelta` | `.text +0  IGUAL` | `.text +0  IGUAL` |
| `permorden` | 535 de 539, **2 ciclos de 6** | **536 de 539, 1 ciclo de 6** |
| `desplaza` DETRAS | 0 en las cuatro | **0 en las cuatro** |
| `dolwhere` (cuenta rangos) | 13.717 B | **10.930 B** |
| **BYTES REALES DEL DOL** | **10.658** | **8.207** (**-2.451**) |
| `trypromo` | DOL ROTO `2d0fe6ca9305` | DOL ROTO `81136d2a84bc` |
| `sha1` del `.o` | `8ec86b5bf98d53eacb3d1be6fc4a1d8d894c427a` | `51f95d5ebbbe674d95ef1d4728efaafca3f20b29`, **tres compilaciones iguales** |

Por seccion del DOL (`agent_ecs_doldiff.py`):

| | r59 | r60 | delta |
|---|---:|---:|---:|
| `text1` | 4.922 | **2.491** | **-2.431** |
| `text2` | 7 | **5** | -2 |
| `data2` (`.rodata`) | 5.715 | **5.697** | -18 |
| `data3` (`.data`) | 14 | 14 | 0 |
| **total** | **10.658** | **8.207** | **-2.451** |

**No promociona**: `epCalculateLocalDirectionalPOS16` y `eProject` siguen VETADAS y no se
han tocado. La tercera que no casa, `UpdatePlatInfo`, tiene 86 medidas negativas de la
r36c/r36e/r36f escritas junto a la funcion; tampoco se ha tocado.

---

## 2. EL HALLAZGO: el PIN DE SINTESIS de un metodo implicito

`permorden` daba **dos ciclos de 6**. El primero valia **2.757 B del DOL** y era una
rotacion pura dentro de `eLight.cpp`:

    nuestro  #98  elRotateLightContext, LoaderLights, UnloaderLights,
                  SetSelectCarLighting, SphericalToCartesian, CartesianToSpherical
    objetivo #98  LoaderLights ... CartesianToSpherical, #103 elRotateLightContext

O sea: **bajar una funcion detras de otras cinco**. Al hacerlo, la funcion **crecio de
312 a 420 B** y la unidad se fue a `.text +60` y **2.951.098 B de DOL**.

`fndiff` dice por que: el objetivo **PLIEGA** `*new_light_context = *light_context;`
(cuatro `bl PSMTX44Copy` mas las copias de campo) y nosotros pasabamos a emitir
`bl __as__20eDynamicLightContextRC20eDynamicLightContext`. Y el `.o` EXTRAIDO **no tiene
ese simbolo**: en el original el `operator=` implicito se pliega siempre.

**El mecanismo, en el fuente del compilador que hay en el arbol:**

* `mark_used` (`cp/decl2.c`) sintetiza un metodo implicito **en su PRIMER USO**, y solo
  si `current_function_decl` no es nulo (el «kludge» de los argumentos por defecto).
* `synthesize_method` (`cp/method.c:2447`) lo compila ahi mismo y llama a
  `finish_function`.
* `finish_function` (`cp/decl.c:14675`) trata aparte a los `DECL_ARTIFICIAL`: pone
  `flag_inline_functions = 1`, **apaga `DECL_INLINE`** «para que `function_cannot_inline_p`
  compruebe nuestro tamano» y llama a `rest_of_compilation`.
* `function_cannot_inline_p` (`integrate.c:150`) mide entonces con
  `INTEGRATE_THRESHOLD`, no con `inline_max_insns`, y ademas mira `get_max_uid()`.

Conclusion practica: **el veredicto de plegado se decide UNA sola vez, en el punto donde
cae el primer uso**, y cambia con ese punto.

### La palanca: una `static inline` MUERTA que ancla la sintesis

```c
static inline void _ecs_r60_probe(eDynamicLightContext *a, const eDynamicLightContext *b) { *a = *b; }
```

Puesta **donde estaba la funcion**, justo antes de `LoaderLights`:

* **no emite un byte**: con ella puesta y sin mover nada, el DOL sale **IDENTICO** al
  byte (`2d0fe6ca9305`, 10.658 B) y `linkdelta` sigue `IGUAL`. Una `static inline` sin
  llamantes no se compila, pero **si** dispara `mark_used` sobre el `operator=`.
* con ella, `elRotateLightContext` se baja a su posicion del objetivo y **sigue casando
  a 312 B**: `fncmp` da los mismos 3 de 539 / 4.384 B.

**Es generico.** Cualquier constructor/operador implicito que haya que «anclar» antes de
mover a su unico usuario se ancla asi. Es la version para metodos sintetizados de la
semilla de pool del brief, y como aquella **cuesta cero bytes**.

---

## 3. LA FACTURA DEL MOVIMIENTO: el pool va PEGADO a la funcion

Mover la funcion se lleva **su pool de constantes**. El pool de `.rodata` de GCC 2.9 se
emite **por funcion y en orden de compilacion**, asi que sacar los 4 B de
`elRotateLightContext` de su sitio abrio **dos huecos de alineacion de 4 B** (donde
estaba, el `double` siguiente pasaba de caer alineado en `0x23D8` a necesitar relleno; y
en el destino, otro tanto). Resultado: `.rodata +8` y **324.255 B de DOL**, porque un
`linkdelta` de `+8` descoloca la `.rodata` de todo lo que va detras.

### La regla de la compensacion (medida, no supuesta)

Del bloque `asm` **sobrevive al enlace `size & 7` de cada simbolo SIN REFERENCIA**
(`-strip-unused-data` se lleva `size & ~7`, con la alineacion 8 de `.rodata`). De ahi:

* **quitarle 4 B de contenido a un simbolo grande NO cambia NADA**: `0xDC` y `0xD4`
  dejan los mismos 4 B de resto. Es la trampa obvia y no funciona.
* lo unico que mueve el enlace es **cambiar el RESTO**, o sea **borrar enteros simbolos
  de menos de 8 B**.

Se borraron dos de 4 B, y **se eligieron entre los que el bloque DUPLICA**: `lbl_803DDA80`
es exactamente el `0.0f` que el objetivo carga desde `elRotateLightContext` (el
`lis lbl_803DDA80@ha` del `.o` extraido), y `lbl_803DF1F8` es otro duplicado del pozo.
Ninguno de los dos esta en `keep.lst`. Con eso `linkdelta` vuelve a `IGUAL`.

Barrido de la pareja a borrar (mismo `.text`, distinto reparto de `.rodata`/`.data`):

| pareja borrada | data2 | data3 | total |
|---|---:|---:|---:|
| `803DDA80` + `803DDA84` | 5.733 | 7 | 8.236 |
| `803DF194` + `803DF198` | 5.719 | 14 | 8.229 |
| `803DF1F8` + `803DF1FC` | 5.719 | 14 | 8.229 |
| **`803DDA80` + `803DF1F8`** | **5.697** | 14 | **8.207** |
| `803DDA88` + `803DF1F8` | 5.696 | 14 | 8.206 |

Se queda la cuarta: 1 B peor que la mejor, pero es la unica con causa (el duplicado del
literal que la funcion movida se lleva consigo).

---

## 4. LA .rodata, DIAGNOSTICADA: 5.697 B, y el orden YA ES EL BUENO

Es el 69 % de lo que queda. La r59 la dejo como «hace falta saber que decide el orden
float/cadena». **Ya no hace falta: no es orden, es POSICION DEL BLOQUE.** Medido:

1. **La `.rodata` del `.o` EXTRAIDO mide 7.952 B; la nuestra 13.344.** Los primeros 7.952
   bytes de la nuestra son **identicos** a la suya salvo 8: el bloque `asm` de la cabecera
   de `zEcstasy.cpp` es copia literal de la `.rodata` entera del objetivo. Los otros
   5.392 son el pozo `$LC` del compilador, **detras**.
2. En el ENLACE sobreviven **23 simbolos `lbl_`/`gap_` (397 B)**, **584 `$LC` (4.419 B)**,
   6 simbolos reales (las cuatro vtables `c*Map`, `PADMASKS`, `TweakSphereMapClr`: 116 B)
   y ~3.000 B de **restos** (`size & 7`) de los `lbl_` estripados. Suman los 7.952, y por
   eso `linkdelta` dice `IGUAL` con el contenido mal colocado.
3. **Mapa `$LC` -> direccion del objetivo** (`lcmap.py`: empareja funciones por nombre y
   lee la reubicacion de la MISMA instruccion en los dos `.o`): **549 de 610 `$LC` se
   resuelven, y salen con SOLO 4 INVERSIONES**. O sea: **el compilador ya emite los
   literales en el ORDEN del objetivo**. El ultimo, `$LC1042`, cae en `803DF564`, que es
   el ultimo dato de la `.rodata` del objetivo — de ahi que los 824 B de cola CASEN, y
   los 792 de cabeza casan porque son restos del bloque.
4. Esos 549 cubren **3.835 de los 7.952 B**. Los otros **4.117 estan en 112 HUECOS**, que
   es lo unico que el bloque `asm` tiene que aportar de verdad. Los mayores:

       803DEE08 +908   803DDD74 +504   803DE944 +412   803DD6D9 +295
       803DDBA4 +256   803DD990 +212   803DE025 +199   803DD658 +128 (el prefijo)

**El trabajo de la ronda siguiente** es entonces el de la r59 con la `.data` pero con 112
fragmentos en vez de 57: partir el bloque `asm`, tirar lo que duplica a un `$LC` mapeado,
y colocar cada hueco entre las dos funciones que emiten los `$LC` que lo rodean. La regla
1 de la r59 (`assemble_asm` no toca `in_section`: o pegado detras de un dato, o acabando
en `.section ".text"`) sigue valiendo tal cual.

Herramientas dejadas en el scratchpad (`ecs/`, no en `scripts/`): `lcmap.py` (mapa `$LC`
-> direccion, deja `lcmap.json`), `cover.py` (cobertura y huecos), `rodlink.py` (layout
REAL del enlace), `rodmap.py` / `rodcmp.py` / `rodbytes.py`.

---

## 5. Lo que queda y lo que NO se ha tocado

* **`.rodata` 5.697 B** — §4, con el plan y las herramientas.
* **`.text` 2.491 B**, de los que:
  * **~400 B: el segundo ciclo de 6**, todo en la COLA de `finish_file`:

        objetivo  Init__20GrandSceneryCullInfo, Init__11DefragFixer, ClassKey_light_flares_cg,
                  _type_map<EmitterData>, _type_map<EmitterGroup>, Flush__14eTextureBucket
        nuestro   _type_map<EmitterData>, _type_map<EmitterGroup>, Init__20GrandScenery...,
                  Init__11DefragFixer, Flush__14eTextureBucket, ClassKey_light_flares_cg

    Los dos `_type_map` son **instanciaciones de plantilla**
    (`instantiate_pending_templates`, al principio de cada vuelta de `finish_file`) y los
    otros cuatro **inlines diferidos**. En el objetivo las plantillas caen **entre**
    `ClassKey` y `Flush`, o sea en la **segunda vuelta**; en el nuestro en la primera.
    **NO SE HA TOCADO**: ni el orden de definicion ni el de primer uso explican las seis
    posiciones a la vez (por definicion sale `Flush` antes que `ClassKey` —`EcstasyEx.cpp`
    va antes que `EmitterSystem.cpp`, que es el ultimo `#include`—, y por primer uso sale
    al reves), y la r57 ya trabajo `ClassKey` para sacarla del punto de parseo. Hace falta
    leer `finish_file` (`cp/decl2.c`) y saber que encola cada vuelta.
  * ~600 B de `epCalculate` y ~54 de `UpdatePlatInfo`: VETADAS / agotadas.
  * el resto son reubicaciones a `.rodata` y `.bss` descolocadas; caen con §4.
* **`.bss`: 109 de 152 dentro mal**, con el diagnostico de la r59 §5 intacto (la segunda
  particion de la cola de `finish_file`). No tocada.
* **`.data`: 14 B**, y no son de `.data`: son punteros a `.rodata`/`.bss` descolocadas.

---

## 6. Coordinacion

* **`lcfix.py --check`: 186 correcciones pendientes, NINGUNA de zEcstasy** (0 lineas con
  `zEcstasy`; son 143 de `zFe` y 43 de `zSpeech`). **Ninguna rompe mi unidad**, y mi
  medida **no depende de que se apliquen**: en el enlace de `trypromo` solo mi `.o` es
  nuestro, asi que las entradas `$LC` de `zFe`/`zSpeech` son inertes.
* **Las 28 entradas `$LC` de zEcstasy en `keep.lst` siguen VALIDAS**: las 28 llevan su
  directiva `# @lc` y `lcfix --check` las resuelve por contenido contra el `.o` recien
  compilado sin proponer ni una correccion. Mover la funcion solo intercambio `$LC454` y
  `$LC455`, y las de `keep.lst` son `$LC147..$LC411` y `$LC889`.
* **No he tocado** `config/GOWE69/*`, `splits.txt` ni `keep.lst`.
* **Cero regresiones en unidades ajenas, medido**: los dos ficheros que toco
  (`src/Speed/Indep/SourceLists/zEcstasy.cpp` y
  `src/Speed/Indep/Src/Ecstasy/eLight.cpp`) aparecen en **una** sola SourceList y en
  **ninguna** regla de `build.ninja`; no he modificado ni una cabecera; `fncmp` da el
  mismo 3 de 539 / 4.384 B antes y despues; `desplaza` da **DETRAS = 0** en las cuatro
  secciones.
* Aviso de higiene: `eLight.cpp` estaba marcado como modificado en el arbol al empezar y
  le hice `git checkout --` para tomar la base. **No se perdio nada**: recompilado, el
  `.o` da `8ec86b5b...`, que es **exactamente** el sha1 que la r59 dejo escrito en su
  informe, y los cuatro bloques `r59 --` del fichero siguen ahi. Era el CRLF.
