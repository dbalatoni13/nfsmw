# Ronda 24 — zEcstasy: cero bytes cerrados, tres correcciones de fuente verificadas

Estado medido (`build_direct.py zEcstasy` antes de **cada** medida):

    antes    140.008 / 145.884 B   95,9721 %   534 funciones al 100 %
    despues  140.008 / 145.884 B   95,9721 %   534 funciones al 100 %
    measure.py --cmp c24ecs_sin.json c24ecs_con.json  ->  +0 B, +0 funciones,
                                                          0 unidades cambian

    audit.py  534 ok, CERO FALLA (dos pasadas: al empezar y al cerrar)

**Cero bytes.** Lo que traigo: (1) **la fuente de `eProject` corregida en dos
puntos, los dos verificados por DWARF y por `lmap`**, (2) **la cabecera
`GXVert.h` corregida** — el original declara `const` en los 25 escritores de
vertice y nosotros en ninguno —, (3) **el `switch` de `epCalculate` indexado por
`ib16`**, que cierra la unica diferencia de DWARF de esa funcion, y (4) tres
vedas nuevas medidas.

**AVISO, y lo pide el brief: BAJA UN PORCENTAJE.** `eProject` pasa de
**88,85075 % a 85,82089 %** (22 -> 25 diffs), **0 bytes**, y es a proposito: el
88,85 % era MENOS fiel. Detalle y justificacion en la §3.

## 0. El encargo, verificado

`triage.py --muro zEcstasy` sobre objetos recien construidos reprodujo el papel
al byte (696 + 5.180 = 5.876 B):

    2072 B  93,30502 %  epCalculateLocalDirectionalPOS16   155 diffs
    2044 B  98,27985 %  UpdatePlatInfo                      74 diffs
     796 B  98,99497 %  GenerateHorizonFogDisplayList        2 diffs
     696 B  98,07471 %  Render__13EmitterSystemP5eView      34 diffs (688/696: faltan stw+lwz)
     268 B  88,85075 %  eProject                            22 diffs

## 1. Aviso de convivencia: `EcstasyE.cpp` cambio DEBAJO de mi

`git status` al arrancar mi sesion daba **solo** los dos ficheros de EAXSound
modificados. A mitad de ronda `src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp`
aparecio modificado (`lbl_803DDFEB/FFF` -> `lbl_803DDFEC/FFC` en
`EnvmapTargetNames`), y **ese fichero es de zEcstasy**. No es mio.

Por eso la medida final **no** la hice contra la base del principio: hice un
**A/B seguido** revirtiendo solo MIS tres ficheros, reconstruyendo y volviendo a
medir. Resultado, comparando las **1.651 funciones** de la unidad una a una:

    funciones sin=1651  con=1651
    eProject   (88,85075, 268/268, 22 diffs) -> (85,82089, 268/268, 25 diffs)
    CAMBIAN: 1

O sea: `a1` (§2) y `g1` (§4) son **exactamente cero** en codigo, y lo unico que
mueve una sola cifra en toda la unidad es `eProject`.

## 2. `epCalculateLocalDirectionalPOS16` — el `switch` va por `ib16` (a1)

La pista fuerte que dejo la r23 era correcta y **se confirma con el DWARF, no
con `regmap`**. `dwbody.py … both` da el orden de declaracion del original:

    unsigned char * ib8;          <-- SIN registro, y declarada la PRIMERA
    unsigned short * ib16; // r5  <-- con r5, declarada la SEGUNDA

Nosotros teniamos exactamente lo contrario (`ib16` primero y sin registro, `ib8`
con r5). La lectura es mecanica: GCC 2.9 borra el pseudo de una variable que es
copia de otra y **no se usa**; el que sobrevive es el que indexa el bucle.
Escrito como lo teniamos (`ib16 = idx+off; ib8 = (u8*)ib16;` y el bucle sobre
`ib8`), `combine` mete el `plus` dentro del set de `ib8` y mata `ib16`.

