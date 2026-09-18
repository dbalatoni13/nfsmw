# Ronda 18 — grupo «bibliotecas de EA» (path, spch, vp6, realmemcard, realcore)

Medida propia: `antes_r18_libs.json` → `despues_r18_libs.json`
(`measure.py` sobre criticalpath, pathaction, pathnode, pathsnd, pathtrack,
spchpick, filesys*, gc_driver, gc_interface, eaxadecf).

    14 unidades  58.824/70.196 B  83,7996%   ->  63.056/70.196 B  89,8285%
    measure.py --cmp:  +4.232 B, +2 funciones, 1 unidad cambia
        +4232 B  +2 fns  .../path/5.01.04/source/cmn/pathaction   1.280 -> 5.512

**La tabla del encargo estaba rancia.** Al medir contra los objetos actuales,
`criticalpath` no estaba al 54,6% sino al 93,9%, `pathsnd`/`gc_driver`/
`gc_interface` ya estaban al **100%** y `filesys` al 94,8%. Lo pendiente real de
mi frente al empezar eran 10.692 B en 15 funciones, no 25 kB.

## Lo cerrado: `pathaction` entero, 4.232 B

| función | dirección | bytes | antes | después |
|---|---|---:|---:|---:|
| `PATHI_serviceaction(PATHEVENT*, PATHACTION*)` | 0x8037A2B8 | 3.252 (813 instr.) | 98,6396% | **100%** |
| `PATHI_getvalue(int,int,PATHTRACK*,PATHEVENT*)` | 0x8037B1AC | 980 (245 instr.) | 96,6122% | **100%** |

- SHA256 del objeto objetivo `build/GOWE69/obj/.../pathaction.o`:
  `b11681266ab4e462498788e29fc0203267b2f2adfc64f3f7f0dc13a0e58fb06b`
- **Censo de llamadas DIRECTAS** (`bl` con destino a esas direcciones en todas
  las secciones ejecutables del ELF original): `PATHI_serviceaction` 1,
  `PATHI_getvalue` 18. Es un censo de `bl` directos: **no prueba** que no haya
  punteros a función ni entradas de vtable apuntando ahí.
- Llamados y contrato: `PATHI_gettrackptr(unsigned)`→r3, `PATHI_stop(PATHTRACK*)`,
  `PATHI_enternode(int,int,int,int)`, `PATHI_queuenode(PATHTRACK*)`,
  `PATHI_getvalue`, `PATHI_conditiondone(PATHACTION*,int,PATHACTION*)`,
  `PATHI_restoretolastwhile`, `PATHI_trackstatus`, `PATHI_printf` (varargs, exige
  `crclr cr1eq`), más las llamadas virtuales por vtable sin thunk
  (`lwz vptr; lha delta; lwz fn; mtlr; add r3,this,delta; blrl`).
- **Prueba**: `python scripts/audit.py Speed/Indep/Libs/path/5.01.04/source/cmn/pathaction`
  da `ok` en las 6 funciones de la unidad (3.252 B/149 ramas/214 relocs/2
  literales en `serviceaction`; 980 B/80/95/0 en `getvalue`).
- **Congelado**: `python scripts/frozen.py cong …/cmn/pathaction`;
  `chk` responde «idéntico al congelado» después de añadir los comentarios.
  (`scripts/frozen.py` estaba **borrado del árbol de trabajo** aunque sí en HEAD;
  lo repuse con `git checkout -- scripts/frozen.py`, sin modificarlo.)

