# Ronda 28 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes de `.text` ganados: 0.** El árbol queda **byte a byte como lo encontré**:
`git status src/Speed/Indep/Src/Camera/` **vacío**, md5 de `UTLVector.h`
`d2d4c8e6…` y de `TrackCop.cpp` `f7c2a733…` (los mismos que dejó la r27), y
`ICEMath.hpp` / `ICEMover.cpp` / `ICEManager.hpp` intactos. **Todos** los ensayos
se hicieron sobre copias parcheadas en un directorio-sombra del scratchpad con
`-I` delante; el árbol compartido no se tocó ni una vez. `audit.py`: **446/446
ok, cero FALLA**, dos pasadas.

Lo que traigo:

1. **La tarea servida del brief no se puede hacer: los cinco pseudos de
   `TerrainVelocityNoise` NO LLEGAN a `global_alloc`.** `alloc.py` sobre el
   `.greg` no puede decir nada de ellos. Y en su sitio queda el mecanismo
   completo, con el volcado de `sched1` ciclo a ciclo. (§1)
2. **`ICEMover::Update`: las 15 filas salen de UNA sola diferencia de RTL**, y el
   pase que la decide es `cse` (`make_regs_eqv`). **Con un control que la
   invierte a voluntad.** 13 formas de fuente barridas. (§2)
3. **El exceso de `.text` de zCamera medido y clasificado: +12.508 B**, y **su
   causa raíz está medida: CUALQUIER bandera de depuración apaga las secciones
   `.gnu.linkonce.t.*`** en este compilador. Sin `-g` esos 12.508 B se van a
   **174 secciones linkonce**. Comprobado también en zLua, zPhysics y zMain.
   **Y con el matiz que lo desmonta como «así lo hizo el original».** (§4)
4. **`-fno-implement-inlines` en zCamera: −2.608 B de exceso, 0 cambios en el
   código casado, 0 símbolos indefinidos nuevos.** (§4.1)
