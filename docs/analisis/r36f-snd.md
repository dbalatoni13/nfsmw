# Ronda 36f — `c36f-snd`: zEAXSound, zEAXSound2 y zSpeech

**Cero funciones cerradas. Más de 950 compilaciones medidas, y un hallazgo que
une DOS funciones de la cola (1.172 B) bajo UNA sola causa identificada y
caracterizada al detalle.** El árbol queda **sin tocar**: `git status` de mi
territorio está vacío y las tres unidades dan exactamente las mismas cifras que
al empezar (§8).

Lo que sí trae la ronda:

1. **El modelo de los dos grupos del cero constante** (§1): explica a la vez
   `MsgPlayMiscSound` (780 B) y `CARSFX_RoadNoise::Play` (392 B), y reduce las
   dos a **una única palanca que falta**. Verificado en los dos compiladores.
2. **El barrido EXHAUSTIVO de las 720 permutaciones de `Csis::FX_UVES`** (§2):
   ninguna cierra; el mejor es `ivspnw` con **6 filas** (la r36e se quedó en 12),
   y el orden del DWARF `ivwpns` deja el **sitio 1 EXACTO** — nadie lo había
   conseguido.
3. **El orden del constructor de `FX_ROADNOISE` es CORRECTO, y hay testigo**
   (§3): `InitSFX` casa hoy al 100 % y las **110** reordenaciones probadas la
   rompen **todas**. Cierra una vía que parecía la buena.
4. **Técnica del testigo** (§7): antes de tocar una cabecera, busca una función
   que HOY case y que use lo mismo. Cuesta una compilación y ahorra una tanda.
5. Cuatro vedas ampliadas con cifras: `MsgBarrier` (+20), `RoadblockFlow::Setup`
   (+8), `cStichWrapper::Play` (+40, pero **de 4 filas a 2**) y `LoadSpeechBank`
   (+5, con el diagnóstico **corregido**).
6. **Los cflags de zEAXSound2 quedan confirmados**: 15 flags barridos, ninguno
   mejora y todos rompen vecinas (§6).

---

## 1. El hallazgo: los DOS grupos del cero constante

Dos funciones de la cola —`SFX_Common::MsgPlayMiscSound` (780 B) y
`CARSFX_RoadNoise::Play` (392 B)— fallan por lo mismo, y no es lo que decían las
rondas anteriores.

### 1.1 La regla, verificada en los dos compiladores

Cuando un constructor `Csis::*` se expande en línea, sus `SetX(0)` se convierten
en una tanda de `stw` de cero. **El cero NO vive siempre en un solo registro:
hay un grupo «largo» y un grupo «corto»**, y la regla es la misma en el objetivo
y en nosotros:

> Los ceros que se **emiten antes** del primer `stw` de un valor no-cero comparten
> el registro de vida larga (uno preservado, el mismo que luego usa el
> `int refCount = 0;` de `GetRefCount()`). Los que se emiten **después** usan un
> registro corto, casi siempre **r0**.

Medido en `MsgPlayMiscSound` sitio 2 (`FX_UVES(2,0,0,0,0,0)`):

    objetivo:  li r27,0 / li r0,0
               stw r27,0x8 (volume) | stw r9,0x4 (iD) | stw r0,0x18 / 0xc / 0x10 / 0x14
               ...  stw r27,0x8(r1)   <- el refCount de GetRefCount, mismo r27
    nuestro:   li r28,0 / li r0,0(iD=r11)
               stw r28,0x8 / 0xc / 0x10 / 0x14 | stw r11,0x4 (iD) | stw r9,0x18

y en `CARSFX_RoadNoise::Play`:

    objetivo:  li r29,0 (grupo largo) + li r0,0 (grupo corto)
               [v,a] [i,p,l,d] [w,t,s,S,h]      <- 5 ceros en el grupo corto
    nuestro:   li r30,0 y NADA MÁS
               [v,a,t,s,S,h,w] [i,p,l,d]        <- los 7 ceros en el grupo largo

**Los 4 B que le faltan a `Play` son ese segundo `li 0`.** No es un problema de
reparto de registros: es que el grupo corto nos sale vacío.

### 1.2 Dónde parte cada compilador

La regla de emisión dentro de cada grupo es **idéntica** en los dos (el orden de
la fuente con el ÚLTIMO `Set` rotado al frente, la que enunció `r36e` §3.1). Lo
único que cambia es **dónde se parte**:

| fuente de `FX_UVES` | nuestro grupo largo | grupo corto |
|---|---|---|
| `viwpns` (el actual) | v, w, p, n | s |
| `ivwpns` (el del DWARF) | v, w, p, n | s |
| `ivspnw` | **v** | s, p, n, w |

> **Nosotros partimos SIEMPRE en `SetStop`** — el `Set` del **último campo de la
> estructura** (`stop`, offset 0x14 de `FX_UVESStruct`). Lo mismo en
> `FX_ROADNOISE`: partiríamos en `SetFilter_Effects_Wet_FX`, el último campo, y
> como ahí ya no queda nada detrás el grupo corto sale **vacío** y perdemos los 4 B.

**El objetivo parte justo detrás de `SetVolume`**, o sea detrás del **primer**
cero, y da igual dónde esté `SetStop`.

### 1.3 Por qué eso encierra las dos funciones

Con el modelo se predice el objetivo exactamente, y se demuestra que **el orden
del constructor no puede arreglarlo**:

- Para que el grupo largo sea `{v}` hace falta que el grupo corto empiece justo
  detrás de `SetVolume`; con nuestra regla eso obliga a poner `SetStop` en la
  posición 3.
- Para que el grupo corto se emita `s,w,p,n` (lo que quiere el objetivo) su orden
  en la fuente tiene que ser `w,p,n,s`, o sea **`SetStop` el último**.

Las dos condiciones se contradicen. **Por eso ninguna de las 720 permutaciones
cierra** (§2), y por eso ninguna reordenación de `FX_ROADNOISE` cierra `Play`
(§3). Lo que hace falta es una palanca que **mueva el punto de partición del
cero**, y no la tenemos.

### 1.4 Lo que NO mueve la partición (todo medido)

| # | ensayo | resultado |
|---|---|---|
| B1 | `ivwpns` + `+m(this->mData.volume)` tras `SetVolume` | 45 filas (base 30) |
| B2 | `ivwpns` + `+m(this->mData)` tras `SetVolume` | 64 |
| B3 | `ivwpns` + `asm("")` tras `SetVolume` | 62 |
| B4 | `ivwpns` + `+m(volume)` ANTES de `SetVolume` | 30 (binario idéntico) |
| B5 | `ivwpns` + `+m(width)` tras `SetWidth` | 45 |
| B6 | `ivwpns` + `asm("":::"memory")` tras `SetVolume` | 62 |
| B7 | B1 + `+m(mData)` al final | 50 |
| A1 | `ivspnw` + `+m(stop)` tras `SetStop` | 39 (base 6) |
| A2 | `ivspnw` + `+m(width)` tras `SetWidth` | 39 |
| A3 | A1 + A2 | 41 |
| A4 | `ivspnw` + `+m(stop)` ANTES de `SetStop` | 6 (binario idéntico) |
| A5 | `ivspnw` + `+m(width)` antes de `SetWidth` | 6 (idéntico) |
| A6 | `ivspnw` + `+m(mData)` al final | 41 |
| A7 | `ivspnw` + `+m(mData)` en medio | 54 |

**Un `+m` sobre un campo colocado ANTES de su propio store es inerte** (A4, A5,
B4: binario idéntico). Detrás, ordena pero no parte. Esto acota el «uso 6» que
`r36e` añadió al catálogo: **el `+m` fija el ORDEN de emisión, no el REPARTO del
registro de la constante.**

Y en `CARSFX_RoadNoise::Play`, desde el sitio de llamada (§4.2) tampoco: seis
formas del bloque `{ new ...; int refcnt = ...; }` y cinco formas de forzar un
cero propio dan el mismo 388 B o revientan (V1 552 B, V2 496 B, V3 552 B,
V5 460 B).

---

## 2. `SFX_Common::MsgPlayMiscSound` (780 B): las 720 permutaciones, barridas

`r36e` §3.1 midió 28 órdenes de los seis `Set` de `Csis::FX_UVES` y dejó
`ivswpn` (12 filas) como mejor. **He barrido las 720**, a 4 s cada una.

    720 permutaciones
    100 cambian el TAMAÑO (772 B contra 780)
      0 cierran
      1 con  6 filas: ivspnw
      4 con  9 filas: ivpsnw, ivspwn, ivswnp, iwspnv
      3 con 10, 1 con 11, 16 con 12, 11 con 13, 13 con 14, 39 con 15, ...