### Ensayos numerados (`PATHI_serviceaction`, 98,6396% → 100%)

    c1  quitar el `nodeinfo != 0 &&` del bucle de secciones     -> 98,89176%  <- queda
        (código defensivo de más: el objetivo desreferencia el
         resultado nulo de PATHI_getnode sin comprobarlo)
    c2  llamada directa a PATHI_readyfornewrequest, sin el
        `if (track->loadingsubbank < 0)` duplicado del llamante -> 99,13776%  <- queda
    c3  `!(millisecs < 0x14)` en vez de `>= 0x14`               -> 99,13776%  descartado (fold lo revierte)
    c4  barreras `__asm__ volatile("":::"memory")` en lastendif
        y lastwhile                                             -> 99,56212%  descartado (sustituido por c18)
    c5  `static volatile unsigned int lastwhile/lastendif`       -> 99,13776%  descartado (volatile NO ordena aquí)
    c6  barrera estrecha `"m"(*action)`                          -> 99,56212%  descartado (sustituido por c18)
    c7  `voice != 0` en vez de `voice == 0` (polaridad del `beq`
        del objetivo en la línea 52 del original)               -> 99,56827%  <- queda
    c8  cadena `!(A || B || C)` con `< 0x14`                     -> 99,56827%  descartado (De Morgan + fold)
    c9  `case 0:` compartiendo cuerpo con `default:`             -> 99,64822%  paso intermedio
    c10 `case 0:` SIN `default:` (el `b` de «sin caso» va al
        final del switch, no al cuerpo de `done = 1`)           -> 99,65437%  <- queda
    c11 `trackhandle` en UNA sentencia: `(mask) & idflags` y
        luego `|= trackFlag`                                    -> 99,73431%  <- queda
    c12 clobber "memory" en los dos sitios                       -> 99,73431%  descartado
    c13 `if (x<0x14) {} else {cuerpo}`                            -> 99,73431%  descartado (jumpifnot vuelve a GE)
    c14 `if (A||B||C) {} else {cuerpo}`                           -> 99,73431%  descartado (ídem)
    c15 `switch (act.waittime.millisecs) case 0x14..0x16`         -> 99,74170%  <- queda
    c16 sin barrera en lastendif                                  -> 99,50800%  descartado
    c17 lectura `volatile` de milliseconds                        -> 99,55720%  descartado (+1 instrucción)
    c18 dependencias de memoria PRECISAS:
        `__asm__("" : "=m"(milliseconds) : "m"(*action))` y
        `__asm__("" : "=m"(*action) : "m"(lastwhile))`          -> 99,93850%  <- queda
    c19 fijar el valor de lastendif a r9                          -> 99,95080%  <- queda
    c20 fijar a r0 el bit de `action->done == 0`                  -> **100%**  <- queda

Ablaciones sobre el 100%: quitar c19 → 99,9877%; quitar el asm de lastendif →
99,754%; quitar el de lastwhile → 99,6064%. Las cuatro construcciones son
necesarias.

### Ensayos numerados (`PATHI_getvalue`, 96,6122% → 100%)

    c21 `PATHI_getnode` como `if (rango) node = X; else node = 0;`
        en pathi.h                                              -> getvalue 99,673%, serviceaction 98,930%  descartado
    c22 `PATHI_getnode` como `if (rango) return X; return 0;`    -> getvalue 100%, serviceaction 99,490%   descartado
    c23 `PATHI_getnode` con dos `if` sueltos                     -> getvalue 96,612%, serviceaction 99,323% descartado
    c24 pathi.h SIN tocar y guarda en el llamante:
        `nodeinfo = track->node >= 0 ? PATHI_getnode(...) : 0;` -> **getvalue 100% y serviceaction 100%**  <- queda

Lo que lo desbloqueó fue el mapa de líneas: en el objetivo el `blt` del test
`< 0` y el `li r3,0` de `getvalue` están atribuidos a **pathaction.cpp:586/591**
(el llamante), mientras que en `serviceaction` los mismos dos están atribuidos a
**pathi.h:800/805** (el inline). O sea: la guarda `>= 0` está en el llamante y el
`< 0` del inline se elimina por redundante. `pathi.h` queda sin tocar y las 13
unidades que lo incluyen no se han recompilado.

