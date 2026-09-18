# Ronda 21 — realcore + vp6 + las nueve unidades de `path` sin tocar

Medida propia con `build_direct.py` antes de **cada** medida
(`antes_r21_rc.json` → `despues_r21_rc.json`). Ninguna de las 17 unidades la
construye `ninja`.

    antes:   53.508 / 56.168 B  95,2642 %  286 funciones al 100 %
    despues: 53.824 / 56.168 B  95,8268 %  287 funciones al 100 %

    measure.py --cmp antes_r21_rc.json despues_r21_rc.json
        +316 B, +1 funcion, 1 unidad cambia, NINGUNA baja
          +316 B  +1 fn  .../file/cmn/filesys   10.304 -> 10.620

Sobre **las 72 unidades de realcore + vp6** (que he reconstruido enteras):

    72 unidades  89.032 / 90.732 B  98,1264 %  460 funciones al 100 %

Ficheros tocados (dos): `.../realcore/6.24.00/source/file/cmn/filesys.cpp` y
`.../realcore/6.24.00/source/file/gc/dvd_device.cpp`. Ninguna bandera,
`configure.py` sin tocar, cero ensamblador.

---

## 0. El encargo estaba CADUCADO en un punto

El brief daba `inittmr` como «268 B al 43,22 %» y sugeria permutacion global de
registros o una constante mal puesta. **El 43,22 % es el porcentaje de la
UNIDAD** (204/472 B); la unica funcion que falla, `TIMER_init`, esta al
**97,015 %** con `reorden local, dmax 2, CERO reales`. `lreg.py` y `litcheck.py`
no aplican: no falta ni sobra nada.

Igual con `criticalpath`: «740 B al 93,85 %» es la unidad (11.300/12.040);
`VP6_PredictFilteredBlock` esta al **94,395 %**, `dmax 4`, CERO reales.

Las otras tres firmas del brief coinciden exactamente con lo que he medido.

---

## 1. La respuesta a la pregunta grande: `true_dependence` NO vale aqui

### En las nueve unidades de `path` sin tocar: **cero bytes, medido**

`pathaction`, `pathcontrol`, `pathdebug`, `pathevent`, `pathrand`, `pathserv`,
`pathsnd`, `pathvol` y `pathreal6` **ya estan las nueve al 100 %**:

    pathaction 5512/5512   pathcontrol 932/932   pathdebug 76/76
    pathevent  2520/2520   pathrand    264/264   pathreal6 916/916
    pathserv   1700/1700   pathsnd    8452/8452  pathvol  4124/4124

    triage.py --muro sobre las nueve  ->  0 funciones, 0 B

No hay ni una funcion near-miss donde probar la vista agregada. La punta suelta
que dejo la ronda 20 (**«el resto de globales de `Path` en las nueve unidades
que no he tocado»**) **no tiene trabajo dentro**: `songprogress`,
`eventrelease`, `eventaction`, `inited`, `paused`, `bankservice`,
`milliseconds`, `debugchannels`, `memalloc`, `memfree`, `timercalls`… no pueden
mover nada porque no queda nada que mover. **Frente cerrado con medida.**

La unica unidad de `path` con near-miss es `pathnode`, y sus dos funciones van
**en contra** (ver §5).

### En `realcore` y en `vp6`: **no, y por una razon estructural**

Tras cerrar `FILE_nametodevice`, en **las 72 unidades de realcore + vp6 quedan
exactamente CUATRO funciones near-miss y 1.700 B**, que son las cuatro de mi
encargo:

    740 B  94,395 %  vp6/criticalpath  VP6_PredictFilteredBlock  reorden, dmax 4, CERO reales
    440 B  93,591 %  realcore/dvd_device StartNonAlignedAyncRead  mr+6 (permutacion de 4 ciclos)
    268 B  97,015 %  realcore/inittmr    TIMER_init                reorden, dmax 2, CERO reales
    252 B  95,159 %  realcore/filesys    AddToQueue                mr.+1, beq+1, mr-1

**Ninguna de las cuatro presenta el sintoma** que delata el mecanismo (*«al
objetivo le sobran cargas de un global que nosotros cacheamos en registro»*):