5. Fidelidad: `ICEShakeTrack` vive en `ICEData.hpp` en el original y en
   `ICEManager.hpp` en el nuestro; el ORDEN de `ICEMath.hpp` no es el del
   original. **Ninguna de las dos cambia el código.** (§5)

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py zCamera --muro`, al empezar y al terminar:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28Camer faltan 1, sobran 2, 1 SUST  lwz-1, mr.+1, cmpwi-1
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1  li+1
    MURO: 3868 ICEMover::Update (15 diffs) / 1192 TerrainVelocityNoise (14) /
          992 TrackCar (15) / 948 TrackCop (2)

`measure.py zCamera`: **113080/125008 B, 90,4582 %, 446 al 100 %**.
Mi arnés (`c28cam_unit.py`, unidad entera contra objdiff, 17-20 s) reproduce la
base de la r27 al dígito: **120941/145125 B, 83,3357 %, 615 fn al 100 % de 627**.

---

## 1. `TerrainVelocityNoise` (1.192 B): la tarea servida no existe, y por qué

### 1.1 Los cinco pseudos no llegan a `global_alloc` — `alloc.py` no aplica

El brief decía: «Queda servido para `scripts/alloc.py` sobre el `.greg` y nadie
lo ha pasado». Lo he pasado. Sale esto:

    ;; pseudos en .lreg: 176 | asignados por LOCAL-alloc: 139 | llegan a GLOBAL-alloc: 37
    ;; 37 regs to allocate: 145 122 126 146 171 180 254 264 279 286 305 87 187 170
       119 181 179 183 283 117 303 84 165 107 291 110 294 93 82 197 92 456 172 163 85 86 83

Los pseudos de las cinco direcciones del pool son **200 (1D88), 227 (1D70),
241 (1D8C), 245 (1D90) y 249 (1D94)** (`.lreg`: «used 2 times … in block 15»).
**Ninguno está en la lista de 37.** Los reparte `local_alloc`, no `global_alloc`,
así que la tabla de `allocno_compare` que modela `alloc.py` **no los ve**.
La r27 apuntó a `global_alloc` sin comprobarlo; **queda corregido**.

### 1.2 El mecanismo verdadero, con el volcado de `sched1` ciclo a ciclo

`c28cam_rtl.py CameraMover.cpp tvn -dS -fsched-verbose-5`, bloque 15 (el cuerpo
de `if (GetVelocityMagnitude() > speed_tresh)`):

    ;;      insn  code  bb  dep  prio  cost  units
    ;;      346   506   0    0     7     1   iu2 : 406      high(1D88 = 20.0f)
    ;;      399   506   0    0     7     1   iu2 : 402      high(1D70 =  0.0f)
    ;;      436   506   0    0     5     1   iu2 : 439      high(1D8C = 0.05f)
    ;;      451   506   0    0     5     1   iu2 : 454      high(1D94 =  0.5f)
    ;;      443   506   0    0     4     1   iu2 : 446      high(1D90 = 0.15f)

Con `issue_rate = 2` el planificador coloca los cinco `elf_high` en la ranura
`iu2` de los ciclos **1, 2, 3, 5 y 7**, en orden de `prio` y, en el empate 7-7,
de `INSN_LUID`:

    ;;  1   381 r214=[r84+0x3c]     |  346 r200=high(1D88)
    ;;  2   382 r215=[r84+0x4c]     |  399 r227=high(1D70)
    ;;  3   376 r209=[r84+0x38]     |  436 r241=high(1D8C)
    ;;  5   387 r220=[r84+0x40]     |  451 r249=high(1D94)
    ;;  7   406 r228=[r200+lo(1D88)]|  443 r245=high(1D90)

**La `prio` de `high(0.0f)` es 7 por construcción**: su cadena es
`high → lfs → fsel#1 (bMax con 0) → fsubs → fsel#2 (bMin con 20) → fmuls →
fmadds`, la más larga de las cinco, porque el `0.0f` alimenta **el primer**
`fsel` y el `20.0f` el segundo. Y esa cadena es una dependencia de datos:
`bClamp(x, 0.0f, MAX) = bMin(MAX, bMax(x, 0.0f))` obliga al máximo antes del
mínimo. **Ninguna forma de fuente que emita estas mismas instrucciones puede
bajar esa prioridad**, así que `sched1` va a subir ese `lis` siempre.

### 1.3 Lo que hace el objetivo no es planificación: es post-`reload`

En el objetivo el reparto es:

    objetivo   lis r9 <-1D88   lis r8 <-1D8C   lis r10 <-1D94   lis r11 <-1D90   lis r9 <-1D70
    nuestro    lis r10<-1D88   lis r8 <-1D70   lis r7  <-1D8C   lis r11 <-1D94   lis r9 <-1D90

y los **`lfs` van en el MISMO orden en los dos lados** (1D88, 1D70, 1D8C, 1D94,
1D90). El `lis` de 1D70 del objetivo está **pegado a su `lfs`** (filas 129-130) y
**reutiliza `r9`**, el registro que muere en la fila 126. Con `r9` reusado hay
una antidependencia que impide subirlo, y por eso `sched2` tampoco lo mueve.

Un `lis` que aparece pegado a su uso **después** de que `sched1` haya subido a
los otros cuatro sólo puede haber nacido **después de `sched1`**: es una
**rematerialización de `reload`** (el pseudo se quedó sin registro duro). Ése es
el «cuatro registros contra cinco»: el objetivo tiene **cuatro** pseudos de
dirección vivos y materializa el quinto en el sitio.

**Consecuencia**: la salida no está en la fuente. Sería un resultado del
repartidor (que `local_alloc` + `global_alloc` dejen uno sin asignar), y ningún
cambio de sentencia que conserve estas instrucciones lo provoca.

La permutación `f0 ↔ f12` de las filas 116-128 es del mismo tipo: los dos lados
emiten `lfs 0x4c, 0x3c, 0x48, 0x38, 0x50, 0x40` en el mismo orden y sólo cambia
qué registro de coma flotante recibe cada uno. Es `local_alloc`.

**Cero ensayos de fuente gastados aquí**, y a propósito: con la `prio` medida no
hay ninguno que pueda funcionar.

---

## 2. `ICEMover::Update` (3.868 B): UNA diferencia de RTL, y el pase que la decide

### 2.1 Las 15 filas son una sola cosa

`fndiff` da 15 filas, todas entre las filas 589 y 617 (la expansión de
`ICE::SignedMod` + `ICEShakeTrack::GetKey`). Y todas cuelgan de **una**:

    fila 596   objetivo  cmpwi r8, 0x0        <- compara el ARGUMENTO (el `lwz`)
               nuestro   cmpwi r11, 0x0       <- compara la COPIA del parametro

`lmap.py` sitúa el bloque en `ICEMath.hpp:136-146` (el cuerpo de `SignedMod`) y
`mr r11, r8` en la **136**, la línea de la declaración: es la copia del parámetro
que hace el «inliner». En RTL (`.jump` y `.lreg` de `ICEMover.cpp` sueltos):

    (insn 2680 (set (reg/v:SI 1045) (reg:SI 1037)))      ; mr  <- copia del parametro
    (insn 4246 (set (reg:CC 1600) (compare:CC (reg/v:SI 1045) 0)))   ; NUESTRA guarda

El objetivo compara **1037**. A partir de ahí se cae todo lo demás: en el
objetivo `a` (la copia) y `frame` son **dos pseudos** que se reparten `r11` y
`r8`; en el nuestro **cse los funde en uno** (`r11`), que además se lleva la `n`
de `GetKey` — y por eso nosotros indexamos con el valor recortado
(`mulli r9, r9, 0x18`) y el objetivo con `n` (`mulli r9, r8, 0x18`).

El DWARF lo confirma desde el otro lado: en el original **`int frame; // r8`**
tiene registro propio; en el nuestro `int frame;` **no tiene ninguno** (se
disolvió).

