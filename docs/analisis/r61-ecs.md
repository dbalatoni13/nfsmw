# r61 -- agente `ecs` (unidad zEcstasy)

## Resumen

| | antes | despues |
|---|---|---|
| `UpdatePlatInfo__27eLightMaterialPlatInterface` (2.044 B) | 28 filas, 99,334640 % | **3 filas, 99,960860 %** |
| `eProject__FfffPA3_fPfN44` (268 B) | 13 filas | 13 filas (sin tocar) |
| `epCalculateLocalDirectionalPOS16` (2.072 B) | 164 insn | 164 insn (sin tocar) |
| `.o` de zEcstasy | `51f95d5ebbbe674d95ef1d4728efaafca3f20b29` | `0a4d26eb82dd3decbce2e0c12c864dfb2eef0ce7` (3 compilaciones) |
| `linkdelta.py zEcstasy` | `.text +0, resto IGUAL` | `.text +0, resto IGUAL` |
| `.rodata` de la unidad (13.344 B) | -- | **byte a byte identica** (volcado propio, `diff -q` limpio) |

`fncmp Speed/Indep/SourceLists/zEcstasy`: sigue en `3 de 539 funciones con el
CODIGO distinto`, pero UpdatePlatInfo pasa de `27 insn` a `3 insn`.

**Bytes de DOL ganados: CERO.** `build.ninja:701` marca
`Speed/Indep/SourceLists/zEcstasy.cpp: Game (linked False)`: la unidad no
entra en el DOL y no puede aportar un byte hasta que las TRES funciones
cierren a la vez. **No promociona** y no he corrido `trypromo`: con tres
funciones con codigo distinto el resultado esta decidido, y relanzar el enlace
mientras otros agentes editan es justamente lo que el protocolo
`tanda -> DOL -> promocion` prohibe.

## 1. Lo aplicado (recogido de la r56, que lo midio y no lo aplico)

Dos ediciones en `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp`:

1. `__asm__("");` detras de `envmap_power = 6.0f;` en el `case 0x68E97F75`
   (ocupa una de las lineas en blanco que ya habia, linea 538). Arregla el
   **horario** del bloque de literales: 28 -> 24 filas.
2. `float envmap_min_b = material_data->EnvmapMinB * envmap_min_scale;`
   (conmutado). Arregla el **reparto** de registros fp: 24 -> 3 filas.

Reproducidas primero en el banco de la r56 (`scripts/agent_ecs_upi.py`, ~2 s
por medida) contra sus numeros exactos: `bar1` 24 filas / 99,745600 %,
`bar1+minbc` 3 filas / 99,960860 %. Los mismos numeros salen en el `.o` de la
SourceList completa.

## 2. EL HALLAZGO: por que la conmutacion mueve el reparto

Es el unico eje de los 18 productos de la funcion que hace algo, y ahora se
sabe por que. Volcado `cc1plus -dg` sobre `eLightE.cpp` (mini-TU, cflags reales
de zEcstasy), bloque `;; Function void eLightMaterialPlatInterface::UpdatePlatInfo()`.

**El diff ENTERO de los dos volcados de conflictos son TRES lineas:**

```
68d67
< ;; 194 preferences: 37
242d240
< ;; 456 preferences: 37
244d241
< ;; 460 preferences: 37
```

37 = f5. 194 = `envmap_min_b`; 456/460 son su cadena `* 128.0f` (insns
1040/1041 del `.lreg`, linea 626 del fuente). El resto del grafo -- 132
pseudos a repartir, el orden de reparto, los 83 conflictos -- es **identico**.
La asignacion de `local_alloc` (`;; Register N in R`, 149 lineas) tambien es
identica: el efecto es SOLO de `global_alloc`.

La causa esta en el fuente de GCC que hay en el arbol,
`orig/prodg/NGC_GNU_SRC/NGC/gcc/global.c:1558`, dentro de `set_preference()`:

```c
  if (GET_RTX_FORMAT (GET_CODE (src))[0] == 'e')
    src = XEXP (src, 0), copy = 0;
```

Para `(set (reg D) (mult A B))` la preferencia de registro de D se toma del
**PRIMER operando** del `mult`. Y unas lineas mas abajo:

```c
  if (src_regno < FIRST_PSEUDO_REGISTER && dest_regno >= FIRST_PSEUDO_REGISTER
      && reg_allocno[dest_regno] >= 0)
    ... SET_REGBIT (hard_reg_preferences, reg_allocno[dest_regno], src_regno);
```

O sea: **si el primer operando es un HARD REG, el destino se lleva una
`hard_reg_preferences` de ese registro.** `register float x asm("frN")` ES un
hard reg en el RTL. Nuestro `envmap_min_scale asm("fr5")` le estaba colgando a
`envmap_min_b` (y por la cadena, a sus dos derivados) una preferencia de f5
que el original no tiene -- porque en el original `envmap_min_scale` es un
**pseudo global sin renumerar** (`reg_renumber == -1` cuando corre
`global_conflicts`, que es la condicion que la funcion comprueba justo antes).

Consecuencias, y esto vale para TODO el arbol, no solo para aqui:

- **Un `register asm` no solo fija un registro: contamina el reparto de todo
  lo que lo use como PRIMER operando de una operacion binaria.** Cuando un pin
  "casi funciona" y deja un ciclo de registros, este es el primer sitio donde
  mirar, y se comprueba en 2 s con `cc1plus -dg` y un `grep " preferences:"`.
- El observable es binario y NO es el porcentaje: es la lista de lineas
  `;; N preferences: R` del bloque de la funcion (R = 32+n para `fn`).
- La conmutacion de la fuente es la palanca MINIMA para quitar una de esas
  preferencias, y por eso no la sustituye ninguna otra forma de fuente (ver
  los negativos).

### Corolario que corrige al decisor de la familia D

El decisor de esta ronda escribio que *"un `register asm` no puede compartir
registro por construccion"* y que por eso el pin fr5 impide el reparto del
objetivo (que mete `diffuse_rng_a` y `envmap_min_scale` las dos en f5).
**Eso es falso, y esta medido**: con `bar1+minbc` la fila 103 sale
`fsubs f5, f24, f7` en los dos lados, o sea que `diffuse_rng_a` SI se lleva f5
con el pin puesto. Lo que el pin rompe no es el reparto de f5, es la
PREFERENCIA del pseudo 194. Su `PASO 2` (trabajar sobre la base limpia de 60
filas) es, medido, un camino peor: ver los negativos del apartado 4.

## 3. Lo que queda: TRES filas, y las dos causas estan nombradas

Medido sobre el `.o` de la unidad entera (`fndiff.py zEcstasy UpdatePlatInfo...`):

```
  98  fmuls f26, f5, f10            | fmuls f26, f10, f5
 333  lis  r11, lbl_803DECFC@ha     | lis  r9, $LC829@ha
 334  lfs  f2,  lbl_803DECFC@l(r11) | lfs  f2, $LC829@l(r9)
```

**Fila 98** la mete la propia conmutacion. El objetivo NO conmuta: emite la
escala PRIMERO en los tres productos `min` (filas 94/96/98 = `f5,f12` /
`f5,f11` / `f5,f10`). Para casar hace falta *el reparto de `minbc` con el orden
de operandos de la base*, y eso exige que la escala no sea un hard reg. **El
pin fr5 es, estructuralmente, lo que impide cerrar esta fila.**

**Filas 333/334** son reparto de GPR en `local_alloc`, y el horario ya es el
del objetivo: los dos lados emiten `lis(6.0), lfs(6.0), lis(0.22), lis(0.86),
lfs(0.22), lfs(0.86)`. Lo unico distinto es que el objetivo reparte
`r11, r9, r11` y nosotros `r9, r9, r11`. `REG_ALLOC_ORDER` (`rs6000.h:940`)
pone r9 antes que r11, asi que en el objetivo la qty del 0.22 se asigna ANTES
que la del 6.0 -- lo cual solo puede venir de un orden PRE-sched2 distinto.
La fuente esta barrida (ver negativos); queda el volcado `-dS`.

## 4. Negativos medidos esta ronda (todos con su cifra)

Banco `scripts/agent_ecs_upi.py` + arneses en `scratchpad/ecs61/`. Salvo nota,
todos con la barrera puesta y tamano 2.044 B.

