# r52 · `crit` — `criticalpath` (VP6_PredictFilteredBlock) y `madidct`

**Cerrado: nada.** Ninguna de las tres funciones llega al 100 %. El árbol queda
**idéntico al último commit** en los dos ficheros del encargo
(`src/Packages/vp6/1.0.6/source/decode/gc/criticalpath.c`,
`src/egami/rcmp/dev/source/decoder/cmn/madidct.cpp`): verificado con `fncmp.py`
antes y después, mismas cifras exactas —`VP6_PredictFilteredBlock` 740 B / 37
insn, `IdctColumn` 632 B / 138 insn, `IdctRow` 496/516 B—. `scripts/` sin tocar,
volcados RTL borrados (sólo los míos; los de `zEcstasy` y `wrn_only` intactos).

Lo que sale de la ronda son **un hallazgo estructural en `criticalpath` que
corrige la fuente**, **el mecanismo exacto de las dos funciones de `madidct`**, y
**seis vetas nuevas cerradas con control**. Ninguna repite las de la r51.

---

## 1. `criticalpath` — el mapa de líneas del original EXISTE y nadie lo había usado

`python scripts/lmap.py Packages/vp6/1.0.6/source/decode/gc/criticalpath
VP6_PredictFilteredBlock` da **línea por instrucción**. Dos cosas de entrada:

- El fichero original **no es `criticalpath.c`**: es **`recon.inl`**, y la función
  empieza en su línea 53 y acaba en la 176.
- **El orden de sentencias de nuestra fuente es el del original, sentencia por
  sentencia.** Correspondencia completa (nuestra línea relativa ↔ `recon.inl`):

      MvShift 62 · MvModMask 63 · SrcPtr=LastFrameRecon 66 · if Mode2Frame 67
      SrcPtr=GoldenFrame 69 · if VpProfile==0||UseLoopFilter==0 73
      mVx 76 · mVy 77 · ModX 80 · ModY 81 · mx= 84 · my= 85 · mx>>= 87 · my>>= 88
      TempBuffer 91 · Stride 92 · TempPtr1=TempPtr2=0 93 · else 94
      VP6_PredictFiltered 98 · TempBuffer=LoopFilteredBlock 99 · Stride=16 100
      TempPtr1=34 101 · TempPtr2=34 102 · ModX 105 · ModY 106
      if(ModX) 110 · if(Mv.x>0) 112 · if(ModY) 116 · if(Mv.y<=0) 118
      if(TempPtr1!=TempPtr2) 122 · if(bp<=3) 127 · ModX<<=1 130 · ModY<<=1 131
      if(VpProfile) 134 · if(Mode==2) 139 · if(Thresh) 142 · 4<<(t-1) 143
      =128 145 · if(t==0||abs&&abs) 148 · if(VarThresh) 154 · Var16Point 156
      FilterBlock 157/161/165/169 · UnpackBlock 174 · fin 176

  O sea: **no hay sentencias de más ni de menos, ni en otro orden**. Todo lo que
  quede es forma de expresión o reparto.

### 1.1 EL HALLAZGO: el árbol de `TempBuffer` de nuestra fuente está MAL

Nuestra línea (una sola sentencia, `recon.inl:91`):

```c
TempBuffer = SrcPtr + pbi->mbi.Recon + pbi->mbi.FrameReconStride * my + mx;
```

que en C es `((SrcPtr+Recon) + stride*my) + mx`. **El objetivo agrupa al revés**,
y se lee directamente en su propio asm (`lmap`, 0x80359980..0x803599BC):

    80359994  mullw r8, r8, r0     r8 = FrameReconStride * my
    803599A4  sraw  r10, r10, r30  r10 = mx
    803599AC  add   r0, r27, r0    r0  = SrcPtr + Recon
    803599B4  add   r8, r8, r10    <-- (stride*my) + mx   AGRUPADO
    803599BC  add   r29, r0, r8    <-- (SrcPtr+Recon) + eso

y el nuestro hace lo contrario:

    38 mullw r10, r10, r0     40 add r11, r27, r11    (SrcPtr+Recon)
    47 add   r11, r11, r10    <-- (SrcPtr+Recon) + stride*my
    51 add   r29, r11, r9     <-- + mx

**La fuente del original es, por tanto:**

```c
TempBuffer = SrcPtr + pbi->mbi.Recon + (pbi->mbi.FrameReconStride * my + mx);
```

(o `+ (mx + ...*my)`, que da el mismo árbol y sale mejor en el reparto).
Comprobado: con los paréntesis nuestro compilador emite `add rX,mullw,mx` y
luego `add r29,base,rX`, igual que el objetivo. `fold` NO reasocia por su
cuenta: con la fuente sin paréntesis salen siempre las dos sumas encadenadas.

