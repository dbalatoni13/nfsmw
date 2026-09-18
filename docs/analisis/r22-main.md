# Ronda 22 — zMain + zGameplay + zBWare

## Verificacion del encargo

Tras `build_direct.py zMain zGameplay zBWare`:

    3 unidades  323260/337020 B  95.9172%  2381 funciones al 100%
    zMain      155436/159776 B  97.2837%  1378 fns
    zGameplay  137232/141472 B  97.0029%   765 fns
    zBWare      30592/35772  B  85.5194%   238 fns

`triage.py --muro`: **5 funciones, 13.744 B** (el encargo decia 13.760 en seis;
los 16 B de mas son `Compare__…IconSort`, que no es trabajo). Las cinco
coinciden con lo caracterizado en `r21-libres.md`, salvo la premisa de
`ESpawnFragment`, que esta MAL (ver abajo).

`audit.py`, tres pasadas (una al empezar, dos al terminar) sobre las tres
unidades: **CERO FALLA**. 1.378 + 765 + 238 simbolos, todos `ok`.

## Resultado

    measure.py --cmp antes.json final.json  ->  +0 B, +0 funciones, 0 unidades cambian

| | antes | despues |
|---|---|---|
| `_bOutput` (zBWare, 5.180 B) | 99,64324 %, 39 filas | **99,84016 %, 37 filas** |
| las otras cuatro | sin cambio | sin cambio |

`matched_code` es todo-o-nada: la mejora de `_bOutput` no mueve bytes todavia.
`frozen.py cong Speed/Indep/SourceLists/zBWare` aplicado (huella 8daeadfc9a571504).

---

## 1. `_bOutput` (zBWare, 5.180 B) — 99,643 -> 99,840 %, el racimo B cerrado

**El mecanismo: un `__asm__` sin salidas es una barrera de planificacion.**

La barrera de la ronda 19/20 era `__asm__("" : : "r"(&number))`. Un asm **sin
operandos de salida es `volatile` implicito**, o sea una barrera COMPLETA para
`sched2`. Eso clavaba el `li r27,0` de `stringOut = nullptr` (linea 809 del
original) delante del `lwz r0,0x4(r8)` de `offset` (834), mientras el objetivo
sube el `lis r11,0x7ff0` (836) por encima de los dos.

Con una salida de memoria el asm **deja de ser volatil**, sigue forzando el
`stfd` de `number` (que es para lo que estaba), y el planificador recupera el
orden del objetivo:

    d6  __asm__("" : "+m"(number));      99,64324 -> 99,84016 %   APLICADO

Los dos INSERT/DELETE de las filas 592-597 desaparecen; lo que queda ahi es una
permutacion 1:1 de registros.

### Ensayos medidos (todos con la unidad construida entera)

    base                                            99,64324   39 filas
    d5  puntero temporal en vez del asm             99,48417   41
    d6  __asm__("" : "+m"(number))                  99,84016   37   <- APLICADO
    d7  `volatile` en la lectura, sin asm           99,48417   41
    d8  __asm__("" : : "m"(number))                 99,53436   39
    d9  __asm__("" : "=m"(number))                  99,30656   49  (5.156 B)
    d11 d6 pero delante de `stringOut = nullptr`    99,84016   37   (empata)
    d12 d6 detras del `offset = ...`                99,48417   41
    e1  `bool group_flag` en el bloque de enteros   identico        APLICADO (DWARF)
    e2  `int radix` antes de `int hexAdd`           identico        APLICADO (DWARF)

### La «pista sin explotar» de la ronda 20 esta CERRADA

`r20-muros.md` apuntaba a un hueco de 25 lineas en el original entre
`stringOut = nullptr;` (809) y `offset = ...` (834). **Comprobado con `lmap.py`:
en todo el volcado no hay NI UNA instruccion atribuida a las lineas 810-833.**
Son lineas que no emiten codigo (un `#if` apagado o un comentario), asi que no
pueden pesar en el RTL. No es una veta.

### Lo que queda (37 filas, todas permutacion pura, sin reorden)

| filas | que | estado |
|---|---|---|
| 550-560 | `subf r9/r10` y el `'0'` en r11/r9 (original 749-756) | el original escribe el `if (FL_FORCEOCTAL)` externo SIN llaves (6 lineas, 753-758), que para el compilador es identico |
| 592-614 | rotacion r8/r9/r11 en la cascada `_nan_table` | el reorden ya esta arreglado; queda solo el reparto |
| 619-678 | rotacion r0/r8/r9/r11 en los `lis @ha` del pool de `%f` | sin ensayos |
| 913/915 | dos `addi` permutados, mismos registros | sin ensayos |