**`ivspnw` (`SetID, SetVolume, SetStop, SetPitch_Offset, SetIntensity,
SetWidth`) deja 6 filas** —la mitad de la mitad que la r36e— y las seis son **la
misma**: los stores de `stop` (0x18) y `width` (0xc) intercambiados, en los tres
sitios:

    obj:   stw r31,0x18 ... stw r31,0xc      (sitio 1)
    ours:  stw r31,0xc  ... stw r31,0x18

Con `ivspnw` **el reparto de registros del cero coincide al byte con el
objetivo** en los tres sitios (r27 largo + r0 corto): lo único que queda es el
orden de dos stores dentro del grupo corto.

### 2.1 Dato nuevo importante: `ivwpns` deja el SITIO 1 EXACTO

`r36e` dio `ivwpns` (el orden que lista el volcado DWARF) por peor: 30 filas.
Es cierto en total, pero **el sitio 1 (`FX_UVES(0,0,0,0,0,0)`) sale idéntico al
objetivo, byte a byte** — las 30 filas están TODAS en los sitios 2 y 3, y todas
son del reparto del cero (§1). Es la primera vez que un orden clava un sitio
entero.

Con eso, la inversión de la regla de emisión y el volcado DWARF coinciden por
tercera vía: **el constructor original es `ivwpns`, y el que hay en el árbol
(`viwpns`, del commit `5d1ac0cf`) está mal.**

### 2.2 Corrección al aviso de `r36e`: `MAIN_AEMS.h` NO es un problema de alcance

`r36e` §6.1 dejó el orden sin tocar «porque `MAIN_AEMS.h` lo usan más unidades».
**El constructor de `Csis::FX_UVES` sólo se instancia en TRES sitios y los tres
están en `SFX_Common.cpp`** (líneas 42, 52 y 57, todos en `MsgPlayMiscSound`):

    src/Speed/Indep/Src/EAXSound/SFX_Common.cpp:42:  new Csis::FX_UVES(0, 0, 0, 0, 0, 0)
    src/Speed/Indep/Src/EAXSound/SFX_Common.cpp:52:  new Csis::FX_UVES(2, 0, 0, 0, 0, 0)
    src/Speed/Indep/Src/EAXSound/SFX_Common.cpp:57:  new Csis::FX_UVES(1, 0, 0, 0, 0, 0)

Es una función miembro **en clase** (o sea inline, no emite símbolo: ver
`nfsmw-en-clase-es-inline`), así que las demás unidades que incluyen la cabecera
no emiten nada. **Cambiar el orden sólo afecta a `MsgPlayMiscSound`.** El motivo
para no aplicarlo es el otro que daba `r36e` —que no cierra—, no el alcance.

**No lo aplico** (6 filas siguen siendo cero bytes y el brief manda revertir).
Queda anotado para quien tenga la palanca de §1.

---

## 3. `CARSFX_RoadNoise::Play` (392 B): el orden del constructor es CORRECTO, con testigo

`r36e` §3.2 dejó como receta «barrer los órdenes de `FX_ROADNOISE`». Lo he hecho
—**110 órdenes**, todos los movimientos de un `Set` a otra posición— y el
resultado es un **negativo con prueba**:

| orden | `Play` | `GenerateRoadNoise` | `InitSFX` |
|---|---|---|---|
| base `ivpatsSlhdw` | tamaño (388/392) | 33 filas | **casa al 100 %** |
| `ivpasSlhdwt` (SetType al final) | **392 B**, 21 filas | 33 | **ROTA** |
| `pivatsSlhdw` | 392 B, 32 filas | 33 | **ROTA** |
| `vpatsSlhidw`, `vpatsSlhdiw` | 392 B, 34 | 33 | **ROTA** |
| los otros 105 | 388/396/otros | 33 | **ROTA** |

> **`InitSFX__16CARSFX_RoadNoise` casa hoy al 100 % y construye el mismo
> `Csis::FX_ROADNOISE` (línea 117 de `CARSFX_Roadnoise.cpp`). Las 110
> reordenaciones la rompen SIN EXCEPCIÓN.** O sea: el orden que hay en el árbol
> es el del original y esta vía está cerrada. Sabemos además por §1 que ninguna
> reordenación PODÍA cerrarla.

Nueve órdenes sí dan el tamaño exacto (392) porque mueven `SetId` —el único
`Set` con argumento variable, que abre un diamante de bloques básicos— y eso
parte el cero en dos. Es la confirmación experimental de §1: **la partición se
mueve, pero no adonde hace falta, y a costa de la vecina.**

