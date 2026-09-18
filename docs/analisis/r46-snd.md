# r46 — sonido: `GinsuSynthData::BindToData` cerrada, y el volcado RTL como herramienta

Fecha: 2026-09-08. Territorio: `zEAXSound`, `zEAXSound2`, `zSpeech`.
Base: `773fb3d4`.

## Resultado

| unidad | antes | despues |
|---|---:|---:|
| `zEAXSound` | 2 funciones / 1.392 B | **igual** |
| `zEAXSound2` | 4 funciones / 2.644 B | **3 funciones / 2.304 B** |
| `zSpeech` | 2 funciones / 912 B | **igual** |

**Ganancia: +340 B y +1 funcion.** `audit.py` de `zEAXSound2` pasa de 926 a
**927** funciones exactas; `zEAXSound` (771) y `zSpeech` (701) no se mueven.
`lcfix.py --check` limpio. Ninguna funcion empeora en ninguna de las tres
unidades.

Se tocan **dos** ficheros, los dos de mi territorio:

- `src/Speed/Indep/Src/EAXSound/Ginsu/ginsudata.cpp` (cierre)
- `src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp` (avance estructural)

---

## 1. CERRADA — `BindToData__14GinsuSynthDataPv` (340 B)

```
# BindToData__14GinsuSynthDataPv  target=100.0%  ours=100.0%  size=340/340
   filas distintas: 0
audit.py:  BindToData__14GinsuSynthDataPv   ok   340 B, 7 ramas, 12 relocs, 4 literales
```

### El diagnostico, en dos herramientas

`dwbody.py zEAXSound2 "GinsuSynthData::BindToData"` da una diferencia
**estructural**, no de registros: el original declara `i` dentro de un bloque
(el `for`) y `period` en un bloque **anidado** dentro de aquel; nosotros
teniamos `i` a nivel de funcion. Eso solo ya justifica `for (int i = 0; ...)`,
pero por si solo es neutro (344 B, 12 filas, igual que la base).

Lo que cierra la funcion lo da `lmap.py`. El mapa de lineas del ORIGINAL dice:

```
800EAF38  mr r7, r0        ginsudata.cpp:174     <- minperiod = mSampleCount
800EAF3C  li r0, -0x1      ginsudata.cpp:172     <- mCurrentBlock = -1
800EAF4C  stw r0, 0xac(r31) ginsudata.cpp:172
```

La linea **172 se emite DESPUES de la 174**: el planificador del original
**retrasa** la materializacion del `-1` hasta pegarla a su store. Rango de vida
4 instrucciones -> se colorea en **r0**.

Nosotros hacemos lo contrario: el `li` se adelanta ~20 ranuras (a la insn 32-33
del bloque), se colorea en **r8**, y r8 deja de estar libre para el puntero
`mCyclePos`. El asignador reparte `mCyclePos` a r5 y la precabecera del bucle
necesita un `mr r8, r5` que el objetivo no tiene: **344 B contra 340**.

### Los escalones medidos

| # | forma | B | filas | % |
|---|---|---:|---:|---:|
| base | — | 344 | 12 | 95,88235 |
| B1 | `for (int i = 0; ...)` (lo que dice el DWARF) | 344 | 12 | 95,87059 |
| B2 | B1 + `mCurrentBlock=-1` tras `minperiod` | 344 | 12 | 95,87059 |
| B3 | B1 + el store **detras del bucle** (diagnostico) | **340** | 7 | 95,08235 |
| B10 | B1 + `int cb=-1; asm("":"+r"(cb):"r"(minperiod));` | **340** | 6 | 97,40000 |
| B11 | B10 + `register ... asm("r0")` | **340** | 6 | 97,40000 |
| B14 | `asm("li %0,-1" : "=r"(cb) : "r"(minperiod))` | **340** | 4 | 99,76471 |
| **B15** | **B14 + `register int cb asm("r0")`** | **340** | **0** | **100** |

B3 es el que prueba la causa: moviendo el store detras del bucle **todo el
reparto del bucle se vuelve el del objetivo** (i en r5, mCyclePos en r8, sin el
`mr` de mas) y las unicas 7 filas son el par `li -1` + `stw` desplazado. A
partir de ahi solo faltaba poner ese par en su sitio sin moverlo de sentencia.

B10 (barrera vacia anclada a `minperiod`) arregla ya **todo el reparto** pero el
`li` sigue adelantandose: una barrera restringe la posicion del `asm`, no la de
la instruccion que define el pseudo. Lo unico que ata **las dos cosas a la vez**
—la ranura minima y el registro— es que el propio `asm` sea el productor.

