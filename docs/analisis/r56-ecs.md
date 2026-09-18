# r56 — zEcstasy: la veda de la r49 se rompe, y el pool de constantes es orden de sentencia

Agente `ecs`. Unidad en propiedad: **zEcstasy**. Ficheros tocados: `eLightE.cpp`,
`eMathE.cpp`, `EcstasyE.cpp` (los tres, solo de `Ecstasy/`). Ninguna cabecera compartida,
ningun `config/`, ningun `keep.lst`.

**Resumen en tres lineas.** (1) La veda de la r49 sobre `UpdatePlatInfo` —«el bloque de
literales del `case 0x68E97F75` es irreducible»— es **falsa**: una barrera vacia lo coloca, y
con la conmutacion ya conocida la funcion baja de **28 filas a 3** (99,334640 % → 99,960860 %).
No la aplico porque `matched_code` es todo-o-nada y las dos palancas son andamio. (2) Lo que
**si** se aplica: el **orden del pool de constantes** es orden de sentencia, nadie lo habia
medido, y zEcstasy tenia **cinco transposiciones en cuatro funciones**; quedan **dos en dos**.
(3) Herramienta nueva y generica: `scripts/agent_ecs_pool.py`, que audita el pool funcion a
funcion en cualquier unidad.

---

## 1. La medida, antes y despues

| | ANTES (HEAD) | DESPUES |
|---|---|---|
| `fncmp` | 3 de 539 funciones, 4.384 B | **3 de 539, 4.384 B** (sin regresion) |
| `sha1` del `.o` | `6db23784f968c622399709e4a6ac10f1ec547dee` | `4c504a893dc32a5b61ba1ff921fe500f69004bd7` (3 compilaciones iguales) |
| `linkdelta` | `.text +0  rodata-632 data+64 bss+32` | **identico** |
| `trypromo` | DOL ROTO `4df2f741e83d` | DOL ROTO `139228feeb10` |
| bytes distintos del DOL | 489.803 | 489.805 (**+2**) |
| **pool transpuesto** (`agent_ecs_pool.py`) | **4 funciones / 5 transposiciones** | **2 funciones / 2 transposiciones** |

Las tres funciones abiertas siguen siendo las mismas y con las mismas cifras:
`epCalculateLocalDirectionalPOS16` (2.072 B, 164 insn), `UpdatePlatInfo` (2.044 B, 27 insn),
`eProject` (268 B, 13 insn). Las dos vetadas no se han tocado.

**Sobre el +2 de bytes distintos del DOL.** Es ruido de coincidencia, y hay precedente en esta
misma unidad: la r55 arreglo el dueno de tres simbolos —un arreglo correcto y comprobado
simbolo a simbolo— y ese mismo contador subio **+33**. Mientras la unidad no promociona su
`.rodata` esta desplazada entera, asi que contar coincidencias byte a byte a offset fijo no
mide nada. La medida que describe el defecto es la del pool, y esa baja de 5 a 2.

---

## 2. LA VEDA DE LA r49 ESTA ROTA

La r49 escribio: «LA FUNCION NO PUEDE LLEGAR AL 100 % Y ESO CIERRA EL FRENTE ENTERO. Las filas
333-339 (el bloque de literales) estan demostradas irreducibles». El argumento era: `prio(lis)`
3 > `prio(lfs)` 2 y hay dos unidades `iu2`, luego no hay forma de fuente que lo cambie.

**Hay dos.** Banco nuevo de ~2 s (`scripts/agent_ecs_upi.py`, compila SOLO `eLightE.cpp`, que
es autocontenido; reproduce la base al digito: 99,334640 % / 28 filas / 2.044 B):

| variante | pct | filas |
|---|---|---|
| base | 99,334640 | 28 |
| **`bar1`** = un `__asm__("")` tras `envmap_power = 6.0f` en el case | 99,745600 | **24** |
| **`bar1+minbc`** = + conmutar el producto de `envmap_min_b` | **99,960860** | **3** |