También negativas, con el mismo testigo roto: cuatro barreras dentro del
constructor entre `SetAzimuth` y `SetType` (`asm("")`, `asm(:::"memory")`,
`asm(:::"r0")`, `asm("# x")`) y tres en otras posiciones. Las siete dejan `Play`
en 388 B y rompen `InitSFX`.

---

## 4. Las demás de la cola, con la cifra

### 4.1 `CARSFX_PreColWoosh::MsgBarrier` (140 B, 2 filas) — **veda, +20 ensayos**

El encargo apuntaba a «combinar familias», que es lo que cerró `TrackCop`. **Lo
he probado y no vale aquí.** La diferencia sigue siendo una sola:

    obj:  addi r3,r10,0x3c / li r4,0x1 / stfs f13,0x34(r10) / fmr f2,f1 / li r5,0 / bl
    nue:  addi r3,r10,0x3c / stfs f13,0x34(r10) / li r4,0x1 / fmr f2,f1 / li r5,0 / bl

| # | ensayo | resultado |
|---|---|---|
| base | — | 2 filas |
| P1/P2 | `asm("" : : : "r0")` antes / después del store | **136 B** |
| P3/P4 | `asm("")` antes / después del store | **136 B** |
| P5 | local `len` + `+r(len)` ANTES del store | 3 filas |
| P6 | local `len` + `+r(len)` DESPUÉS (= M1 de la r36d) | 3 |
| P7/P8/P9 | local + ranura, tres combinaciones | 136 B |
| Q1/Q2 | objeto en local + `+r(fade)` antes / después | 3 |
| Q3..Q6 | **ranura sobre el objeto + argumento sobre `len`** (las dos familias, 4 órdenes) | **4** |
| Q7 | eje «a quién»: `asm("":"+r"(fade):"r"(len))` | 3 |
| Q9 | `+m(this->mDurationActive)` antes del store | 2 (igual que base) |
| R1 | `this` en local + barrera antes | **144 B** |
| R2 | `this` en local + barrera después | 3 filas, 3 reub |

**Dato de método nuevo y caro**: *cualquier* barrera TOTAL en esa función
—`asm("")` con o sin clobber— hace **desaparecer el `mr r10, r3`**: `this` se
queda en r3 toda la función y el objeto baja a **136 B**, 4 B por debajo del
objetivo. No es una mejora, es un cambio de forma completo (82 % de parecido).

**La combinación de familias (Q3..Q6) EMPEORA**: cada familia por separado deja
3 filas y juntas dejan 4. Es el contraejemplo a la regla de `TrackCop`; la regla
sigue mereciendo probarse, pero no es universal.

### 4.2 `cStichWrapper::Play` (384 B): de 4 filas a **2** — y por qué no baja

`r36e` §3.4 dio E1 por «4 filas». Con `fncmp` (que cuenta instrucciones, no
fuzzy) **E1 deja 2**, y son las dos únicas:

```c
cSampleWarpper **refs = this->ActiveSamplesRefs;   // <- ASIGNADA PRIMERO
int off = i * sizeof(cSampleWarpper *);
*reinterpret_cast<cSampleWarpper **>(reinterpret_cast<char *>(refs) + off) = new ...;
```

    obj:  li r3,0x20 / slwi r30,r28,2 / bl __nw__ / addi r31,r29,0x24 / ...
    E1:   li r3,0x20 / addi r31,r29,0x24 / bl __nw__ / slwi r30,r28,2 / ...

Con E1 **el reparto de registros y el orden de operandos del `stwx`/`lwzx` son
YA los del objetivo**; lo único que queda es cuál de los dos rellena la ranura
delante del `bl`. Y ahí está el nudo, confirmado con 40 ensayos:

- **La base tiene las POSICIONES bien y los REGISTROS cruzados**; E1 al revés.
  El orden de **asignación** decide las dos cosas a la vez (confirmo F1=E2,
  F2=E1 de la r36e) y **no se pueden separar desde la fuente**.
- Meter el `new` en un temporal entre las dos locales **no las separa**:
  K1 4, K2 2, K3 2, K4 4, K5 4, K6 2 — sigue mandando el orden de asignación.
- Formas mixtas (el store por `refs[i]` y las lecturas por el desplazamiento
  explícito, y al revés) para mover `n_refs`: Y1 4, Y2 2, Y3 2, Y4 4, Y5 2, Y6 4.