### Los cuatro «último recurso» que quedan en el árbol

Están comentados en la fuente, no emiten ninguna instrucción y pasan `audit.py`:

1. `__asm__("" : "=m"(Path::milliseconds) : "m"(*action))` — declara que la carga
   de `milliseconds` depende del almacenamiento de `action->done`.
2. `register unsigned int endifms asm("r9")` — el objetivo deja ese valor en r9.
3. `__asm__("" : "=m"(*action) : "m"(lastwhile))` — la carga de `action->done` no
   puede subir por encima del almacenamiento de `lastwhile`.
4. `register unsigned int notdone asm("r0")` — el bit negado va a r0.

**Por qué hicieron falta**: con `-fno-strict-aliasing` (que sí está en los cflags
de la unidad) el compilador *debería* ser conservador entre `unsigned int
milliseconds` y el campo de bits `unsigned int done : 1` de `PATHACTION`, y no lo
es: adelanta la carga por encima del almacenamiento. No he encontrado la forma de
fuente que lo reproduzca; las barridas están en c3–c6, c12, c13, c16, c17 y en la
lista del punto 1 de los comentarios. **Esto es lo primero que hay que volver a
atacar**: si aparece la forma real, caen los cuatro de golpe.

## Lo que NO he cerrado, y qué sé de cada uno

Total pendiente en mi frente tras la ronda: **6.460 B** en 13 funciones.

### `criticalpath` (vp6) — 740 B, `VP6_PredictFilteredBlock` 94,3946%

Diferencia concentrada en `recon.inl:76-94` (el bloque `ModX/ModY/mx/my/
TempBuffer/Stride/TempPtr`): mismas instrucciones, distinto reparto de registros
y distinto orden dentro del bloque. La función ya lleva **cinco** `register …
asm()` de la ronda anterior (r23-r27). El orden de sentencias del original que
da el mapa de líneas es: 76 mVx, 77 mVy, 80 ModX, 81 ModY, 84 mx, 85 my, 87/88
los shifts, 91 TempBuffer, 92 Stride, 93 TempPtr1=TempPtr2=0 — **es el mismo que
el nuestro**. No he probado nada aquí: sólo diagnóstico.

### `pathtrack` — 1.292 B

- `PATH_createstreamimp` 424 B, **99,92453%**. Las 106 instrucciones son
  idénticas; **lo único que falla es el marco de pila: 0x28 en el original y
  0x20 en el nuestro**. El DWARF del original dice que la única local en pila es
  `trackimp // r1+0x8`, así que los 8 B de más son un *temporal del compilador*
  colocado por encima de `trackimp`. Ensayos: `int pad[2]` sin usar → GCC lo
  elimina, sin cambio; `volatile int pad[2]` → el marco **sí** crece a 0x28 pero
  el hueco cae en r1+0x8 y empuja `trackimp` a 0x10 (peor), y lo hace tanto si se
  declara antes como después. Dato suelto: en el original las líneas 101-109 no
  emiten ninguna instrucción (`byterate` en la 100 y `CreateStreamTrack` en la
  110), y el recuento de instrucciones coincide, así que no falta código.
- `PATHI_inittrack` 680 B, 81,1%. El orden de sentencias del original
  (líneas 404-434, leído del mapa de líneas) **coincide con el nuestro**
  campo a campo, incluidos los dos `fade` que sólo inicializan `fadefrom` y
  `fadenum`. Lo que difiere es la planificación dentro del bloque recto de
  inicializaciones y una instrucción de más por nuestro lado (684 vs 680 B).
- `PATHI_getmastertrack` 188 B, 66,5%. Sin mirar.

### `spchpick` — 2.184 B

- `iSPCH_MakeSampleRequests` 524 B, **98,3206%**. **Un solo bloque de diferencia**
  (4 instrucciones): el objetivo emite `lwzx r3,r31,r9` *antes* de `add r30,r31,r9`
  y nosotros al revés; el resto es el arrastre de registros. Mismo orden de
  sentencias en la fuente.