### El cambio retenido

```cpp
    int minperiod = this->mSampleCount;
    register int currentBlock __asm__("r0");
    __asm__("li %0,-1" : "=r"(currentBlock) : "r"(minperiod));
    this->mCurrentBlock = currentBlock;

    for (int i = 0; i < this->mCycleCount; i++) {
```

Emite **exactamente** la instruccion que el objetivo tiene (`li r0,-1`), en su
registro y en su ranura. No es cero bytes con `asm` puesto: es la palanca 5 del
brief («para lo que en C no sale, sin pasar la unidad a `.s`») y deja la funcion
byte a byte. Tambien se retira el `// UNSOLVED`.

Si la coordinacion prefiere C puro, **B10 es la alternativa sin cuerpo `asm`**:
340 B exactos y 6 filas (el `li` en r11 y adelantado). Pero son cero bytes.

---

## 2. AVANCE ESTRUCTURAL RETENIDO — `Play__16CARSFX_RoadNoise…` (392 B)

```
antes:   388/392 B   94,938774 %   29 filas
ahora:   392/392 B   99,642860 %    7 filas
```

Es la receta que r42 aisló y **no retuvo por encargo** (`Wet_FX = -1`, el ultimo
argumento del `new Csis::FX_ROADNOISE` de `Play`). Los setters saturan los
negativos a cero, asi que el estado final es identico; lo que cambia es que el
front-end deja de colapsar todos los ceros en un unico pseudo. La retengo
—con comentario en la fuente— porque **el tamaño pasa a ser exacto** y el
encargo lo autoriza explicitamente para avances estructurales sin `asm`.

Vecinas comprobadas y **sin regresion**: `InitSFX` 444 B/100 %,
`MsgPlayMiscSound` 780 B/100 %, `MsgBarrier` 140 B/100 %.

### Lo que queda, medido con el volcado de `global_alloc`

Las 7 filas son **un intercambio de color entre dos allocnos**: el objetivo pone
el cero persistente en r29 y `&this->mData` en r27; nosotros al reves.

El `.greg` de la unidad (ver §5) lo cuantifica:

```
;; 24 regs to allocate: 137 90 98 107 113 152 153 210 228 97 106 227 89 151
                        82 83 84 104 207 259 154 156 134 258
154 in 29   156 in 27
;; 154 conflicts: 134 137 151 154 156 161 187 197 207 210 213 239 258 259 0 1 3 4 5 9 10 11
;; 156 conflicts: 134 137 151 154 156 161 187 197 206 207 210 213 239 258 259 0 1 3 4 5 9 10 11
```

`154` es `&this->mData` y `156` el cero. Salen en ese orden de
`allocno_compare`, y `find_reg` recorre 31,30,29,28,27…: al llegar 154 estan
tomados r31 (137), r8 (151), r30 (207) y r28 (259) -> r29; a 156 le queda r27.
**Para cerrarla hay que invertir la prioridad de esos dos allocnos**, es decir
subir `n_refs` del cero por encima de 4 (para que `floor_log2` pase de 1 a 2) o
alargar el rango de vida de `&mData`.

**Y eso esta fuera de mi territorio**: los dos pseudos nacen dentro del
constructor inline `Csis::FX_ROADNOISE`, en
`src/Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h`, que entra tambien en
`zEAXSound`. Lo dejo como propuesta con la medida (§6).

Negativo medido aqui: **P2**, pasar los ceros por una local
(`int z = 0;` y `FX_ROADNOISE(ID, z, …)`) para poder nombrarlos en un `asm`,
**dispara a 504 B / 58 filas** — con un argumento no literal cada setter emite
su saturacion completa. Via cerrada.

---

## 3. Medido y NEGATIVO (todo revertido)

### `GenerateRoadNoise__16CARSFX_RoadNoise` — base 1.240 B, 23 filas, 97,1129 %

`dwbody.py` dice que **el original no tiene la local `float slipBoost`** que
nosotros declaramos (nosotros: `slipBoost // f7`). Parecia el caso de libro de
«la local que roba el registro». **No lo es:**

| # | ensayo | B | filas | % |
|---|---|---:|---:|---:|
| base | — | 1240 | 23 | 97,1129 |
| G1 | quitar `slipBoost`, usar `0.1f` en la expresion | **1232** | 29 | 96,6097 |
| G2 | `slipBoost` declarada DESPUES del `bLength` | **1232** | 29 | 96,6097 |
| G3 | los dos `fmadds` de `fRightVol` juntos, antes del `bLength` | 1240 | **51** | 96,1097 |
| R1 | `SFXCTL_Wheel *wc` + `asm("":"+r"(wc):"f"(fRightVol))` | 1240 | 28 | 95,8710 |