Los tres primeros son pseudos **cortos** repartidos por `local-alloc`
(`qty_compare_1`, prioridad `floor_log2(n_refs)*n_refs*size/(death-birth)`), no
por `global_alloc`: la tabla de `lreg.py` no los ensena. Sus vidas y refs son
IGUALES a las del objetivo (comprobado fila a fila), asi que el empate lo rompe
el numero de qty, o sea el orden de primera referencia dentro del bloque basico.

### DWARF (`dwbody.py`): diferencias reales sin explotar

- **Sobran cuatro etiquetas**: el original solo tiene `GENERIC_HEX`,
  `GENERIC_INT`, `GENERIC_FLOAT`; nosotros anadimos `OUTPUT`, `positioned`,
  `Z_INT` y `VECT_OUTPUT`. Cuatro `goto` que el original no tiene.
- `int * __ptr` (original) contra nuestro `unsigned int * __ptr`.
- `int shift; // r27` en el original (tiene registro) y en el nuestro no.
- `char * p` declarado el PRIMERO de su bloque en el original.

---

## 2. `_._14ESpawnFragment` (zMain, 2.164 B, 98,869 %) — la premisa del encargo es FALSA

**«El reparto de ranuras de pila NO coincide» es un artefacto de leer el diff
por indice de fila.** Las ranuras coinciden TODAS. Emparejando por valor:

    objetivo  0x90<-f3(0.0)  0x94<-f3  0x98<-f3  0x6c<-f3  0x7c<-f3  0x8c<-f3  0x9c<-f1(1.0)
    nuestro   0x90<-f2(0.0)  0x94<-f2  0x98<-f2  0x6c<-f2  0x7c<-f2  0x8c<-f2  0x9c<-f3(1.0)

y `m_inner` esta en `r1+0x60`, o sea `[0][3]=0x6c`, `[1][3]=0x7c`, `[2][3]=0x8c`,
`[3][0..2]=0x90/94/98`, `[3][3]=0x9c`: exactamente los seis ceros y el uno de
`VU0_quattom4`. **No hay ningun temporal de mas ni de menos.**

Lo unico que pasa es una **rotacion de tres FPRs** entre las tres constantes que
`gcse`/`loop` izan, mas dos intercambios del planificador:

| constante | objetivo | nuestro |
|---|---|---|
| `1/32767` (`lbl_803F0B48`, de `_Q4c::Decompress`) | f2 | f1 |
| `1.0f` (`lbl_803F0B4C`, de `quattom4`) | f1 | f3 |
| `0.0f` (`lbl_803F0B50`) | f3 | f2 |

### El pase que decide, identificado

Las tres las reparte **`local-alloc`** (aparecen en `;; Register N in M` del
`.lreg` y NO en la lista «regs to allocate» de `.greg`): pseudos 203 (1/32767,
5 refs / 134 insns), 321 (1.0, 5/114) y 363 (0.0, 7/138). Los FPR volatiles se
reparten en orden descendente (f0 primero, luego f13..f1), asi que las tres
constantes, que son las de vida mas larga, se llevan las ULTIMAS: la primera en
asignarse coge f3, la siguiente f2 y la ultima f1.

    orden objetivo:  0.0 , 1/32767 , 1.0
    orden nuestro :  1.0 , 0.0     , 1/32767

Con `prioridad = floor_log2(n_refs)*n_refs*size/(death-birth)`, la palanca es
**alargar la vida del pseudo de `1.0` por debajo de la de `1/32767`**: en el
objetivo `1.0` muere en el `stfs f1,0x9c(r1)` (la ULTIMA instruccion del grupo,
indice 183) y en el nuestro muere 15 ranuras antes, en el `fsubs f3,f3,f9` del
indice 168 — donde `local-alloc` reutiliza su registro como destino porque ya
esta muerto. La causa raiz es, pues, **donde coloca `sched1` el
`stfs 1.0f -> [3][3]`**, y el reparto es la consecuencia, no el origen.

### Lo comprobado, y por que la fuente NO es el problema

- `dwbody.py`: el cuerpo DWARF **casa entero** (mismas locales, mismo arbol de
  inlines; solo cambian dos NOMBRES de variable, `ib_fragment`/`ib_owner`).
