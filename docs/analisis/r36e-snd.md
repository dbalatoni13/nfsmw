# Ronda 36e — `c36e-snd`: zEAXSound y zEAXSound2

**Dos funciones cerradas, +692 B, dos palancas nuevas y una corrección al
encargo: las tres funciones que encabezaban mi cola YA CASABAN.**

    zEAXSound2   NFSMixMapState::CreateSubMixChannels   336 B   99,286 % -> 100 %
    zEAXSound    startnextrequest                       356 B   94,303 % -> 100 %

Verificación completa en §5. Dos ficheros tocados, **sin commit**:

    src/Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMapState.cpp
    src/Speed/Indep/Src/EAXSound/realstream/src/stream.cpp

---

## 0. Corrección al encargo: las tres de «UNA instrucción» ya casaban

El encargo mandaba empezar por `UpdateMixerOutputs__14SFXCTL_Physics` (1.212 B),
`CreateState__9EAXCopCarUi` y `CreateState__8EAXTruckUi` (120 B cada una) —
«1.452 B juntas»—. **`fndiff` da `target=100.0% ours=100.0%` en las tres.** Y no
eran tres: la versión de `fncmp.py` con la que se armó el encargo daba **26
falsos positivos en zEAXSound** (toda la familia `CreateObject__*Ui` /
`CreateState__*Ui`, `UpdateTorque` y `UpdateMixerOutputs`, 3.808 B) y **9 en
zEAXSound2** (856 B).

La causa, que conviene tener escrita porque volverá a aparecer:

    obj:   lis r3, 0x8046                   <- SIN reubicación
    ours:  lis r3, gAudioMemoryManager@ha   <- CON reubicación

El `addi @l` que emparejaría con ese `lis` está **en otro bloque básico** (el
`lis` está en la rama corta y el `addi` detrás de la confluencia, tras un `b`),
así que dtk no pudo reconstruir la pareja al extraer el objeto y dejó el valor
crudo. `fncmp` contaba «reubicación en un lado y no en el otro» como diferencia.

`fncmp.py` se arregló a mitad de ronda (`a26b2ecc`) y **ahora las dos unidades
dan exactamente las cifras oficiales**: zEAXSound 3 funciones / **1.748 B**,
zEAXSound2 7 funciones / **3.900 B**. La cola real era:

| B | función | unidad |
|---|---|---|
| 1.240 | `GenerateRoadNoise` | zEAXSound2 |
| 1.008 | `SetupNextLoad` | zEAXSound |
| 780 | `MsgPlayMiscSound` | zEAXSound2 |
| 672 | `CARSFX_Turbo::ProcessUpdate` | zEAXSound2 |
| 392 | `CARSFX_RoadNoise::Play` | zEAXSound2 |
| 384 | `cStichWrapper::Play` | zEAXSound |
| **356** | **`startnextrequest`** | zEAXSound |
| 340 | `BindToData` | zEAXSound2 |
| **336** | **`CreateSubMixChannels`** | zEAXSound2 |
| 140 | `MsgBarrier` | zEAXSound2 |

> **Regla de método:** `fncmp` es para ELEGIR; antes de gastar una sola
> compilación en una función, `fndiff <unidad> <símbolo>`. Cuesta 3 s y me habría
> ahorrado creer que tenía 1.452 B a un paso.

---

## 1. Palanca nueva: la **local de desplazamiento explícito** rompe la veda del orden de operandos de `lwzx`/`stwx`

`r36d-fe` §3.2 dejó `CreateSubMixChannels` a **una fila** y la declaró
inalcanzable: *«Nueve formas de fuente dan exactamente el mismo `lwzx`. El orden
del `PLUS` en el direccionamiento indexado no se alcanza desde la fuente»*.

    obj:   lwzx r30, r11, r10        (puntero, índice)
    ours:  lwzx r30, r10, r11        (índice, puntero)

**Sí se alcanza.** Basta meter el desplazamiento en BYTES en una local propia:

```c
int nOffset = n * sizeof(stSubMixChProc);
pSMSD = reinterpret_cast<stSubMixChProc *>(
            reinterpret_cast<char *>(this->m_pFirstInstance->m_MixStateParams.pSubMixChProcs)
            + nOffset)->pMixChData_S;
```

