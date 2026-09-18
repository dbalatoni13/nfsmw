# Ronda 20 — familia `path` (pathtrack, pathnode, pathinit, pathbank, pathreal)

Medida propia: `base_r20_path.json` → `despues_r20_path.json`, con
`build_direct.py` antes de **cada** medida (`ninja` no construye ninguna de las
seis unidades).

    antes:    9.600 / 12.576 B  76,3359 %   72 funciones al 100 %
    despues: 11.112 / 12.576 B  88,3588 %   76 funciones al 100 %

    measure.py --cmp base_r20_path.json despues_r20_path.json
        +1.512 B, +4 funciones, 3 unidades cambian
          +64 B  +1 fn  .../cmn/pathreal    432 -> 496    (100 %)
         +580 B  +1 fn  .../cmn/pathinit  1.752 -> 2.332  (100 %)
         +868 B  +2 fn  .../cmn/pathtrack 1.180 -> 2.048

La linea base del encargo coincide exactamente con la medida al empezar
(1752+432+916+1180+1760+3560 = 9.600 B). No estaba rancia.

**`pathi.h` es cabecera compartida**, asi que la medida de control es sobre las
**14 unidades** que la incluyen, no sobre las cinco del encargo:

    14 unidades  33.180 -> 34.692 / 36.156 B   186 -> 190 funciones al 100 %
    measure.py --cmp: +1.512 B, +4 funciones, **3 unidades cambian, ninguna baja**

Ficheros tocados (cinco, ninguna bandera, `configure.py` sin tocar):

* `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathi.h`
* `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp`
* `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathinit.cpp`
* `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp`
* `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathreal.cpp`

`audit.py` da **cero fallos en las 14 unidades** (dos pasadas), salvo el falso
positivo de `pathreal6` documentado en §5. `pathinit`, `pathreal`, `pathtrack` y
`pathbank` quedan congeladas con `frozen.py cong` y `chk` responde «identico al
congelado».

---

## 1. El hallazgo de la ronda: la heuristica de `true_dependence`

`PATHI_getmastertrack` estaba al 66,5 % y la diferencia era **una sola cosa
repetida seis veces**: el objetivo **recarga `Path::pfstate` despues de cada
escritura por `pfstate->`** y nosotros lo cacheamos. `PATH_addmapfile` (580 B,
82,5 %) tenia exactamente el mismo sintoma amplificado: `triage` decia
`faltan 21, sobran 2, lwz+18`, y esos 18 `lwz` son **dos por cada una de las
nueve lineas** `Path::pfstate->X = pmap + Path::pfstate->pmap->Y` (una recarga
de `pfstate` y otra de `pfstate->pmap`).

### El mecanismo, con caso minimo

`true_dependence` de GCC 2.9 termina asi:

    return (memrefs_conflict_p (...)
        && ! (MEM_IN_STRUCT_P (mem) && rtx_addr_varies_p (mem)
              && ! MEM_IN_STRUCT_P (x) && ! rtx_addr_varies_p (x))
        && ! (MEM_IN_STRUCT_P (x) && rtx_addr_varies_p (x)
              && ! MEM_IN_STRUCT_P (mem) && ! rtx_addr_varies_p (mem)));

`pfstate->campo = v` es **struct + direccion variable** y la carga de un global
escalar es **!struct + direccion fija**: la primera clausula dispara siempre y la
escritura **nunca** invalida la carga. `MEM_IN_STRUCT_P` de un global solo lo
pone un **tipo agregado**.