- **Cualquier `asm` dentro de ese bucle cuesta 4 B**: la salida del asm no
  coalesce y aparece un `mr r31, r0` (G2, G3, G6, G7, G8, H1, H2, H3, B1, B3 →
  388 B). Probado también con la restricción **`"+b"`** (que excluye r0) por si
  era eso: B1 388 B, B3 388 B. No lo es.
- Barreras y ejes sobre `i`: P1 13, P2 23, P3 13, P4 14, P5 13, P6 23, P7 23.
- Pin de `refs` a r31 sobre E2: 35 filas. `n_refs` (1, 2 y 3 asm de entrada)
  sobre E2: 388 B las tres.
- Diagnóstico: `-fno-schedule-insns2` deja E1 en 10 filas y `-fno-schedule-insns`
  en 43 — **las dos pasadas participan**, no es una sola.

**No aplico E1**: son cero bytes. Pero es la mejor base conocida y quien la
retome debe atacar **sólo la ranura del `bl`**, no el reparto.

### 4.3 `Speech::RoadblockFlow::Setup` (596 B, 4 filas) — veda confirmada, +8

    obj:  lwz r9,0(r31) / lwz r0,0x2e4(r9) / lha r3,0x2e0(r9) / mtlr r0 / add r3,r31,r3 / blrl
    nue:  lwz r9,0(r31) / lha r3,0x2e0(r9) / lwz r0,0x2e4(r9) / add r3,r31,r3 / mtlr r0 / blrl

El objetivo carga el puntero de función antes que el delta del thunk. **Ocho
formas más, siete de ellas con el binario IDÉNTICO a la base**:

| # | ensayo | resultado |
|---|---|---|
| A1 | `__asm__("# d");` DETRÁS de la llamada (el idioma que ya usan las otras tres virtuales del fichero) | idéntico |
| A2 | el mismo delante | idéntico |
| A3 | `asm("" : "+r"(primary))` delante | idéntico |
| A4 | leer el vptr + `asm("":"+r"(primary):"r"(vp))` para que r9 no muera en el `lwz` | **600 B** |
| A5 | llamar por otra local | idéntico |
| A6 | `asm("":"+r"(primary):"r"(primary))` | idéntico |
| A7 | `asm("" : "+m"(*primary))` | idéntico |
| A8 | `ai->GetDispatch()` cacheado en local | idéntico |

Sumadas a las cuatro de la r36d son **doce formas y once binarios idénticos**.
La expansión de la llamada virtual es impermeable a la fuente en esa posición.

### 4.4 `Speech::Manager::LoadSpeechBank` (316 B, faltan 4 B) — diagnóstico CORREGIDO

El encargo decía que el objetivo «guarda la referencia `type` en r11». **No es
`type`**: el volcado DWARF lo aclara,

    static int Manager::LoadSpeechBank(CLUMP_IDX_FILEtag *index /* r12 */,
                                       int &type /* r4 */, int &number /* r5 */,
                                       SPEECH_BANK *sb /* r6 */) {
        unsigned int key;  // r7
        int lower;         // r5
        int upper;         // r8
        int i;             // r10
    }

`type` muere en la instrucción 1 (`lwz r0, 0x0(r4)`) y el objetivo **reutiliza
r4** para la base del array `index->item` (`addi r4, r12, 0x58`). Nosotros la
ponemos en r7 y metemos `key` en r11; el DWARF dice que **`key` va en r7**.

Lo que falta de verdad son **dos `mr` del puntero base contra uno nuestro**:

    obj:   mr r11,r4 (antes del bucle)  y  mr r4,r11 (DENTRO del bucle)
           r4 en las insn 11 y 19; r11 en las 29, 37, 41 y 47
    nue:   mr r4,r7 (una sola vez, antes del bucle)
           r7 en las 11, 19 y 29; r4 en las 37, 41 y 47

O sea: los dos partimos el rango de vida del puntero base en dos, pero **el
objetivo lo parte más pronto y refresca la copia en cada vuelta del bucle**. Es
la firma de `GetElapsedTime` que ya anotó la r36d, pero con el sujeto corregido.

Pines medidos (la palanca que faltaba probar, con la cardinalidad de uno):

| # | ensayo | tamaño |
|---|---|---|
| base | — | 312 (objetivo 316) |
| C1 | `register unsigned int key asm("r7")` (lo que dice el DWARF) | 312, y **descoloca `upper` e `i`** |
| C2 | `register int lower asm("r5")` | **308** |
| C3 | `register int upper asm("r8")` | 312 |
| C4 | `register int i asm("r10")` | 312 |
| C5 | C1 + C4 | 312 |

