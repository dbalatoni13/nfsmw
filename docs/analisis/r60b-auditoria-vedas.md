# La auditoria de vedas: cuales caducaron

Sintesis para la ronda 61. 28 funciones abiertas, 33.668 B. **17 CADUCAS (18.128 B), 7 DUDOSAS (9.208 B), 4 FIRMES (6.332 B).**

Todo lo que sigue es de solo lectura: no he compilado nada ni tocado `src/`. Las cifras que dependen de un `.o` de `build/GOWE69/src` van marcadas PROVISIONAL. Lo que si he hecho es verificar en el arbol las lineas que sostienen las recetas, y **tres de ellas estaban rancias** (seccion final).

---

## El resultado en una tabla

| Unidad | Funcion | Bytes | Veda | Clase de causa | Veredicto |
|---|---|---:|---|---|---|
| zCamera | `__static_initialization_and_destruction_0` | 3604 | r50 | reparto | CADUCA |
| zWorld2 | `HolePunchAvoidables__8WRoadNav...` | 2980 | r48 | reparto | CADUCA |
| zEcstasy | `UpdatePlatInfo__27eLightMaterialPlatInterface` | 2044 | r56 | reparto | CADUCA |
| zPlatform | `ActualReadJoystickData__Fv` | 1588 | r47 (fuente) | reparto | CADUCA |
| zEAXSound2 | `GenerateRoadNoise__16CARSFX_RoadNoise` | 1240 | r50 | pool | CADUCA |
| zCamera | `Update__19TrackCarCameraMoverf` | 992 | r53 | reparto | CADUCA |
| zWorld2 | `InitAtSegment__8WRoadNavscf` | 816 | r49 | reparto | CADUCA |
| vp6/criticalpath | `VP6_PredictFilteredBlock` | 740 | r49 | planificacion | CADUCA |
| zTrack | `GetLoadingPriority__13TrackStreamer...` | 708 | r52 / r36f | estructura | CADUCA |
| zWorld | `DefragmentPool__9CarLoader` | 684 | r52 | reparto | CADUCA |
| madidct | `IdctColumn` | 632 | r48 | reparto | CADUCA |
| madidct | `IdctRow` | 516 | r48 | reparto | CADUCA |
| zEagl4Anim | `EvalState__Q29EAGL4Anim14FnRawStateChanf` | 456 | r60 | reparto | CADUCA |
| LibSN/steering | `SimThread_Init` | 324 | r51 | planificacion | CADUCA |
| LibSN/steering | `Effect_Init` | 276 | r51 (muro r22) | reparto | CADUCA |
| zEcstasy | `eProject__FfffPA3_fPfN44` | 268 | r49 | planificacion | CADUCA |
| LibSN/steering | `CookValues` | 260 | r51 / iter5 | reparto | CADUCA |
| zCamera | `Update__8ICEMoverf` | 3868 | r51 | reparto | DUDOSA |
| zEcstasy | `epCalculateLocalDirectionalPOS16` | 2072 | r54 | reparto | DUDOSA |
| LibSN/steering | `SimThread_Step` | 924 | r51 | reparto | DUDOSA |
| zPhysicsBehaviors | `UpdateLoaded__Q217SuspensionTraffic4Tireffff` | 856 | r54 | pool | DUDOSA |
| zSpeech | `Setup__Q26Speech13RoadblockFlow` | 596 | r48 | planificacion | DUDOSA |
| LibSN/steering | `HandleTriggers` | 588 | r51 | reparto | DUDOSA |
| zWorld | `SetMemoryPoolSize__9CarLoaderi` | 304 | iter6 | planificacion | DUDOSA |
| zWorld | `RenderFlaresOnCar__13CarRenderInfo` | 2908 | r54 | pool | FIRME |
| zEagl4Anim | `Initialize__Q25EAGL413DynamicLoader...` | 2352 | r60 | estructura | FIRME |
| zWorld | `UpdateWheelYRenderOffset__13CarRenderInfo` | 876 | r54 | pool | FIRME |
| LibSN/steering | `Effect_PerformEnvelope` | 196 | resume-20260908 | reparto | FIRME |

---

## Las que hay que reabrir YA

Ordenadas por probabilidad x bytes. La probabilidad es mia y va explicita para que se pueda discutir.

### 1. `__static_initialization_and_destruction_0` — zCamera, 3604 B, coste BAJO

**Causa, literal** (`src/Speed/Indep/Src/Camera/Movers/Cubic.cpp:357-386`): «Y ES `sched2`, NO LA FUENTE (r50, una compilacion por bandera) ... apagar sched2 no mueve un registro, asi que lo que difiere del objetivo es el MAPA valor->registro (el nuestro SYM=r11/LIT=r9/CONST=r10, el suyo SYM=r10/LIT=r11/CONST=r9): **la palanca esta en el ASIGNADOR, no aqui**.»

La veda **no dice «probado y negativo»: dice «la palanca esta en el asignador» y ahi se para**. En la r50 el catalogo no tenia ninguna palanca de asignador de enteros; nacio en la r53 (`docs/analisis/brief-r53.md:66-72`) y nadie volvio.

**Palanca**: r53, entrada `"r"(x)` en un asm — sube `n_refs` y con ello el numerador de `QTY_CMP_PRI` sin gastar ranura. Las tres cantidades tienen `n_refs=2`: el numerador empata y decide la vida.

**Receta**: en `Cubic.cpp:386`, sobre `bAngle HydraulicsLookAngle = bDegToAng(5.0f);` (NO tocar `bMath.hpp`, es cabecera compartida):
```c
static inline bAngle _cam_pin(bAngle a) { asm("" : "+r"(a)); return a; }
bAngle HydraulicsLookAngle = _cam_pin(bDegToAng(5.0f));
```
**Observable**: `fndiff.py zCamera __static_initialization_and_destruction_0` da hoy 6 filas (549/557/558/561/562/564, 3604/3604). La hipotesis es buena **si y solo si la fila 549 de nuestro lado pasa a `li r9, 0x38e`**. Menos filas sin `r9` llevandose la constante es paso lateral, no causa. **Tamano clavado en 3604 B**: si se mueve, el asm gasto ranura (`brief-r53.md:88`) y la medida no vale.

**Contexto obligatorio**: el andamio `_r60_ec[20]` de `ICEReplay.cpp:196-226` tiene que seguir en pie (sin el la funcion vuelve a 3.620 B / 54 filas) y el resto de zCamera en `139 funciones != 100%`.

### 2. `HolePunchAvoidables` — zWorld2, 2980 B, coste MEDIO

**Causa, literal** (`src/Speed/Indep/Src/World/Common/WRoadNetwork.cpp:2029-2060`, r47): «`allocno_compare` ordena por pri = (int)(floor_log2(n_refs)*n_refs/live_length*10000) ... 30000/492 = 60,97 y 30000/494 = 60,72: los dos truncan a 60, EMPATAN, y gana el pseudo menor.»

La veda de la r48 es de las fuertes — modelo escrito, ~15 variantes — **pero busco su «UN INSN MENOS» SOLO DENTRO DEL BUCLE** y se prohibio el preencabezado con una medida que no cubre lo que hace falta (`WRoadNetwork.cpp:2062-2065`): «Un `asm("")` ANTES del bucle NO cuenta: los insns de fuente previos al bucle caen delante de los movables». Eso es sobre **anadir** una insn de fuente. Las tres definiciones que empatan son **movables que loop.c iza**, y entre ellas hay OTROS ocho movables que la propia r48 cuenta (`WRoadNetwork.cpp:2058-2061`).