La local vale 8 B reales: sin ella el tamaño deja de ser exacto. **El DWARF que
no lista una local no siempre significa que sobre.** Anotado como veda.

Las 23 filas son un solo sintoma: el objetivo emite
`fmadds f29,f31,f11,f31` en la insn **102** y `fmadds f29,f29,f10,f29` en la
**114**; nosotros en la **114** y la **134**, la segunda ya en el bloque de
despues del `bso`. El resto son renombres arrastrados (f10/f11, f7/f9/f12 y las
bases `@ha`). R1 era el intento de subir la prioridad del primer `fmadds`
haciendolo predecesor de la carga de `m_pWheelCtl`; la local `wc` cambia el CSE
y empeora.

### `ProcessUpdate__12CARSFX_Turbo` — base 672 B, 8 filas, 97,5 %

`dwbody.py` da **una sola** diferencia de cuerpo: `TmpBlowoffVol` en r3 en el
original y en r9 en el nuestro. No falta ni sobra ninguna local. Todo el diff
cuelga de que el objetivo emite `mr. r7, r3` en la ranura **30** (pegado al
`lwz r11,0x60(r31)`) y nosotros en la **39**.

| # | ensayo | B | filas | % |
|---|---|---:|---:|---:|
| base | — | 672 | 8 | 97,5000 |
| T1 | `asm("":"+r"(nDMixOut):"r"(Az))` **tras** el desplazamiento | 672 | 45 | 96,1905 |
| T2 | el mismo `asm` **antes** del desplazamiento | 672 | 35 | 97,8274 |
| **T3** | `int blowoffVol=this->BlowoffVol; asm("":"+r"(blowoffVol):"r"(Az));` | 672 | **6** | **98,6905** |
| T12 | T3 + `register int TmpBlowoffVol asm("r3")` | **652** | 23 | 94,1369 |
| T14 | control: la local **sin** el `asm` | 672 | 8 | 97,5000 |
| T16 | T3 + T1 (cadena de dos) | 672 | 43 | 97,3810 |
| T17 | anclar el producto: `int prod=…; asm("":"+r"(prod):"r"(Az));` | 672 | 20 | 94,7917 |

T3 es la mejor forma conocida y **sube el `mr.` de la ranura 39 a la 35**,
ademas de colocar bien el `lis …@ha`. **No cierra** y son cero bytes con `asm`
puesto, asi que se revierte (fichero restaurado byte a byte,
SHA-256 `aeb16a29eefd55b1…`).

**Veda razonada, no solo empirica**: para llegar a la ranura 30 el `mr.` tiene
que ganarle en `rank_for_schedule` a `lis r8,0x4330`, que encabeza la cadena
larga del `float`. La unica manera de subir `INSN_PRIORITY` del `mr.` por encima
de esa es hacerlo **predecesor** de esa cadena, y el unico predecesor posible es
el valor post-desplazamiento (T1/T16/T17) — que destroza el bloque. `lis
r8,0x4330` lo genera la conversion `int -> float`, no hay expresion C delante de
el. **Sin una palanca nueva, `ProcessUpdate` esta cerrada por construccion.**

### `Play__13cStichWrapperPC10SND_Params` (zEAXSound) — base 384 B, 4 filas

`dwbody.py` senala que el original **no tiene la local `int scale`**. Medido:

| # | ensayo | B | filas | % |
|---|---|---:|---:|---:|
| base | — | 384 | 4 | 99,6875 |
| S1 | quitar `int scale = 0x7FFF` y usar el literal | **388** | 15 | 97,4479 |

Segundo caso de «local que el DWARF no lista pero hace falta». Revertido
(SHA-256 `bf74a541f1b4a320…`).

Diagnostico nuevo con el `.greg` de `zEAXSound`, por si sirve a quien la retome:
los dos pseudos son **115** (el desplazamiento `i*4`) y **116** (la base
`this+0x24`); tienen **la MISMA lista de conflictos** y `allocno_compare` ya
coloca 116 **antes** que 115 — sin embargo 116 recibe r30 y 115 r31. O sea que
**el orden de allocno no es lo que decide aqui**, al contrario de lo que suponia
r36f («el orden de asignacion decide las dos cosas a la vez»); `find_reg`
descarta r31 para 116 por algo que no es un conflicto declarado. Quien la
retome deberia atacar `find_reg`, no el orden de las sentencias.