| # | qué | % | difs |
|---|---|---|---|
| base | — | 99,28571 | 9 |
| Q2 | los tres pines de `r36d-fe` §3.2 | 99,88095 | 1 |
| S1 | **sólo** la local `nOffset`, sin pines | 99,40476 | 8 |
| S3 | Q2 + el mismo cast **en línea, sin local** | 99,88095 | **1** |
| **S4** | **Q2 + la local `nOffset`** | **100** | **0** |
| S5 | S4 con `*(T**)` en vez de `->pMixChData_S` | 100 | 0 |
| R1 | S4 + `asm("" : "+r"(nOffset))` | 97,44048 | 4 |
| R4 | barrera sobre `n` (sin la local de offset) | 95,65476 | 5, **+4 B** |

**Lo que decide es la LOCAL, no el cast** (S3 contra S4: mismo texto, una con
local y otra sin; 1 fila contra 0). Y los dos efectos son **independientes**: la
local sola cierra la fila del `lwzx` (S1: 9 -> 8 difs) y los pines cierran las
ocho de reparto.

### Por qué

`print_operand_address` de rs6000 imprime el orden del `PLUS` de la RTL tal
cual (sólo intercambia si `XEXP(x,0)` es `r0`). Con `ptr + n` la dirección llega
a `combine` como `(plus (reg ptr) (ashift (reg n) 3))`; la canonicalización de
operandos conmutativos de `combine` intercambia cuando **`XEXP0` es un objeto
(`'o'`: REG/MEM/CONST) y `XEXP1` no lo es** — y un `ASHIFT` no lo es. De ahí sale
nuestro `(índice, puntero)`. Con el desplazamiento ya calculado en un pseudo los
dos operandos son REG, no hay intercambio, y sale el orden del objetivo.

**Alcance:** cualquier `lwzx`/`stwx`/`lbzx`… con los operandos cruzados. Es una
veda que el proyecto daba por cerrada.

---

## 2. `startnextrequest` (356 B, 22 difs -> 0): el pin del DWARF, **uno solo**

La función no la había mirado ninguna ronda anterior. Las 22 filas eran una
**rotación de cuatro registros salvados**: nuestro r28 -> r27 del objetivo,
r29 -> r28, r30 -> r29, r27 -> r30.

El volcado DWARF la da entera:

```
static void startnextrequest(struct STREAMHEADERtag * strm /* r31 */, int priority /* r27 */) {
    struct REQUESTSTRUCTtag * req;  // r28
    int lockstate;
    int nopendingrequest;           // r30
}
```

| # | qué | % | difs |
|---|---|---|---|
| base | — | 94,30337 | 22 |
| U1 | pin `req` a r28 | 94,52809 | 18 |
| **U2** | **pin `nopendingrequest` a r30** | **97,75281** | **2** |
| U3 | U1 + U2 | 85,89888 | 24, **+8 B** |
| U4 | U3 + `priority` copiado a una local pinchada a r27 | 85,89888 | 24, +8 B |

> **Regla:** con una rotación de N registros, **pinchar uno** arrastra a los
> demás; pinchar dos los pelea. U2 sola coloca `priority` en r27 y `req` en r28
> sin tocarlos. Dos pines son peores que uno y encima engordan el marco.

Las dos filas que quedaban eran la posición de un store:

    obj:   lwz r28,0x6c(r31) / li r0,2 / lwz r9,0x60(r31) / stw r0,0x4(r28) / stw r9,0x120(r28)
    ours:  lwz r28,0x6c(r31) / li r0,2 / stw r0,0x4(r28) / lwz r9,0x60(r31) / stw r9,0x120(r28)

El objetivo iza la carga de `strm->datatail` **por encima** del store a
`req->state`. Se consigue **intercambiando las dos sentencias**:

```c
req = strm->curreq;
req->datastart = strm->datatail;      // <- antes
req->state = STREAMREQUEST_EXECUTING; // <- después
```