**Tercera rama, nunca evaluada**: quitar UNO de los ocho movables que caen entre def(901) y def(505) da live(901)=491 sin tocar el bucle. Con la formula de la r47: pri = 62/61/60 → 505→cr4, 901→cr3, 334→cr2, **que es el reparto del objetivo**. Y como N (insns anadidas al bucle) sigue en 0, el segundo empate f16/f17 ni se despierta.

**Palancas**: r54 (posicion de la declaracion / `update_equiv_regs`) para hundir la carga del literal 0,2f; r51 (`__asm__("")` como corte de region) para frenar un movable.

**Receta**: PASO 0 sacar el `.greg` y **escribir la lista de las ocho insns** entre def(901) y def(505) — la r48 las conto y no las escribio; sin esa lista no hay palanca. Luego bajar la declaracion de `extra_width` (`WRoadNetwork.cpp:2065`) para que el pseudo de la constante no cruce de bloque.
**Observable de causa (antes que el diff)**: en el `.greg`, `live(901)` de 492 a 491 **con `live(505)` quieto en 483**. Si 505 tambien baja, la insn estaba en el sitio equivocado aunque el porcentaje se mueva.
**Observable primario**: filas 176 y 178 de `cmpwi cr3,r29,0`/`cmpwi cr2,r31,0` a `cr2`/`cr3`, 4 filas a 0. **Control: 2980/2980**; 2984 o 2988 = se metio una insn de verdad (trampa que ya pillo a dos variantes de la r48).

### 3. `UpdatePlatInfo` — zEcstasy, 2044 B, coste MEDIO

**La veda ya esta rota por escrito y nadie recogio el trabajo.** `eLightE.cpp:311-350` (r56): «LA VEDA DE LA r49 ESTA ROTA — el bloque de literales del case 0x68E97F75 SI se mueve, y con el la funcion baja de 28 filas a 3». Las razones de no aplicarlo son de POLITICA (`docs/analisis/r56-ecs.md:84-90`: «3 filas valen cero bytes, igual que 28»), no de fisica. Y r57..r60 heredaron la etiqueta sin volver a medir.

**Lo que aporta esta auditoria** (y r56 mando comprobar y nadie comprobo): `symbols/mw_dwarfdump.nothpp:722366-722407` lista las locales del original y **son exactamente las nuestras, en nuestro orden** — la hipotesis «falta una local» esta MUERTA. Pero el mismo volcado destapa lo otro: el original mete **DOS locales en f5** — `float diffuse_rng_a; // f5` (:722376) y `float envmap_min_scale; // f5` (:722394), con vidas disjuntas (`r53-ecs.md:120-124`). Nuestro `register float envmap_min_scale asm("fr5")` — **verificado hoy en `eLightE.cpp:415`** — hace f5 exclusivo todo el ambito, y por eso el pseudo 115 no puede tenerlo nunca. **El andamio que sostiene la funcion desde la r36b es tambien lo que impide cerrarla**, y ningun `register asm` puede compartir registro por construccion.

**Receta**: banco propio, `python scripts/agent_ecs_upi.py --rows` (~2 s, no toca `src/`).
- PASO 1 (1 medida): quitar el pin de `eLightE.cpp:415` y correr `regmap.py`. **Observable**: en la columna del ORIGINAL, `diffuse_rng_a` y `envmap_min_scale` las dos en f5. Si regmap no las empareja asi, mi lectura del DWARF esta mal y se para aqui.
- PASO 2: **con el pin quitado** (base 60 filas, `r53-ecs.md:207`), repetir SOLO los ejes que r56 declaro ganadores con el pin puesto: `bar1` en `eLightE.cpp:537` y el barrido +N sobre `envmap_power`. **Observable**: la fila 103 pasa de `fsubs f1, f24, f7` a `fsubs f5, f24, f7` **sin que la 290 pierda su f31**. Las 21 filas del ciclo f1<->f31<->f5 caen juntas o no cae ninguna.

**Aviso**: las ~90 medidas de r56 y las 86 de r36f son solidas pero **todas se midieron CON el pin fr5 puesto**. Ninguna es una medida de la base limpia.

### 4. `ActualReadJoystickData` — zPlatform, 1588 B, coste BAJO — **BLOQUEADA, hay otro agente dentro**

`git diff` da +55 lineas sin commitear en `src/Speed/GameCube/Src/JoyE.cpp`. El agente vivo ya la tiene en **1.588 B EXACTOS / 9 filas** (cifras suyas, PROVISIONALES). No tocar hasta que commitee.

**Lo que la auditoria aporta y el no ha visto — el andamio se bloquea a si mismo**: su primer asm es `asm volatile("extsh 0,8; mr 9,0")`, que **cablea r8**. Y las 9 filas que quedan son que «el objetivo da un registro DISTINTO al `data` de cada bloque de eje (RightX r8, RightY r10, LeftX r11) y nosotros reutilizamos r8». O sea: la correccion que hay que hacer **rompe el asm**. Eso explica sin misterio por que su `register int data asm("r10"/"r11")` sale peor (1.580 B / 19 filas): el pin no fracasa por el asignador, fracasa porque el andamio nombra el registro a mano.

**Receta (para despues del commit)**:
1. Quitar los hardcodes: `asm volatile("extsh %0,%1; mr 9,%0" : "=r"(ext) : "r"(data) : "r9");`. **Observable**: tamano **sigue** en 1.588 B y aparece `stb r0,0x4(r29)` en lugar de `stb r8` (la novena fila). 1.592 = revertir.
2. Solo entonces, `register int data asm("r11")` en el bloque de LeftX. Si vuelve a caer a 1.580, el pin es sintoma.
3. Palanca r53, nunca probada aqui porque **hasta la r60 no habia ningun asm**: `asm volatile("andi. 11,9,0x8000" : : "r"(data_rightY) : "cr0","r11");`. **Observable**: 1.588 B clavados y el bloque de RightY de r8 a r10.

**Deuda que hay que pagar antes de promocionar**: los tres asm de T6 escriben r0, r9, r11 y CR0 **sin declarar un solo clobber**. Con GCC 2.95 eso no es estilo: si el asignador pone un pseudo vivo en r9/r11 ahi, el codigo sale mal y el objeto puede seguir casando por casualidad mientras el DOL no. Al declarar los clobbers hay que remedir; si las 9 filas se mueven, la medida de T6 estaba apoyada en la omision.

### 5. `GenerateRoadNoise` — zEAXSound2, 1240 B, coste BAJO

**Causa: es de DATOS y estaba tapada por la herramienta.** Las «16 filas» que r50/r55..r58 citan salen de `fndiff.py`, que pasa `-c function_reloc_diffs=none`; `fncmp` sobre la misma funcion reporta **«20 insn, 73 REUB»**. Las 73 son los `@ha/@l` contra ranuras de pool permutadas. Nadie estaba mirando el defecto porque la herramienta lo tapaba (`memory/nfsmw-comparador-reubicaciones.md`).