* `StartNonAlignedAyncRead` — el objetivo y nosotros cargamos `gCurRead` el
  **mismo numero de veces** (9 filas `lis/addi/stw @l`, todas presentes en los
  dos lados). Las 9 filas que `triage` cuenta como `faltan/sobran` son
  **solo el nombre**: el objetivo lo llama `gCurRead_804CCFA0` (sufijo del
  troceador) y nosotros `gCurRead`. Descontadas, la firma real es
  **`faltan 6, sobran 0`, los seis `mr`**.
* `AddToQueue` — no toca ningun global; trabaja sobre `device->` (campos de
  struct en direccion variable, que es el lado que la heuristica *ignora*).
* `TIMER_init` — hay un global escalar (`bIsTimerInited`) y la diferencia SI es
  la posicion de su `stb`, pero **no la mueve el aliasing**: la he barrido con
  `volatile` (§4) sin ningun efecto. La decide el planificador.
* `VP6_PredictFilteredBlock` — no hay ninguna escritura a memoria en el bloque
  que falla, asi que `true_dependence` no tiene nada que invalidar.

**Conclusion medida: el mecanismo de la ronda 20 esta agotado.** No es que
falle en realcore/vp6: es que **no hay ninguna funcion con el sintoma** en las
72 unidades. Es un caso claro de lo que dice
`nfsmw-extrapolar-frentes`: un hallazgo en una biblioteca no es un frente
hasta contarlo en las demas.

---

## 2. CERRADA: `FILE_nametodevice` (316 B, 98,734 % -> 100 %)

Es el hallazgo transportable de esta ronda.

### El sintoma

`triage`: `faltan 1, sobran 0, bne+1`. Una sola rama. El objetivo tiene, al
entrar al bucle de busqueda de dispositivo:

    lwz  r31, 0x7c(r9)        ; Itr = AllocatedDevices.Begin()
    cmpwi r31, 0x0
    beq  .L_out               ; *Itr == 0
    li   r0, 0x0              ; found = false
    cmpwi cr4, r0, 0x0
    bne  cr4, .L_out          ; <-- ESTA nos faltaba
    .L_body:  ...
    cmpwi r31, 0x0
    beq  .L_out
    beq  cr4, .L_body         ; la copia del test al final

Nosotros emitiamos **el `cmpwi cr4` pero no el `bne cr4`**: la guarda de
entrada del bucle.

### El mecanismo

`duplicate_loop_exit_test` (jump.c) **copia** el test de salida al final del
bucle y deja el original arriba como guarda. La guarda de `!found` la teniamos,
pero **plegada**: `cse.c` trabaja por *bloques basicos extendidos* y, cuando
`found = false` cae en el MISMO bloque extendido que la guarda, `fold_rtx`
sabe que `r0 == 0`, la rama nunca se toma y desaparece. El `cmpwi cr4`
sobrevive porque cr4 se reusa al final del bucle, donde el bloque ya es un
punto de union y la equivalencia se ha perdido.

En el objetivo la guarda **no se pliega** porque `found = false` esta **antes de
la cadena `if/else if`**, o sea en el bloque de entrada de la funcion, separado
de la guarda por todas las etiquetas de la cadena.

### La palanca: **mover la definicion fuera del bloque extendido de la guarda**

    c1  la condicion entera en un `while (*Itr != 0 && !found)`   -> 91,582 %  descartado
        (aparece el `bne cr4` pero el bucle deja de rotar: el back-edge
         se vuelve un `b` incondicional)
    c2  ESTRUCTURA ORIGINAL intacta (`if (*Itr != 0) { for (; !found;) ... }`)
        y **`found = false;` subido junto a las declaraciones**, encima de
        `if (strchr(name, ':') != 0)`                              -> **100 %**  <- queda

`audit.py`: `ok 316 B, 13 ramas, 24 relocs, 0 literales`.
`frozen.py cong` sobre `filesys`: `25297d19fb1195d4`; `chk` dice
«identico al congelado».