### 2.2 El pase es `cse`, y es `make_regs_eqv` — con control

La guarda `cmpwi/bge` es una **copia del test de salida** que crea
`expand_end_loop` (insns 4246/4247, ya presentes en el volcado `.jump`), y nace
usando el mismo pseudo que el test de abajo. Quien podría cambiarla es `cse`:
al ver `(set 1045 1037)` llama a `make_regs_eqv(1045, 1037)`, que hace canónico
al **nuevo** pseudo si **(a)** su último uso cae fuera del bloque de cse y
**(b)** vive más que el primero de la clase. Nuestro 1045 cumple las dos (es la
variable del bucle, y el bloque de cse acaba en la etiqueta del bucle), así que
gana él y la guarda se queda con la copia.

**Control que lo prueba** (caso mínimo, 0,2 s por compilación, `c28cam_min.cpp`):

    d3  (base)  el argumento muere en la copia          guard = r10  (la COPIA)
    d1          el argumento se USA despues de GetKey   guard = r8   (el ARGUMENTO)  <-- forma del objetivo

Basta con que el pseudo del argumento viva más que `a` para que la guarda pase a
compararlo. **Eso es exactamente lo que le pasa al original** — y no hay forma
de fuente que lo consiga sin emitir código de más, porque el argumento
(`FloatToInt(...)`) se usa **una sola vez**.

### 2.3 Trece formas barridas

Caso mínimo (`c28cam_sw.py`, 0,2 s cada una), puntuando `guard/subf/slwi`:

    s0  base                                                 = nuestro
    s1  `return a - c;` / `return 0;`                        BORRA la copia del parametro (-3 insns)
    s2  `int c = a / b; a -= c * b;`                         = s0
    s3  `while (a < 0) a += b;` sin llaves                   = s0
    s4  `if (b <= 0) return 0;` + bloque                     no compila (bloque anonimo)
    s5  `if (a<0) do { a += b; } while (a<0);`               = s0
    s6  `for (; a < 0;)`                                     = s0
    s7  `c *= b;`                                            = s0
    s8  `} else { return 0; } return a;`                     BORRA la copia (-1 insn)
    s9  `a = a - c;`                                         = s0
    s10 `if (0 < b)`                                         = s0
    g1  `GetKey`: `(Clamp(n,0,N-1) == n)`                    = s0
    g2  `GetKey`: `(n != Clamp(...)) ? 0 : &Keys[n]`         = s0
    g3  `GetKey`: `if (...) { return &Keys[n]; } return 0;`  = s0