**El defecto medido** (volcando las dos `.rodata` con `powerpc-eabi-objdump -s -j .rodata`; el objetivo es el EXTRAIDO y no cambia, nuestro lado es PROVISIONAL):
```
NUESTRO  0x08f8: 3c23d70a 3dcccccd 3e19999a   (0.01f, 0.1f, 0.15f)
OBJETIVO 0x0a08: 3c23d70a 3e19999a 3dcccccd   (0.01f, 0.15f, 0.1f)
```

**Palanca**: la SEMILLA DE POOL de la r59 (`docs/analisis/r59-main.md:33-58`), que nace **nueve rondas despues** de la veda. La r50 barrio VEINTE posiciones de la declaracion, pero cada una movia a la vez el pool, la def y el bloque: 16 filas con el pool mal, 55 con el pool bien. La semilla los desacopla.

**Receta — CON LAS LINEAS CORREGIDAS HOY** (el fichero crecio ~22 lineas desde que se escribio el encargo): insertar entre `CARSFX_Roadnoise.cpp:405` (`ftemp = bLength(...m_bvTotalRightWheelSlip) * 0.01f;`) y `:417` (`float slipBoost = 0.1f;`):
```c
if (0) { ftemp = 0.15f; }
```
**Observable #1, binario y sin porcentajes**: la linea `08f0` de nuestro `.rodata` tiene que pasar de `... 3c23d70a 3dcccccd` a `... 3c23d70a 3e19999a`, con `3dcccccd` desplazado a `0900`. Si no ocurre, la semilla no llego a `force_const_mem`: revertir.
**#2**: `size=1240/1240`. **#3**: `fncmp` baja de `20 insn, 73 reub`. **#4**: la unidad sigue en 1 de 930 — `$LC571` y `$LC573` tienen 8 menciones y solo 6 dentro de esta funcion, o sea que **otra funcion comparte esas entradas**.

**Dos correcciones al encargo, leidas hoy en el fuente**: (a) el bloque r60 de `CARSFX_Roadnoise.cpp:380-393` **ya nombra la semilla como la palanca que falta** y ya midio dos negativos que importan: `float clampSlip = 0.15f;` local delante del if da **OBJETO IDENTICO** («el front-end lo pliega, no da un punto de DEF nuevo») — justo por eso hace falta el `if (0)` y no un local. (b) ese mismo bloque documenta **otra** transposicion: `$LC566 <-> lbl_803D9A40` y `$LC567 <-> lbl_803D9A3C`, «566 y 567 estan INTERCAMBIADOS». Puede que haya **dos pares** transpuestos, no uno; hay que reconciliarlo antes de dar EXP-A por completo.

### 6. `InitAtSegment` — zWorld2, 816 B, coste BAJO

**Causa, literal** (`WRoadNetwork.cpp:2746-2761`, r49): «pseudo 176 = el VALOR de fNodes 5 refs / 31 insns -> 3225; pseudo 212 = el `(high $LC472)` del 0,0f 2 refs / 8 insns -> 2500. Gana 176 ... y se lleva r11. El objetivo reparte 212 primero. Para invertirlo hace falta UNA de estas dos: live(212) <= 6 ... live(176) >= 41».

La veda enumera dos salidas y **las dos son la misma idea: invertir la prioridad**. Hay una tercera: no invertir la prioridad, sino **quitarle r11 al ganador**. Si 176 se reparte primero y `find_free_reg` recorre r9, r11, r10 con r9 ocupado, basta con que r11 tambien lo este para que 176 caiga en r10 y le deje r11 a 212 — el reparto del objetivo. `rs6000.h:3163` lista `{"r10",10},{"r11",11}` en `ADDITIONAL_REGISTER_NAMES` y `rs6000.h:932` confirma el orden r9,r11,r10.

**En los tres bloques de comentario de esta funcion (:2657, :2716, :2798) NO HAY UNA SOLA MEDIDA de un clobber de r10 ni de r11**, que son justo los dos registros que se pelean. Lo barrido es r23/r24/r26/r27/r28 — el racimo de preservados, otra cosa.

**Receta**: `__asm__("" : : : "r11");` entre `WRoadNetwork.cpp:2791` y `:2792`. Cuatro variantes de posicion, una compilacion cada una (la cuarta es la simetrica, `"r10"` en :2794-2795).
**Observable primario**: la fila 87 de `lwz r11,_12WRoadNetwork.fNodes@l(r9)` a `lwz r10,...` y la 88 de `lis r10,$LC596@ha` a `lis r11,...`. **Control: 816/816**; si sube, el clobber fuerza spill.
**Observable de causa**: en el `.lreg`, 176 en r10 y 212 en r11. Si los dos acaban en r12/r8, los rangos SI se solapan y la r36d tenia razon.

**Nota de precision que hay que dejar escrita**: `brief-r53.md:65` refuta `asm("" : : : "rN")` — pero lo refuta **como palanca de PRIORIDAD** (subir `n_refs`), medido 14→89 filas. Aqui se usa como palanca de **OCUPACION** (`regs_live_at`), que es el mecanismo que describe `memory/nfsmw-barrera-selectiva.md:158-161`. No se contradicen. Que la proxima ronda no lea la refutacion de r53 como si cubriera esto.
**Aviso**: verificado hoy que `InitAtSegment` **no tiene ningun bloque `asm()`** donde colgar una entrada `"r"(x)` — solo `register int guard asm("r28");` en `WRoadNetwork.cpp:2810`. La palanca r53 no esta disponible aqui sin crear un asm.

### 7. `VP6_PredictFilteredBlock` — vp6/criticalpath, 740 B, coste BAJO

**Causa, literal** (`docs/analisis/r49-resto.md:176-198`): «EL MURO, demostrado — no hay forma de fuente ... Para eso su `INSN_PRIORITY` tendria que ser mayor que 2. Y no puede serlo». Y su propia grieta, dos parrafos despues (:190-193): «sus ModX/ModY tienen que tener un dependiente DENTRO del bloque que nosotros no tenemos ... **Lo primero NO LO HE SABIDO EXPRESAR**».

**Un uso a coste cero SI existe y es posterior**: `brief-r53.md:65-69`, «anadir la variable como entrada `"r"(x)` a un `asm` que YA existe ... sin gastar ranura». Un operando de entrada es una dependencia real. Y la gratuidad esta medida en el propio bloque de r48: `criticalpath.c:1309`, «`rec` con `pbi` de entrada 25 (identico)». Lo que r48 midio negativo son asm **INDEPENDIENTES** de solo entrada (744 B, `criticalpath.c:1302-1304`), que gastan ranura. La combinacion no la mide ni r48 ni r49.

**PASO 0 obligatorio, verificado hoy**: el arbol **no tiene** L1/L2 — `criticalpath.c:1355` es `TempBuffer = SrcPtr + pbi->mbi.Recon + ...` a pelo, y las declaraciones `int mx; int my;` estan en :1343-1344. Hay que reaplicar la base de r48 (`criticalpath.c:1273-1283`) y confirmar **25 filas / 740 B** antes de nada.