### `LoadSpeechBank__Q26Speech7Manager…` (zSpeech) — base 312/316 B

Nos falta **un `mr`**: el objetivo parte la base del array `index->item` en dos
pseudos, con `mr r11,r4` en la precabecera del `while` y `mr r4,r11` **dentro**
del bucle (linea 981 del original segun `lmap`); nosotros llevamos una sola
copia. `lmap` confirma ademas que el orden de sentencias del original es
exactamente el nuestro: 964 (`key`), 967 (`lower`), 968 (`upper`), 969 (`i`),
970 (el `if` exterior), 971 (el `while`), 972/974/978 (el `<` y las dos ramas),
980 (`i = (lower+upper)>>1`), 981 (el `==` con `goto found`). El `mr` que nos
falta cuelga de la 981.

r31 dejo escrito que la firma es de `loop.c`/`move_movables`. **Lo he
descartado con medida**, y de paso he barrido los flags de la unidad (sobre
`zSpeech` entero, con `tamfn.py`):

| flag | `LoadSpeechBank` | resto de la unidad |
|---|---:|---|
| (base) | 312 | delta 4 B, 702/703 exactas |
| `-fno-gcse` | **324** | delta 1.432 B, 598/703 |
| **`-fno-cse-follow-jumps`** | **316 (exacto)** | delta 444 B, 650/703 |
| `-fno-cse-skip-blocks` | 312 | delta 1.188 B, 628/703 |
| `-fno-rerun-cse-after-loop` | 312 | delta 3.592 B, 587/703 |
| `-fno-thread-jumps` | 312 | delta 80 B, 693/703 |
| `-fno-expensive-optimizations` | 312 | delta 188 B, 678/703 |
| `-fno-force-addr` | 312 | delta 1.628 B, 608/703 |
| `-fno-force-mem` | 312 | delta 4 B, 702/703 |
| `-fno-rerun-loop-opt` | 312 | delta 60 B, 694/703 |

**El pase responsable es `cse-follow-jumps`**, no `loop.c` ni `gcse`: es cse1
siguiendo el salto lo que unifica los dos webs de `index->item`. Compilado
`zSpeech` entero con `-fno-cse-follow-jumps` (objeto en
`scratchpad/r46_snd/zSpeech_nocfj.o`), `LoadSpeechBank` mide **316 B exactos**
pero baja a **91,31645 %** (el `mr` que aparece no es el del objetivo) y la
unidad pierde 53 funciones. **No es una propuesta de flags**: es el diagnostico
de que la palanca de fuente tiene que romper esa unificacion **en un solo
punto**, dentro del bucle, sin romperla en el `if` exterior.

Vedas de r28/r31 (la etiqueta `found`, los cuatro `goto`, la local de puntero
base `item`, las 6+9 formas) siguen en pie; no repeti ninguna.

### `SetupNextLoad` y `RoadblockFlow::Setup`

No tocadas, conforme al encargo. Sus vedas (r36c/r36f/r45 y r45-speech) siguen
enteras.

---

## 4. Vedas nuevas que dejo escritas

1. **Una local que el DWARF del original no lista NO significa que sobre.** Dos
   casos medidos en una sola ronda: `slipBoost` en `GenerateRoadNoise`
   (quitarla: 1.240 -> 1.232 B) y `scale` en `cStichWrapper::Play`
   (quitarla: 384 -> 388 B). El DWARF de `-gdwarf+` se come locales sin
   ubicacion; usalo para **anadir** estructura (bloques, ambitos), no para
   quitar declaraciones.
2. **`ProcessUpdate__12CARSFX_Turbo`**: cerrada por construccion mientras no
   exista una palanca que baje `INSN_PRIORITY`. Siete formas medidas arriba.
3. **`LoadSpeechBank`**: no es `loop.c` ni `gcse`; es `cse-follow-jumps`.
   `-fno-gcse` la empeora a 324 B.
4. **`Play__16CARSFX_RoadNoise`**: pasar los ceros del constructor por una local
   para poder nombrarlos (504 B). Via cerrada desde el `.cpp`.

## 5. Herramienta nueva: el volcado RTL del compilador real

El driver `ngccc.exe` **se come `-da`/`-dg`**, por eso nadie los habia usado.
Invocando las dos etapas a mano si salen:

```sh
export SN_NGC_PATH='…/build/compilers/ProDG/3.9.3'
ngccc.exe -E <los cflags reales de la unidad> -o u.i src/.../zEAXSound2.cpp
cc1plus.exe <los mismos cflags, sin los -I ni los -D> -quiet -dg -dl u.i -o u.s
```