**Regla general que sale de aqui** (vale para cualquier bucle con guarda):
*si al objetivo le sobra una rama de guarda sobre una variable que nosotros
inicializamos justo antes del bucle, la inicializacion tiene que estar en OTRO
bloque basico extendido.* No es `volatile`, no es una bandera: es **donde esta
la sentencia**.

El mapa de lineas lo confirmo antes de tocar nada: en el objetivo `li r0,0`,
`cmpwi cr4` y `bne cr4` cuelgan los tres de `filesys.cpp:2354` (la condicion),
no de una linea propia — el `li` lo bajo el planificador desde arriba.

---

## 3. `AddToQueue` (252 B, 95,159 %) — NO cierra, pero el DWARF corrige la fuente

### Lo que dice el DWARF del original (`libdwarf.py` sobre `filesys.cpp`)

    void FILEOPERATION::AddToQueue() {          // this: r31
        struct FILEDEVICE *device;              // r30
        struct ListSingleIterator<FILEOPERATION> Itr;   // r11   <- ITERADOR, en REGISTRO
        struct FILEOPERATION *prev;             // r10
        unsigned int newoppriority;             // r8
        /* anonymous block */ { unsigned int curpriority; /* r9 */ }
        inline operator*()  x1   (fuera del bloque anonimo)
        inline operator*()  x3   (DENTRO del bloque anonimo)
        inline operator++(int)   0x80382548 -> 0x80382554  (fuera del bloque)
    }

O sea: `for (Itr = device->pending.Begin(); *Itr != 0; Itr++)` con un
`unsigned int curpriority = ...;` declarado en el cuerpo (que es lo que crea el
bloque anonimo) y `prev = *Itr;` dentro de el.

### La diferencia real

    faltan 2, sobran 1:  el objetivo hace `mr. r10, r11` + `beq`,
                         nosotros `mr r10, r11` y nada mas.

`mr.` es la fusion (por `combine`) de `prev = *Itr` con el `if (node != 0)` de
`ListSingleIterator::operator++`. **Nosotros lo perdemos porque GCC pliega ese
test**: al bloque del incremento solo se entra por el `goto nextOperation` que
esta dentro de `if (current != 0 && …)`, asi que sabe que no es nulo.

### Ensayos

    c3  `while (*Itr != 0 && newoppriority >= …) { prev = *Itr; Itr++; }`
                                                              -> 83,651 %  descartado
        (disposicion equivocada: test arriba y `b` de vuelta abajo;
         el objetivo tiene `b` al test arriba y el test abajo)
    c4  forma de `goto` + iterador de la clase                 -> 90,143 %  descartado
        (**el `goto` manda el iterador a la PILA**: `stw r0,0x8(r1)` /
         `stw r11,0x8(r1)`; el DWARF dice que el original lo tiene en r11.
         Con esto queda descartado que el original use `goto`.)
    c5  forma de `goto` + puntero pelado + incremento escrito como
        `if (prev != 0) { current = prev->next; }` (sin `else`, como
        `operator++`)                                          -> 95,159 %  <- queda
    c6  c5 + `asm("" : "+r"(current));` antes de la copia      -> 93,571 %  descartado
        (**el `beq` SI aparece**, pero la barrera cuesta un `mr r11,r10`
         y un `cmpwi r10,0` extra: 2 de mas por 1 de menos)
    c7  `for (Itr = Begin(); *Itr != 0;) { curpriority…; break; prev; Itr++; }`
                                                              -> 83,651 %  descartado
    c8  c7 con el incremento en la cabecera del `for`          -> 83,651 %  descartado

**Se queda c5**, que mide exactamente igual que la linea base (95,15873 %) pero
lleva la fuente al estado que dice el DWARF: `device` en vez de `dev`,
`newoppriority` en vez de `sortkey`, y el incremento sin `else`.

### Vedas nuevas, con la sentencia barrida

* **Barrida la sentencia del bucle en cinco formas** (`while` con la condicion
  completa, `for` con y sin incremento en la cabecera, `goto` con iterador,
  `goto` con puntero): **ninguna forma de bucle de C reproduce la disposicion
  `b .L_test; cuerpo; .L_test:` salvo el `goto`**. El «roll» de
  `expand_end_loop` (que mueve la condicion de entrada al final) **no dispara
  en esta version del compilador para este bucle**, medido cuatro veces.