**Receta**: sobre L2, sin crear ningun asm nuevo:
```c
{ int rec = pbi->mbi.Recon;
  __asm__("" : "+r"(rec) : "r"(ModX), "r"(ModY));
  TempBuffer = SrcPtr + rec + pbi->mbi.FrameReconStride * my + mx; }
```
**Observable, en este orden y ninguno es el fuzzy**: (a) **740 B**; 744 = gasto ranura, 736 = se perdio una insn, las dos muertas. (b) **El que decide**: `schedtrace.py criticalpath VP6_PredictFilteredBlock -dS -fsched-verbose-5`, columna `prio` de las insns 129 (ModX) y 134 (ModY). En la base valen `prio 2` (`r49-resto.md:185-186`). **La hipotesis es buena si y solo si suben a 3 o mas.** Si siguen en 2, el operando de entrada no crea arco en haifa y **la veda pasa a FIRME sin mas discusion**. (c) posicion: los dos `and` a las ranuras 8 y 16 de 27.

### 8. `Update__19TrackCarCameraMoverf` — zCamera, 992 B, coste MEDIO

La veda es de dos piezas y **solo una es tecnica**. La de prioridad — «no se aplica nada: 4 filas son 0 B» (`r53-cam.md:209`) — se escribio cuando zCamera tenia 5 funciones abiertas; hoy quedan 3 y `linkdelta.py zCamera` da `.text +0 resto IGUAL`. El precedente de que ese argumento caduca esta **en esta misma unidad**: `ICEReplay.cpp:198-199`, «ANDAMIO r60 (medido en la r50, aplicado aqui por primera vez porque la meta paso de `matched` a `linked`)».

La tecnica — «hace falta una palanca que actue ENTRE `local_alloc` y `sched2`» (`r53-cam.md:206-209`) — **describe exactamente el PIN DE REGISTRO**, y el pin no esta en ninguno de los dos barridos: los 36 de r51 y los 24 de r53 son ordenes de declaracion y de almacenamiento, o sea formas de fuente, que no llegan al asignador. La exclusividad que r53 declara imposible deja de serlo al separar las dos decisiones: el ORDEN con las sentencias (`zxy`), los REGISTROS con el pin.

**Receta**: PASO 1 reproducir r51+r52 (fantasmas fr0/fr13 + CamCopy decl `xyz`/store `xzy`) → **4 filas [107,109,137,141]**, 992/992. Si no da exactamente esas cuatro, el arbol cambio y hay que re-medir. PASO 2 (una compilacion): store `zxy` + pin de los tres a fr12/fr0/fr13.
**Observable**: filas 107 y 109 desaparecen, emitiendo `stfs f12,0x98(r31)` / `stfs f0,0x90(r31)` / `stfs f13,0x94(r31)` en ese orden. **992 B**; si sube, el pin obligo a una copia.
**Si el pin empeora**: no insistir. Las 2 filas del `addi r3,r1,0x38` (137/141) las causa **el propio andamio** (`r51-cam.md §2.4`) — hay que RE-MEDIR quitando piezas, no anadiendo. Es el caso CullParts.

### 9. `GetLoadingPriority` — zTrack, 708 B, coste MEDIO

**La medida que funda la veda es invalida.** r36f rechaza la forma (a) por 32 filas contra 28 y **en la misma frase reconoce que «DA EL ORDEN DEL OBJETIVO»**. Descarto el arbol correcto para conservar 4 filas de un desplazamiento de registro — `memory/nfsmw-medidas-que-enganan.md`, y lo que ya dice el comentario r46 que sigue en el fuente: «El porcentaje sube con el arbol equivocado».

**Receta, PASO 1 — con las lineas corregidas hoy** (`TrackStreamer.cpp:2207-2208`, no 2211):
```c
float angle_factor = (90.0f - bClamp(angle, 20.0f, 90.0f)) * 0.014285714f;
float adjusted_distance = distance * (1.0f - angle_factor * speed_factor * 0.66999996f);
```
**Observable, y NO es el porcentaje**: la fila 146 `fmuls f30,f30,f11` **tiene que DESAPARECER** y el racimo 151-156 quedar instruccion por instruccion como el objetivo. **El numero de filas puede empeorar y eso no invalida el paso** — es exactamente lo que midio r36f (28→32) y por lo que se descarto mal. Si el `fmuls` de la 146 no desaparece, entonces si es veda firme.

**PASO 2, el premio gordo (~40 filas)**: la veda de r52 solo cierra el orden de declaracion. La causa real, «la sentencia que crea un temporal de 16 B», **nunca se ha probado ni una vez**. `FloatVector` es POD sin constructor (`Espresso.hpp:8-12`), asi que un temporal de 12 B solo sale de un inicializador agregado, una expresion de tipo clase sin ligar, o una llamada que devuelve struct — y la tercera esta descartada por el DWARF. Candidato 1: `FloatVector pos = { section->Centre.x, section->Centre.y, 0.0f };` en vez de las tres asignaciones.
**Observable binario**: `stwu r1, -0x110(r1)` a **-0x120**. Oraculo calibrado por r52 con dos controles: **16 B mas = 75 filas → 32**. PROHIBIDO rellenar con una local inventada (dos precedentes: ICEMover, y `pos2` revertido en 9de121ee).

### 10. `DefragmentPool` — zWorld, 684 B, coste MEDIO

El informe que cerro la ronda **escribe el ataque y no lo ejecuta** (`r52-world.md:288-290`): «ahora que sabemos que ese `live` lo fija sched1, el ataque correcto es el horario del bloque, no la fuente». Los 14 clobbers de r52 no son ese ensayo: se pusieron en dos posiciones **fuera** de los rangos que hay que mover (antes del bucle) y los catorce llevan clobber de un registro concreto, que es la palanca refutada en `brief-r53.md:65`. La funcion **no tiene hoy ningun asm** (`grep -n asm CarLoader.cpp` solo da comentarios).

**Receta**: `__asm__("");` pelado (r51, corte de region) **bisecado dentro del cuerpo del bucle**, `CarLoader.cpp:2110-2185`, cuatro posiciones.
**Criterio de descarte barato — esto es lo que hace el ensayo de minutos**: si una posicion deja las siete columnas `live` del `.greg` identicas a la base (`CarLoader.cpp:2038-2046`), se tira **sin mirar filas ni porcentaje**.
**Umbral**: el ciclo A se invierte si `live(222) <= 47` o `live(82) >= 173`. **Ojo: el comentario del fuente en `CarLoader.cpp:2050` dice 163 y esta RANCIO** — la correccion es de la r50 (`r50-world.md:262-265`).

### 11-13. `IdctColumn` (632) e `IdctRow` (516) — madidct, coste BAJO/MEDIO

Las dos van juntas y las dos son CADUCAS, pero **de bytes no cobrables a corto plazo**: la unidad esta al 48-50 %.

`IdctColumn` es la mas clara de todo el informe: la causa esta identificada al byte (`madidct.cpp:155-161`, «los dos guardamos src[4] en 0x8(r1), pero el objetivo lo sigue teniendo en r5 ... nosotros hacemos `lwz r11,0x8(r1)`») y **contra ella no se ha disparado ni una palanca**. Todo lo apuntado como negativo son (a) variantes de la forma de MULT que rompen el tamano exacto (612 B), y (b) la frase «IdctColumn no se mueve en ninguno de los ensayos» — pero esos ensayos estan **dentro del cuerpo de IdctRow**. Que una funcion no se mueva cuando editas otra no es un negativo medido.
**Receta**: `int s4 = src[4];` tras `madidct.cpp:23`, sustituyendo las tres apariciones (linea 25 la cadena de `or`, :54 y :57). **Observable**: el `lwz r11, 0x8(r1)` de nuestro indice 16 desaparece; histograma `lwz` 20→18 y `mr` 10→12; **632 B clavados**.