- `lmap.py` da las lineas del `UVectorMathGC.hpp` original: 459 `xx`, 460 `yy`,
  461 `zz`, 463 `xy`, 464 `xz`, 465 `xw`, 467 `yz`, 468 `yw`, 470 `zw`,
  472 `[0][0]`, 475 `[0][3]`, 477 `[1][0]`, 478 `[1][1]`, 480 `[1][3]`,
  484 `[2][2]`, 485 `[2][3]`, 487/488/489 `[3][0..2]`, 490 `[3][3]`. **Es
  sentencia a sentencia el orden que ya tenemos** (la unica diferencia es una
  linea en blanco entre `yw` y `zw`). La forma de la fuente esta confirmada.
- Ensayos:

      c1    `result[3][3] = 1.0f;` delante de los tres ceros    IDENTICO
      diag  `result[3][3]` con otra constante (parte el pseudo) 95,87 %, 2.180 B
            — pero CONFIRMA la direccion: bajar los refs de `1.0` lo mueve de f3 a f2

**Veda**: barrido el orden de las sentencias del ultimo bloque de `quattom4`; no
mueve nada. **Aritmetica para el que siga**: hace falta que la prioridad de
`1.0` baje de la de `1/32767`, o que la de `1/32767` suba de la de `1.0`; con
`n_refs` 5 y 5 eso es alargar la vida de `1.0` (o darle a `1/32767` un sexto
uso). Ninguna de las dos sale de reordenar la fuente de `quattom4`.

---

## 3. `__8GTriggerRCUi` (zGameplay, 2.544 B, 99,583 %) — cuatro hallazgos DWARF, la instruccion sigue

Sigue siendo **una sola instruccion**: el objetivo pone `li r22,1`
(`mWorldTrigger.fType = 1`, linea 116) DETRAS del `bl MATRIX4_multyrot` y
nosotros delante.

**Lectura nueva del entrelazado** (el orden completo, no solo la fila marcada):

    objetivo  lfs f1 . fneg . mr r3 . fmuls . mr r4 . bl . li r22
    nuestro   lfs f1 . mr r3 . fneg . mr r4 . fmuls . li r22 . bl

Nosotros metemos `mr r3` en el hueco de latencia del `lfs f1` y el objetivo no.
O sea: en el objetivo `fneg` gana el desempate contra `mr r3` en esa ranura, y
en el nuestro no. **Es `sched2`, y el `li r22,1` es el que cae al hueco que
sobra al final.** No es el orden de las asignaciones del fuente.

### Cuatro diferencias estructurales que el DWARF SI destapa

1. **El original llama a `UMath::MultYRot`**, no a `MATRIX4_multyrot` (el DWARF
   trae el inline `MultYRot` en los dos sitios, y `lmap.py` atribuye los dos
   `mr r3,r27` / `mr r4,r27` a `UMath.h:455`). **Nuestro `UMath::MultYRot` de
   `UMath.h` lleva un `r = m;` que MultXRot y MultZRot no llevan**; con el, la
   llamada cuesta 224 B de mas (84,07 %). Sin el (`MATRIX4_multyrot(&m, a, &r)`,
   igual que sus dos hermanas) el objeto sale **byte a byte identico** al actual.
   **NO lo he tocado: `UMath.h` esta fuera de mis tres unidades** y su unico otro
   llamante es `World.cpp:237` (zWorld). Queda como hallazgo: la cabecera miente.
2. `size.x = size.y = size.z = radius + radius;` — el original no tiene `diam`
   (los tres `stfs` cuelgan de una sola linea, la 88). **APLICADO**, identico.
3. `UMath::Vector4Make(center, radius)` — el original usa la sobrecarga
   `(const Vector3&, float)`, no la de cuatro floats. **APLICADO**, identico.
4. `bool showIconBasedOnBin` (el original es `bool`, no `int`). **APLICADO**,
   identico.

### Lo que el DWARF dice pero MIDE PEOR (no aplicado, anotado en el fuente)

- El original **no tiene** `halfWidth` ni `halfLength`: sin ellos,
  `size.x * (size.x * 0.25f)` es una multiplicacion conmutativa cuyo segundo
  operando es el mas complejo, GCC canonicaliza al reves y el `fmadds` sale con
  los multiplicandos cambiados. Medido: 99,49686 % (9 filas) y 99,56761 % (5).