En la base **`i` ya está en r10 y `upper` en r8**, como manda el DWARF: los
únicos desajustes son `key` (r11 contra r7) y la base del array (r7 contra r4).
C1 fuerza `key` a r7 pero de paso convierte `add r7,r0,r11` en `add r7,r7,r0` y
empuja a los demás: es el caso «si el pin empeora, es síntoma» de la memoria.

### 4.5 `CARSFX_Turbo::ProcessUpdate` (672 B, 11 filas) — diagnóstico afinado

Las once filas siguen colgando del `mr. r7, r3` que sale **nueve ranuras tarde**.
Dato nuevo: **ese `mr.` es la primera instrucción del clamp de
`SetAzimuth(Az)`**, y el objetivo lo sube hasta pegarlo al `bl GetDMixOutput`,
**veintidós ranuras antes de su propio `bge`**. Está en el MISMO bloque básico
que todo el cálculo en coma flotante, así que es una decisión pura de
`rank_for_schedule`: al competir con el `lis r8,0x4330` (que encabeza la cadena
larga del `float`), nuestro planificador prefiere el `lis` por prioridad. Una
barrera sólo puede añadir restricciones, no izar — lo confirman las once formas
de la r36e. **Para esta hace falta bajar la prioridad de la cadena del float, no
subir la del `mr.`**; no conozco palanca que lo haga.

### 4.6 `EAXAemsManager::SetupNextLoad` (1.008 B, 23 filas)

Sin tocar por presupuesto. Anoto el reparto exacto para quien la coja, que no
estaba escrito: es una **rotación de seis registros**, no de uno —
`this` r29→r27, el contador del bucle r26→r31, r28→r29, r27→r28, r7→r29, r6→r7 —
más el `beq` de cross-jumping de la insn 195 y la posición del
`lis gFastMem@ha` (insn 84/86). El contador que hay que pinchar (r26 en el
objetivo) **vive dentro de `DeleteRefToAsset`, inlineada**, no en
`EAXAemsManager.cpp`: el pin hay que ponerlo en la cabecera de esa plantilla,
que es de mi territorio pero no de este fichero.

### 4.7 `GenerateRoadNoise` (1.240 B, 33 filas) y `BindToData` (340 B)

Sin tocar. De `GenerateRoadNoise` dejo localizado el foco: las 33 filas caben en
las instrucciones **95-135**, y todo cuelga de que el objetivo emite el **primer
`fmadds f29, f31, f11, f31` doce ranuras antes** que nosotros y el segundo
(`fmadds f29, f29, f10, f29`) veinte antes. El resto son renombres de f10/f11 y
de las bases `@ha` que arrastra ese adelanto.

---

## 5. Lo que NO he podido reproducir del encargo

Nada: las diez funciones de la cola verificada salen con `fncmp` **exactamente**
con las cifras del encargo, y `fndiff` las confirma una a una. Es la primera
ronda de este territorio en la que la cola del encargo no traía humo.

---

## 6. Los cflags de zEAXSound2 quedan confirmados

`nfsmw-cflags-por-biblioteca` avisa de que los cflags de estas unidades se
copiaron en bloque de `snd`. Los he barrido sobre `CARSFX_Roadnoise.cpp`
(15 flags, mirando las **siete** funciones del fichero a la vez):

| flag | `Play` | `GenerateRoadNoise` | vecinas rotas |
|---|---|---|---|
| (base) | 388/392 | 33 filas | **0** |
| `-fno-gcse` | 388/392 | **95 filas** | 5 |
| `-fno-cse-follow-jumps` | 388/392 | 33 | 3 |
| `-fno-cse-skip-blocks` | **396**/392 | 1256/1240 | 3 |
| `-fno-rerun-cse-after-loop` | **416**/392 | 1184/1240 | 4 |
| `-fno-expensive-optimizations` | 388/392 | 65 filas | 2 |
| `-fno-force-mem` | 388/392 | 33 | 3 |
| `-fno-force-addr` | **380**/392 | 1244/1240 | 3 |
| `-fno-move-all-movables` | 388/392 | 33 | 2 |
| `-fno-rerun-loop-opt`, `-fno-strength-reduce`, `-fno-thread-jumps`, `-fno-caller-saves`, `-fno-function-cse`, `-fno-peephole` | 388/392 | 33 | **0** |