`IdctRow`: caduca solo el RESIDUO. La parte estructural esta resuelta (con la receta, 516/516 B y 129 insn contra 129, `mtctr` en la misma ranura). Lo que sobrevive es permutacion + `mfctr` 2/1. Palanca: r53, entrada `"r"(tN)` en **los dos asm que ya existen** (18 ensayos, cero bytes). **Observable**: 516 B y 129 insn clavados; lo que tiene que moverse es `mfctr` de 1 a 2. Un ensayo que baje filas sin tocar `mfctr` es ruido.

### 14-17. La cola (coste bajo, bytes pequenos)

- **`EvalState` (zEagl4Anim, 456 B)** — CADUCA **por combinacion**, no por familia: r60 barrio pin, fantasma y barrera **por separado** y cada una es firme. Lo que falta es el pin sobre **una local que todavia no existe**. Los dos pines de r60 son sobre `k` e `i`, y `k` **tiene la direccion tomada** (`RawStateChan.cpp:237`) — regla 1 del pin: se ignora en silencio. Se pincho justo la variable que no se puede pinchar. **La receta ya casa al 100 % en la funcion hermana del mismo fichero**: `FindTime`, verificado hoy en `src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp:263-266` — `k = c->GetKeyData(0); register int keyOffset asm("r9") = i * c->GetKeySize(); k += keyOffset;`. Aplicarlo en :215 y :235 (verificados: los dos son `k = c->GetKeyData(i);`). **Observable**: filas 39 y 78 de `add r4,r30,r11` a `mr r4,r11`; fila 19 de `mr r12,r0` a `mr r3,r0` (r12 es el ULTIMO GPR de REG_ALLOC_ORDER: mientras salga r12, r11 sigue sin liberarse); **456 B clavados** — todas las formas que sacan la base del if bajan a 404-424 B y son falsos positivos. **CARRERA: el fichero tiene +22 lineas sin commitear ahora mismo.**
- **`SimThread_Init` (steering, 324 B)** — causa de PLANIFICACION pura, y la unica palanca de MWCC que la ataca es `#pragma scheduling`. Se midio **solo en global** y por eso se descarto; **nunca acotado a la funcion**. El acotado por funcion esta demostrado en este fichero — verificado hoy: `steering.c:649/670` (`opt_common_subs off/on`) y `steering.c:955/1007` (`optimization_level 2/4`). Y r22 barrio 28 pragmas sobre Effect_Init y 22 sobre HandleTriggers pero **ninguno sobre SimThread_Init**. Receta: `#pragma scheduling off` en :1343 y `on` en :1382. **Observable**: filas 51-55 colapsan (`li r3,0`+`mtctr r0` delante del `lfs f1,@sda21`). **324/324**; y `fncmp` tiene que seguir dando 30 exactas o el pragma se derramo.
- **`eProject` (zEcstasy, 268 B)** — CADUCA por **BASE ENVENENADA**, y lo dice la propia r49: `eMathE.cpp:196-206`, «el asm es NUESTRO ... **QUIEN VUELVA A ESTA FUNCION DEBE PARTIR DE QUE LA FORMA CORRECTA NO LO LLEVA**». Nadie volvio; verificado hoy que sigue puesto (`eMathE.cpp:241`). Prueba dura de que la base se movio: el **precio del andamio derivo** — r49 lo midio en 4 filas, `r53-ecs.md:209` en 9. Receta: **borrar `eMathE.cpp:241` y medir**. Si sale un tercer numero distinto de 17 y de 23, ninguna veda de esta funcion es reutilizable. Luego r51 (`__asm__("")` sin operandos entre :240 y :243) — nunca probado aqui.
- **`Effect_Init` (steering, 276 B)** y **`CookValues` (steering, 260 B)** — las dos caducan porque **hay dos mitades medidas por separado que nadie combino**. Effect_Init: `effect_clear` (95,0 %) y `effect_seed` (91,14 %) de iter4, y los 20 ensayos de r51 se midieron sobre la base de 88,62 %, no sobre `effect_clear`. Ningun informe reporta el observable que decide: **si `effect_seed` produjo o no `stmw r26`**. CookValues: la forma mejor medida (`cook_lo`, 99,08 %, 9 filas) **no esta en el arbol** — hoy vamos a 95,77 %/13 filas, y la veda se escribio contra la base mala. Y iter5 se declara incompleta y nombra lo que no probo: «una dependencia causal distinta que controle SIMULTANEAMENTE `_bits`, `_v` y `_mn`». Es la CADENA DE N DEPENDENCIAS (r37/r46), nunca transpuesta a MWCC. CookValues es la de menor confianza de las 17: **6 negativos con la palanca buena**.

---

## Las FAMILIAS

Esto es lo que cambia el orden de ataque. **Si una familia cae, caen todas a la vez.**

### Familia A — «cuantos pseudos `@ha` sobreviven al pool» (`qty_const` de cse2)
**Miembros**: `RenderFlaresOnCar` (2908 B, FIRME, `CarRender.cpp:2577-2580` — verificado hoy), `UpdateWheelYRenderOffset` (876 B, FIRME, `CarRender.cpp:2001-2004` — verificado hoy), `UpdateLoaded` (856 B, DUDOSA, `SuspensionTraffic.cpp:270-272` — verificado hoy). **4.640 B.**
**Mecanica**: el objetivo mantiene un `(high <literal>)` vivo en un GPR preservado a lo largo de un bucle o de una llamada, y nosotros lo rematerializamos — o al reves. Las tres tienen la misma firma y las tres estan cerradas contra `cse2`.
**Miembro mas barato para probarla, y NO es ninguna de las tres**: `ActualReadJoystickData` de zPlatform. El censo de r54 (`r54-loaded.md §6`) conto **199 casos** de `lis rPRESERVADO, lbl_*@ha` con >=2 usos en el original, 41 con etiqueta de union entre los usos y **solo 4 sin bucle**; de esos cuatro, `ActualReadJoystickData` es **el unico con CERO etiquetas de union**, o sea el unico donde el paso esta al alcance de cse1 sin pelear nada. **Observable**: una sola linea `COPY-PROP: Replacing reg N in insn M with reg <el pseudo del high>` en el volcado `-dG`. r54 midio **CERO** lineas COPY-PROP en todas sus variantes; una sola significa que la familia esta viva.
**Si ese grep sale vacio ahi, el frente del `lis` preservado esta cerrado por construccion en todo el proyecto y las tres pasan a FIRME por herencia.** Es la prueba mas barata por byte de informacion del informe y **ninguna ronda la ha hecho**.