Caso minimo con las banderas reales de la unidad
(`-O2 -fno-strict-aliasing -ffast-math -G8`), tres escrituras por un puntero a
struct y cuenta de cargas del global:

    v1  extern S *pfstate;                                    1 carga (cachea)
    v2  extern S *volatile pfstate;                           3 cargas, pero
                                                              hoisteadas al principio
    v3  ((int*)pfstate)[i] = ...                              2 cargas
    v5  static S *pfstate;  (miembro estatico de clase)       1 carga
    v6  con __attribute__((section(".sdata")))                1 carga
    v7  con -G0                                               1 carga
    v8  extern S *pfstate[1];  y  pfstate[0]->x = ...         3 cargas  <- EXACTO
    v9  struct G { S *p; }; extern G Path;  Path.p->x = ...   3 cargas  <- EXACTO
    v10 referencia   S *&r = pfstate;                         1 carga
    v11 puntero-a-puntero  S **pp = &pfstate;                 1 carga
    v12 (&pfstate)[0]->x                                      1 carga
    v13 (*&pfstate)->x                                        1 carga
    v14 struct anonima en namespace                           3 cargas (simbolo mal)
    v15 union anonima en namespace                            1 carga (simbolo mal)
    v16 cast a referencia-a-array (macro)                     3 cargas, pero
                                                              materializa la direccion con `la`

**v8 reproduce el objetivo instruccion a instruccion**, incluida la rotacion de
registros (`lwz r9 ... stw ... lwz r9 ... stw ... lwz r11`), y **conserva el
simbolo** `_4Path.pfstate` y el `@sda21`.

### Lo que NO es

* **No es `volatile`.** El volcado DWARF del original dice
  `struct PATHFINDERSTATE * pfstate; // size: 0x4, address: 0x804FF5B8`, sin
  cualificador, y `volatile` si aparece en el volcado (4.127 veces) cuando lo
  hay. Ademas `volatile` daria **mas** cargas de las que el objetivo tiene: en
  el bucle de `getmastertrack` el objetivo carga `pfstate` **una vez** y la
  reutiliza para `track[t]`, para leer `masterlatency` y para escribirlo.
* **No es la version del compilador.** Las cinco ProDG del arbol (3.5, 3.5b140,
  3.7, 3.8.1, 3.9.3) cachean igual en el caso minimo.
* **No son las banderas.** Barridas sobre el caso minimo: `-fstrict-aliasing`,
  `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-expensive-optimizations`, `-O1`,
  `-fvolatile`, `-fno-schedule-insns`. Solo `-fvolatile-global` y `-O0` recargan,
  y los dos dan un reparto de cargas distinto al del objetivo.

### Por que la vista agregada va POR FUNCION y no en la cabecera

Probado y **revertido**: con `#define pfstate pfstate_agg[0]` para las trece
unidades que incluyen `pathi.h`,

    +188 B  pathtrack      -896 B  pathnode
    +392 B  pathinit       -572 B  pathevent
                            -96 B  pathserv
    ------------------------------------------
    -984 B netos, -4 funciones

Es decir: **la heuristica no falta siempre en el objetivo**. Donde el original
si cachea, dar tipo agregado al global mete recargas que el objetivo no tiene.
Por eso la vista agregada se usa **solo donde el objetivo demuestra la
invalidacion**, y por eso `pfstate` se queda declarado como escalar (que es lo
que dice el DWARF) con la vista agregada al lado.

### Donde se ha aplicado, y lo que ha cerrado

| funcion | global | antes | despues |
|---|---|---:|---:|
| `PATHI_getmastertrack` (188 B) | `Path::pfstate` | 66,511 % | **100 %** |
| `PATH_addmapfile` (580 B) | `Path::pfstate` | 82,503 % | **100 %** |
| `PATHI_inittrack` (680 B) | `Path::volscale`, `Path::defaultfxbus` | 82,512 % | **100 %** |
| `PATH_setallocator` (64 B) | `PathToIAllocator::memimp` | 52,750 % | **100 %** |

En `PATHI_inittrack` y `PATH_setallocator` la palanca es la **misma heuristica
en la otra direccion**: la carga (o la escritura) de un global escalar en
direccion fija se puede mover libremente respecto de las escrituras a campos de
struct, y el objetivo no la mueve. En `PATH_setallocator` el sintoma era que el
`stw` de `memimp` se hundia por debajo de las tres cargas de `flags`; en
`PATHI_inittrack`, que `lwz defaultfxbus` y `lbz volscale` se izaban por encima
de los `stw` de `track->`.

---

## 2. Ensayos numerados