**Ninguno mejora y la base es la única con cero vecinas rotas.** El frente de
flags está cerrado para zEAXSound2; la causa de §1 es de la fuente o de una
palanca, no de la línea de compilación.

---

## 7. Método: dos cosas que valen para todos

### 7.1 El arnés que NO toca el fichero original

El arnés de la r36e escribía la variante **encima** del `.cpp` del árbol y lo
restauraba en un `finally`. Con varios agentes trabajando a la vez eso es una
bomba: se me cayó una escritura a mitad (`OSError: [Errno 22]`) y el fichero
podría haber quedado con la variante puesta. El arnés de esta ronda escribe la
variante como **fichero HERMANO** (`__vsnd_<tag>.cpp` / `__vsnd_<tag>_<hdr>.h`,
mismo directorio para que los `#include` relativos casen) y la borra al acabar;
**el original no se abre en escritura nunca**.

Para variar una **cabecera** sin sobrescribirla: se escribe la copia modificada
como hermana y se `#include` **la primera** en el stub. Su propia guarda
(`#ifndef ENVIRO_AEMS_H`) impide que se lea después la de verdad. Cuesta cero y
funcionó a la primera con `ENVIRO_AEMS.h` y `MAIN_AEMS.h`.

Ciclos medidos: zEAXSound2 6 s, zEAXSound (`STICH_PlayBack.cpp`, PRE
`#include "types.h"`) 7 s, zSpeech 9-12 s. Para zSpeech el PRE bueno **no es una
lista de `#include`**: es **el texto literal de `SourceLists/zSpeech.cpp` hasta
la línea que incluye tu `.cpp`** (`SL.split(MARCA)[0]`). Reproduce el objeto real
sin adivinar nada.

### 7.2 La técnica del TESTIGO

> **Antes de reordenar algo de una cabecera, busca una función que HOY case al
> 100 % y que use lo mismo.** Si tu cambio la rompe, el orden que hay en el árbol
> es el bueno y la vía está cerrada — aunque tu función mejore.

`InitSFX` costó una compilación y cerró en firme el frente que `r36e` dejaba
abierto como «la receta a probar» (§3). Y a la inversa: `Csis::FX_UVES` **no
tiene testigo** (sus tres instancias están en la misma función), así que ahí
reordenar es libre (§2.2).

---

## 8. Verificación

**Nada aplicado.** `git status` de mi territorio:

    src/Speed/Indep/Src/EAXSound   (sin cambios)
    src/Speed/Indep/Src/Speech     (sin cambios)
    src/Speed/Indep/SourceLists    (sin cambios)

**`fncmp` antes y después de la ronda, sobre las TRES unidades** (reconstruidas
con `build_direct.py`), byte a byte iguales:

    zEAXSound    2 de 773 funciones -- 1392 B     (SetupNextLoad, cStichWrapper::Play)
    zEAXSound2   6 de 930 funciones -- 3564 B     (GenerateRoadNoise, MsgPlayMiscSound,
                                                   Turbo::ProcessUpdate, RoadNoise::Play,
                                                   BindToData, MsgBarrier)
    zSpeech      2 de 703 funciones --  912 B     (RoadblockFlow::Setup, LoadSpeechBank)

**EMPEORAN: ninguna.** Las listas de «sólo nombres de símbolo distintos» (181,
162 y 122 entradas) salen idénticas.

**Ficheros temporales**: `find src -name "__vsnd*"` → vacío. No queda ni uno.

**`lcfix.py --check`**: informa de **324 correcciones pendientes repartidas en 17
unidades** — zAI (16), zAnim (5), zCamera (21), zEAXSound (30), zEAXSound2 (26),
zEcstasy (14), zFe (10), zFe2 (16), zFeOverlay (10), zLua (14), zMain (14),
zPhysics (31), zPhysicsBehaviors (27), zSim (30), zSpeech (22), zWorld (23),
zWorld2 (15). **No es de esta ronda**: no he tocado ningún fichero, mis tres
unidades dan el mismo código que al empezar, y la lista incluye unidades que no
he construido nunca (zAI, zCamera, zPhysics, zSim...). Mientras medía, otro
agente lanzó un build completo (todos los `.o` de `SourceLists` quedaron con la
misma marca de tiempo). **Lo dejo sin tocar y lo aviso: conviene pasar
`lcfix.py` una vez ANTES de verificar el DOL de esta tanda.**

---

## 9. Propuestas fuera de mi parche