### Familia B — «el empate de `QTY_CMP_PRI` / `allocno_compare`»
**Miembros**: `__static_init` (3604 B, los tres `n_refs=2` empatan), `HolePunchAvoidables` (2980 B, 60,97 y 60,72 truncan al mismo 60), `ActualReadJoystickData` G1 (1588 B, 156,25 y 156,74 truncan al mismo 156), `InitAtSegment` (816 B, 3225 contra 2500), `DefragmentPool` (684 B, tres ciclos). **9.672 B — la familia mas gorda del proyecto.**
**Mecanica, una sola formula**: `pri = floor_log2(n_refs)*n_refs/live_length`. Dos entradas y **solo dos**: subir `n_refs` (familia C) o mover `live` (r51 corte de region / r54 posicion de declaracion). Cuatro de los cinco tienen el umbral escrito con numero.
**Miembro mas barato**: `DefragmentPool`, porque es el unico con **criterio de descarte sin compilar el diff** — si el `.greg` no mueve una columna `live`, se tira. Los siete valores base estan escritos en `CarLoader.cpp:2038-2046`.
**El de mayor palanca**: `ActualReadJoystickData`, donde r48 ya demostro que **el empate se rompe haciendo crecer la funcion UNA instruccion** (`r48-plat.md:120-145`: base 1.580 mal, un nop 1.584 bien, dos nops 1.588 bien). Eso es una prueba de existencia para toda la familia: **el empate es de ventana estrecha y una sola insn lo voltea**.

### Familia C — «la palanca de enteros de la r53: entrada `"r"(x)` en un asm que YA existe»
**Miembros**: `__static_init` (3604), `IdctRow` (516), `VP6_PredictFilteredBlock` (740, aqui contra `INSN_PRIORITY` en vez de contra `n_refs`), `ActualReadJoystickData` paso 3 (1588), `GetLoadingPriority` paso 3 (708), `ICEMover` paso 4 (3868, DUDOSA). **11.024 B.**
**Mecanica**: `brief-r53.md:66-72`. Es **posterior a casi todas las vedas del dossier** y refuta la forma de la r52. La memoria lo confirma: hasta la r52 el catalogo solo tenia palancas de coma flotante. Todos los barridos de reparto entero anteriores a la r53 se hicieron **sin ella**.
**Miembro mas barato**: `IdctRow` — tras el PASO 0 ya hay **dos asm** donde colgar entradas por cero bytes, y son 18 ensayos sueltos con observable de histograma (`mfctr` 1→2).
**El que decide el mecanismo**: `VP6`, porque su observable (`prio` 2→3 en la traza) contesta si un operando de entrada crea arco de dependencia en haifa — un dato que ninguna otra funcion del dossier mide.
**Aviso de coste**: en `ICEMover` **no hay ningun asm previo** en la ventana (el camino es entero, `bMath.hpp:105-131`), asi que habria que crear uno y eso gasta ranura.

### Familia D — «la base envenenada: el andamio que tapa la causa»
**Miembros**: `eProject` (268 B, `eMathE.cpp:241` — verificado presente), `UpdatePlatInfo` (2044 B, `eLightE.cpp:415` — verificado presente), `ActualReadJoystickData` (1588 B, los tres asm de T6 con registros cableados), `HolePunchAvoidables` (2980 B, el pin `fr11` de `WRoadNetwork.cpp:1968` — verificado presente, y la barrera de :2064), `Update__19TrackCar` (992 B, las 2 filas del `addi` las causa el andamio). **7.872 B.**
**Mecanica**: no es una palanca, es su reverso (`r49`, «un asm de deuda puede TAPAR la causa real» + `memory/nfsmw-andamios-caducan.md`). En las cinco, **todos los negativos del historial se midieron con el andamio puesto** — ~90 medidas en UpdatePlatInfo, 33 en eProject, ~15 en HolePunchAvoidables, 60 en TrackCar. Ninguna es una medida de la base correcta.
**Miembro mas barato, y es el que hay que correr PRIMERO de todo el informe**: `eProject`. Una compilacion, borrar una linea, banco de 1,4 s. **El numero que salga decide si la familia es real**: r49 dijo 17 filas sin el andamio, r53 dijo 23. **Si sale un tercer numero, ninguna veda de ninguna de las cinco es reutilizable** y hay que rehacer los barridos desde la base limpia. Es el ensayo de mayor informacion por segundo del proyecto.
**Precedente que cierra el argumento**: CullParts cerro al 100 % **quitando** un asm que tres rondas habian validado.

### Familia E — «la unidad de Metrowerks»
**Miembros**: las seis de `src/LibSN/steering.c` — `SimThread_Step` (924), `HandleTriggers` (588), `SimThread_Init` (324), `Effect_Init` (276), `CookValues` (260), `Effect_PerformEnvelope` (196). **2.568 B, 6 de las 28 funciones abiertas del proyecto en un solo `.c`.**
**Mecanica**: mwcceppc GC/2.7 coalesce copias antes del reparto y no responde a ninguna palanca de GCC. Cinco de las seis son «reparto» y la sexta es «planificacion». Lo que SI esta probado al 100 % en este fichero: la **ranura explicita** (union de 8 B + productores `asm { }`) en `VDevice_RecalcGammaTable` — verificado hoy, `steering.c:798-808`, `asm { fsubs value, number, bias }` — y el **pragma acotado por funcion**, verificado hoy en `steering.c:649/670` y `:955/1007`.
**Miembro mas barato, y ademas responde por la familia**: `SimThread_Init`, un `#pragma scheduling off/on` acotado. Si el pragma acotado funciona ahi, se abre el eje para las otras cinco; si se derrama y rompe a las 30 vecinas exactas, el eje del pragma acotado queda cerrado para la unidad entera con una sola compilacion.
**Nota de orden**: `Effect_PerformEnvelope` es FIRME y `HandleTriggers` es la mas cercana a FIRME. **Effect_Init y CookValues comparten un mecanismo entre ellas**: las dos tienen una mitad medida en `iter4`/`iter5` que no esta en el arbol y contra la que nadie ha vuelto a medir. Recuperar `scratchpad/codex_20260908_iter4_steering` y `codex_20260908_iter5_*` es un solo paso que sirve a las dos.

### Familia F — «el orden del pool de flotantes, desacoplado del reparto» (semilla r59)
**Miembros**: `GenerateRoadNoise` (1240 B, dos palabras transpuestas medidas en `.rodata`), `UpdatePlatInfo` paso 3 (2044 B — r36f midio 27 filas moviendo `envmap_power` dentro del case y **lo rechazo porque «REORDENA EL POOL»**, `eLightE.cpp:409-412`; la semilla devuelve el pool a su sitio por cero bytes), `ActualReadJoystickData` (5 de las 11 filas de coste de X2 son «rotacion de las tres constantes flotantes izadas»). **4.872 B.**
**Mecanica**: `r59-main.md:33-58` — «Los dos ordenes estan DESACOPLADOS en el original, y desde las declaraciones no se puede: reordenarlas arregla el pool y rompe la funcion». Eso describe el atolladero de r50 en Roadnoise palabra por palabra (16 filas con el pool mal, 55 con el pool bien) y el rechazo de r36f en UpdatePlatInfo.
**Miembro mas barato**: `GenerateRoadNoise` EXP-A — una linea, **y el observable es un `objdump -s -j .rodata`, no un porcentaje**. Ademas es la unica del informe cuyo defecto es de DATOS: nuestra `.rodata` de zEAXSound2 tiene hoy dos palabras en el sitio equivocado, `linkdelta` dice «IGUAL» porque los tamanos cuadran, y `trypromo` sigue en DOL ROTO.