### `PATHI_getmastertrack` (pathtrack, 188 B) y `PATH_addmapfile` (pathinit, 580 B)

    c1  sonda: alias de `_4Path.pfstate` con tipo array[1] en pathtrack.cpp,
        `PFS->` en las seis referencias de getmastertrack        -> 100 %   <- queda
    c2  la misma sonda en pathinit.cpp para las diez de addmapfile
                                                                 -> 100 %   <- queda
    c3  `extern PATHFINDERSTATE *pfstate_agg[1] __asm__(...)` movida a
        pathi.h + `#define pfstate pfstate_agg[0]` (las 13 unidades)
                                                                 -> **-984 B**  descartado
    c4  declaracion en pathi.h SIN el macro, uso solo en las dos funciones
                                                                 -> +768 B  <- queda
    c5  `extern ... PATH_SDA __asm__(...)`  -> error de sintaxis; el orden
        correcto es `__asm__(...)` ANTES del atributo
    c6  ablacion: quitar la vista agregada de una de las dos las devuelve a
        66,5 % / 82,5 % exactos

### `PATHI_init` (pathinit, 188 B) — bug de `audit.py`, coste cero bytes

`audit.py` lo daba al 100 % con `FALLA: reubicacion a otro simbolo:
_4Path.timercalls contra _4Path.timertimespent`. Los cuatro contadores se ponen a
cero con `stw r9,<sym>@sda21` y **objdiff no ve la diferencia** porque el simbolo
va en la reubicacion. El orden emitido rota el ultimo al principio, asi que:

    fuente  timercalls, tasktimespent, taskcalls, timertimespent -> FALLA
    fuente  tasktimespent, taskcalls, timertimespent, timercalls -> ok  <- queda

Con eso los cuatro `stw` apuntan al simbolo correcto. Es el patron
«el 100 % que miente» y lo caza `audit.py`, no el porcentaje.

### `PATHI_loadbankdata` (pathbank, 396 B): 88,848 % -> 87,778 %, pero **sin nada que falte ni sobre**

    c7  `subbank->ready/subbanknum` antes de `char newpath[512]` y
        `fileop = PATH_UNLIKELY_VALUE` justo antes de LoadFile
                                                     -> 82,162 %   <- queda
        (con esto el juego de registros salvados pasa de `stmw r26`
         a `stmw r27`, que es el del objetivo)
    c8  `subbank = track->trackimp->GetSubBankPtr(...)` en vez de usar
        el resultado directamente en el `if`          -> 87,778 %   <- queda
        (el objetivo hace `cmpwi r3,0` y rematerializa `li r0,0`;
         nosotros haciamos `mr. r27,r3` y reusabamos el cero conocido
         tras el `bne`. Al asignar a `subbank` la equivalencia muere)
    c9  `fileop` antes del sprintf                    -> 81,556 %  descartado
    c10 `fileop` antes del strlen                     -> 77,475 %  descartado
    c11 `fileop` antes de `char newpath[512]`         -> 91,737 %  descartado
        (sube el porcentaje pero deja el `lis 0xbedf` diez filas antes;
         es la trampa del porcentaje, c8 esta estructuralmente mas cerca)
    c12 `int fileop = PATH_UNLIKELY_VALUE;` junto a newpath   -> 77,475 %
    c14 declaracion con inicializador junto a la llamada      -> 87,778 %
    c16 `fileop` declarado antes que `len`                    -> 87,778 %
    c18 orden de declaracion segun el DWARF del original
        (`subbank`, `newpath[512]`, `len`, `fileop`)          -> 87,778 %  <- queda

**Tras c8+c18 `triage` dice `faltan 0, sobran 0`** y la unica diferencia es
**una** decision del planificador: el objetivo emite `lis r0,0xbedf` justo
despues del `sprintf` y antes de `lwz realimp@sda21`, y nosotros lo emitimos
cinco filas mas tarde (y por eso el puntero de la vtable cae en r9 y no en r0).

### `PATHI_inittrack` (pathtrack, 680 B): 82,512 % -> 100 %

    d1  `c = (c && *p) ? (char)(*p|0x20) : (char)0;` en vez del if/else
                                             -> 83,188 %  <- queda
        (el objetivo calcula el valor en r0 y hace `mr r9,r0`; con el
         if/else escribiamos r9 en las dos ramas. Era el `mr+1` que
         `triage` venia senalando desde la ronda 19; tras d1 el
         multiconjunto queda exacto)
    m1  vistas agregadas de `Path::volscale` y `Path::defaultfxbus`
        dentro de inittrack                  -> **100 %**  <- queda