| # | qué | % | difs |
|---|---|---|---|
| U2 | (referencia) | 97,75281 | 2 |
| V1/V2/V5 | local para `strm->datatail` delante | no compilan (salto sobre inicialización) |
| **V3** | **los dos stores en el otro orden** | **100** | **0** |
| V4 | `asm("" : "+m"(req->state))` entre los dos | 85,67416 | 28, **+8 B** |

La barrera de memoria **no vale** aquí, y por el motivo general: sólo puede
AÑADIR restricciones, y lo que hace falta es *quitar* la que impide izar la carga
por encima del store. Reordenar la fuente sí lo hace.

---

## 3. Lo que queda a un paso, con la receta medida (**NO aplicado**)

`matched_code` es todo-o-nada: dejar una función al 99,9 % no aporta un byte y sí
deja deuda de `asm`. Va todo revertido, con la receta para arrancar desde ahí.

### 3.1 `SFX_Common::MsgPlayMiscSound` (zEAXSound2, 780 B): **28 -> 12 difs**

El eje es el orden de los `Set` del constructor de `Csis::FX_UVES`
(`MAIN_AEMS.h:1628`). **28 órdenes medidos.** El mejor con diferencia:

```c
FX_UVES(int iD, int volume, int width, int pitch_Offset, int intensity, int stop) {
    this->SetID(iD);
    this->SetVolume(volume);
    this->SetStop(stop);
    this->SetWidth(width);
    this->SetPitch_Offset(pitch_Offset);
    this->SetIntensity(intensity);
```

| orden | % | difs |
|---|---|---|
| `viwpns` (el actual, commit `5d1ac0cf`) | 99,40513 | 28 |
| `ivwpns` (= el del DWARF) | 97,26154 | 30 |
| **`ivswpn`** | **99,93846** | **12** |
| `vswpni` | 99,83077 | 13 |
| `ivwspn` | 99,83077 | 13 |
| `viswpn` / `vsiwpn` | 99,81538 | 16 |
| `iwpnsv` | 99,45128 | 27 |
| `wpnvsi`, `vwpnsi` | 99,39-99,41 | 28 |
| `wpnivs`, `wpnvis`, `vwpnis`, `vinwps` | 99,38-99,39 | 31 |
| `vpwnis` | 99,39487 | 30 |
| `iwpnvs` | 97,23077 | 34 |
| `viwpsn`, `wpnisv`, `wpnsiv`, `wpnsvi` | 96,06-96,08 | 36-38 |
| `isvwpn`, `sivwpn`, `swpniv`, `swpnvi`, `iswpnv`, `siwpnv`, `svwpni`, `sviwpn` | 94,94-94,96 | 40-44, **-8 B** |

Con `ivswpn` **desaparecen todas las filas de registro y de reparto del cero** en
los tres sitios y lo único que queda son **cuatro stores rotados, idénticos en
los tres**:

    obj:   … 0x18 … 0xc 0x10 0x14        (stop, width, pitch, intensity)
    ours:  … 0x14 … 0x18 0xc 0x10        (intensity, stop, width, pitch)

#### La regla de emisión, confirmada y con su inversa

`r36c-snd` §3.2 la enunció para el sitio 1; la he verificado en cuatro órdenes:

> **En un sitio donde los seis argumentos son la constante 0, la emisión es el
> orden de la fuente con el ÚLTIMO `Set` rotado al frente.**

    fuente viwpns -> emisión s,v,i,w,p,n   (medido, es la base)
    fuente ivswpn -> emisión n,i,v,s,w,p   (medido)
    fuente ivwpns -> emisión s,i,v,w,p,n   = OBJETIVO byte a byte (medido)

Invertida sobre el sitio 1 del objetivo (`s,i,v,w,p,n`) la regla da **un único
orden posible: `ivwpns`** — que es **exactamente el que lista el volcado DWARF**
para el original (`SetID, SetVolume, SetWidth, SetPitch_Offset, SetIntensity,
SetStop`). O sea: **el constructor original es `ivwpns` y el nuestro (`viwpns`,
del commit `5d1ac0cf`) está mal.**