Sobre la unidad entera (17-20 s cada una), contra `ICEMover 3868 B 99,90693 %,
15 diffs`:

    j1  `SignedMod` con `return a - c;` / `return 0;`        99,90693 %  15 diffs  = BASE
    j4  `GetKey` con `(Clamp(...) == n)`                     99,90693 %  15 diffs  = BASE
    e1  `int frame = FloatToInt(...); frame = SignedMod(...)` 99,90693 % 15 diffs  = BASE
    f1  `current_sec = fParam*length; current_sec *= fFreq*30`  99,55016 %  24 diffs  PEOR
    f2  `current_sec = fParam*length*(fFreq*30.0f)`             99,87073 %  20 diffs  PEOR

`s1` y `s8` **borran** el `mr` de la copia del parámetro, que el objetivo **sí
tiene** (`mr r11, r8`), así que están descartadas aunque cambien el reparto.
`e1` arregla el `mulli` en el caso mínimo pero **no transfiere** a la unidad.

### 2.4 Lo que dice el DWARF y no es accionable

`dwbody.py zCamera "ICEMover::Update" both`:

- `SignedMod` es **idéntica** en los dos lados: firma `(int a, int b)`, un
  bloque anónimo con `int c; // r0` y **ninguna local más** (esto veta cualquier
  variante con un `int r` intermedio, que era la hipótesis natural).
- el bloque anónimo mide **0x20 en el original y 0x10 en el nuestro**: la nota
  `NOTE_INSN_BLOCK_END` está detrás del `minus` en nuestro `.jump` y aparece al
  principio de `bb69` en el `.lreg` — la mueve el planificador al reemitir
  notas. Es síntoma, no palanca.
- `current_sec` está en **f0** en el original y en **f13** en el nuestro (el
  código es idéntico); `f1`/`f2`, que persiguen esa lectura, **empeoran**.
- La diferencia de `Normalize`/`Copy` que ya avisa `HERRAMIENTAS` sigue ahí y
  sigue sin ser accionable; **no la he tocado**.

---

## 3. `TrackCar::Update` (992 B) y `TrackCop::Update` (948 B)

`TrackCop` se queda con la demostración de la r27 (es
`optimize_reg_copy_2` de `regmove`, gobernada por `-fexpensive-optimizations`,
que no se puede quitar). **No he gastado ensayos ahí**: la pregunta abierta del
brief («¿se puede evitar que `cse` funda los dos `&look_offset`?») exige dos
pseudos vivos a la vez para la misma dirección, y eso emite un `addi` de más
por construcción — la r27 ya lo razonó y sus diez formas lo confirmaron.

`TrackCar` no lo había mirado nadie. Sus 15 filas son dos racimos:

    filas 107-111   los tres `stfs` a r31+0x90/0x94/0x98
                    objetivo: 0x98, 0x90, 0x94   (= el orden de sus `lfs`, que
                                                   CASAN: 0x20, 0x18, 0x1c)
                    nuestro : 0x90, 0x94, 0x98   (= el orden de la fuente)
    filas 113-123   f12 <-> f13 en el bloque r1+0x18/0x1c/0x20

Los tres `stfs` son terminales: misma `prio`, cero dependientes, así que los
ordena `INSN_LUID`, o sea el orden que dejó `sched1`. En el objetivo `sched1`
reordenó **también** las tiendas; en el nuestro sólo las cargas. Es el mismo
frente que `TrackCop`, con la misma raíz (el orden de la cadena que entra a
`sched2`).

---

## 4. El exceso de `.text`: medido, clasificado, y con la causa raíz cazada