- `iSPCH_ChooseSamples` 412 B, 98,0%. El objetivo usa **un registro salvado más**
  (`stmw r17` frente a nuestro `stmw r18`) para guardar `bank + 0xC` desde antes
  del bucle y luego hace `add r31,r17,r0` + `addi r5,r31,0x2`; nosotros plegamos
  las constantes en `add r31,bank,r0` + `addi r5,r31,0xe`. Ensayo: mover la base
  del bucle a `bank + 0xC` y pasar `sampleData + 2` → **93,99%** (GCC gasta dos
  registros salvados más). Revertido.
- `iSPCH_SentenceLength` 152 B, 90,5%; `iSPCH_ChooseSentence` 848 B, 82,2%;
  `iSPCH_SentenceIsContextMatch` 248 B, **32,9%**. Este último tiene la
  aritmética de direcciones de los accesores de `spchlib.h:928-958` en otro
  orden y con otros desplazamientos — huele a «cabecera que miente» (stride o
  offset mal en un accesor), no a planificación. Es el candidato con más margen
  y no lo he tocado.

### `pathnode` — 644 B

- `PATHI_calcwaitbeat` 336 B, **98,8691%**. Sólo reparto de registros de coma
  flotante: el objetivo usa f9 para la constante mágica de conversión y f11 para
  `scalar`; nosotros f10 y f8. Ensayo: mover el pin `asm("fr11")` de `notes` a
  `scalar` → **96,4286%** (y 328 B). Revertido.
- `PATHI_nextnode` 308 B, 93,05%. El objetivo gasta un registro salvado (r31) que
  nosotros no, y mantiene `Path::pfstate` vivo en r6 a través del inline de
  `PATHI_getnode` en vez de recargarlo.

### `filesys` (realcore) — 568 B

`FILE_nametodevice` (316 B, 98,73%) y `AddToQueue` (252 B, 95,08%). Las notas de
`scratchpad/ai_filesys_round7_notes.md` documentan dos rondas de barridos
agotados sobre las dos. **No he vuelto a tocarlas**: repetirlas era lo más caro
que podía hacer.

### `eaxadecf` (snd/coda) — 1.316 B al 0%

`configure.py:981` **ya declara**
`Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf.cpp`: basta con
crear el fichero para que entre en el build, sin tocar `configure.py`. El frente
sigue entero y sin empezar. (Igual pasa con `mpeghufftables` y `mpegl3base`, que
tampoco tienen fuente.)

## Qué NO he probado

- **`eaxadecf`**: ni una línea. Es 1.316 B y 9 funciones escritas desde cero.
- **`iSPCH_SentenceIsContextMatch`** (248 B al 32,9%) y **`iSPCH_ChooseSentence`**
  (848 B al 82,2%): sólo los he mirado por encima.
- **`PATHI_getmastertrack`** (188 B al 66,5%): ni lo he abierto.
- **`VP6_PredictFilteredBlock`**: sólo diagnóstico, cero ensayos.
- **El permutador** (`permuter.py`) sobre ninguna de las near-miss de
  planificación. Cinco rondas sin cerrar nada lo desaconsejaban, pero en
  `MakeSampleRequests` (4 instrucciones, un solo bloque) podría ser barato.
- **La cadena completa** (link + DOL): ninguna de mis unidades está enlazada
  (`pathaction` es `linked False`), así que el DOL no puede cambiar; no he
  ejecutado la construcción completa.
- **Cabeceras compartidas**: `pathi.h` y `spchlib.h` quedan sin tocar. Los
  ensayos c21-c23 sobre `pathi.h` se revirtieron y sólo se compiló `pathaction`
  con ellos; las otras 12 unidades que incluyen `pathi.h` no se recompilaron
  porque la cabecera volvió a su estado original antes de terminar.