El problema es que en los sitios 2 y 3 (`FX_UVES(2,0,0,0,0,0)` y
`FX_UVES(1,0,0,0,0,0)`, con `iD` no nulo) **la regla no aplica**: con `ivwpns` la
emisión sale `v,w,p,n,i,s` y el objetivo quiere `v,i,s,w,p,n`, con el cero de
larga vida repartido distinto (r27 sólo para `volume` en el objetivo; r28 para
`volume,width,pitch,intensity` en el nuestro). `ivswpn` arregla ese reparto en
los tres sitios a cambio de dejar la rotación de cuatro.

#### Dato nuevo de palanca: el `+m` fija el orden de la FUENTE

| # | qué | % | difs |
|---|---|---|---|
| J0 | `ivswpn` | 99,93846 | 12 |
| J1 | J0 + `asm("" : "+m"(this->mData))` tras los seis `Set` | 90,01538 | 39 |
| J2 | J0 + `asm("")` total | 83,66154 | 47 |
| J3 | J0 + `+m` sobre `mData.intensity` | 89,62564 | 39 |
| J4 | J0 + `+m` sobre `mData.stop` | 89,96410 | 37 |
| J5 | `ivwpns` + `+m(mData)` | 89,62051 | 51 |
| J6 | J1 + clobber `"r0"` | 90,01538 | 39 |
| J7 | J0 + `+m` sobre `mpClass` | 96,30769 | 25 |
| K1 | `sivwpn` + `+m(mData)` | 87,09231 | 59 |
| K2 | `viswpn` + `+m(mData)` | 90,01538 | 39 |
| K4 | `+m(mData)` **antes** de los seis `Set` | 94,33846 | 26 |
| K5 | J0 + `SetIntensity` repetido al final | 99,93846 | 12 (idéntico: el store duplicado se borra) |

Un `asm("" : "+m"(this->mData))` detrás de un grupo de stores **cancela la
rotación y los deja en el orden de la fuente**. Es una palanca útil en general
—es la primera forma conocida de fijar el orden de emisión de los miembros de un
constructor— pero **aquí no puede cerrar**: el sitio 1 quiere `s,i,v,w,p,n` y los
sitios 2/3 quieren `v,i,s,w,p,n`, y un solo constructor no da los dos.

### 3.2 `CARSFX_RoadNoise::Play` (zEAXSound2, 392 B objetivo / 388 nuestros)

**Diagnóstico cerrado, misma familia que §3.1.** Los 4 B que faltan son **un
`li 0` entero**: el objetivo tiene DOS registros con cero y nosotros uno.

    obj:   li r29,0 (volume 0x8, azimuth 0x10) + li r0,0 (type 0x14, secNoise 0x18,
                                                          speed 0x1c, hiPass 0x24, wetFX 0x2c)
    ours:  li r30,0 para los SIETE stores de cero

El corte del objetivo cae **justo después de `azimuth`**, o sea entre el 4.º y el
5.º argumento de `FX_ROADNOISE(ID,0,0x1000,0,LOOP,0,0,25000,0,0x7FFF,0)`. El
orden de emisión del objetivo es `[vol,azim] [id,pitch,loPass,dryFX]
[wetFX,type,secNoise,speed,hiPass]` —el último `Set` rotado al frente de su
grupo, igual que en §3.1— y el nuestro `[los siete ceros por offset]
[los cuatro no-ceros por offset]`. La receta a probar es el barrido de órdenes de
`FX_ROADNOISE` (`ENVIRO_AEMS.h:311`) como en §3.1; no me dio tiempo.

### 3.3 `GinsuSynthData::BindToData` (zEAXSound2, 340 B objetivo / 344 nuestros): 12 -> **7 difs**

El DWARF dice `int i; // r5` **dentro de un bloque**, y `minperiod // r7`.
Pinchar `i` a r5 baja de 12 a 7 filas sin tocar el tamaño:

```c
register int i asm("r5") = 0;
```