**a1** — declarar `ib8` primero y hacer que el bucle indexe por `ib16`:

    unsigned char *ib8 = index_buffer + display_list_header_offset;
    unsigned short *ib16 = (unsigned short *)ib8;
    ... ((unsigned char *)ib16)[data_stride] ...   (20 lineas sustituidas)

    epCalculate  93,30502 %  ->  93,30502 %   2072/2072   155 diffs  (IDENTICO)
    regmap       87 locales iguales, 5 distintas  ->  89 iguales, 3 distintas

**Cero bytes y cero instrucciones**, pero la unica diferencia de DWARF que no
era de reparto **desaparece**. Lo que queda son **tres** locales
(`dcrg` obj f3 / nuestro f5, `dcba` f2/f4, `scba` f4/f13) y el marco.

### 2.1 El marco +8, ya con las dos aritmeticas contadas

Corrijo el §4 de la r23 en un punto: **el objetivo TAMBIEN tiene un hueco.**
Extraidas todas las referencias a `(r1)` de los dos lados, las ranuras
referenciadas son **identicas byte a byte** hasta 0x7f:

    0x8 0xc (SF) | 0x10 0x18 0x20 0x28 0x30 0x38 0x40 (PS) | 0x48 [ref 0x4c]
    0x50 0x58 0x60 (PS) | 0x68 [0x6c] 0x70 [0x74] 0x78 [0x7c]   = 120 B, acaba en 0x80

    objetivo: vars_size 0x80 -> fpmem 0x88   => 8 B que NADIE referencia
    nuestro : vars_size 0x88 -> fpmem 0x90   => 16 B que nadie referencia

O sea: **los dos tienen ranura fantasma; nosotros una mas.** No es «una ranura
de mas» contra «ninguna»: es «dos» contra «una». Eso descarta que la cause una
sola conversion concreta de la fuente y apunta a `reload` (un pseudo derramado
cuyas referencias se comen `reload_inheritance`/las herencias). No he sabido
identificar cual.

Aviso que sigue en pie: aunque se cierren los 8 B quedan las ~106 filas de
permutacion de `f2..f13` del bucle interno. **No es una funcion de un solo muro.**

## 3. `eProject` (268 B) — DOS correcciones de fuente, y el porcentaje BAJA

Esta es la funcion donde de verdad ha avanzado el diagnostico. `dwbody.py both`
enfrenta los dos cuerpos y la diferencia es brutal:

    ORIGINAL                      NUESTRO (antes)
    float oneOverW;               float halfVP2; // f12
    float clipX;  // f13          float clipX;   // f0
    float clipY;  // SIN REGISTRO float clipY;   // f13
    float clipZ;  // f10          float clipZ;   // f10
    float halfVP2;// f0           float oneOverW;
    float halfVP3;// f13          float halfVP3; // f12

### 3.1 `clipY` no tiene registro en el original porque es POSITIVA (p6/p2)

Que una local **asignada y usada** no tenga localizacion DWARF solo pasa si su
pseudo acaba con **cero referencias**. Y `lmap.py` dice donde esta el truco:

    8010AD08  lfs f0, 0x4(r30)          eMathE.cpp:255     <- *sy
    8010AD0C  fnmadds f12, f12, f7, f11 (SIN NOTA)         <- hereda 255
    ...
    8010ACDC  lfs f7, 0xc(r31)          eMathE.cpp:245     <- pm[3], linea de clipY
    8010ACE4  lfs f11, 0x10(r31)        eMathE.cpp:245
    8010AD00  lfs f12, 0x1c(r1)         eMathE.cpp:245

**Las cargas de `clipY` estan en la linea 245 y el `fnmadds` NO.** Eso solo
sale de una cosa:

    float clipY = eye.y * pm[3] + eye.z * pm[4];        // 245, POSITIVA
    ...
    *sy = vp[1] + halfVP3 + halfVP3 * -clipY * oneOverW; // 255

`combine` funde el `neg` (que esta en la linea 255) con el `fmadds` que define
`clipY` (linea 245), **borra el set de `clipY`** y deja el `fnmadds` en el sitio
del `neg`. Resultado: las cargas se quedan en 245, el `fnmadds` cae en 255, y el
pseudo de `clipY` muere -> sin registro. **Exactamente lo que enseña el
objetivo.** Nosotros teniamos `clipY = -(…)` y por eso el `fnmadds` nacia con
destino `clipY` y la variable se llevaba f13.