Con `bar1` desaparece el par INSERT/DELETE: el bloque pasa a tener el **orden y el numero de
instrucciones del objetivo**, y solo queda un `lis r9` donde el objetivo pone `lis r11`.

### El mecanismo, leido en la traza

`python scripts/schedtrace.py --file src/.../eLightE.cpp --like zEcstasy UpdatePlatInfo -dS
-fsched-verbose-5`, bloque 73:

    insn  prio cost  units
    508   3    1     iu2   : 525 511      (lis 6.0)
    511   2    2     lsu   : 525          (lfs 6.0)
    514   3    1     iu2   : 525 517      (lis 0.22)
    ...
    Ready list (t = 1):  520 514 508   -> 508 y 514 (dos iu2)
    Ready list (t = 2):  517 511 520   -> 520 (prio 3) y luego 511

`prio(lfs) = 2` porque **su unico dependiente dentro del bloque es el `b` final** (prio 1).
Y ahi esta la causa que la r48/r49 no nombraron: **este case es el UNICO cuyo cuerpo son tres
constantes y nada mas**. En los cases vecinos (`0x89946400`, `0x8812634B`) los `lfs` alimentan
un `fmuls`, suben de prioridad, y **el orden nos sale ya igual que el objetivo sin tocar nada**.
O sea: no es «`lis` gana a `lfs`», es que **a nuestros `lfs` les falta consumidor**.

Prueba directa del mecanismo, sin barrera: `sel_p6_ep` (`__asm__("" : "+f"(envmap_power))` tras
la sentencia) le da al `lfs` un dependiente y **produce exactamente el orden de sched1 del
objetivo** (`lis6, lis22, lfs6, lis86, lfs22, lfs86`), aunque a cambio desordena el reparto FP
(8 filas con `n33_2+minbc`).

### Por que NO lo aplico

1. `matched_code` es todo-o-nada: 3 filas valen **cero bytes**, igual que 28.
2. `minbc` esta **demostrado que no es la fuente original** (el objetivo emite `fmuls f26,f5,f10`
   y la forma conmutada emite `f26,f10,f5`; es una de las 3 filas que quedan).
3. La barrera es un `asm` de cero bytes, y la regla 6 dice que un `asm` que no paga es deuda.

Queda todo escrito en el comentario de `eLightE.cpp` (lo ve `previo.py UpdatePlatInfo`, que ya
lista `r56`), con las ~90 medidas negativas de la ronda.

### Las 3 filas que quedan, y que hace falta para cerrarlas

    98   fmuls f26,f5,f10 | f26,f10,f5      <- lo mete `minbc`
    333  lis r11          | lis r9          <- reparto GPR del pool
    334  lfs f2,(r11)     | lfs f2,(r9)

Para 333/334: `QTY_CMP_PRI` (`local-alloc.c:1568`) es
`log2(n_refs)*n_refs*size/(muerte-nacimiento)` — gana el rango **mas corto**, y el primero
cogido se lleva `r9` (`REG_ALLOC_ORDER` de rs6000 es `0, 9, 11, 10, 8, ...` y `r0` no vale de
base). Para que salga `r11, r9, r11` el orden **pre-sched2** tiene que ser
`lis6, lis22, lfs6, lis86, lfs22, lfs86`, con el `0.22` como rango mas largo y `6.0`/`0.86`
disjuntos. `bar1` acierta el orden final por otro camino y por eso deja el registro mal.

**La via viva**: `minbc` sobra el dia que se pueda quitar el pin `fr5`, porque **el pin es lo que
crea la preferencia de `f5`** que envenena al pseudo 115 (diagnostico de la r48). Sin pin la
banda volatil sale corrida un registro (`f5→f6→f7→f8→f5`, 57 filas con barrera): eso es
`nfsmw-local-de-mas-roba-registro` y **no se ha buscado con el DWARF**.