* **Barrida la sentencia del incremento en tres formas** (ternario con `else`,
  `if` sin `else` sobre `current`, `if` sin `else` sobre `prev`): GCC pliega el
  test nulo en las tres. **Solo la barrera `asm` lo devuelve, y cuesta 2
  instrucciones.**

---

## 4. `TIMER_init` (268 B, 97,015 %) — muro de planificador de DOS filas

    objetivo:  li r0,1 | lis r3,TIMER_restore@ha | addi r3,r3,@l | stb r0,bIsTimerInited@sda21 | bl
    nosotros:  li r0,1 | lis r3,TIMER_restore@ha | stb r0,…      | addi r3,r3,@l               | bl

`faltan 0, sobran 0`. Las dos instrucciones estan; el planificador mete nuestro
`stb` en el hueco entre el `lis` y el `addi`.

Es un empate en `rank_for_schedule`: `addi` y `stb` tienen la misma prioridad
(los dos dependen del `bl` — `flush_pending_lists` encadena toda escritura
pendiente a la llamada) y el desempate cae del lado del `stb`.

    f1  `void (*restorefn)() = TIMER_restore;` antes del `stb`
        (para que el `addi` nazca antes en orden LUID)          -> 96,119 %  descartado
        (baja el `li r0,1` en vez de subir el `addi`; ademas el DWARF del
         original solo tiene `now` r27, `Period` r29 y `tmp` r9: **no hay
         ninguna local de tipo puntero a funcion**)
    g1  `static volatile unsigned char bIsTimerInited`          -> 97,015 %  descartado
        (cifra IDENTICA: `volatile` no cambia nada aqui, y no esta en el DWARF)

**Veda**: barrida la sentencia `bIsTimerInited = 1;` en sus tres formas
(en su sitio, con temporal para el puntero a funcion delante, y con el global
`volatile`). El orden de fuente no puede ser otro: el `stb` esta **entre**
`OSSetPeriodicAlarm(...)` y `SYSTEM_addexit(...)` en el objetivo, y el
planificador no cruza llamadas, asi que cualquier otro sitio en la fuente lo
sacaria de ese hueco. **`true_dependence` no aplica: no hay ninguna carga que
invalidar.**

---

## 5. `StartNonAlignedAyncRead` (440 B, 93,591 %) — ciclo de 4 del asignador

El brief lo daba como «falta codigo de verdad, no es reparto». **Medido, es al
reves**: de las 15 que `triage` da por «faltan», **9 son solo el nombre del
simbolo** (`gCurRead_804CCFA0` contra `gCurRead`, ver §1). La firma real es
**`faltan 6, sobran 0`** y los seis son `mr`:

    mr r28, r3    (FileInfo -> r28)      x1
    mr r7,  r4    (MemPointer -> r7)     x1
    mr r3,  r28   (FileInfo de vuelta a r3 antes de cada DVDReadAsyncPrio)  x4

Es decir: **el objetivo saca `FileInfo` de r3 y lo guarda en un registro
salvado**, y nosotros lo dejamos en r3 (que es donde llega) y nos ahorramos las
seis copias. Es una **permutacion de cuatro pseudos en ciclo**:

    valor              nosotros   objetivo
    MemPointer            r4        r7
    PreBytes              r7        r3
    FileInfo              r3       r28
    MemStartAligned      r28        r4

`lreg.py` da la tabla y explica por que: los cuatro se asignan en orden de
prioridad y **se llevan r4, r7, r3, r28 en ese orden**; lo unico que cambia es
**quien va primero**.

    pseudo  n_refs  live_len  prioridad  reg
    83  MemPointer        8      43       5581   r4
    130 PreBytes          4      31       2580   r7
    82  FileInfo          5      76       1315   r3
    100 MemStartAligned   3      27       1111   r28