Medido, **con el orden de declaracion viejo** (ensayo p6):

    eProject  88,85075 %  ->  88,85075 %   268/268   22 diffs   IDENTICO
    regmap    13 locales iguales / 4 distintas  ->  14 iguales / 3 distintas

Es decir: **la correccion de `clipY` es GRATIS** — mismo porcentaje, mismo
recuento de diffs, una local mas casada.

### 3.2 El orden de declaracion (p1/p2), y por que lo dejo puesto

El DWARF y `lmap` coinciden en el orden del original y **no es el nuestro**:

    243  float oneOverW;                 <- declarada VACIA, antes de clipX
    244  float clipX = eye.x*pm[1] + eye.z*pm[2];
    245  float clipY = eye.y*pm[3] + eye.z*pm[4];
    246  float clipZ = eye.z*pm[5] + pm[6];
    248  oneOverW = eRecip(-eye.z);
    251  float halfVP2 = vp[2]*0.5f;     <- lfs 0x8(r30) = vp[2], linea 251
    252  float halfVP3 = vp[3]*0.5f;     <- lfs 0xc(r30) = vp[3], linea 252
    254/255/256  *sx / *sy / *sz

y ademas la linea 238 del original lleva **las dos** tiendas del retorno
temprano (`*sy = 0.0f; *sx = 0.0f;`), que nosotros teniamos en dos lineas.

    p1  solo el orden (= q1/q2 de la r21)          -> 85,82089 %  25 diffs
    p2  orden + clipY positiva + lineas alineadas  -> 85,82089 %  25 diffs
        regmap: 14 iguales / 3 distintas (clipX, halfVP2, halfVP3)

**Lo dejo puesto**, y esto es lo que hay que discutir: el porcentaje baja
**3,03 pp** y los bytes no se mueven (`matched_code` es todo-o-nada: 88,85 y
85,82 valen los mismos **cero** bytes). A cambio queda un esqueleto **fiel a dos
fuentes independientes** (orden de declaracion del DWARF + numeros de linea de
`lmap`) y, de propina, **la linea del original = la nuestra + 107** en toda la
funcion, que es una herramienta real para el que siga. Hay un comentario
`// NON_MATCHING:` encima de `eProject` que lo explica, para que nadie lo lea
como una regresion.

Si se prefiere el porcentaje: **quitar solo el reorden** (dejar `clipY`
positiva, §3.1) devuelve el 88,85075 % con la misma mejora de DWARF. Es un
`git diff` de siete lineas.

### 3.3 Lo unico que falta en `eProject`, ya cuantificado

Con p2 quedan **tres** registros y **son el mismo problema**:

    local      objetivo  nuestro
    halfVP2    f0        f13
    clipX      f13       f0
    halfVP3    f13       f12

`REG_ALLOC_ORDER` de rs6000 para FPR, deducido de nuestros propios volcados, es
**f0, f13, f12, f11, f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, f31…f14**. Y
`lreg.py` (ya arreglado) da los tres numeros que deciden:

    pseudo  var       n_refs  live_len  prioridad  reg
    107     clipX     2       3         6666       f0
    143     halfVP3   3       5         6000       f12
    137     halfVP2   3       6         5000       f13

Se asigna en orden de prioridad descendente y cada uno coge el primero libre de
`REG_ALLOC_ORDER` en su rango. Con `clipX` primero se lleva f0 y `halfVP2` se
queda el f13; **en el objetivo `halfVP2` va primero**. La condicion es
aritmetica y estrecha:

    3/len(halfVP2) > 2/len(clipX)     hoy 3/6 = 5000  contra  2/3 = 6666

o sea **`halfVP2` con `live_length` 4 (7500) o `clipX` con 5 (4000)**. El
desempate por numero de allocno NO sirve: `clipX` (107) es menor que `halfVP2`
(137) tambien en el original, porque el DWARF pone `clipX` antes.