| # | qué | % | difs | B |
|---|---|---|---|---|
| base | — | 95,88236 | 12 | 344 |
| **X1** | **pin `i` a r5 (a nivel de función)** | **96,23529** | **7** | 344 |
| X2 | el mismo pin dentro de un bloque propio | 96,16470 | 8 | 344 |
| Y3 / Y6 / Z1 | X1 + mover `mCurrentBlock=-1` / `+m` antes / `+m` con clobber r0 | 96,23529 | 7 | 344 |
| Y5 | X1 + `+m` tras el store | 96,08235 | 9 | 344 |
| X4 | X1 + local `cyclePos` | 96,05882 | 10 | 344 |
| Z6 | X1 + cantidad fantasma en r0 | 94,70588 | 11 | 344 |
| Y2 / Y4 | X1 + local para el `-1` con barrera | 95,87059 | 12 | 344 |
| X5 | pin `ptr` a r30 | 95,88236 | 12 (binario idéntico) | 344 |
| Y1 | X1 + pin del `-1` a r0 | 95,82353 | 13 | 344 |
| X3 | local `cyclePos` sola | 95,70588 | 14 | 344 |
| **Z5** | **X1 + pin `minperiod` a r7** | 95,64706 | 17 | **336** |
| Z4 | X1 + `cp` local pinchado a r8 | 95,23529 | 22 | 344 |
| Z3 | X1 + `cp` pinchado sólo en el bucle | 93,23529 | 23 | 348 |

**Negativo que confirma a `r36d-fe`**: el árbol de ámbitos no es la causa.
W1 (`for (int i = 0; …)`), W2 (mover `mCurrentBlock`), W3 (`minperiod` declarado
arriba), W4 y W5 (bloque explícito para `i`) dan **las 12 filas y 344 B en las
cinco**.

Lo que queda con X1: nuestro `li r8,-1` sale **18 ranuras antes** que el
`li r0,-1` del objetivo, y por eso la base `r10+r9` aterriza en r4 y hace falta
un `mr r8, r4` en el preencabezado del bucle — **ése es el 4 B de más**. Nueve
formas para mover el `li` (Y1..Y6, Z1, Z2, Z6) no lo tocan.

### 3.4 `cStichWrapper::Play` (zEAXSound, 384 B, 4 difs)

`r36d-fe` lo dejó como *«dos temporales sin nombre en el fuente: no hay dónde
pinchar ni a quién subirle `n_refs`»*. **Sí hay dónde**, pero las dos mitades
están **acopladas**: se arregla una y se rompe la otra, siempre 4 filas.

    obj:   slwi r30,r28,2 / bl __nw__ / addi r31,r29,0x24 / … / stwx r3,r31,r30 / lwzx r0,r31,r30
    base:  slwi r31,r28,2 / bl __nw__ / addi r30,r29,0x24 / … / stwx r3,r30,r31 / lwzx r0,r30,r31

Con una local para la base más la forma de desplazamiento explícito de §1:

```c
cSampleWarpper **refs = this->ActiveSamplesRefs;      // <- asignada PRIMERO
int off = i * sizeof(cSampleWarpper *);
*reinterpret_cast<cSampleWarpper **>(reinterpret_cast<char *>(refs) + off) = new …;
```

| # | qué | % | difs | qué queda |
|---|---|---|---|---|
| base | — | 99,68750 | 4 | registros r30 <-> r31 |
| **E1** | **`refs` asignada primero + `off`** | 95,83334 | **4** | **registros OK**, el `addi` sale ANTES del `bl` |
| E2 | `off` primero + `refs` | 99,68750 | 4 | posiciones OK, registros cruzados |
| F1 | declara `refs,off`; **asigna** `off,refs` | 99,68750 | 4 | = E2 |
| F2 | declara `off,refs`; **asigna** `refs,off` | 95,83334 | 4 | = E1 |
| E3 | E1 + local `slot` | 95,83334 | 4 | = E1 |
| C3 | sólo local `refs`, sin `off` | 95,62500 | 6 | registros OK, orden del `stwx` cruzado |
| G1 | `refs` local, índice en línea | 95,62500 | 6 | = C3 |
| E4 / C1 | sólo `off`, sin `refs` | 94,26 / 93,43 | 37 | |
| C2 | `off` pinchado a r30 | 97,38541 | 44 | |
| C5 / F3 / F4 | pines de `refs` / `off` | 87,3-88,0 | 40-50 | |
| C4 / F5 / F6 / G2 / G3 / G4 | barreras y ranuras | 88,6-96,1 | 17-29 | |