Salen `u.i.greg` (global_alloc) y `u.i.lreg` (local_alloc). ~22 s y ~17 MB para
una SourceList entera. Por funcion trae:

- `;; N regs to allocate: a b c …` — **es el orden de asignacion ya ordenado por
  `allocno_compare`**, no la lista cruda;
- la matriz de conflictos completa (pseudos y **registros duros**);
- `;; Register dispositions:` con el `reg_renumber` final;
- las preferencias (`;; 83 preferences: 4`).

Con eso un near-miss de reparto deja de ser adivinanza: se ve **quien se reparte
antes y por que el que va primero no cogio el registro que esperabas**. Lo he
validado contra una reproduccion minima y contra las dos funciones de §2 y §3.
Verificado el orden de `find_reg`: **31, 30, 29, 28, 27, 26, 25, 24, 23…** (y
salta el que tenga el conflicto duro anotado).

## 6. Propuestas fuera de territorio

1. **`ENVIRO_AEMS.h` — cerrar `Play__16CARSFX_RoadNoise` (392 B).** Falta
   invertir la prioridad de dos allocnos que nacen en el constructor inline
   `Csis::FX_ROADNOISE`: subir `n_refs` del cero persistente a >= 4 (con un
   `asm` de entrada, eje «a quien») o alargar el rango de `&this->mData`. El
   `.greg` de §2 da los numeros exactos (allocnos 154 y 156, conflictos
   identicos salvo el 206). **Gate: las dos SourceLists**, `zEAXSound` y
   `zEAXSound2`, porque la cabecera entra en las dos. Con la receta `Wet=-1` ya
   integrada, el ensayo parte de 7 filas, no de 29.
2. **`zSpeech` / `LoadSpeechBank`**: no cambiar flags. El dato util es que
   `-fno-cse-follow-jumps` da el tamaño exacto: la forma de fuente correcta es
   la que impida a cse1 unificar `index->item` **solo** entre el `lwzx` de
   cabecera del bucle y el de su cola. Vale la pena mirarlo con el `.greg`
   /`.lreg` de §5 antes de gastar mas formas ciegas.
3. Anotar en la memoria del proyecto la herramienta de §5 y la veda 1 de §4.

## 7. Verificacion

```
build_direct.py zEAXSound zEAXSound2 zSpeech      3 ok, 0 fallidas
fncmp zEAXSound     2 de 773 -- 1392 B   (SetupNextLoad, cStichWrapper::Play)
fncmp zEAXSound2    3 de 930 -- 2304 B   (GenerateRoadNoise, ProcessUpdate, Play)
fncmp zSpeech       2 de 703 --  912 B   (RoadblockFlow::Setup, LoadSpeechBank)
audit.py zEAXSound  exit 0   771 ok, 0 FALLA
audit.py zEAXSound2 exit 0   927 ok, 0 FALLA      (926 antes)
audit.py zSpeech    exit 0   701 ok, 0 FALLA
lcfix.py --check    todas las entradas @lc estan al dia
git diff --check    limpio en los dos ficheros tocados
```

SHA-256 de los ficheros de mi territorio al terminar (16 primeros):

```
CARSFX_Turbo.cpp    aeb16a29eefd55b1   (= base, restaurado)
STICH_PlayBack.cpp  bf74a541f1b4a320   (= base, restaurado)
SpeechManager.cpp   38695b6c9d07fa50   (= base, sin tocar)
CARSFX_Roadnoise.cpp 22cdde7ebfbb0052  (Wet=-1 + comentario)
ginsudata.cpp        c957b8df6ea3bcc7  (cierre)
```

Los cinco ficheros conservan CRLF puro (`CRLF == LF` en los cuatro CRLF;
`SpeechManager.cpp` es LF puro y no se toco).

## 8. Artefactos

`scratchpad/r46_snd/`:

- `base/` — copias de entrada de los cinco fuentes;
- `ginsudata_B15_100.cpp` — la version que cierra, aislada;
- `zEAXSound.i`, `zEAXSound2.i` y sus `.greg` (volcados de `global_alloc`);
- `play_greg.txt` — la seccion de `cStichWrapper::Play` ya extraida;
- `zSpeech_nocfj.o` y `nocfj.json` — la unidad con `-fno-cse-follow-jumps`;
- `grn_base.txt` — el `fndiff` completo de `GenerateRoadNoise`;
- `audit_*.txt` — las tres auditorias finales;
- `m.py` — el arnes de medida (compila la unidad y saca filas por funcion);
- `mini/` — la reproduccion minima con la que se valido el volcado RTL.