### 1.2 Por qué importa: arregla EXACTAMENTE el frente que nombró la r48

La r48 dejó escrito que lo que quedaba era «nuestro `lwz r8,0x88(r31)`
(FrameReconStride) ocupa la ranura 28, que en el objetivo es el `lhax` de mVx».
El paréntesis lo arregla, y el volcado RTL dice por qué (`scripts/rtldump.py
criticalpath VP6_PredictFilteredBlock -dS --extra "-fsched-verbose-5"`, bloque
básico 4):

| insn | qué es | prio base | prio con paréntesis |
|---|---|---:|---:|
| 91 | `lhax` de mVx | **8** | **9** |
| 142 | `lwz 0x88` (FrameReconStride) | 9 | 9 |

Al agrupar, la cadena de `mx` gana un eslabón y la de la multiplicación no, así
que 91 y 142 **empatan a 9** y el desempate lo gana 91 por LUID (mVx se lee
antes en la fuente). En el volcado se ve literalmente:

    base   ;; Ready list (t = 3):  158 156 152 636 633 630 138 91 142
           ;;    --> scheduling insn <<<142>>>
    parén  ;; Ready list (t = 3):  158 156 152 636 633 630 138 142 91
           ;;    --> scheduling insn <<<91>>>

y en el objeto la ranura 28 pasa a ser `lhax r8,r7,r9`, la del objetivo.

### 1.3 Pero NO cierra, y esto es lo que queda

Con el paréntesis las filas suben de 40 a 43 (`+ (mx + stride*my)`) o 48
(`+ (stride*my + mx)`), porque el reparto de la COLA empeora (`TempPtr1` cae en
`r5` en vez de `r8` y se descolocan las tres parejas `add r3,r29,r8` /
`add r4,r29,r27` de las llamadas a `FilterBlock`). Barrido completo, todo
740/740 B salvo nota:

    base (HEAD) ........................................ 40 filas  94,39459 %
    + (stride*my + mx) ................................. 48        89,75676
    + (mx + stride*my) ................................. 43        90,81081
    + (Recon + (stride*my + mx)) ....................... 60        89,72973
    &SrcPtr[Recon] + (stride*my + mx) .................. 48        (objeto = t1)
    + (my*stride + mx) ................................. 34        90,94595
    receta r48 (asm "+r"(mVx) + local `rec` con barrera)  25        95,00000  <- el suelo
    receta r48 con cualquiera de los paréntesis ........ 50        95,00000

**La receta de la r48 se reproduce exacta** (25 filas, 95,0000 %, 740 B) y sigue
siendo el suelo. Matriz completa medida esta ronda (≈130 compilaciones):
{3 árboles de `TempBuffer`} × {las 32 combinaciones de quitar los 5 pines} ×
{corte de región en 8 posiciones, `"+r"(mVx)`, `"+r"(mVy)`, `"+r"(ModX)`,
`"+r"(ModY)`, local `rec`}. **Nada baja de 25 filas.**

### 1.4 Lo que queda es UNA decisión de `sched1` que GCC 2.95 no puede tomar

Ordenando el bloque del objetivo por ciclos de emisión (2 insn/ciclo, que es el
`issue_rate` del 750):

    c1 86,99 | c2 102,88 | c3 91,630 | c4 123,**112** | c5 124,142 | c6 126,116
    c7 134,117 | c8 144,**107** | c9 119,138 | c10 130,636 | c11 140,152
    c12 146,156 | c13 148,158

Los dos `and` de ModX (107) y ModY (112) tienen **INSN_PRIORITY 2** —no tienen
ningún dependiente dentro del bloque— y el objetivo los emite **por delante** de
insns de prioridad 9 (142) y 5 (119, 138). El nivel 1 de `rank_for_schedule` es
justo la prioridad, así que **ese orden no es alcanzable desde nuestro RTL**:

- no hay forma de fuente que baje la prioridad de 142 por debajo de 2 —su cadena
  es `lwz → mullw(coste 4) → add → add`, o sea 2+4+2+1 = 9 fija—;
- ni que suba la de los `and`, que por definición no tienen sucesor en el bloque.

O sea: **el grafo de dependencias del objetivo no es el nuestro**, y la
diferencia ya no está ni en el orden de sentencias (§1, mapa de líneas) ni en el
árbol de `TempBuffer` (§1.1, corregido). Quien siga tiene que atacarlo desde el
RTL del bloque, no desde formas de fuente: ese eje está agotado.

### 1.5 Vetas nuevas cerradas en `criticalpath`, con control

1. **`-mcpu` — cerrado.** En `build.ninja` no hay **ningún** `-mcpu`, y el
   defecto **es el 750**: `base`, `-mcpu=750` y `-mcpu=740` dan el mismo bloque
   byte a byte. Los otros ocho modelos (`603e`, `604`, `common`, `860`, `601`,
   `403`, `505`, `821`, `powerpc`) dan una secuencia de mnemónicos DISTINTA y
   más lejos del objetivo. Muere la sospecha «nos falta el modelo de
   planificación».