- El original **no tiene** `triggerWord` ni `triggerFlags` (tiene un
  `StaticData * pTriggerData` que nosotros no tenemos). Pero el bitfield directo
  `mWorldTrigger.fFlags = 0x4810d;` da `rlwimi` donde el objetivo hace
  `clrrwi`+`or`: **99,3239 %, 2.540 B**. La forma actual (acumulador + store) es
  la que reproduce el objetivo.

### Ensayos numerados

    gc1  UMath::MultYRot con el `r = m` actual        84,06604 %  2.768 B
    gc2  UMath::MultYRot sin el `r = m`               99,583336   identico
    gc3  quitar diam + halfWidth + halfLength         99,48113    10 filas
    gc3a quitar solo `diam`                           99,583336   identico  APLICADO
    gc3b quitar halfWidth/halfLength (rama dims)      99,49686     9 filas
    gc3c quitar halfWidth (rama Width)                99,56761     5 filas
    gc4  `mWorldTrigger.fFlags = 0x4810d;`            99,3239     2.540 B
    gc5  `bool showIconBasedOnBin`                    99,583336   identico  APLICADO
    gc6  `Vector4Make(center, radius)`                99,583336   identico  APLICADO

---

## 4. `__12EPlayRaceNISP7GMarkerPCcT2iiT2T2` (zMain, 2.176 B, 99,485 %)

No he gastado ensayos de fuente (el encargo trae seis ya barridos y dice que la
forma de la expresion no decide). Lo que aporto es el **censo DWARF**, con tres
diferencias reales sin explotar:

- **Al original le sobra un nivel de bloque**: `parms` y
  `bTriggerMomentCopsAffected` viven un `{ }` MAS ADENTRO que en el nuestro.
- **Nos sobra una expansion de `ListableSet::GetList`** dentro del `First(...)`
  inlineado (bloque de `iter`/`list`): el original resuelve el bucket sin pasar
  por `GetList`. Eso vive en `UListable.h`, cabecera compartida.
- `float dot;` no tiene registro en el original y en el nuestro es `f1`.

Y la permutacion de GPR, ya emparejada con las locales del DWARF:

    original: numRacers r25, racerVehicle r29, i r30
    nuestro : numRacers r26, racerVehicle r30, i r28

## 5. `GenerateIndex__15GRaceParameters` (zGameplay, 1.680 B, 99,036 %)

Un ensayo nuevo, contra la unica pista que quedaba (el DWARF del original **no
tiene** ni `pflags` ni `flags`):

    c1  sin locales, `index->mFlags |= X;` en cada if   92,93571 %  1.732 B, 123 filas

Con `|=` directo GCC recarga `index->mFlags` en cada vuelta (13 instrucciones de
mas), porque las `GetXxx()` intermedias pueden aliasarlo. **El objetivo mantiene
el acumulado en r30, luego SI hay un temporal**; que el DWARF no lo nombre es una
de esas diferencias no accionables. La forma actual sigue siendo la mejor de las
seis medidas. Confirmada como muro.

---

## Que NO he probado

- `_bOutput`: nada sobre los racimos 619-678 y 913/915; nada sobre las **cuatro
  etiquetas de mas** (quitar `OUTPUT`/`positioned`/`Z_INT`/`VECT_OUTPUT` es
  reestructurar una funcion de 5 kB); nada sobre `int * __ptr` ni sobre el
  `int shift` que el original tiene en r27.
- `ESpawnFragment`: ninguna variante que alargue la vida del pseudo de `1.0f`
  (no se me ocurre ninguna que no contradiga el mapa de lineas); ninguna de
  `_Q4c::Decompress` (la division escrita como reciproco, etc.).
- `GTrigger`: `StaticData * pTriggerData` (el original tiene ~12 lineas mas entre
  `fDataOffset` y el `GetCollection()`); nada mas sobre el orden de 113-117.
- `EPlayRaceNIS`: **ningun ensayo de fuente**. El nivel de bloque de mas y el
  `GetList` que sobra estan sin tocar.
- `UMath.h` (`MultYRot`) y `UListable.h` (`First`): cabeceras compartidas fuera
  de mis unidades, no las he tocado; hacen falta A/B globales.

## Aviso de entorno

Al empezar la ronda **el disco C: tenia 6 MB libres** (100 % lleno) y una
compilacion me fallo con `cc1plus: I/O error` — que es exactamente la trampa
«disco lleno se lee como *la variante no cambia nada*» de `HERRAMIENTAS.md`.
He borrado del scratchpad los ficheros anteriores al 4 de septiembre (rondas ya
cerradas): **7,3 GB libres**. Conviene barrerlo entre rondas.