**Regla 4 del brief, aplicada.** `objdump -h` de nuestro `.o` contra el del
troceador:

                    nuestro      objetivo
    .text           0x2192C      0x1E850      **+12.508 B**
    .rodata         0x1F50       0x2350
    .data           0x0B2C       0x0C40

Los 907 símbolos sin pareja (17.136 B contando datos) se reparten así:

    plantilla STL      31 simbolos   6.000 B   (reserve__vector<…>, find__H2Z…, _Rb_tree…)
    metodo del juego  128 simbolos   5.052 B   (Attrib::TAttrib<T>::Get x30, accesores en clase)
    literal/dato      713 simbolos   4.436 B
    plantilla UTL       5 simbolos   1.448 B   (_Storage<…>, push_back…)
    destructor         30 simbolos     200 B

### 4.1 `-fno-implement-inlines`: −2.608 B, y no cuesta nada

    base                          .text 137.516 B   120941/145125 B  83,3357 %  615 fn
    -fno-implement-inlines        .text 134.908 B   120941/145125 B  83,3357 %  615 fn

**Ni un byte de diferencia en el código casado** (las cinco funciones vigiladas
dan el mismo porcentaje y el mismo número de diffs), **cero símbolos
indefinidos nuevos** (370 en los dos), y los «métodos del juego» sobrantes bajan
de **128 (5.052 B) a 64 (2.444 B)**: se van los accesores en clase de
`ICEMover`/`CameraMover` (`GetNearClip`, `GetNoiseAmplitude`, `GetSimSpeed`…,
84 B cada uno) y `GetDistanceTo__11CameraMoverPC8bVector3` (164 B). Es el mismo
efecto que el brief midió en zFEng. **No he tocado `configure.py`: es una
medida, no un cambio.**

### 4.2 La causa raíz de las plantillas: **es la bandera de depuración**

El brief decía: «nuestro `zLua.o` tiene 24 secciones y **CERO
`.gnu.linkonce.*`** … en los cflags no hay `-fno-weak`». La causa es otra, y está
medida. Con un fichero de prueba de tres líneas (`c28cam_tpl.cpp`, una plantilla
de función y una clase-plantilla) y **los cflags exactos de zCamera**:

    (sin ninguna -g)                    .gnu.linkonce.t.twice__H1Zf_X01_X01   <- SI
    -gdwarf+                            0 secciones linkonce
    -gdwarf                             0
    -gdwarf-2                           0
    -g                                  0
    -gstabs                             0
    -gdwarf+ -fno-implicit-templates    0

**Cualquier** bandera de depuración apaga `.gnu.linkonce.*` en este GCC 2.95.3 SN.
Y el efecto sobre la unidad entera es exactamente el exceso:

    zCamera  con -gdwarf+   .text 137.516
    zCamera  sin -gdwarf+   .text 121.336  +  16.180 B en 174 secciones .gnu.linkonce.t.*
                                             (121.336 + 16.180 = 137.516, al byte)

Y no es cosa de zCamera. Compilando a mi scratchpad, sin `-gdwarf+`:

    unidad     .text sin -g    linkonce.t (secciones)   .text del objetivo
    zCamera      121.336        16.180  (174)              125.008
    zLua          94.944        18.228  (138)               94.264
    zPhysics     115.468        74.616  (663)              146.124
    zMain        150.900        29.064  (514)              159.776

Los 18.228 B de linkonce de zLua **son** los +18.908 B de exceso que el brief le
atribuye, y sin ellos su `.text` se queda a 680 B del objetivo: es la
explicación de «`zLua` figura al 100 % y `trypromo.py` dice DOL ROTO».

**El matiz que hay que decir, porque desmonta la lectura fácil:** el original
**también** se compiló con `-gdwarf+` (su volcado trae los atributos de la
extensión GNU de DWARF-1, `GCC Source File Name Offset` / `GCC Source Info
Offset`), así que **sus objetos tampoco pudieron llevar `linkonce`**. O sea:
apagar `-g` es una vía para que **nuestro** DOL salga como el suyo, pero **no es
lo que hizo el original**; cómo se quedó él con una sola copia sigue abierto.

