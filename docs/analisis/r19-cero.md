# r19 — frente "escribir desde cero": sfir + eaxadecf

Linea base `base_r19_cero.json`:

| unidad | matched | total | fns |
|---|---|---|---|
| `Libs/snd/9/source/library/mix/sfir` | 0 | 936 B | 0/1 |
| `Libs/snd/9/.../extern/coda/cmn/eaxadecf` | 0 | 1316 B | 0/9 |

`ninja` no construye ninguna de las dos (middleware). Se compila con
`python scripts/build_direct.py <fichero>` y se mide con `scripts/libdiff.py`.

## 1. sfir — `calcFIRCoeffs(SNDFIRSTATE*, int)`

- Direccion `0x8036EDBC..0x8036F164`, tamano `0x3A8` = 936 B, 234 instrucciones.
- **Unica funcion de la unidad**: o casa entera o la unidad vale 0.
- El `.cpp`/`.c` YA EXISTE (`src/.../mix/sfir.c`) y ya esta en `configure.py:925`
  como `NonMatching`. **No era escribir desde cero: era un near-miss al 85,363%.**
- Censo de llamantes (`bl` directas; no prueba ausencia de punteros a funcion):
  `sfbpffir8.c:57` (filtType 4), `sfhpffir8.c:54` (3), `sflpffir8.c:54` (2).
  Las tres unidades hermanas estan **cerradas** y comparten `SNDFIRSTATE`
  (0x3C: `history[8]`, `coef[5]`@0x20, `cutLow`@0x34, `cutHigh`@0x38).
- Llamados: `SNDI_sin(float)`, `SNDI_cos(float)` (externas, ya en el ELF).
- Constantes del objetivo (pool de `auto_05_80412F60_rodata.s`), **en orden**:
  `0.8`, magico `4503601774854144`, `3.1415927`, `0.5`, `0.7853982`, `0.0`,
  `0.46`, `0.54`, `1.0`. El pool de GCC se emite en orden de expansion.

### Diagnostico de la 85,363% inicial (4 frentes)

1. **`0.7853982` se crea ANTES que `0.0`** en el objetivo; en nuestra fuente
   `sum = 0.0f;` precede al bucle que usa `0.7853982f` → orden invertido.
2. **`fmuls f1, f29, f1`** (constante en el PRIMER operando) x2. `fold` de GCC
   intercambia siempre `REAL_CST * x` → `x * REAL_CST`, asi que el primer
   operando del objetivo **no puede ser un literal**: es una variable.
   (Contraste dentro de la misma funcion: `cnt * 3.1415927f` sale con la
   constante SEGUNDA, `fmuls f29, f0, f30` — confirma que fold si intercambia.)
   1 y 2 tienen la misma causa: una local con `0.7853982f` asignada antes de `sum`.
3. **Rama invertida** en `filtType==3`: el objetivo hace `bne`→`fsubs`, con el
   `fadds` en caida. Eso es `if ((cnt & 1) == 0) sum += ...; else sum -= ...;`,
   no `if (cnt & 1) sum -= ...; else sum += ...;`.
4. **PRE/GCSE de mas**: nuestra version guarda `halfLen*4` y `&pfir->coef[0]` en
   registros vivos toda la funcion (con insercion en la arista del `default`);
   el objetivo los **recalcula** en cada caso y tres veces mas al final.

### Ensayos

| # | variante | medida | veredicto |
|---|---|---|---|
| c1 | local `tmpFloat = 0.7853982f;` antes de `sum = 0.0f;`, uso `tmpFloat * cnt`, y `if ((cnt & 1) == 0) sum += ...; else sum -= ...;` | **85,363 -> 89,252 %** | **se queda**. Cierra los frentes 1, 2 y 3: el pool sale en el orden del objetivo, los dos `fmuls f1, f29, f1` casan y la rama de paridad casa |
| c2 | + `default: break;` en el switch | 89,252 % | descartado: identico |
| c3 | `halfLen = 4;` DESPUES de la guarda | 86,962 % (924 B) | descartado: cprop propaga el 4 y sale `stfs f0, 0x10(r10)`; el objetivo mantiene `slwi r0, r26, 2` |
| c4 | `int halfLen = 4;` en la declaracion | 89,252 % | identico |
| c5 | contador distinto en el bucle final | 89,145 % | peor |
| c6 | `sum` declarado el ultimo / sin la local muerta / `coef[cnt] = coef[cnt]/sum` | 89,252 % | identicos |
| c7 | puntero local `p = pfir` para la cola | 89,252 % | identico (copy-prop lo deshace) |
| c8 | llaves por `case` | 89,252 % | identico |
| c9 | bucle de ventana como `do {} while` | 83,479 % (956 B) | peor |
| c10 | `if (filtType != 3) {} else {...}` en la cola | 89,252 % | identico |
| c11 | `pc = pfir->coef` para el bucle de normalizacion | 88,825 % (936 B) | da el tamano exacto y **empeora** — el septimo caso de la trampa del brief |
| c12 | cola en bloque anidado con su propio contador | 89,145 % | peor |