**Contra la fila 98 (el reparto sin conmutar):**

- **Conmutar cualquier otro producto**: los 18 `X = A * material_data->B [- C]`
  de la funcion, uno a uno -> 24-56 filas. **Solo `envmap_min_b` baja** (3).
  `envmap_max_b` es el peor (56). Eje agotado.
- **Formas de `min_b` que NO conmutan** (8 medidas, todas **24 filas**, o sea
  cero efecto): temporal para la carga; `envmap_min_b = scale; *= MinB`;
  `register` en el temporal; producto muerto conmutado delante o detras;
  parentesis y cast; **recargar `material_data->EnvmapMinScale`** (cse lo
  pliega a f5); `const`/`volatile` sobre el pin; copia declarada ANTES del pin.
  `min_b = MinB; *= scale` da 41. La conmutacion es el UNICO disparador.
- **Copia `_s` de la escala solo para `min_b`**: 24 filas, y el volcado dice
  por que: `local_alloc` **renumera `_s` a f5** y la preferencia vuelve
  (`;; 195 preferences: 37`). Copia compartida por los TRES productos:
  CERO preferencias de f5 -- mata tambien las cuatro buenas (440/444/448/452)
  -> 57 filas. Compartida por `g`+`b` 86 filas (2.048 B), por `r`+`b` 26.
- **Sacar `_s` del bloque** para que `local_alloc` no la renumere, con un
  `__asm__("" : : "f"(_s))` de cero bytes dentro de un case (3 sitios):
  86-176 filas, y ademas **engorda a 2.048/2.056 B**.
- **`asm` que emite el `fmuls` con el orden bueno y los operandos al reves**
  (`__asm__("fmuls %0,%2,%1" : "=f"(b) : "f"(MinB), "f"(scale))`): **63 filas**.
  El asm parte el bloque de planificacion y se lleva por delante las cargas
  63-71. Las tres variantes (con y sin temporal, orden directo) dan 63.
- **Pines sobre los productos**: `envmap_min_b` en fr0/1/4/8/12/25/26/30/31 ->
  42-114 filas; `envmap_min_g` fr1 -> 78; `envmap_min_r` fr30 -> 67; los tres
  a la vez -> 125-128 **y 2.036 B** (adelgazan la funcion: no valen).

**Contra el pin (el `PASO 2` del decisor de la familia D):**

| variante | filas |
|---|---|
| sin pin | 60 |
| sin pin + barrera | 57 |
| sin pin + barrera + minbc | 48 |
| sin pin + `diffuse_rng_a asm("fr5")` | 77 |
| sin pin + `rng_a fr5` + barrera | 73 |
| sin pin + `rng_a fr1` + barrera | 65 |
| sin pin + `rng_a fr31` + barrera | 60 |
| con pin + `rng_a fr5` + barrera | 81 (**2.040 B**) |

La base limpia es MUCHO peor que la pinchada, y no es un ciclo sino cuatro: sin
pin la banda volatil sale corrida entera (nuestro f5 -> objetivo f6, f6 -> f7,
f7 -> f8, f8 -> f5) mas el par f31/f1. **Quitar el pin a pelo no es el camino.**

- **CANTIDAD FANTASMA sobre la base sin pin** (`register float _gh asm("frN");`
  + `__asm__("" : "=f"(_gh));` + `__asm__("" : "+f"(_gh));`), N = 4..10, en
  tres sitios y en dos formas (rango de cero y rango que ABARCA todo el bloque
  envmap): **21 medidas, las 21 dan 57 filas y objeto identico**. El fantasma
  se borra entero: los dos `asm` son NO volatiles y su salida no se usa, asi
  que `flow` los tira y no llega a existir rango de vida. **Correccion a
  `memory/nfsmw-cantidad-fantasma.md`: la receta de dos asm no volatiles NO
  fabrica rango si nadie consume el valor.**
- Con el consumidor VOLATIL (`__asm__("" : : "f"(_gh))`), que si sobrevive:
  **260-268 filas y la funcion adelgaza a 1.960/1.952 B**. Peor todavia.

**Contra las filas 333/334:**