Lo que sí está comprobado es que se quedó con una sola: en `config/GOWE69/symbols.txt`
cada uno de estos aparece **una vez**, y las tres direcciones caen dentro del
rango de código de **zAI** (`0x800034A0 → 0x80045E3C`), que es la primera
SourceList del enlace:

    Get__CQ26Attribt7TAttrib1ZbUi                             = .text:0x8003ACB0  size:0x50
    reserve__…Z13WCollisionTri…_type_WCollisionVectorUi        = .text:0x8003B72C  size:0x1BC
    find__H2ZPP10IExplosionZP10IExplosion_4_STLX01X01RCX11_X01 = .text:0x8003D140  size:0xB0

En nuestros objetos son **símbolos débiles (`w`) en `.text` plano**, uno por cada
unidad que los instancia. (Aviso de convivencia: el agente `c28lo_` de esta
misma ronda está trabajando este frente —`c28lo_firstwins.py`, `nolo.ld`,
`dupA/dupB`—; dejo la medida por si le sirve de contraste, no la he seguido.)

---

## 5. Fidelidad de cabeceras (no cambia el código, pero conviene anotarlo)

`lmap.py` sobre `ICEMover::Update` da el fichero y la línea del ORIGINAL para
cada instrucción del racimo de `SignedMod`/`GetKey`:

- **`ICEShakeTrack::GetNumKeys` está en `ICEData.hpp:307` y `GetKey` en
  `ICEData.hpp:309`**. En nuestro árbol la clase entera vive en
  `ICEManager.hpp:136-158`. No cambia el código emitido.
- **El orden de `ICEMath.hpp` no es el nuestro.** Del mapa de líneas del
  original: `FloatToInt` en **131-133**, `SignedMod` en **136-147**,
  `Clamp(int,int,int)` en **152-154**. En el nuestro: `Clamp(int)` 128,
  `Clamp(float)` 132, `Clamp(Vector3*)` 136, `ToDegrees` 146, `SignedMod` 150,
  `FloatToInt` 164.
- Dentro de `SignedMod`, las líneas del original encajan con el `while` **sin
  llaves** (137 `if (b > 0)`, 138 `while (a < 0)`, 139 `a += b;`, 140
  `int c = a / b;`, 141, 142, 145 `a = 0;`, 146 `return a;`). El nuestro tiene
  las llaves, que es lo que desplaza la 140 en un renglón.

Ninguna de las tres cambia una instrucción (`s3`, con el `while` sin llaves,
sale idéntica). Las anoto para quien persiga fidelidad de ficheros.

---

## 6. Ensayos numerados, con su cifra

    -- unidad entera (c28cam_unit.py, directorio-sombra con -I, 17-20 s)
    base                                    120941/145125 B  83,3357 %  615 fn   ICEMover 15 diffs
    j1   SignedMod `return a - c;`/`return 0;`                83,3357 %  615 fn   15 diffs  = BASE
    j4   GetKey `(Clamp(n,0,N-1) == n)`                       83,3357 %  615 fn   15 diffs  = BASE
    e1   `int frame = FloatToInt(...); frame = SignedMod(...)`83,3357 %  615 fn   15 diffs  = BASE
    f1   current_sec = fParam*length; current_sec *= fFreq*30 83,3357 %  615 fn   24 diffs  PEOR
    f2   current_sec = fParam*length*(fFreq*30.0f)            83,3357 %  615 fn   20 diffs  PEOR
    nii  -fno-implement-inlines                               83,3357 %  615 fn   .text -2.608 B

    -- caso minimo (c28cam_min.cpp + c28cam_sw.py, 0,2 s)
    s0 s2 s3 s5 s6 s7 s9 s10 g1 g2 g3     todas identicas a s0
    s1, s8                                 borran el `mr` del parametro (el objetivo lo tiene)
    s4                                     no compila
    d1 (control)                           argumento vivo despues -> la guarda pasa a compararlo
    d2, d3 (controles)                     confirman el modelo

    -- banderas (probe de plantillas + unidad entera)
    sin -g / -gdwarf+ / -gdwarf / -gdwarf-2 / -g / -gstabs / -fno-implicit-templates
                                           SOLO «sin -g» produce .gnu.linkonce.t.*