**El dato de método:** el orden de DECLARACIÓN no pinta nada; lo que decide es el
orden de **ASIGNACIÓN** de las dos locales (F1 = E2 y F2 = E1). Y decide las dos
cosas a la vez: *la que se asigna primero se emite antes del `bl` y se lleva
r31*. El objetivo quiere lo contrario —el `slwi` primero y el `addi` con r31—, y
ninguna de las 14 formas los separa.

### 3.5 `CARSFX_Turbo::ProcessUpdate` (zEAXSound2, 672 B, 8 filas)

**Las ocho filas cuelgan de una sola**: el `mr. r7, r3` (el `x` de
`SetAzimuth(Az)` con su comparación) sale **nueve ranuras tarde**. Con él en su
sitio `r3` muere y el objetivo lo reutiliza para `srawi r3, r11, 15`; nosotros lo
tenemos vivo y gastamos r30, y de ahí las filas 40, 41, 51 y 62. El
`lis LC539@ha` adelantado dos ranuras es la quinta.

El DWARF confirma que las locales y los bloques son correctos
(`nDMixOut // r30`, `Az // r3`, `TmpBlowoffVol // r3` en un bloque anónimo, y un
segundo bloque con `Az // r30`). **Once formas, ninguna mejora:**

| # | qué | % | difs |
|---|---|---|---|
| base | — | 97,50000 | 8 |
| T5 | local para `BlowoffVol` | 97,50000 | 8 (binario idéntico) |
| T2 | `asm("")` total tras el `if/else` | 97,32143 | 12 |
| T1 / T3 | barrera sobre `Az` tras el `if/else` / antes de `SetAzimuth` | 97,26190 | 14 |
| T6 | barrera sobre `Az` dentro de cada rama | 96,30952 | 10, **+8 B** |
| T7 | barrera sobre `nDMixOut` | 97,82738 | **35** |
| H1 | `SetAzimuth` delante del cálculo | 84,04166 | 40 |
| H2 / H3 | `SetAzimuth` entre el shift y el float | 79,97024 | 49 |
| H4 | H1 + objeto en local | 81,26190 | 46, −8 B |

T7 es un ejemplar de libro de lo que avisa `nfsmw-medidas-que-enganan`: **el
fuzzy SUBE (97,50 -> 97,83) y las diferencias pasan de 8 a 35.**

Es la misma frontera que `MsgBarrier`: el objetivo **iza** una instrucción por
encima de código de otra sentencia, y la barrera sólo puede añadir restricciones.

### 3.6 `EAXAemsManager::SetupNextLoad` (zEAXSound, 1.008 B, 23 filas)

Dato nuevo que corrige el diagnóstico de `r36c-snd` («21 filas de `this`
r27/r29»): **una de las filas no es de registro, es un salto**.

    insn 195   obj:  beq +0x240   |  ours: beq +0x3c0

El objetivo llega a la cabecera del bucle **a través del `b` de la insn 144** y
nosotros saltamos directos al destino final. Es *cross-jumping* de `jump.c`, no
reparto: la función no cierra sólo con pines. Y el DWARF **no ayuda**: el volcado
trae `// ERROR: Failed to process tag E5B398 (GlobalSubroutine)` justo en el
hueco 0x800AB3A0–0x800AB790, que es `SetupNextLoad`. Es la única función de mi
territorio sin cuerpo en el volcado.

### 3.7 `GenerateRoadNoise` (zEAXSound2, 1.240 B, 23 filas) — sin tocar

El DWARF confirma lo que ya decía `regmap`: nos sobra `float slipBoost // f7` y
al original le falta registro en `float half`. `r36c-snd` §5 barrió siete formas y
dejó E4 (barrera tras el primer `fmadds`) como mejor parcial. **No la he tocado**:
el presupuesto de la ronda daba para una función más y elegí las que cerraban.

---

## 4. Herramienta: `fncmpo.py`, la criba por FICHERO

Todo el trabajo de la ronda salió de un bucle de **1 a 8 segundos**: compilar UN
`.cpp` con `mn_repro.py` y comparar **todas** sus funciones contra el objeto
objetivo de la unidad. `fncmp.py` sólo acepta el nombre de una unidad; le añadí
cuatro líneas para que acepte **dos rutas `.o` explícitas** (queda en el
scratchpad como `fncmpo.py`; merece subir a `scripts/`):

    python scripts/mn_repro.py zEAXSound2 Speed/.../NFSMixMapState.cpp
    python fncmpo.py build/GOWE69/obj/Speed/Indep/SourceLists/zEAXSound2.o r_..._NFSMixMapState.o