El mapa de lineas del original (`libdiff --lines`) confirma que el orden de
sentencias del bloque de inicializacion (lineas 404-428) **ya coincidia** campo
a campo con el nuestro; lo unico que fallaba era donde se izaban las dos cargas
de globales.

### `PATH_setallocator` (pathreal, 64 B): 52,750 % -> 100 %

El mapa de lineas fue decisivo: en el original **las tres copias de campo estan
en la MISMA linea (75)**, o sea que es una asignacion de struct entera.

    h1  `PathToIAllocator::memimptags = flags;` (una sentencia)
                                             -> 43,438 %
        (baja el porcentaje pero ya deja las tres cargas vivas a la vez
         en tres registros, que es lo que hace el objetivo)
    h2  h1 + vista agregada de `PathToIAllocator::memimp`
                                             -> **100 %**  <- queda

Con h2 `pathreal` queda al **100 % (496/496 B)**.

### `PATHI_calcwaitbeat` (pathnode, 336 B, 98,869 %) — veda

El DWARF del original dice `float scalar; // f11`, `float fevery; // f11`,
`float foffset; // f0`, y **no tiene ninguna local llamada `notes`**: el pin
`register float notes asm("fr11")` que hay en el arbol pone en f11 el temporal,
no `scalar`.

    k1  sin `notes`, `scalar = (float)beatinfo->notes / (float)note;`  -> 98,512 %
    k2  k1 + `register float scalar asm("fr11")`                        -> 96,429 %
    k3  `float notes` sin pin                                           -> 98,512 %

Las tres bajan; el pin actual (98,869 %) se queda. La diferencia que queda son
**14 registros de coma flotante permutados** (nosotros f8 para `scalar`, f10 para
la constante magica; el objetivo f11 y f9). Barrida la sentencia
`scalar = notes / note` en sus tres formas.

---

## 3. Vedas por bandera, medidas

Las dos pasadas de planificacion son **necesarias** en las cinco unidades
(porcentaje ponderado por unidad, `-c function_reloc_diffs=none`):

    unidad      base     -fno-schedule-insns   -fno-schedule-insns2
    pathreal    87,10 %        37,90 %                31,45 %
    pathtrack   55,34 %        12,62 %                15,21 %
    pathbank    81,63 %         0,00 %                 8,91 %
    pathnode    84,68 %         5,42 %                 5,52 %
    pathinit   100,00 %        12,86 %                36,54 %

No hay nada que ganar por bandera de planificacion en esta biblioteca.

---

## 4. Lo que queda: 1.464 B en 4 funciones

`triage --muro` sobre las cinco unidades al terminar:

    308 B  93,052 %  pathnode  PATHI_nextnode         faltan 3, sobran 1  stw+1, mr+1
    424 B  99,925 %  pathtrack PATH_createstreamimp   8 registros
    396 B  87,778 %  pathbank  PATHI_loadbankdata     reorden local, dmax 4, CERO reales
    336 B  98,869 %  pathnode  PATHI_calcwaitbeat     14 registros

* **`PATHI_nextnode`** — el objetivo gasta **un registro salvado mas** (`stwu
  -0x10`, `stw r31,0xc`) para dejar `forreal` en r31, y con eso puede mantener
  `Path::pfstate` vivo en r6 (`mr r6,r9`) a traves del inline de
  `PATHI_getnode`; nosotros metemos `forreal` en r12 (no hay llamadas) y
  **recargamos** `pfstate` en el punto de union. Es la direccion CONTRARIA a la
  de §1: aqui el objetivo cachea y nosotros no, asi que la vista agregada no
  sirve (meteria mas cargas). El orden de locales del DWARF (`nodeinfo`,
  `branches`, `track`, `i`, `nextnode`) ya coincide con el nuestro.