1. **`Csis::FX_UVES` (`MAIN_AEMS.h:1628`) está en el orden equivocado y ahora hay
   tres pruebas independientes**: el volcado DWARF, la inversión de la regla de
   emisión, y —nuevo— que con `ivwpns` **el sitio 1 sale exacto**. El orden bueno
   es `SetID, SetVolume, SetWidth, SetPitch_Offset, SetIntensity, SetStop`. No lo
   aplico porque deja 30 filas en los sitios 2 y 3 por §1, pero **el aviso de
   alcance de la r36e sobra**: el constructor sólo se instancia en
   `MsgPlayMiscSound`.

2. **La palanca que falta, formulada para el catálogo.** Hace falta algo que
   mueva **el punto donde GCC 2.9 parte la constante 0 en dos pseudos** dentro de
   un mismo bloque básico. Hoy partimos en el store al **último campo de la
   estructura** y el original parte **detrás del primer store de cero**. Vale
   1.172 B en mi territorio (780 + 392) y muy probablemente más en otros: cualquier
   constructor `Csis::*` con varios argumentos constantes iguales tiene el mismo
   patrón. Lo que ya sé que NO la mueve: el orden de los `Set` (720+110 medidas),
   `+m` sobre el campo o sobre `mData` en cualquier posición (15 medidas), la
   barrera total y la de `"memory"` (7), 15 flags de optimización, y forzar un
   cero propio desde el sitio de llamada con `register ... asm("r0")` (5).

3. **Subir el modo «dos rutas» a `fncmp.py`** — lo volví a necesitar y lo volví a
   parchear en el scratchpad. El parche es el que ya proponía `r36e` §6.2 y sigue
   sin estar. Con él el ciclo de trabajo baja de 40 s a 6 s.

4. **`SetupNextLoad` necesita un pin dentro de `DeleteRefToAsset`** (§4.6). Quien
   la coja tiene que empezar por localizar esa plantilla, no por
   `EAXAemsManager.cpp`.

---

## 10. Para la memoria del proyecto

**Entrada nueva — «los dos grupos del cero»** (§1):

> Cuando un constructor inline hace varios `SetX(0)`, GCC 2.9 reparte la
> constante en DOS pseudos: uno de **vida larga** (preservado, compartido con
> otros ceros de la función) para los stores emitidos **antes** del primer store
> de un valor no-cero, y uno **corto** (casi siempre r0) para los de después.
> Dentro de cada grupo la emisión es el orden de la fuente con el último `Set`
> rotado al frente. **Nosotros partimos en el store al ÚLTIMO CAMPO de la
> estructura; el original parte detrás del PRIMER cero.** Si al objetivo le sobra
> un `li 0` respecto a ti, es esto y no el reparto de registros. Reordenar el
> constructor NO lo arregla: mueve el grupo corto y el orden dentro de él a la
> vez, y las dos condiciones se contradicen (demostrado con las 720
> permutaciones de `FX_UVES`).

**`nfsmw-extrapolar-frentes` gana una técnica — el TESTIGO** (§7.2):

> Antes de reordenar algo de una cabecera compartida, busca una función que HOY
> case al 100 % y que use lo mismo. Si tu cambio la rompe, el orden del árbol es
> el original y la vía está cerrada. `InitSFX` cerró en una compilación un frente
> que la ronda anterior dejaba como «la receta a probar». Y si NO hay testigo
> (todas las instancias están en la función que estás arreglando), reordenar es
> libre aunque la cabecera parezca compartida.

**`nfsmw-barrera-selectiva`, acotación del uso 6** (§1.4):

> `asm("" : "+m"(campo))` fija el ORDEN de emisión de un grupo de stores, pero
> **no toca el reparto del registro de la constante**. Y **puesto ANTES de su
> propio store es inerte**: binario idéntico (tres medidas: A4, A5, B4).

**Contraejemplo a «las familias se combinan»** (§4.1):

> En `MsgBarrier` la barrera de ranura sobre el objeto y la de argumento sobre el
> `1` dejan **3 filas cada una** y **4 juntas**. La combinación de familias que
> cerró `TrackCop` no es universal; sigue mereciendo probarse, pero cuenta como
> ensayo, no como regla.

**Aviso de arnés** (§7.1):

> Un arnés que escribe la variante ENCIMA del fichero del árbol es peligroso con
> varios agentes a la vez (se me cayó una escritura con `OSError 22`). Escribe la
> variante como fichero HERMANO y bórrala; para una cabecera, una copia hermana
> incluida la primera en el stub —su propia guarda tapa la original—.