2. **Separar declaración de inicialización — NO-OP.** El mapa de líneas sugiere
   que el original declaraba primero (línea 75 emite nota y no código, mVx en la
   76 y mVy en la 77). Probadas cuatro formas (`int mVx, mVy, mx, my;` y
   asignaciones después; una por línea; `int mVx, mVy;` + `int mx, my;`;
   `int mx, my;` en una línea): **las cuatro dan el MISMO código**. Control
   pasado: el sha del objeto cambia en las cuatro (información de depuración),
   o sea que compilaron de verdad y aun así el código no se mueve.
3. **El corte de región `__asm__("")` es DESTRUCTIVO aquí.** 24 variantes (8
   posiciones de sentencia × 3 árboles de `TempBuffer`): **las 24 dan 736 B**,
   4 B menos que el objetivo. Confirma y amplía la nota de la r48.
4. **El clobber entero `__asm__("" : : : "rN")` sólo muerde donde el registro
   está DISPUTADO.** Sobre la receta de 25 filas, puesto en la rama `else`:
   `r5`, `r9`, `r10`, `r11` y `r25` dan un objeto **byte a byte idéntico al del
   `__asm__("")` pelado** (sha `c3eae80d9196`); sólo `r0` (hogar de MvModMask
   ahí) y `r28` (hogar de ModX) cambian algo —34 y 56 filas, los dos peores—.
   Matiz importante para el catálogo: la palanca de la r51 existe, pero **no es
   una palanca general**: si el registro que nombras no lo estaba peleando
   nadie, el clobber no aporta nada sobre la barrera.
5. **Quitar pines nunca ayuda, con ningún árbol.** Las 32 combinaciones de los 5
   pines × 3 árboles: el mejor de cada familia es el que los lleva TODOS
   (t0 40 filas, t4 43, t1 48). Reconfirma la r47 sobre una base nueva.

`promote.py` sigue diciendo **`LIMPIA: se puede marcar Matching`**: los 12.040 B
de `linked` de esta unidad cuelgan sólo de esta función.

---

## 2. `madidct` — el mecanismo, nombrado

### 2.1 `IdctColumn` NO es una permutación de registros

Es lo primero que hay que quitarse de encima, porque la r51 recomendó
«veda hasta que aparezca un permutador». Enfrentando los dos flujos y buscando
una **biyección** de registros consistente (`scratchpad`, sonda de un uso):

    objetivo 188 insn, nuestro 188 insn
    mismo mnemónico en la misma ranura:      109 / 188
    filas que NO encajan en NINGÚN renombrado: 153

Con 79 ranuras de mnemónico distinto no hay permutación posible. **Un permutador
de registros no arreglaría esta función.**

### 2.2 Dónde empieza exactamente la divergencia: la instrucción 13

Las 13 primeras instrucciones son iguales salvo nombres. La primera diferencia
real está **dentro del atajo del cero**, `(src[1]|…|src[7]) == 0`:

| ranura | objetivo | nuestro |
|---:|---|---|
| 11 | `stw r5, 0x8(r1)` (vuelca src[4]) | `stw r8, 0x8(r1)` (igual) |
| 13 | `lwz r6, 0x18(r31)` — **src[6]** | `lwz r8, 0x14(r31)` — **src[5]**, y **reusa r8** |
| 14 | — | `lwz r9, 0x18(r31)` — src[6] |
| 16 | `lwz r17, 0x14(r31)` — src[5] | `lwz r11, 0x8(r1)` — **RECARGA src[4]** |
| 17 | `or r0, r0, r5` (src[4] **sigue en registro**) | — |

Los dos vuelcan `src[4]` a `0x8(r1)` con el mismo `stw`. La diferencia es que el
objetivo **no vuelve a tocar `r5`** hasta después del `or`, así que la herencia
de `reload` vale y no hay recarga; nosotros metemos `src[5]` **en el mismo `r8`**
que tenía `src[4]`, matamos la herencia y pagamos un `lwz` de pila. **Eso, y sólo
eso, es el histograma `lwz` 18/20 · `mr` 12/10 que midió la r48**: dos recargas
nuestras contra dos `mr` suyos. El tamaño ya cuadra (632/632) porque se
compensan.

### 2.3 `IdctRow`: los 20 B son UNA instrucción, `mtctr` contra `mtlr`

    ranura 4  objetivo  lwz r0, 0x18(r3)      nuestro  lwz r0, 0x18(r3)   (igual)
    ranura 8  objetivo  mtctr r0              nuestro  mtlr r0