- **Permutar el cuerpo del `case 0x68E97F75`**: las 6 ordenes x 0..3 barreras
  x 4 posiciones = **78 medidas**. El orden de la fuente (power, min_a, rng_a)
  con la barrera detras del `power` es el minimo (3 filas); el segundo mejor
  es 4 (`apr` y `rap` con la barrera en la posicion 1).
- **Clobber `__asm__("" ::: "rN")`** para r0/r8/r9/r10/r11/r12 en las 3
  posiciones del case: **18 medidas** -> 3, 5, 6 o 7 filas. Ninguna mejor.
- **Referencia extra al mismo literal** con `if (0) { envmap_magic = K; }` para
  6.0/0.22/0.86 en cuatro sitios (subir `n_refs` de la qty y con ello su
  `QTY_CMP_PRI`), mas barreras extra delante, en medio y detras: las 8 dan
  **3 filas exactas**. El reparto r9/r11 no responde a nada de la fuente
  probado hasta hoy.

## 5. Familias del brief: lo que he verificado en MI unidad

- **Familia D ("la base envenenada"): confirmada MUERTA, y con un matiz nuevo.**
  Mi base reproduce los numeros del decisor sin desviacion (28 filas con pin,
  60 sin). Pero su punto 3 -- "el pin es lo que impide el reparto porque un
  `register asm` no puede compartir registro" -- es **falso**: con la barrera y
  la conmutacion, `diffuse_rng_a` se lleva f5 CON el pin puesto (fila 103
  identica en los dos lados). Lo que el pin rompe es la preferencia del pseudo
  194, y eso se lee en `global.c:1558`. Su `PASO 2` (rehacer `bar1` y el
  barrido de `envmap_power` sobre la base de 60 filas) lo he ejecutado: 57 y 48
  filas, muy por detras de las 3 de la base pinchada. **No lo recomiendo.**
- **Familia F (orden del pool): NO APLICA aqui, comprobado.** La `.rodata` de
  la unidad es byte a byte identica antes y despues, y el bloque de literales
  del case ya sale en el orden del objetivo (DECFC, DED00, DED04). La semilla
  de pool no tiene nada que colocar. El "paso 3" que el decisor de la familia F
  le encarga a `UpdatePlatInfo` (reabrir el movimiento de `envmap_power` dentro
  del case por sus 27 filas y devolver el pool con la semilla) queda
  **sin objeto**: mover `envmap_power` dentro del case forma parte de las 78
  permutaciones que he barrido y ninguna baja de 3 filas, que es lo que ya
  tenemos sin mover nada.
- Familias A y E: no tocan zEcstasy.

## 6. Via viva para quien retome UpdatePlatInfo

1. **Fila 98.** Hay que quitar el pin fr5 sin perder f5 en la escala. La
   cantidad fantasma esta descartada con medida (apartado 4). Lo que queda sin
   probar es atacar el ORDEN de reparto de la banda f5-f8 en la base sin pin:
   en el objetivo nuestro valor de f8 va a f5, o sea que se asigna el ULTIMO de
   los cuatro y en nosotros el PRIMERO. Eso es prioridad de `global_alloc`
   (`allocno_compare`), no preferencia.
2. **Filas 333/334.** Volcado `-dS` del bloque para leer el orden PRE-sched2 de
   los tres `lis`/`lfs`. La fuente ya esta barrida (96 medidas entre
   permutaciones, clobbers y referencias extra).

## 7. Administrativo

- **Correcciones de `lcfix.py` pendientes: CERO.** La `.rodata` de la unidad no
  se ha movido un byte (volcado propio de las 13.344 B, `diff -q` limpio), asi
  que no hay ni un `$LC` desplazado. **Ninguna entrada venenosa para otra unidad.**
- **Propuestas para `keep.lst` / `splits.txt` / `configure.py`: NINGUNA.**
- Ficheros tocados: solo `src/Speed/GameCube/Src/Ecstasy/eLightE.cpp`
  (2 lineas de codigo + 1 bloque de comentario que `previo.py` ya indexa).
- Scratchpad: `scratchpad/ecs61/` (102 kB, solo arneses y las dos listas de
  preferencias `p_bar1.txt` / `p_minbc.txt`). Los 22 MB de volcados RTL,
  `.i` y `.json` estan borrados.