Eso da, en un segundo, la lista de funciones de **ese fichero** que no casan. Es
lo que me permitió comprobar «no he roto ninguna vecina» **antes** de aplicar cada
parche, en vez de después de un `build_direct` de 40 s. Sobre la unidad entera
coincide al byte con el censo oficial.

### Los PRE que faltaban (el arnés miente sin ellos)

| unidad / fichero | PRE | tiempo |
|---|---|---|
| zEAXSound `realstream/src/stream.cpp` | `#include "types.h"` + `#include "Speed/Indep/bWare/Inc/bDebug.hpp"` + `#include <string.h>` | 2,5 s |
| zEAXSound `STICH_PlayBack.cpp` | `#include "types.h"` | 7 s |
| zEAXSound2 `NFSMixMapState.cpp`, `CARSFX_Turbo.cpp`, `CARSFX_Roadnoise.cpp`, `SFX_Common.cpp`, `ginsudata.cpp` | ninguno | 1-8 s |

**El aviso**: con un PRE incompleto el arnés **no falla, miente**. Con sólo
`types.h`, `stream.cpp` compila pero `restartstream` sale con 9 instrucciones
distintas (un `bGetTicker` implícito) que en la unidad real no existen. La prueba
de que un PRE es bueno es que `fncmpo` sobre ese fichero reproduzca
**exactamente** las funciones pendientes que da la unidad — ni una más.

---

## 5. Verificación

**`fndiff` al 100 % en las dos cerradas:**

    # startnextrequest__FP15STREAMHEADERtagi      target=100.0%  ours=100.0%  size=356/356
    # CreateSubMixChannels__14NFSMixMapState      target=100.0%  ours=100.0%  size=336/336

**`fncmp` antes/después sobre las DOS unidades.** La instantánea de «antes» la
tomé revirtiendo los dos ficheros y recompilando, para medir con la MISMA versión
de `fncmp` (§0). El `diff` de las dos salidas completas es exactamente esto y
nada más:

    zEAXSound   3 funciones / 1.748 B  ->  2 funciones / 1.392 B   (-startnextrequest)
    zEAXSound2  7 funciones / 3.900 B  ->  6 funciones / 3.564 B   (-CreateSubMixChannels)

**EMPEORAN: ninguna.** Las listas de «sólo nombres de símbolo distintos» (181 y
162 entradas) salen byte a byte iguales antes y después.

**Secciones:** los dos `.cpp` dan `.text/.rodata/.data/.bss/.sdata/.sbss`
**idénticas en tamaño** antes y después (ni un literal nuevo).
`python scripts/lcfix.py --check` -> *«todas las entradas @lc están al día»*.
`linkdelta` sobre el árbol entero:

    zEAXSound2   .text +0    rodata-1184 data-288 bss-32
    zEAXSound    .text +0    rodata-1544 data-416 bss+64

(los deltas de datos son los de la unidad; este parche no los toca.)

**Sin commits.** `git status` de mi territorio:

    M src/Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMapState.cpp
    M src/Speed/Indep/Src/EAXSound/realstream/src/stream.cpp

Quitada además la marca `// UNSOLVED` de `CreateSubMixChannels`, que ya no lo es.

---

## 6. Propuestas y avisos fuera del parche

1. **El constructor de `Csis::FX_UVES` está en el orden equivocado.** El volcado
   DWARF lo da como `SetID, SetVolume, SetWidth, SetPitch_Offset, SetIntensity,
   SetStop` y la fuente tiene `SetVolume` delante desde el commit `5d1ac0cf`. Lo
   dejo **sin tocar** porque `MAIN_AEMS.h` lo usan más unidades y porque el orden
   correcto empeora la única función que lo mide (30 filas contra 28). Pero quien
   retome §3.1 debe saber que el orden bueno está identificado y que el problema
   real son los sitios con `iD` no nulo.