### Orden de ataque que sale de las familias

1. **`eProject`** (familia D) — 1 compilacion. Decide si 7.872 B de historial son reutilizables.
2. **`ActualReadJoystickData` E3 / el grep de COPY-PROP** (familia A) — decide 4.640 B de FIRMES/DUDOSAS de golpe. Hacerlo sobre arnes de mini-TU propio, **nunca sobre `build/GOWE69/src`**.
3. **`GenerateRoadNoise` EXP-A** (familia F) — 1 linea, observable binario, arregla `.rodata`.
4. **`DefragmentPool`** (familia B) — descarte sin compilar el diff.
5. **`SimThread_Init`** (familia E) — 1 pragma, responde por seis funciones.
6. **`VP6` PASO 1** (familia C) — decide si el operando de entrada llega a haifa.
7. Lo demas por bytes: `__static_init`, `HolePunchAvoidables`, `UpdatePlatInfo`.

---

## Las FIRMES, y por que

Hay que distinguir **estructural** (la maquina no puede producir el estado del objetivo desde ninguna fuente) de **agotada** (todas las palancas conocidas medidas y negativas — reabrible si nace una palanca nueva).

### Estructurales de verdad

**`Initialize__Q25EAGL413DynamicLoader` (2352 B, zEagl4Anim) — ESTRUCTURAL.** Es el caso de libro. La instruccion de mas es el `b` de `emit_jump_if_reachable` que sigue al `do_jump_if_equal` de un nodo de VALOR UNICO. Para que `{2}` salga como RANGO, `stmt.c:5875` exige un antecesor con `low == high+1`, y los unicos antecesores posibles de `{2}` en un arbol construido por `balance_case_nodes` con esta lista son `{3}` y `{8}`: `high=2` lo convierte en valor unico, `high=7` solapa con `{3}` y con `[4,7]`. Anadir nodos no ayuda porque el hijo derecho de `{3}` tiene que ser `[4,7]` **solo** para que `node_is_bounded` le de el `bgt` directo. Probado por medida ademas de por construccion: 622 medidas en r36e, 39 en r46, 60 en r47 y **161 combinaciones en r60**, ninguna por debajo de 28 filas / 2.356 B, y las que restauran el pivote `{8}` vuelven a **28 exactas**.
**Consecuencia dura**: los 4 B de diferencia son enteros de esa instruccion, asi que las otras 14 filas — que **si tienen receta viva** en `r47-ecs.md:213` — no pueden promocionar nada mientras el switch no caiga. `matched_code` es todo-o-nada.

**`UpdateWheelYRenderOffset` (876 B, zWorld) — ESTRUCTURAL.** La condicion de victoria esta escrita con numero (`r54-flares.md §5`): «CODE_LABELs entre insn 932 y insn 974 >= 1. Hoy son 0». Es inalcanzable por construccion: `gcse` inserta con `insert_insn_end_bb` al final del bloque que precede al bucle y `loop` inserta justo antes de `NOTE_INSN_LOOP_BEG`, que es el final de **ese mismo bloque** — no existe posicion de fuente entre las dos. Medido, no supuesto: `w_label` mete la etiqueta, la etiqueta existe en la RTL, cae antes de la insercion de gcse, y sale la base exacta. Segunda via cerrada en el fuente del compilador: `make_regs_eqv` (`orig/prodg/NGC_GNU_SRC/NGC/gcc/cse.c:997-1012`) exige que el ultimo uso del pseudo nuevo sea posterior al del viejo, y el unico uso del nuevo es el `lo_sum` contiguo.
Y no se veto «probando N formas»: se veto con la **metrica del mecanismo** (`hicount.py`, cuantos `(high 0.0f)` sobreviven a cada pase), con control positivo (`w_neg2` saca 876/876 al byte, o sea que la metrica es sensible). `r54-flares.md §6`: «No eran formas de la sentencia equivocada: es que ninguna forma de la sentencia puede tocar `qty_const`».

### Agotadas, no estructurales — reabribles si nace una palanca

**`Effect_PerformEnvelope` (196 B, steering) — AGOTADA, confianza alta.** Cumple el criterio duro: la palanca correcta (`asm { }` nativo de MWCC, nacida el 2026-09-08 y probada al 100 % en este mismo fichero) sobre el observable correcto (la aparicion del `mr r5,r9` y el salto 192→196 B), negativa, y **posterior** a las vedas de r22/r47/r48. Tres vias medidas: 10 formas de C en r22 (incluida h9, la que el propio objetivo sugiere), 14 mas en r51, y `asm { mr out, mag }` con ambos operandos `register` — MWCC coalesce **tambien esa copia ASM**. Reconfirmado con registro fisico en iter5 (`hard6`).
**La unica puerta sin medir**: que `out` **nazca de un productor independiente** en vez de una copia. Antes de gastar un ensayo hay que explicar por que no cae en h1/h2 de r22, que midieron eso mismo en 204 B y 85,92 %.

**`RenderFlaresOnCar` (2908 B, zWorld) — AGOTADA, y es la mas grande del bloque FIRME.** El pase esta identificado con control de bandera (`-frerun-cse-after-loop` mueve la cuenta de `(high 0.0f)` de 16 a 1 y es la unica de doce que la mueve, pero la bandera es obligatoria: quitarla empeora 30 funciones). El mecanismo es `qty_const` y la condicion de `make_regs_eqv` no puede cumplirse, igual que en su hermana.
**Lo unico vivo es una PREGUNTA de diagnostico, no una palanca** (`r54-flares.md §9`): por que `pre_delete` de `gcse.c` deja UNA ocurrencia sin tocar en el objetivo y las tres en nosotros. Se contesta leyendo las tablas en el volcado `.gcse` (ya trae `22 substs, 13 insns created`) y **los volcados por pase estan guardados en `scratchpad/r54_flares/rfc_*.txt`**, o sea que no hay que recompilar. Ese hilo lo comparte con `UpdateWheelYRenderOffset`: **una respuesta sirve a 3.784 B**.
Segundo hilo, mas debil y **yo no lo abriria**: la rama de r46 que da el tamano exacto (2908 B, 62 filas) se abandono despues de UN solo pin, y sobre ella nunca se probaron `set_preference` (r47) ni la entrada `"r"(x)` (r53). Pero r49 midio que en esa rama seguimos materializando dos `@ha` en el preencabezado: la estructura no casa y el reparto es sintoma.

---

## Lo que esta auditoria NO puede decidir

Las siete DUDOSAS. En todas hay un argumento a favor y otro en contra que **solo se separan compilando**.