**Estado: muro medido en 89,252 %, 940 B contra 936 B del objetivo.**

### Que queda y por que (diagnostico firme)

Todo el diff restante cuelga de **una sola decision del compilador**: nuestra
compilacion aplica **PRE/LCM de gcse** a dos expresiones y el objetivo no:

- `(plus pfir 32)` = `&pfir->coef[0]`
- `(ashift halfLen 2)` = `halfLen*4`

Se ve en el sitio exacto: en la **arista del `default`** del switch aparecen
insertadas `slwi r28, r11, 2` y `addi r29, r30, 0x20` (insercion en arista, que
solo hace PRE), en cada `case` sale un `mr r28, r0` de mas, y en la cola nos
ahorramos los cinco recalculos que el objetivo **si** hace (`slwi r0, r26, 2` x2
y `addi rX, r29, 0x20` x3). El renombrado de registros que llena el diff
(r26/r11, r28/r27, r29/r30) es consecuencia de esos dos pseudos vivos de mas.

Diagnostico de banderas (solo diagnostico; las banderas NO se tocan):
`-fno-gcse` 72,222 % - `-fno-rerun-cse-after-loop` 64,111 % -
`-fno-expensive-optimizations` 89,209 % - `-fno-cse-follow-jumps`,
`-fno-cse-skip-blocks`, `-fno-thread-jumps` sin efecto -
`-fno-force-mem` 88,932 %. **Ninguna llega al 100 %**, asi que no es "gcse de
mas" a secas: gcse hace falta, es la colocacion de LCM la que difiere.

Dato secundario util: `cprop` de GCC 2.95 **no** propaga `halfLen=4` dentro del
`slwi` porque `(ashift (const_int 4) (const_int 2))` no es un insn reconocido y
`validate_change` lo rechaza; **si** lo propaga en `subfic r0, r31, 0x4` porque
el patron admite constante en el primer operando. Por eso conviven `li r26,4` y
`slwi r0, r26, 2` con un `4 - cnt` ya plegado. Los dos lados hacen lo mismo aqui.

### Vedas (que barri y no cierra)

- **La colocacion de `halfLen = 4`** (declaracion, antes de la guarda, despues):
  las tres formas medidas; ninguna quita el PRE de `pfir+32`.
- **Punteros locales a `pfir->coef`**, en la cola y en el bucle de
  normalizacion: copy-prop los deshace o empeoran.
- **Ambitos anidados y contadores separados** en los `case` y en la cola: es la
  palanca que cerro `process_raw_block` de eaxadecf, y aqui **no mueve nada**,
  porque PRE trabaja sobre expresiones, no sobre variables.
- **Estructura del bucle de ventana** (`for` contra `do/while`) y polaridad del
  `if` de la cola.

### Lo que NO he probado en sfir

- El **permutador** (`scripts/permuter.py`) y `regmap.py`: los dos estan
  cableados a `Speed/Indep/SourceLists` y, ademas, el volcado DWARF de
  `calcFIRCoeffs` **no trae ni una local** (solo los dos parametros, `pfir`/r29 y
  `filtType`/r28), asi que regmap no tendria con que emparejar.
- **Restricciones de registro**: no las he tocado. El brief las permite solo tras
  documentar la busqueda de la forma de fuente; la busqueda esta documentada
  arriba, pero el fallo es una decision de *colocacion de codigo* (PRE), no un
  reparto de registros, y una restriccion vacia no la deshace.
- Sacar el cuerpo de un `case` a una **funcion estatica que se inline**, para que
  el inliner cree pseudos nuevos y rompa la unificacion de expresiones.
- Mapa de lineas: `symbols/debug_lines.txt` **no sirve aqui** — las 234
  instrucciones de `calcFIRCoeffs` estan todas atribuidas a `sfir.c` linea 11.

## 2. eaxadecf — **CERRADA AL 100 %** (1.316 B, 9/9 funciones)

El fichero **no existia**; `configure.py:981` ya lo declaraba, asi que solo hubo
que escribirlo. Lo que dio el interfaz y los datos:

- **DWARF** (`python scripts/libdwarf.py cu eaxadecf.cpp`): trae `CEAXABLKDecf`
  (0xA8: `decodexa`@0x0, `EAXAVARSF xav`@0x4, `MXAPACKETF xap`@0x94), `EAXAVARSF`
  (0x90) y `MXAPACKETF` (0x14) **con desplazamiento y nombre de cada campo**, y
  el prototipo de las nueve funciones. Nada de esto hubo que deducirlo.
- **`src/Speed/Indep/Src/EAXSound/Ginsu/ginsudata.cpp`** trae el MISMO algoritmo
  XA con las mismas tablas: `xafilterf[2][4]` y `xatablef[16][16]` se copiaron de
  ahi y casan byte a byte con `_3Snd.xafilterf` / `_3Snd.xatablef`.
- `mtdecf.cpp` (hermana, cerrada) dio el patron de `operator new`/`delete` sobre
  `Snd::CODANew` / `Snd::CODADelete`, y `supxalf.cpp` la definicion de `XAFSTATE`.

**La primera compilacion dio 156/1316 B con 4 funciones ya al 100 %.** El resto
salio de estos hallazgos, todos reutilizables:

| # | funcion | de -> a | que era |
|---|---|---|---|
| e1 | ctor | 99,8 -> 100 % | **orden de las sentencias**: `sampledatasize, numsamples, residual, sample1, sample2`. Los `stw` salen permutados por el planificador, pero la permutacion es funcion del orden de la fuente: barrer las 5! permutaciones da la buena en segundos |
| e2 | `Feed` | 98,8 -> 100 % | igual: `numsamples, sampledatasize, psrc` |
| e3 | `Decode` | 88,8 -> 92,5 % | el bucle de residuo NO es `*p++ = *q++`: el objetivo **recarga los dos punteros** despues del `stfs`, asi que son tres sentencias, `*xap.pdst = *xav.presidue; xap.pdst++; xav.presidue++;` (pdst primero) |
| e4 | `Decode` | 92,5 -> 98,2 % | `i = 0;` **antes** de `decodexac(&xap)`: en el objetivo el contador vive en un registro salvado (r30) porque cruza la llamada. Y `decoded += k` **despues** de los dos `-= k` |
| e5 | `Decode` | 98,2 -> **100 %** | `k = min(residual, n)` como **`if/else` completo**, no `k = n;` + `if`. Solo cambia el sitio del `cmpw` respecto del `mr`; el ternario da 98,05 % |
| e6 | `decodexac` | 85,6 -> 86,9 % | orden de los operandos del `fmadds`: el objetivo es `pdst[-1] * f0`, no `f0 * pdst[-1]` (los dos son variables: `fold` no reordena) |
| e7 | `decodexac` | 86,9 -> **100 %** | **local `ps` para el `*psrc == 0xEE`**, cargada antes del bucle y refrescada al final del cuerpo. Explica las tres cargas de `pxap->psrc` del objetivo (precabecera, bloque `else`, latch) |
| e8 | `process_raw_block` | 60,5 -> 85,3 % | los pares de bytes NO son `*p++`: son `pb[0]=psrc[0]; pb[1]=psrc[1]; psrc += 2;` — un solo `stw` de `psrc` por par, no dos |
| e9 | `process_raw_block` | 85,3 -> 93,8 % | `pb` **asignado dentro del `if`**, no en la declaracion |
| e10 | `process_raw_block` | 93,8 -> **100 %** | **un segundo puntero declarado en el ambito del bucle** (`unsigned char *pbs = (unsigned char *)&s;`). Es lo que produce el `mr r30, r8` del objetivo: pseudo nuevo -> cse lo vuelve copia -> `loop.c` la iza a la precabecera. Con la MISMA variable reasignada dentro del bucle sale 64,5 %; con una variable nueva **antes** del bucle, 93,8 % |

`audit.py` pasa las nueve. `.text` 1316/1316 y `.data` 1056/1056 al 100 %,
`_3Snd.xafilterf` y `_3Snd.xatablef` incluidos. Congelada con `frozen.py cong`.

**Queda por hacer (no lo he tocado porque el encargo dice que no toque
configuracion): promover `eaxadecf.cpp` de `NonMatching` a `Matching` en
`configure.py:979-982`.** Mientras siga en `NonMatching`, el enlace coge el
objeto ORIGINAL de `build/GOWE69/obj/`, no el nuestro: por eso este trabajo **no
puede** haber movido el DOL.