* **`PATH_createstreamimp`** — reconfirmada la medida de la ronda 18: las 106
  instrucciones son identicas y solo falla el marco (0x28 contra 0x20). El
  hueco son **8 bytes alineados a 8** entre `trackimp` (r1+0x8) y los registros
  salvados, o sea una ranura DFmode pedida **despues** de `trackimp` (cuya
  ranura nace tarde, al expandir `&trackimp` en la linea 110). Probado y
  descartado en esta ronda: `(void)(double)byterate;` (GCC pliega la conversion
  antes de pedir la ranura, sin cambio) y `volatile double pad;` en el bloque
  interior (la ranura cae en 0x8 y empuja `trackimp` a 0x10, ademas de emitir un
  `lfd`). El mapa de lineas dice que **las lineas 101-109 del original no emiten
  ninguna instruccion**.
* **`PATHI_loadbankdata`** — multiconjunto exacto, orden de declaracion segun el
  DWARF, registros salvados correctos; queda **una** posicion de planificador.
* **`PATHI_calcwaitbeat`** — permutacion de FPRs; ver la veda k1-k3.

---

## 5. `audit.py`: la pasada del principio

Una pasada al empezar y otra al terminar sobre las **14 unidades** que incluyen
`pathi.h`. Resultado:

1. **`pathinit::PATHI_init`** — FALLA real, corregida (ver §2). Estaba al 100 %
   por objdiff y escribia a cuatro globales cruzados.
2. **`pathreal6::__11PathToReal6`** — `FALLA: reubicacion a otro simbolo:
   gcc2_compiled._80399588 contra REAL_abortmessage__FPCce`. **Falso positivo
   confirmado**: el volcado DWARF pone `REAL_abortmessage` en
   `0x80399588 -> 0x80399660`, exactamente la direccion a la que apunta la
   reubicacion; `audit.py` resuelve esa direccion al marcador `gcc2_compiled.`
   que cae ahi. Es el mismo patron del sufijo `_<addr>` que documenta
   `docs/analisis/r18-libsn.md` §8. `pathreal6` esta al 100 % (916/916 B) antes
   y despues, y no la he tocado.
3. Las otras doce unidades: **cero fallos** en las dos pasadas.

---

## 6. Que NO he probado

* **`permuter.py`** sobre ninguna de las cuatro que quedan. En
  `PATHI_loadbankdata` (una sola posicion, multiconjunto exacto, mismos
  registros salvados) es el caso mas barato que he visto para intentarlo.
* **`PATHI_nextnode`**: no he probado ninguna variante de fuente, solo
  diagnostico. No he intentado forzar el registro salvado con
  `register int forreal asm("r31")` (seria restriccion de registro, y el brief
  la deja como ultimo recurso).
* **La otra mitad de la asimetria de §1**: no he investigado por que en
  `pathnode`/`pathevent`/`pathserv` el objetivo SI cachea. Si hubiera una regla
  que separase los dos casos, la vista agregada podria ir en la cabecera y valer
  mas bytes; tal como esta, la aplicacion es por funcion y con medida.
* **El resto de globales de `Path`**: solo he dado vista agregada a `pfstate`,
  `volscale`, `defaultfxbus` y `memimp`. No he barrido si `songprogress`,
  `eventrelease`, `eventaction`, `inited`, `paused`, `bankservice`,
  `milliseconds`, `debugchannels`, `memalloc`, `memfree`, `timercalls`… mueven
  algo en las nueve unidades de `path` que no he tocado (`pathaction`,
  `pathcontrol`, `pathdebug`, `pathevent`, `pathrand`, `pathserv`, `pathsnd`,
  `pathvol`, `pathreal6`), que ya estan al 100 % salvo `pathevent` y `pathserv`
  (tambien al 100 %). **Este es el frente mas prometedor que dejo abierto para
  otras bibliotecas**: la heuristica de `true_dependence` no es cosa de `path`.
* **La cadena completa (link + DOL)**: ninguna unidad de `path` esta enlazada
  (`linked False`), asi que el DOL no puede cambiar; no he ejecutado la
  construccion completa ni he tocado `configure.py`.
* **`pathreal6`, `pathaction`, `pathsnd`, `pathevent`**: fuera del encargo y ya
  al 100 %; solo las he recompilado y auditado por ser clientes de `pathi.h`.