| Funcion | Bytes | Que falta saber | Coste |
|---|---:|---|---|
| `Update__8ICEMoverf` | 3868 | El mapa de lineas del objetivo pone las dos mitades del clamp entero en **dos lineas consecutivas** (`bMath.hpp:232` y `:233`) y nuestro `bClamp(int)` es **una sola sentencia** — verificado hoy, `bMath.hpp:193-195`. Dos lineas consecutivas es la firma de un temporal con nombre = un pseudo mas = la quinta cantidad que r51 dice que falta. Pero los numeros de linea no cambian el codigo por si mismos, y cae en **cabecera compartida**, asi que aunque salga bien **se propone, no se aplica**. Exige sombrear `bMath.hpp` con `-I` (se puede: se incluye con ruta de proyecto entre comillas, `Camera.cpp:4`) **con el control obligatorio de meter un error de sintaxis** y comprobar que no compila. | alto |
| `epCalculate...POS16` | 2072 | La causa declarada (la ranura huerfana del marco) **es FIRME** — medida despues de las tres cesuras, con `.greg` de los dos lados y lectura de `reload1.c`. Pero la propia r54 dice que **esa causa ya no es el bloqueo** (`r54-epcalc.md §6`): «El siguiente paso acotado YA NO ES EL MARCO ... lo que no casa son las ~102 filas, CONCENTRADAS EN 364-453». Y sobre esa region hay una palanca anterior que aqui no se probo nunca porque nacio en otra funcion: `set_preference` (r48) sobre los once `__builtin_ps_mul/madd` de `EcstasyEx.cpp:3587-3619`, cuyo primer operando es libre de conmutar. Peor relacion coste/beneficio del informe: ~102 filas en el mejor caso. | alto |
| `SimThread_Step` | 924 | El mecanismo (MWCC se niega a emitir una copia) se probo con la palanca correcta pero **en la funcion hermana**. Aqui la copia **cruza una llamada**, que es lo que obliga al objetivo a usar un salvado: una copia que sobrevive a una llamada no es coalescible si los dos rangos se solapan. **Nadie ha atacado el SOLAPAMIENTO, solo la copia.** Observable: 920→**924 exactos** con `subf r7,r0,r3` + `mr r31,r7` detras. Cualquier variante que clave 924 sin esa pareja es la trampa medida de q3. | alto |
| `UpdateLoaded` | 856 | **La propia r54 refuta el mecanismo que nombra**: sostiene la veda sobre que `cse_end_of_basic_block` corta en una etiqueta con `LABEL_NUSES==2`, y admite que **el objetivo tiene esos dos mismos `bso`**. Verificado hoy sobre el asm del ORIGINAL (`build/GOWE69/asm/.../zPhysicsBehaviors.s`, `bso .L_8024B4B4` en 8024B488 y 8024B4A8, con el `lis r30` en 8024B464): el original TIENE el `lis` sirviendo a dos usos separados por esa etiqueta. **El pase que produjo el `lis r30` del original sigue sin identificar**, y todos los negativos son contra una cadena que demostrablemente no ocurrio. Ademas r54 publica una palanca positiva en su §5 y **no la aplica a la funcion donde la descubrio**. | medio |
| `Setup__Q26Speech13RoadblockFlow` | 596 | Veda posterior a las tres cesuras, con volcado y citas de `haifa-sched.c:4158`: perfil de FIRME. **Pero tiene un agujero medido hoy**: el objetivo emite **SEIS talones de llamada virtual en esta misma funcion** y cinco son delta-primero (casan) y solo el sexto es pfn-primero; tres de esos cinco tienen la misma forma estructural que el sexto. Si la asimetria fuese del modelo de costes de rs6000, el objetivo seria delta-primero tambien en el sexto. Como el planificador es determinista, **la entrada RTL del original diferia**, luego si existe una diferencia de fuente — aunque no diga cual. Y 28 formas negativas casi todas de la misma sentencia es evidencia DEBIL. Paso 1 barato: repetir el barrido de 15 banderas mirando a Setup y a las 702 vecinas; `-fno-expensive-optimizations` es la candidata (r48 solo midio dos de las quince). | medio |
| `HandleTriggers` | 588 | **La mas cercana a FIRME de las seis de steering.** El unico eje que MWCC escucha esta barrido AL COMPLETO — las 24 permutaciones, con el minimo alcanzado — mas 22 pragmas por funcion desde r22. Eso no es «12 variantes de la sentencia equivocada», es un barrido cerrado del espacio. Lo unico que la salva: r22 §6 deja escrito «No he probado restricciones de registro», y su equivalente en MWCC (`asm { }` con registro fisico) nunca se apunto aqui. En contra, y pesa: iter5 probo exactamente eso en esta unidad (`hard4`, `hard6`) y MWCC o lo fundio o roto el resto del grafo. **La ultima de las seis.** | alto |
| `SetMemoryPoolSize` | 304 | La palanca posterior (r51, corte de region) **se probo aqui pero en UNA sola posicion**, «detras de la asignacion», que empuja en la direccion contraria. La simetrica — `const char *nm = "Cars"; __asm__("");` **antes** de `CarLoader.cpp:628` — no aparece en ninguna de las 25 variantes. Lo que impide llamarla CADUCA: las dos mitades por separado ya costaron 4 B cada una. **Criterio de descarte**: 308 B = se tira sin mirar filas, y **la funcion pasa a FIRME**. Probabilidad baja; una compilacion la cierra en un sentido o en el otro. | bajo |

---

## Correcciones al encargo, medidas hoy en el arbol

Las recetas del dossier traen lineas rancias. Antes de gastar una compilacion:

1. **`CARSFX_Roadnoise.cpp`**: la receta dice «entre la 394 y la 395» y «detras de la 383». **Real hoy**: `ftemp = bLength(...m_bvTotalRightWheelSlip) * 0.01f;` esta en **:405** y `float slipBoost = 0.1f;` en **:417** (el bloque r60 de :380-393 corrio el fichero). Ademas ese bloque **ya nombra la semilla de pool** y **ya midio que un `float clampSlip = 0.15f;` local da OBJETO IDENTICO** — el front-end lo pliega. Y documenta **otra** transposicion ($LC566/$LC567) distinta de la que mide el encargo.
2. **`RawStateChan.cpp`** no esta en `src/Speed/Indep/Src/Animation/` sino en **`src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp`**. El patron de `FindTime` esta verbatim en **:263-266** y las dos lineas a cambiar (**:215** y **:235**) son las dos `k = c->GetKeyData(i);`. **El fichero tiene +22 lineas sin commitear ahora mismo.**
3. **`TrackStreamer.cpp`**: la receta dice «linea 2211». **Real**: `angle_factor` en **:2207** y `adjusted_distance` en **:2208**.
4. **`criticalpath.c`**: confirmado que **L1 y L2 de r48 NO estan en el arbol** — :1355 es `TempBuffer = SrcPtr + pbi->mbi.Recon + ...` a pelo y las declaraciones `int mx; int my;` estan en :1343-1344. El PASO 0 es obligatorio y no es formalismo.
5. **`WRoadNetwork.cpp`** esta en `src/Speed/Indep/Src/World/Common/`, no en `.../World/`. Y **`InitAtSegment` no tiene ningun bloque `asm()`**, solo `register int guard asm("r28");` en :2810 — la palanca r53 no esta disponible ahi sin crear uno.
6. Confirmados presentes y sin tocar: `eMathE.cpp:241` (`__asm__("" : "+f"(halfVP2))`), `eLightE.cpp:415` (pin `fr5`), `WRoadNetwork.cpp:1968` (pin `fr11`), y los dos pares de pragma acotado de `steering.c` (:649/:670 y :955/:1007) mas el productor `asm { fsubs }` de :798/:803.
7. **Ficheros ocupados por la otra tanda**: `src/Speed/GameCube/Src/JoyE.cpp` (+55) y `src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp` (+22). Ninguna medida sobre `build/GOWE69/src` de zPlatform o zEagl4Anim vale hasta que commiteen.