Barridos y **revertidos**, ninguno mueve un solo registro:

    p3  `clipX * halfVP2 * oneOverW` (invertir el producto en *sx)
        85,97015 %  (24 diffs)  268/268.  Mismos tres registros.
    p4  `0.5f * vp[2]` y `0.5f * vp[3]` (constante primero)
        85,82089 %  IDENTICO.
    p5  `float halfVP2, oneOverW;` declaradas juntas al principio y halfVP2
        asignada en su sitio (bajar el numero de pseudo)
        85,82089 %  IDENTICO.  **Confirma que decide la PRIORIDAD, no el
        desempate por allocno.**

## 4. `GXVert.h` — el original declara `const` en los 25 escritores (g1)

La r23 lo dejo anotado como «cabecera que miente» sin probarlo. Contado sobre el
volcado DWARF entero del original, **no son tres funciones: son todas**:

    112  inline void GXColor1x8(const unsigned char x)
    111  inline void GXColor1x16(const unsigned short x)
    109  inline void GXPosition3f32(const float x, const float y, const float z)
     …   25 firmas distintas, TODAS con `const` en TODOS los parametros

Nuestro `GXVert.h` no tiene ni uno. **g1**: `const` en los seis macros
(`FUNC_1/2/3/4PARAM`, `FUNC_INDEX8`, `FUNC_INDEX16`).

Es cabecera compartida, asi que el A/B va por objetos y sobre el **cierre
transitivo**. Los ficheros que usan estos inline son solo seis
(`EcstasyE/EcstasyEx/eStrip/eVShader/Movie_GC/AcidFX_G`) y caen en **tres**
SourceLists; `dolphinsdk/src/gx/GXVert.c` esta entero dentro de `#if DEBUG` y no
compila, y `DEMOInit.c`/`G2D.c` no estan dados de alta (no hay `.o`).

    zEcstasy                      +0 B  (1.651 funciones, ninguna cambia)
    zPlatform + zPhysicsBehaviors  261.452/265.556 B  98,4546 %  1.252 al 100 %
                                   CON y SIN, medido seguido: +0 B, 0 unidades

**Cero efecto en codigo, en las tres unidades.** Se queda porque hace fiel el
DWARF y porque quita un falso positivo permanente de `dwbody.py`: hasta hoy
`dwbody` señalaba esta diferencia en **toda** funcion que escriba vertices.

Nota de mecanismo, por si sirve en otro sitio: en GCC 2.9 un parametro
`TREE_READONLY` deja que `expand_inline_function` use el argumento **sin
copiarlo a un pseudo**. Aqui no cambia nada porque los argumentos ya son
registros o constantes, pero es un cambio real de RTL en el caso general.

## 5. `GenerateHorizonFogDisplayList` (796 B, 2 diffs) — dos vedas nuevas

El diff sigue siendo **un intercambio**, y solo uno:

    objetivo  … fmr f30,f22 · andi. r11,r31,0x1 · srawi r0,r0,1 · xoris …
    nuestro   … fmr f30,f22 · srawi r0,r0,1 · andi. r11,r31,0x1 · xoris …

Lo que añado al diagnostico de la r21: el `andi.` **alimenta DOS ramas** (las de
las lineas 767 y 771, `beq` en 80106F74 y 80106FA0) **con un solo compare**;
cr0 sobrevive nueve instrucciones. Aun asi la cadena del `srawi` dentro del
bloque es de 7 eslabones (`srawi -> xoris -> stw -> lfd -> fsub -> frsp ->
fmuls f28`) contra 1 del `andi.` (`andi. -> beq`), asi que **por prioridad
nuestro orden es el que sale**, y el del objetivo solo puede venir de que el
`srawi` no estuviera listo ese ciclo. No he encontrado que lo produzca.

    g2  intercambiar las sentencias `grid_pointX` y `grid_pointY`
        98,99497 -> **91,22613 %** (24 diffs) y **800 B** (4 de mas). REVERTIDO.
        VEDA: la posicion relativa de grid_pointX/grid_pointY.
    g3  las dos ternarias escritas como `if (i & 1) … else …`
        98,99497 %  IDENTICO, 2 diffs. REVERTIDO (neutro).
        VEDA: ternaria contra if/else en las dos condiciones.