Para que `MemStartAligned` fuese el primero (que es lo que hace el objetivo)
haria falta `floor_log2(n)*n/live_len*10000 > 5581`, o sea **8 referencias con
`live_len` 27**, o `live_len < 6`. Ninguna de las dos se puede escribir: la
variable se guarda en `gCurRead` y se compara 40 instrucciones despues.
**Muro del asignador, cuantificado.**

    c11 `int sizealigned = …;` declarado dentro del `if (Size <= 31)`,
        que es lo que dice el DWARF (`readSize` r5 a nivel de funcion +
        un bloque anonimo con `sizealigned` r5, rango 0x8038554C)
                                                              -> 93,591 %  <- queda
        (cifra IDENTICA; se queda porque es la fuente correcta y cuesta 0 B)

`audit.py` sobre las 12 funciones al 100 % de `dvd_device`: cero fallos, dos
pasadas.

---

## 6. `VP6_PredictFilteredBlock` (740 B, 94,395 %) — permutacion + planificador

`faltan 0, sobran 0`, `dmax 4`. Las ~27 instrucciones del bloque
`recon.inl:75-94` son **las mismas**, con los registros permutados y el orden
movido 2-4 filas. Lo que `triage` marca (3 DELETE / 3 INSERT) es el punto ciego
documentado de objdiff: no puede emparejar filas que solo difieren en el
registro.

El DWARF confirma que **nuestra fuente ya es la del original**, incluido el
bloque anonimo con `mVx`/`mVy`/`mx`/`my`:

    pbi r31, OutputPtr r24, bp r23
    SrcPtr r27, TempBuffer r29, TempPtr1 r8, TempPtr2 r27, ModX r28,
    ModY r25, BicMvSizeLimit r6, Stride r26, MvShift r30, MvModMask r3
    bloque anonimo: mVx r10, mVy r0, mx r10, my r0

La funcion lleva **cinco `register X asm("rN")` nuestros**. Siguiendo la regla 4
del brief, lo primero fue quitarlos:

    h1  quitados los CINCO pines (OutputPtr, bp, TempPtr2, ModY, Stride)
                                                              -> 93,232 %  descartado
    h2  quitado SOLO el pin de `ModY`                          -> 94,016 %  descartado
    h3  pines `mVx` -> r10 y `mVy` -> r0 (los del DWARF)       -> 93,746 %  descartado
    h4  `TempBuffer = SrcPtr + Recon + (FrameReconStride * my + mx);`
        (el objetivo agrupa `stride*my + mx`, nosotros asociamos por la
        izquierda)                                            -> 89,757 %  descartado

**Los cinco pines se quedan: son lo mejor medido (94,395 %).** Este es el
contraejemplo de la regla 4 en esta ronda — aqui el pin **no** es la causa, es
lo que sostiene el porcentaje.

**Veda**: barrida la sentencia `TempBuffer = SrcPtr + pbi->mbi.Recon +
pbi->mbi.FrameReconStride * my + mx;` en sus dos asociaciones, y barrida la
declaracion del bloque en cuatro combinaciones de pines.

---

## 7. `PATHI_nextnode` (308 B, 93,052 %) — confirmado que va al REVES

La ronda 20 lo aviso; ahora esta medido fila a fila:

    objetivo                          nosotros
    stwu r1, -0x10                    stwu r1, -0x8
    stw  r31, 0xc(r1)   <- salva r31  (no salva nada)
    ...
    lwz  r9, _4Path.pfstate@sda21     lwz r9, _4Path.pfstate@sda21
    lwz  r10, 0x18(r9)                lwz r10, 0x18(r9)
    mr   r6, r9         <- CACHEA     (no cachea)
    ...
    lwz  r8, 0x0(r7)                  lwz r9, _4Path.pfstate@sda21  <- RECARGAMOS
    addi r9, r6, 0x40                 addi r9, r9, 0x40

**El objetivo cachea `Path::pfstate` en r6 y nosotros lo recargamos**: es el
sintoma EXACTAMENTE INVERSO al de la ronda 20. La vista agregada
(`pfstate_agg[1]`) meteria **mas** recargas, o sea empeoraria. Confirmado que
no sirve aqui.

    i1  `register int forreal asm("r31") = _forreal;`
        (la ronda 20 lo dejo sin probar; el objetivo gasta ese registro
        salvado y con el libera uno para mantener `pfstate` vivo)
                                                              -> 93,182 %  descartado
        **y ademas GENERA CODIGO MALO**: GCC pone `mr r31, r5` pero
        **no emite el `stw r31` del prologo** (`stwu r1,-0x8` y solo se
        salva LR). Pisamos r31 del llamante sin salvarlo.
        **Veda dura: no pinar r31 con `register asm` en esta biblioteca.**