---

## 7. Vedas nuevas (con la sentencia barrida)

1. **`TerrainVelocityNoise` no es `global_alloc` y `alloc.py` no aplica**: los
   cinco pseudos de dirección (200, 227, 241, 245, 249) los asigna
   `local_alloc`; la lista `;; 37 regs to allocate:` del `.greg` no los contiene.
   Corrige la veda 5 de la r27, que ya había corregido a la r26.
2. **El `lis` del `0.0f` no se puede bajar desde la fuente.** Su `elf_high`
   (insn 399) tiene `prio` 7 en `sched1` porque el `0.0f` alimenta el **primer**
   `fsel` de `bMin(MAX, bMax(x, 0.0f))`; con `issue_rate 2` el planificador lo
   coloca en el ciclo 2 sí o sí. El del objetivo está pegado a su `lfs` y
   reutiliza `r9`: nació **después de `sched1`** (rematerialización de `reload`).
   Barrida: la forma de `bClamp(bDot(...), 0.0f, accel_max)` — no hay ninguna que
   acorte esa cadena sin cambiar las instrucciones.
3. **`ICEMover::Update`: las 15 filas son una sola diferencia de RTL** — la
   guarda del `while (a < 0)` compara el ARGUMENTO en el objetivo y la COPIA del
   parámetro en el nuestro— y la decide `make_regs_eqv` de `cse`. **Control
   `d1`**: en cuanto el pseudo del argumento vive más que `a`, la guarda pasa a
   compararlo. Barridas **13** formas de `SignedMod` y de `GetKey`: idénticas, o
   borran el `mr` del parámetro que el objetivo tiene.
4. **El DWARF veta el `int r` intermedio en `SignedMod`**: el original sólo
   declara `int c` en un bloque anónimo, igual que nosotros.
5. **`current_sec` NO es el producto entero** aunque el DWARF del original lo
   ponga en `f0`: las dos formas que lo intentan (`f1`, `f2`) suben de 15 a 24 y
   a 20 diffs.
6. **`.gnu.linkonce.*` y depuración son incompatibles en este compilador**: seis
   variantes de `-g` medidas, todas a cero secciones linkonce; sólo sin `-g`
   aparecen. Y **el original llevaba `-gdwarf+`**, así que linkonce no puede ser
   como lo resolvió él.

---

## 8. Lo que NO he probado

- **static-init (3.604 B, 54 diffs)**: cero ensayos. Las vedas de la r22/r25/r26
  (dirección descendente descartada, +26..31 insns RTL muertas, árbol de inlines
  303/303) siguen en pie y el brief las da por cerradas.
- **`_Storage::assign` (1.156 B)**: cero ensayos. La r27 demostró que aun
  cerrándolo daría **cero bytes**.
- **`LoadCameraShakes` (168 B)**: cero ensayos (cerrada por DWARF en la r26).
- **`TrackCop` (948 B)**: cero ensayos; sólo he confirmado que sigue en 2 diffs.
  **No he intentado la pregunta abierta del brief** (impedir que `cse` funda los
  dos `&look_offset`): exige dos pseudos vivos a la vez para la misma dirección,
  que emite un `addi` de más.
- **`TrackCar` (992 B)**: sólo el diagnóstico de §3, **cero ensayos de fuente**.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): **no aplicado**, el
  brief lo condicionaba a cerrar algo con ganancia de bytes y no he ganado
  ninguno.
- **`permuter.py`**: sigue sin usarse en zCamera.
- **La vtable `_vt.Q33Sim9Collision9IListener`**: sigue siendo el único símbolo
  ausente; no lo he tocado.
- **`configure.py` / cflags**: no he tocado nada. `-fno-implement-inlines` y
  «sin `-gdwarf+`» son **medidas**, no propuestas aplicadas; para promocionar
  harían falta `trypromo.py` y el permiso del brief.