---

## 3. EL ORDEN DEL POOL DE CONSTANTES ES ORDEN DE SENTENCIA

`force_const_mem` mete cada literal en el pool **en el punto del expand**, y
`output_constant_pool` lo vuelca en orden de insercion. Luego **el orden de la `.rodata` de cada
funcion es el orden de sus sentencias**, y una transposicion delata una sentencia que falta.
`rodorden.py` compara **cadenas** y no mira esto; `reorden.py` da `.rodata CONTENIDO 0` (el
falso 0 del brief, 554 etiquetas sin pareja). Nadie lo habia medido.

Herramienta: **`python scripts/agent_ecs_pool.py <unidad>`**. Para cada funcion saca la
secuencia de valores de `.rodata` que tocan sus reubicaciones, en orden de direccion, y marca
`SOLO ORDEN` cuando la multiserie numerica es la misma y solo cambia el orden. (Aviso metido en
el propio script: el objeto extraido **fusiona** constantes anonimas en un solo `lbl_`, asi que
comparar tamanos de cadena da falsos positivos; por eso solo se comparan los numericos.)

### Los tres arreglos aplicados, todos de CERO bytes

| funcion | el objetivo crea | la sentencia que faltaba |
|---|---|---|
| `UpdatePlatInfo` | `0.0f` **entre** el `8.0f` y el `6.0f` | `diffuse_rng_a = 0.0f;` MUERTO en el hueco de lineas en blanco del `case 0x8812634B` |
| `eSin` | `6.28319, 0, 1, 3.14159, 1.5708` | las declaraciones de `pi`/`piover2` van **detras** de los dos `while` y del `flip_sign = 1.0f` |
| `__InitMatrices` | `0.0f` antes del `0.81f/0.84f` | `transx`/`transy` declaradas **arriba** con `= 0.0f` |

Las tres estan **verificadas byte a byte**: `fncmp` da las mismas 3 funciones antes y despues,
y las funciones afectadas ya casaban y siguen casando. El almacenamiento muerto se lo lleva
`flow` **despues** de que el expand haya creado la entrada del pool: por eso cuesta cero.

La evidencia de cada uno no es adivinanza:
* `UpdatePlatInfo`: el **case hermano** `0x2388DD82` trae escrita la misma sentencia
  (`diffuse_rng_a = 0.0f;`), y el hueco de lineas en blanco del `0x8812634B` ya estaba ahi.
* `eSin`: el DWARF declara `twopi, flip_sign, pi, piover2, result` y **ese orden no cambia** al
  mover las declaraciones detras de los `while` (las sentencias no salen en el DWARF).
* `__InitMatrices`: el DWARF del ORIGINAL da `fe_scale, transx, transy, gcn_scale` y **nosotros
  teniamos** `fe_scale, gcn_scale, transx, y_scale, height_scaled, transy`.

---

## 4. Lo medido y NO aplicado (con su cifra)

1. **`bar1+minbc`** en `UpdatePlatInfo`: 28 → **3 filas**, 99,334640 → **99,960860 %**, 2.044 B.
   Cero bytes de `matched_code` y dos andamios. Ver §2.
2. **La transposicion de cabecera de `UpdatePlatInfo`**, la unica que le queda:
   objetivo `1.06535e+09, 1, dbl`, nuestro `1, dbl, 1.06535e+09`. Siete formas medidas
   (`FloatAsInteger(1.0f)` en linea, `OneAsFloat` no const, declarado tarde, temporal vivo
   `specular_scale`, temporal vivo `envmap_scale`, producto delante y negado, sentencia muerta).
   **Solo una funciona** —una sentencia muerta con `SpecularPower * 1065353200.0f` delante de
   `OneAsFloat`— y **el DWARF la prohibe**: el original no tiene ninguna local de mas.
   Es orden de expansion de los operandos del `MINUS`, y no hay forma de fuente encontrada.