2. **Meter el modo «dos rutas» en `fncmp.py`** (§4). No dejo un fichero
   duplicado porque `fncmp.py` lo está tocando otro agente y se quedaría rancio;
   el parche es este, dentro de `main()`, justo donde hoy pone
   `pa, pb = par('obj'), par('src')`:

   ```python
   if rel_u.endswith('.o'):
       pa, pb = sys.argv[1], sys.argv[2]
       detalle = [x for x in sys.argv[3:] if not x.startswith('--')]
   else:
       pa, pb = par('obj'), par('src')
   ```

   y saltarse el `AUSENTE` cuando el símbolo no está en el segundo objeto (al
   comparar contra un solo `.cpp` faltan, lógicamente, las demás funciones de la
   unidad). Convierte el ciclo de trabajo de 40 s en uno de 1 s, con la
   comprobación de regresión incluida.

3. **`fncmp.py` acaba de arreglar los falsos positivos del `lis` sin reubicar**
   (§0). Conviene rehacer el reparto de la ronda siguiente con la versión nueva:
   si en zEAXSound sobraban 3.808 B de humo, en otros territorios también.

---

## 7. Para la memoria del proyecto

**`nfsmw-comparador-reubicaciones` / `nfsmw-el-cero-que-miente` ganan un caso:**
un `lis` cuyo `addi @l` está en OTRO bloque básico sale **sin reubicación** en el
objeto extraído. Cualquier criba que compare reubicaciones tiene que tratar
«reubicación en un lado y valor crudo en el otro» como IGUALES si el `@ha` cuadra.
Valía 4.664 B de falsos positivos en dos unidades.

**Palanca nueva — la local de desplazamiento explícito** (§1):

> Si un `lwzx`/`stwx` sale con `(índice, puntero)` y el objetivo tiene
> `(puntero, índice)`, mete el desplazamiento en BYTES en una **local propia** y
> escribe la dirección como `(T*)((char*)ptr + off)`. Lo que decide es la local,
> no el cast: el mismo texto en línea deja la fila. Motivo: `combine` intercambia
> los operandos de un `PLUS` conmutativo cuando el primero es `'o'` y el segundo
> no lo es, y un `ASHIFT` no lo es; con el desplazamiento ya en un pseudo los dos
> son REG y no hay intercambio.
> **Reabre la veda de «el orden del `PLUS` indexado no se alcanza desde la
> fuente»** (`r36c-fe` §1, `r36d-fe` §3.2 con nueve formas).

**`nfsmw-pin-de-registro` gana una regla de cardinalidad** (§2):

> Ante una **rotación** de N registros salvados, pincha **UNO SOLO** —el que el
> DWARF señale— y los demás caen detrás. Dos pines se pelean: en
> `startnextrequest`, uno da 2 filas y dos dan 24 filas y **+8 B**.

**`nfsmw-barrera-selectiva` gana un sexto uso y un límite** (§3.1, §3.5):

> **Uso 6 — `asm("" : "+m"(objeto))` detrás de un grupo de stores cancela la
> rotación del planificador y los deja en el orden de la FUENTE.** Es la primera
> forma conocida de fijar el orden de emisión de los miembros de un constructor.
> **Límite reconfirmado:** no sirve cuando el objetivo IZA una instrucción por
> encima de código de otra sentencia (`Turbo::ProcessUpdate`, once formas;
> `startnextrequest`, donde la cura fue reordenar la fuente).

**Regla de emisión de constructores** (§3.1), medida en cuatro órdenes:

> Con todos los argumentos constantes iguales, la emisión de los `Set` de un
> constructor es **el orden de la fuente con el ÚLTIMO rotado al frente**.
> Invertirla sobre el objetivo da el orden del original **de forma única** — es un
> método para recuperar el orden de un constructor sin barrer las N!
> permutaciones. En `FX_UVES` coincidió con el DWARF a la primera.
> **Deja de valer en cuanto uno de los argumentos NO es la misma constante que
> los demás**: ahí manda el reparto del registro con el cero.

**Aviso de método**: el orden de **declaración** de dos locales no cambia nada;
el que decide reparto y posición es el orden de **asignación** (§3.4, F1 = E2 y
F2 = E1). Cuatro medidas.