- **No he mirado si `-fno-implement-inlines` es seguro fuera de zCamera.** En
  zCamera no aparece ningún indefinido nuevo, pero el riesgo teórico (que otra
  unidad llame fuera de línea a un accesor en clase) no lo he medido en el árbol.

---

## 9. Herramientas (scratchpad, prefijo `c28cam_`)

    c28cam_unit.py   compila zCamera ENTERA con un DIRECTORIO-SOMBRA (-I delante)
                     que lleva copias parcheadas de ficheros del arbol, la mide
                     con objdiff y saca el % y el numero de diffs de las cinco
                     funciones vigiladas. 17-20 s. El arbol NO se toca.
                     Los argumentos que empiezan por '-' van al compilador.
    c28cam_rtl.py    preprocesa con los cflags exactos de zCamera y llama a
                     cc1plus A MANO con las letras de volcado que le pases.
                     OJO: en este build solo existen -dj (.jump), -ds (.cse),
                     -dt (.cse2), -dl (.lreg), -dg (.greg), -dS (.sched),
                     -dR (.sched2). **-dk NO produce .regmove.**
    c28cam_h.py      compila UN .cpp suelto con los cflags de zCamera (0,2-5 s).
    c28cam_ex.py     extrae una funcion de un volcado RTL (;; Function ...).
    c28cam_sw.py     barrido sobre el caso minimo; puntua guard/subf/slwi.
    c28cam_min.cpp   caso minimo FIEL de SignedMod+GetKey+llamante (0,2 s).
    c28cam_tpl.cpp   probe de 3 lineas para las secciones .gnu.linkonce.
    c28cam_j1/j4/e1/f1/f2.py   los parches de §2 (ICEMover.cpp es CRLF).
    c28cam_ice.lmap  el mapa de lineas del objetivo de ICEMover::Update.
    c28cam_ice.dwboth  los dos cuerpos DWARF completos.
    c28cam_ice.diff / c28cam_car.diff   los fndiff.

**Trampas nuevas para `HERRAMIENTAS.md`:**

- **`-dk` no existe** en este `cc1plus`: no hay volcado `.regmove`.
- Con `-dR`/`-dS`/`-dl`/`-dg` los volcados salen **junto al `.ii`**, no en el
  `cwd` de `cc1plus`.
- **`ICEMath.hpp` es LF y `ICEMover.cpp` es CRLF**: un parche con `\n` sobre
  `ICEMover.cpp` falla en silencio.
- El caso mínimo de `SignedMod` **sí es fiel** para la guarda y el `subf`, pero
  **no** para el `mulli` del índice (`e1` lo arregla en el mínimo y no en la
  unidad). Comprobar siempre en la unidad.

## 10. Verificación final

    build_direct.py zCamera                       ok
    triage.py zCamera --muro                      identico al brief (3 near-miss, 4 muros)
    measure.py zCamera                            113080/125008 B  90,4582 %  446 al 100 %
    audit.py Speed/Indep/SourceLists/zCamera      446/446 ok, CERO FALLA  (dos pasadas)
    md5 UTLVector.h    d2d4c8e66789362ea032f7af80a6c70a   (= r27)
    md5 TrackCop.cpp   f7c2a733a30af8b7e83c25b92813a648   (= r27)
    md5 ICEMath.hpp    ec9d9c201fb6b4138fd210f42f97c8d6
    md5 ICEMover.cpp   edc3423a7989bf96b6ce5925412a7983
    md5 ICEManager.hpp 4bf44755d305a929827e6f99f20f28d8
    git status src/Speed/Indep/Src/Camera/        vacio
    git status src/Speed/Indep/SourceLists/       vacio

`frozen.py` no lo he tocado: no he cambiado nada. Disco: arranqué con 16 GB
libres y termino con 14 GB; mis ficheros ocupan **menos de 1 MB** (he borrado los
`.json` de objdiff, los `.o`, los `.ii`, los `.s` y los volcados RTL).