Los dos aparcan `src[6]` en un registro especial nada más leerlo. El objetivo
elige **CTR** y lo recupera **dos veces** (`mfctr r5` para `src[2]-src[6]` y
`mfctr r0` para `src[2]+src[6]`), más dos `mr` de reload; nosotros elegimos
**LR** y sólo lo recuperamos **una vez**, porque `r0` todavía tiene el valor
cuando llega el primer uso y `reload` lo hereda. Las cinco instrucciones que nos
faltan (`mtctr` +1, `mfctr` +2, `mr` +2) son exactamente eso, y cuadran al byte
con el déficit de 20 B. No es «un valor vivo más» como se venía diciendo: es la
**herencia de `reload`** en el primer uso.

(Dato de contraste que ata el diagnóstico: en `IdctColumn` los dos lados aparcan
DOS valores y con el mismo reparto —`mtlr` en la ranura 9 y `mtctr` en la 18 en
los dos—, así que el orden LR/CTR no está fijado por `REG_ALLOC_ORDER`: depende
del pseudo, y por eso el pin no lo alcanza.)

### 2.4 Vetas nuevas cerradas en `madidct`, con control

1. **La forma de `MULT` está AGOTADA.** La r51 midió 6 formas; van 8 más, todas
   `static inline` y todas con **objeto idéntico**: `unsigned long long` sobre la
   suma, temporal `long long`, cast sólo en `b`, cast en los dos, `0x8000` en vez
   de `32768`, sin el `(int)` final, con `& 0xffffffff`, y firma `long`/`long`.
   Control pasado: `m_tmp` y `m_long` cambian el sha del objeto (depuración) y el
   código no se mueve. Suma 14 formas medidas; el eje se cierra.
2. **La cadena de `or` del atajo del cero — cerrada.** Ocho formas: `!(...)`,
   asociada a la derecha, por parejas, invertida, con temporal, `0 == (...)`, y
   **intercambiando `src[5]` y `src[6]`** (que es justo lo que pide §2.2).
   Resultado: la base es la mejor con 152 filas; el intercambio 5↔6 da **156**
   (peor), la asociada a la derecha 155, la invertida 154, y por parejas 158 con
   **636 B**. `!(...)`, `0 == (...)` y el temporal dan el objeto de la base.
   **Desde la fuente no se cambia el orden de las cargas.**
3. **El clobber entero es un NO-OP al principio de una función.** Puesto nada más
   entrar en `IdctColumn`, **once registros distintos** (`r5..r12`, `r16`, `r17`,
   `r30`), la pareja `r8`+`r16` y el `__asm__("")` pelado dan **los doce el mismo
   objeto** (sha `68e7a211c90d`), y sólo `r30` se separa. Razón: ahí no hay
   ningún pseudo vivo que reasignar. Partiendo la condición para meterlo en
   medio (partirla es en sí un no-op, objeto idéntico), los cinco registros
   probados vuelven a dar el objeto del corte pelado (sha `895c57c76f61`, 628 B).
   Junto con §1.5.4: **la palanca de la r51 exige que el registro esté
   disputado en ese punto; si no, es la barrera y nada más.**

`promote.py egami/rcmp/dev/source/decoder/cmn/madidct` confirma la r51: sólo
`.text 1484/1504` y el desplazamiento de `idctcompute` (−20 B, la consecuencia).
Cero trabajo de datos. **Pero hacen falta las DOS funciones**, y `IdctRow` no
llega a 516 B sin los dos `asm` de la receta de la r46 —que además dejan ~120
filas distintas de 154—, así que no se deja puesta (regla 7).

---

## 3. Veredicto

- **`criticalpath`**: la fuente tenía un error real y está identificado (§1.1);
  corregirlo arregla el frente que nombró la r48 y **no cuesta bytes** (740/740
  en las dos formas). No lo dejo puesto porque no cierra la función y sube las
  filas de 40 a 43 (regla de no dejar trabajo a medias), pero **es un minuto
  aplicarlo** y quien siga debe empezar por ahí, no por la fuente de HEAD. Lo que
  queda detrás es una decisión de `sched1` demostrablemente inalcanzable desde la
  fuente (§1.4): la unidad no se reparte otra vez sin alguien que la ataque desde
  el RTL.
- **`madidct`**: la veda de la r51 se mantiene, pero ahora **con el mecanismo
  nombrado** —herencia de `reload` en las dos funciones (§2.2, §2.3)— y con la
  corrección de que **no es trabajo para un permutador** (§2.1): 153 de 188 filas
  no encajan en ningún renombrado.
- Las **seis vetas cerradas** (`-mcpu`, declaración partida, corte de región en
  criticalpath, clobber sin disputa ×2, formas de `MULT`, cadena de `or`) son lo
  que impide que la r53 vuelva a gastar media ronda en lo mismo.