Vedas anteriores que siguen: `int odd = i & 1;` izado (r21 h1, neutro) y
`grid_pointY` antes de `multiple = i/2` (r18-plat c16, 91,482 %).

## 6. `UpdatePlatInfo` (2.044 B) — corrijo el recuento de la r23

La r23 decia «solo quedan **tres** locales con registro distinto». Con
`regmap.py` sobre el nombre completo (`eLightMaterialPlatInterface::UpdatePlatInfo`
— con `UpdatePlatInfo` a secas la herramienta **no encuentra la funcion** y sale
con codigo 2) son **once**, y hay un ciclo de 4:

    diffuse_min_a     f7 -> f6      envmap_min_scale  f5 -> f8
    diffuse_max_scale f6 -> f5      envmap_min_g      f1 -> f31
    diffuse_rng_a     f5 -> f2      envmap_min_b     f26 -> f8
    envmap_power      f2 -> f1      envmap_max_scale  f8 -> f7
    envmap_max_r     f25 -> f26     envmap_max_g     f31 -> f25
    envmap_max_b      f8 -> f7
    PERMUTACION ciclo de 4: f7 -> f6 -> f5 -> f8 -> f7

`dwbody.py` da **cero** diferencias estructurales: mismas locales, mismo orden
de declaracion, mismo arbol de bloques. Es `local_alloc` puro sobre `f1..f13`.

Y una lectura util del `.text`: la unica «instruccion movida» de la r22 esta
bien localizada pero **mal identificada**. Con el mapa de lineas y el layout de
`eLightMaterialData` (feData en +0x30 de `eLightMaterial`, o sea `0x50(r3)` =
`DiffuseMinA`), el `fsubs fX, f24, fY` es `diffuse_rng_a` y las lineas del
original van **dos por debajo** de las nuestras en ese tramo. El objetivo
calcula `min_r, min_g, min_b, max_r, max_g, max_b` en **orden de fuente**;
nosotros `min_r, min_g, max_r, max_g, min_b, max_b`. Es consecuencia del
reparto, no causa.

**Cero ensayos de fuente aqui**: con once registros y un ciclo de 4 no me ha
dado tiempo despues de `eProject`. El `c5` de la r22 (quitar el `asm` del case
`0x68E97F75`) sigue puesto y sigue dando los seis registros salvados correctos.

## 7. `Render__13EmitterSystemP5eView` (696 B) — sin tocar

No lo he tocado. Lo unico que añado es la lectura del sentido: **no nos falta un
allocno, al objetivo le SOBRA una decision de derrame**. Nosotros usamos los 18
salvados r14-r31 y le damos registro al centinela `&this->mEmitterGroups`; el
objetivo usa 17 (r15-r31), **deja r14 libre** y aun asi derrama el centinela a
`0xc8(r1)` (de ahi el `stw`+`lwz` que nos faltan y los 8 B de marco). Un registro
libre y aun asi derrama: eso no es presion, es que a ese allocno le sale mas
barata la memoria que el registro (`regclass` le pone `prefclass = NO_REGS`), y
eso depende del **numero y la profundidad de bucle de sus referencias**. Con el
DWARF identico (47 inlines, mismas locales, mismos registros) no se toca desde
la fuente sin inventar codigo.

## 8. Herramientas

- **`c24ecs_lines.py`** (scratchpad) — lee y reescribe un rango de lineas
  **conservando el final de linea de CADA linea**. `eMathE.cpp` tiene CRLF y LF
  **mezclados por linea** (l.128 CRLF, 129 LF, 131 CRLF…): cualquier
  `str.replace` con un patron multilinea falla en silencio o corta mal. Me costo
  dos intentos y una compilacion rota.
- **`c24ecs_try.py`** — aplica una sustitucion, construye, mide las cinco
  funciones y **revierte**, con `--keep` para quedarse. Aborta si el build falla
  (sin eso se mide el `.o` rancio: es la trampa de HERRAMIENTAS §5).