3. **`eRecalculateOthographicProjection`**: el objetivo crea el `dbl` de int→float antes del
   `1/540`. `dwbody.py` destapa que **la funcion es estructuralmente otra** aunque case byte a
   byte: el original usa `cFrontendDatabase::GetVideoSettings()` (que no tenemos; nosotros
   ponemos `GetUserProfile(0)->GetOptions()`), declara `WasPal50`/`WasWidescreen` como **`bool`**
   y no `int`, y sus locales son `transx, transy, transz, half_height, display_width, gcn_scale`
   —**sin `xscale` ni `yscale`**—. Es reescritura, no una linea; no la he hecho porque la funcion
   hoy casa y romperla cuesta mas que la transposicion.
4. **`__InitMatrices` sigue con dos locales de mas** (`y_scale`, `height_scaled`) contra el DWARF.
   Hoy no cuestan un byte.
5. **`agent_ecs_pool.py` en las otras 26 unidades**: no lo he corrido (regla de propiedad). Si en
   zEcstasy habia 5 transposiciones en 142 funciones con pool, el frente entero merece el barrido.

---

## 5. Sorpresas, todas medidas

* **El `.o` cambia de `sha1` con la fuente IDENTICA.** Dos veces: `01c03f3c → 6db23784` con
  `eLightE.cpp` en HEAD, y `650bdc09 → ec820e7f` con mis tres ficheros restaurados byte a byte.
  Tres compilaciones seguidas dan **siempre** el mismo sello, y `fncmp`/`linkdelta`/pool salen
  identicos. Es **deriva ambiental**: somos siete agentes sobre el mismo arbol y alguien toca una
  cabecera compartida. **El aviso del brief se cumple, pero la causa no es `build_direct`**: es
  el arbol moviendose debajo. Corolario: un `sha1` distinto **no** invalida por si solo una
  medida; hay que contrastar con `fncmp`.
* **Con `-gdwarf+`, anadir COMENTARIOS cambia el `sha1` del `.o`** (los numeros de linea van en
  el DWARF) y **no cambia el DOL** (mismo `139228feeb10` antes y despues de los comentarios).
* **`linkdelta` de zEcstasy se movio de `rodata-416` a `rodata-632` sin que yo tocara nada**:
  comprobado restaurando HEAD y volviendo a medir. La cifra `-416` del dossier esta **caducada**.
* **`trypromo.py` no acepta la ruta completa del `.cpp`** que pide el encargo: con
  `src/Speed/Indep/SourceLists/zEcstasy.cpp` contesta «NO esta en la lista de enlace»; hay que
  pasarle `Speed/Indep/SourceLists/zEcstasy`.
* Un `__asm__("")` sin operandos de salida **es volatil por definicion** en GCC: `bar1` y `barv`
  dan el mismo objeto al byte.

---

## 6. Herramientas que dejo en `scripts/`

| | |
|---|---|
| `agent_ecs_upi.py` | banco de ~2 s para `UpdatePlatInfo` (compila solo `eLightE.cpp`); `--rows` lista las filas distintas |
| `agent_ecs_combo.py` | ~45 parches combinables con `+` sobre `eLightE.cpp`; `--list` |
| `agent_ecs_sweep.py` | barrido de tres ejes (barrera x pin x n33) |
| **`agent_ecs_pool.py`** | **auditoria del orden del pool, funcion a funcion, en CUALQUIER unidad** |

## 7. Lo siguiente para zEcstasy

1. **Quitar el pin `fr5`** de `envmap_min_scale`: es el que crea la preferencia que envenena al
   pseudo 115 y obliga a `minbc`. Antes hay que explicar la banda volatil corrida un registro
   con el DWARF (`nfsmw-local-de-mas-roba-registro`), que nadie ha mirado.
2. Cerrar las dos transposiciones que quedan (§4.2 y §4.3).
3. Las dos vedas (`epCalculate`, `eProject`) siguen en pie y **no las he tocado**.