Revertido; `pathnode` vuelve a 93,05195 % exactos y `git status` sale limpio
bajo `src/Speed/Indep/Libs/path/`.

`PATHI_calcwaitbeat` (336 B, 98,869 %) sigue como la dejo la ronda 20:
permutacion de 14 registros de coma flotante, veda k1-k3 vigente. No la he
tocado.

---

## 8. `audit.py`: las dos pasadas

**Primera pasada** (al empezar) sobre las cinco unidades del encargo mas las
nueve de `path`:

* `dvd_device` (12 fns), `filesys` (12), `inittmr` (3), `criticalpath` (12),
  `pathnode` (9): **cero FALLA**.
* `pathaction`, `pathcontrol`, `pathdebug`, `pathevent`, `pathrand`,
  `pathserv`, `pathsnd`, `pathvol`: **cero FALLA**.
* `pathreal6::__11PathToReal6`: `FALLA: reubicacion a otro simbolo:
  gcc2_compiled._80399588 contra REAL_abortmessage__FPCce`. **Es el falso
  positivo ya documentado en `r20-path.md` §5**, reproducido identico en las
  dos pasadas. No la he tocado (esta al 100 %, 916/916 B).

**Segunda pasada** (al terminar), sobre `dvd_device`, `filesys`, `filesys_c`,
`filesys_cc`, `inittmr`, `criticalpath` y `pathnode`: **cero FALLA**, incluida
`FILE_nametodevice` recien cerrada.

**Lista de FALLA reales: ninguna.**

---

## 9. Que NO he probado

* **`permuter.py`** en ninguna de las cuatro. `TIMER_init` es el caso mas barato
  que he visto para intentarlo: **dos filas, `faltan 0 sobran 0`, mismos
  registros salvados, una sola posicion de planificador**. Mas barato aun que
  el `PATHI_loadbankdata` que proponia la ronda 20.
* **`cc1plus -dG`** (mecanismo 2 de la seccion 8 de HERRAMIENTAS) sobre
  `PATHI_nextnode`, que es donde tocaria: el reparto entre `lis @ha` y recarga
  lo deciden `gcse`/PRE, y ahi se veria la decision literal en vez de deducirla.
* **`dwbody.py`** en ninguna. He usado `libdwarf.py cu/fns`, que da locales y
  registros, pero no he comparado el arbol de inlines de `VP6_PredictFilteredBlock`
  ni el de `StartNonAlignedAyncRead` contra el nuestro.
* **Bajar `n_refs`/`live_length` de `MemPointer`** en `StartNonAlignedAyncRead`
  reescribiendo las cinco expresiones que lo usan. He calculado que subir
  `MemStartAligned` es imposible, pero **no he probado a BAJAR el de
  `MemPointer`** por debajo de 2580 (haria falta `n_refs` 4 con `live_len` 43,
  o sea quitarle cuatro referencias), que es la otra mitad del ciclo.
* **Renombrar `gCurRead`** para que coincida con `gCurRead_804CCFA0`. No cuesta
  bytes (`audit.py` resuelve la reubicacion bien), pero **ensucia la firma de
  `triage.py` en 9 filas** y ha hecho que el brief diagnosticara «falta codigo»
  donde solo hay reparto de registros. Es cosa de `configure.py`/`splits.txt`,
  que tengo prohibido tocar.
* **La cadena completa (link + DOL)**: ninguna unidad de realcore, vp6 ni path
  esta enlazada, asi que el DOL no puede cambiar. No he ejecutado la
  construccion completa ni he tocado `configure.py`.
* **`filesys_c` / `filesys_cc`**: incluyen `filesys.cpp`; los he reconstruido y
  medido (544/544 B cada uno, sin cambio) pero no he auditado sus funciones una
  por una mas alla de la pasada normal.