- **`regmap.py` necesita el nombre COMPLETO con clase.** `regmap.py zEcstasy
  UpdatePlatInfo` sale con «no encuentro … parecidas:
  eLightMaterialPlatInterface::UpdatePlatInfo» y **codigo 2**. Un agente que no
  mire el codigo de salida se queda sin la tabla y cree que no hay diferencias.
- **Borrar el cache de `regmap` entre medidas.** `build/regmap/our_zEcstasy_*.nothpp`
  y su `idx_*.pkl` llevan el mtime en el nombre, pero `dwbody.py` cogia el
  volcado viejo. `rm -f build/regmap/our_zEcstasy_* build/regmap/idx_our_zEcstasy_*`
  antes de cada `regmap`/`dwbody`.
- **`lreg.py` arreglado funciona**: reproduce el reparto exacto de `eProject`
  (45 pseudos, prioridades 6666/6000/5000) y de ahi salio el modelo de la §3.3.
  Con `--rtl` **no** saca el cuerpo RTL de `.greg` en esta unidad (solo 49
  lineas: el volcado que deja `-dg` trae las disposiciones, no las insns).
- **Disco**: no lo he llenado. Empece con 14 GB y termino con 14 GB; no he
  hecho ni un `-dr`.

## 9. Que NO he probado

- **El permutador**, ni guiado ni ciego, en ninguna funcion. Con `eProject` a
  268 B y **tres** registros mal, es la candidata mas barata que ha tenido el
  permutador en seis rondas.
- **`epCalculate`**: no he identificado el pseudo que pide la segunda ranura
  fantasma de 8 B. Falta el `-dr`/`.lreg` de esa funcion (~100 MB de disco) y
  cruzar los `assign_stack_local` de `reload` con las herencias.
- **`epCalculate`, las tres locales que quedan** (`dcrg`/`dcba`/`scba`): cero
  ensayos.
- **`UpdatePlatInfo`**: cero ensayos de fuente. Los tramos
  `d = -5..-4, +2..+5, +8..+12` de la ventana de la r22 **siguen sin tocar**, y
  ahora ademas se sabe que lo que queda no es `global_alloc` sino once
  registros de `local_alloc` con un ciclo de 4.
- **`GenerateHorizonFog`**: no he probado a alargar la cadena del `andi.` ni a
  acortar la del `srawi` (las dos piden cambiar codigo), ni un caso minimo de
  20 lineas con el bucle suelto — que es lo que en la r23 abrio `fold`.
- **`Render`**: nada. La via seria bajar el coste en registro del centinela
  (menos referencias o mas profundidad de bucle), y eso significa inventar
  codigo.
- **`eProject` con `clipX` alargada a `live_length` 5**: no he encontrado la
  forma de fuente. Es la unica condicion que falta y esta escrita en la §3.3.
- **`frozen.py cong`**: NO he vuelto a congelar. `frozen.py chk` dice «HA
  CAMBIADO» (por `eProject` y por el `EcstasyE.cpp` de otro agente), y
  congelarlo ahora metería en la huella un cambio que no es mio.

## 10. Lo que queda en el arbol

Tres ficheros, y **los tres dan +0 B**:

- `src/Speed/GameCube/Src/Ecstasy/EcstasyEx.cpp` — `a1`: `ib8` declarada
  primero y las 20 lineas del `switch` indexando por `(unsigned char *)ib16`.
- `src/Speed/GameCube/Src/Ecstasy/eMathE.cpp` — `p2`: `eProject` con `clipY`
  positiva y la negacion en el sitio de uso, el orden de declaracion del DWARF,
  las dos tiendas del retorno temprano en una linea, y un comentario
  `// NON_MATCHING:` de 8 lineas que explica el 85,82 % y da el desfase de
  lineas (+107) y la condicion que falta.
- `src/Speed/GameCube/bWare/GameCube/dolphinsdk/include/dolphin/gx/GXVert.h` —
  `g1`: `const` en los seis macros. **Cabecera compartida, A/B hecho sobre las
  tres SourceLists afectadas.**
