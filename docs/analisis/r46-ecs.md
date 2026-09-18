# R46 — `zEcstasy` + `zEagl4Anim`

## Resultado

**Cerrada `FnStatelessQ::EvalSQT` al 100 %: 1.480 B.** `zEagl4Anim` pasa de 4
funciones y 5.008 B pendientes a **3 funciones y 3.528 B**. `zEcstasy` queda
byte a byte como entró (4 funciones, 5.180 B); sus dos ficheros sólo llevan
comentarios de veda.

```text
python scripts/fndiff.py zEagl4Anim EvalSQT__Q29EAGL4Anim12FnStatelessQfPfPCQ29EAGL4Anim8BoneMask
# EvalSQT__Q29EAGL4Anim12FnStatelessQfPfPCQ29EAGL4Anim8BoneMask  target=100.0%  ours=100.0%  size=1480/1480
```

| unidad | antes | después |
|---|---|---|
| `zEcstasy` | 4 fn / 5.180 B | 4 fn / 5.180 B (idénticas) |
| `zEagl4Anim` | 4 fn / 5.008 B | **3 fn / 3.528 B** |

Ficheros tocados, los tres de territorio: `FnStatelessQ.cpp` (el cierre),
`eagl4supportdlopen.cpp` y `EcstasyEx.cpp` (**sólo comentarios**; verificado con
un censo SHA-256 función a función del `.o`: **0 de 646 funciones de `zEcstasy`
cambian de bytes**). Sin commit, sin `ninja`, sin tocar `configure.py`,
`config/GOWE69/*` ni `splits.txt`.

---

## 1. `EvalSQT` (1.480 B) — CERRADA. La extensión que no se puede quitar en C

### 1.1 El diagnóstico, ahora por RTL

Sobraban dos `clrlwi r0, r0, 16` idénticos (1.488 contra 1.480 B) y eran las
**únicas** diferencias. Volqué el RTL con `-da` sobre `FnStatelessQ.cpp`
**compilado solo** —reproduce la función byte a byte, salvo los campos
reubicados de las ramas, en **0,4 s** en vez de los 4 s de la unidad— y el
`.combine` lo dice literal:

```text
(insn 157 (set (reg:SI 144) (plus:SI (subreg:SI (reg:HI 628) 0) (const_int -1))))
(insn 158 (set (reg:SI 145) (zero_extend:SI (subreg:HI (reg:SI 144) 0))))
(insn 159 (set (reg:CC 146) (compare:CC (reg/v:SI 108) (reg:SI 145))))
```

La **insn 157 ya es exactamente el `subi r0,r5,1` del objetivo**. Lo único que
sobra es la 158, que es la truncación que pide el `static_cast<unsigned short>`.

El pseudo 628 lo crea PRE, y tiene **cinco definiciones: tres por `lhz` y dos
por COPIA** (`(set (reg:HI 628) (reg:HI 119))` en el bloque del `else if` y
`(set (reg:HI 628) (reg:HI 590))` en el de `mNumKeys - 2`). Una definición por
copia envenena `reg_nonzero_bits` en `set_nonzero_bits_and_sign_copies` —que
corre con `nonzero_sign_valid == 0`, así que `nonzero_bits` de un REG devuelve
la máscara entera—, y por eso `combine` no puede tirar el `zero_extend`. En el
mismo volcado, donde el pseudo tiene **una sola definición por carga**
(`mNumKeys - 2`, insns 1518/1520) sale `(plus (subreg:SI (reg:HI 590)) -2)` sin
extensión ninguna y casa.

**En C no hay salida**: con el cast la truncación va detrás de la resta (2
filas), sin el cast la extensión va delante (17 filas). Una forma pide truncar y
la otra extender.

### 1.2 La palanca: un `asm` que consume el campo con restricción `"r"`

```cpp
{
    int lastKey;
    __asm__("subi %0,%1,1" : "=r"(lastKey) : "r"(statelessQ->mNumKeys));
    if (floorKey >= lastKey) {
        slerpReqd = false;
    }
}
```

El operando de entrada tiene tipo `unsigned short`, o sea **modo HImode**: GCC
le pasa el registro **crudo** del `lhz` (r5) sin extenderlo, porque el `asm` no
pide SImode. La instrucción que emite es la que el objetivo tiene ahí, así que
**no es deuda**: el tamaño pasa de 1.488 a 1.480 B exactos y el diff a cero.

**Regla nueva para el árbol.** Cuando sobra una extensión de cero de un campo de
media palabra (`clrlwi rX,rY,16`), y el pseudo que la alimenta tiene
definiciones por copia (las mete PRE), **no lo intentes con formas de fuente**:
un `asm` cuyo operando de entrada sea el campo con su tipo estrecho recibe el
registro sin extender, y si la operación que hace falta es una sola instrucción
que el objetivo ya tiene, el `asm` la emite y la función casa. La firma para
buscarlo en otras unidades es `triaje.py` con **`sobra` > 0 y `regs = falta =
otro = 0`** más una diferencia de tamaño múltiplo de 4.

### 1.3 Los negativos, con cifras (r46; base 99,45946 % / 2 filas / 1.488 B)

| forma (en los DOS sitios salvo donde diga) | % | filas | tamaño |
|---|---:|---:|---:|
| local `int` + barrera `"+r"` | 98,4162 | 17 | 1.488 |
| local `int` sin barrera | 98,0514 | 17 | 1.488 |
| local `unsigned int` + cast a int | 98,0514 | 17 | 1.488 |
| sin cast | 98,0514 | 17 | 1.488 |
| `floorKey + 1 >= mNumKeys` | 97,3108 | 36 | 1.492 |
| local `int(nk-1)` + barrera | 97,8892 | 18 | 1.488 |
| `asm("" ::: "memory")` delante | 95,7135 | 30 | **1.460** |
| ídem volátil | 95,7135 | 30 | 1.460 |
| `asm("" ::: "memory")` + cast | 95,4432 | 31 | 1.464 |
| `asm("" ::: "memory")` sólo en el 2.º | 96,9162 | 26 | **1.480** |
| `asm("" : "+m"(mNumKeys))` sin cast | 95,7135 | 30 | 1.468 |
| ídem sólo en el 2.º | 96,9162 | 26 | 1.480 |
| renombrar la base `asm("" : "+r"(statelessQ))` | 94,6189 | 35 | 1.484 |
| copia local de `statelessQ` + barrera | 94,4162 | 37 | 1.484 |
| type-pun `asm("" : "=r"(nk) : "0"(campo))` | 98,4162 | 17 | 1.488 |
| type-pun con local `unsigned short` | 98,4162 | 17 | 1.488 |
| local `unsigned short` + `"+m"` + cast | 98,0514 | 17 | 1.488 |
| `asm("subi")` con local `unsigned short` intermedia | 99,0946 | 7 | 1.488 |
| `asm("subi")` sólo en el 1.º sitio | 99,2973 | 3 | 1.488 |
| **`asm("subi")` con el campo directo, los dos sitios** | **100,0000** | **0** | **1.480** |

Aviso: las dos variantes de barrera de memoria bajan a **1.460/1.468 B**, por
debajo del objetivo: matan la PRE entera (se van los cuatro `lhz`/`mr` que el
objetivo sí tiene). Un tamaño más pequeño que el objetivo no es «casi».

---

## 2. `DynamicLoader::Initialize` (2.352 B) — los 4 B están en la FORMA DEL ÁRBOL del `switch`

No cerrada. Base 98,87585 % / 28 filas / 2.356 contra 2.352 B. Localizados los
4 B: el árbol de decisión del `switch (sheader->sh_type)`.

```text
objetivo   cmpwi 8/beq FIN | cmplwi 8/ble BAJO | <ALTO> | b FIN |
           BAJO: cmpwi 3/beq | cmplwi 3/bgt FIN | cmplwi 1/ble FIN |
                 y CAE al cuerpo de SHT_SYMTAB                        (14 insn)
nuestro    cmpwi 8/beq FIN | cmplwi 8/bgt ALTO |
           BAJO: cmpwi 3/beq | cmplwi 3/bgt FIN | cmpwi 2/beq SYMTAB |
           b FIN | <ALTO> | b FIN                                     (15 insn)
```

Dos cosas a la vez, y las dos vienen de `emit_case_nodes` (`stmt.c`):

1. El objetivo emite **el subárbol ALTO en línea y salta al BAJO**; nosotros al
   revés. La nuestra es la rama «ninguno de los dos hijos está acotado», que
   emite siempre el izquierdo primero.
2. Su hoja del `case 2` usa la forma de **RANGO** (`LT node->low -> default`,
   que sale `cmplwi 1; ble` porque el valor es 2, y después cae al cuerpo); la
   nuestra usa la de **valor único** (`beq cuerpo` + el `b default` del padre),
   que gasta una instrucción más.

**Medido y negativo (8 variantes sobre la unidad + 31 combinaciones sobre un
micro-fichero con los mismos cflags, 0,3 s cada una).** Ninguna emite un `ble`
detrás del `cmplwi 8`:

| variante | tamaño | filas |
|---|---:|---:|
| base | 2.356 | 28 |
| partir `SHT_RELA ... SHT_NOTE` en cuatro `case` | 2.356 | 28 (objeto idéntico) |
| partirlo en dos rangos | 2.356 | 28 (idéntico) |
| `case SHT_LOPROC+5:`/`+6:` sueltos | 2.356 | 28 (idéntico) |
| `case SHT_NOBITS:` sin rango | 2.356 | 28 (idéntico) |
| partir `SHT_LOPROC+7 ... SHT_HIUSER` en dos | 2.356 | 28 (idéntico) |
| `+ case SHT_NULL ... SHT_PROGBITS` (o `0`, o `1`) | 2.364 | 32 |
| `+ case SHT_SHLIB` / `10..11` / `SHT_LOPROC..+4` | **2.320** | 40 |
| `+ case 12 ... SHT_LOPROC-1` | 2.336 | 44 |
| `+ SHLIB..LOPROC+4` sobre la unidad | 2.320 | 40 |

**Hallazgo colateral útil**: GCC **funde los `case` contiguos con la misma
etiqueta**, así que partir un rango en `case` sueltos da el objeto idéntico; y
si el cuerpo de un `case` queda vacío y coincide con el del `default`, el nodo
puede desaparecer del árbol entero (en el micro, poner `break` en `case 4...7`
colapsa el árbol de 21 a 6 instrucciones).

**Por qué no sirve mover cases**: el pivote lo fija `balance_case_nodes` con
`i = (nodos + rangos + 1) / 2`; con 7 nodos y 3 rangos, `i = 5` y la caminata
para en el nodo `{8}`. Cualquier `case` nuevo por debajo de 8 mueve el pivote y
rompe todo el árbol. Los 24 `regs`/`otro` restantes (rotación r29/r30 alrededor
de `strlen`/`memmove` y el orden de operandos de `lwzx r4,r11,r29`) son
posteriores y no se pueden juzgar hasta que el árbol case.

---

## 3. `epCalculateLocalDirectionalPOS16` (2.072 B) — el marco son 8 B de UNA ranura huérfana

No cerrada (93,305 % / 155 filas). Lo que sí queda cerrado es **qué son los 8 B
de marco de más** (`stwu r1,-0x178` contra `-0x170`), que cuatro rondas daban
por «ranura secundaria huérfana» sin medirlo.

Censo de **todas** las ranuras con base `r1` de los dos objetos:

```text
0x08 0x0c 0x10 0x18 0x20 0x28 0x30 0x38 0x40 0x4c 0x50 0x58 0x60 0x6c 0x74 0x7c
       IDENTICAS en los dos, y coinciden UNA A UNA con las que el DWARF le da al
       original (specular_power, specular_bias, dmin_rg ... hack_scale,
       envvdotn, specular_a).
temporal de conversion int<->float (stw+stw+lfd y stfd+lwz, 8 B):
       objetivo 0x88   nuestro 0x90
CR:    objetivo 0x94   nuestro 0x9c
stmw:  objetivo 0x98   nuestro 0xa0    (el juego de preservados es IDENTICO)
```

O sea: **no falta ni sobra ningún local con nombre, y no sobra ningún
preservado**. Los 8 B son una ranura de 8 B en `0x80..0x8f` que **ninguna
instrucción referencia** — el objetivo tiene una y nosotros dos. Es un
`assign_stack_temp` de más que sobrevive a que su código desaparezca, no un
problema de reparto.

Ensayos r46, todos con **el mismo binario** (155 filas, marco 0x178):

| forma | resultado |
|---|---|
| partir `specular_a = my_fpow(...) * 0.5f` en dos sentencias | idéntico |
| partir `t1 = t0*pow_scale + pow_bias` dentro de `my_fpow` | idéntico |
| `int it1 = (int)t1; *(int*)pt = it1;` | idéntico |
| sacar el `merge00` que alimenta `my_fpow3` a su sentencia | idéntico |
| `V2(arn_AlphaMin)` / `V2(alpha_rng)` en vez de los casts `(sn_ps)` | 90,15 / 214 filas, marco 0x178 |
| sólo `V2(arn_AlphaMin)` | 93,19 / 156, 0x178 |
| sólo `V2(alpha_rng)` | 91,05 / 211, 0x178 |

Ablaciones (rompen la semántica, sólo miden el marco):

| ablación | marco |
|---|---|
| sin la llamada a `my_fpow` | 0x140 |
| sin `my_fpow3` | 0x150 |
| sin el bloque `if (arn_HackAlpha)` | **0x170** — falso positivo |
| sin el `if (IsInGame())` interno de `hack_scale` | 0x178 |

El 0x170 del bloque `arn_HackAlpha` **es un falso positivo**: al quitarlo muere
`alpha_rng` y con ella su ranura de 0x4C; el marco baja por eso. Lo confirma que
las dos variantes `V2()` del mismo bloque, que conservan `alpha_rng`, siguen en
0x178.

**Siguiente paso acotado**: bisecar el cuerpo del bucle interno por sentencias
mirando sólo el `stwu`. La ranura huérfana tiene que venir de una sentencia con
**dos temporales de 8 B vivos a la vez** (`free_temp_slots` sólo corre al final
de cada sentencia); las candidatas son las dos `(sn_ps)vdotn` del `ps_sel` y las
dos del `merge00` que alimenta `my_fpow3`.

---

## 4. `GenerateHorizonFogDisplayList` (796 B, 2 filas) — eje nuevo, negativo

El muro está medido desde la r30: `prio(srawi) = 12` contra `prio(andi.) = 2` en
el mismo ciclo, y `rank_for_schedule` mira la prioridad antes que nada. La r36f
descartó retrasar el `srawi` y subir la prioridad del `andi.`.

**Lo que faltaba probar y he probado**: emitir el `srawi` **desde un `asm`**, por
si un `asm_operands` —que no casa ninguna unidad funcional de haifa— rompiera el
desempate. No lo rompe:

| forma | % | filas | tamaño |
|---|---:|---:|---:|
| base | 98,99497 | 2 | 796 |
| `srwi`/`add` en C + `asm("srawi %0,%1,1")` | 98,9447 | 3 | 796 |
| ídem con restricción de emparejar `"0"` | 98,9447 | 3 | 796 |
| la división entera completa en un solo `asm` | 96,9598 | 7 | 796 |

Las tres mantienen los 796 B (el `asm` emite las mismas instrucciones) pero
ninguna adelanta el `andi.`. Veda ampliada.

---

## 5. Lo que NO he tocado, y por qué

- **`UpdatePlatInfo`** (2.044 B, 27 filas): `triaje` da `regs=26`, y la r36f dejó
  86 medidas en los dos ejes que faltaban (pines limpios del ciclo de tres,
  barrido del registro del pin de `envmap_min_scale`, corte de `sched1`, segunda
  salida en los tres `asm` existentes). Lo único que baja de 28 sigue siendo
  mover `envmap_power = 6.0f;` dentro del `case`, que reordena el pool `$LC`.
- **`eProject`** (268 B, 13 filas): 33 medidas en la r36f con la receta de
  `Render`, ninguna baja de 14 filas.
- **`FindMatchTime`** (720 B, 11 filas): la diferencia es que el objetivo carga
  el sesgo `0x4330000080000000` **antes** del primer `bso` y nosotros dentro del
  `if`; r38 y r43 agotaron locales, unión, barreras y el inicializador. No hay
  forma C nueva sustentada por la evidencia; hace falta una palanca sobre la
  constante que **genera** la conversión, no sobre una constante C (medido: GCC
  no comparte el literal y lo duplica, 736 B).
- **`EvalState`** (456 B): reproduje la hipótesis 1 de la r43
  (`GetKeyData(0) + idx * GetKeySize()` en las cuatro llamadas) y sale
  **95,263 % / 48 insn**, no el 97,76 % / 21 insn que dice el informe de la r43;
  la base de aquella ronda no era ésta (el `int keyIdx = mKeyIdx;` de ahora ya
  estaba dentro). Retirada. Respetada la veda de la r44 (helper privado con los
  accesores públicos: 66,89 % / 492 B, salva r28).

---

## 6. Propuestas fuera de territorio

1. **Barrer el árbol entero con la firma de `EvalSQT`.** `triaje.py` la marca
   sola: `sobra > 0` con `regs = falta = otro = 0` y diferencia de tamaño
   múltiplo de 4. Cuando lo que sobra es un `clrlwi rX,rY,16` (o un `extsh`)
   sobre un campo de media palabra, la salida es el `asm` con el operando de
   entrada **del tipo estrecho**, no una forma de fuente. En la instantánea de
   hoy, los candidatos con `sobra` y pocas filas fuera de mi territorio son
   `Setup__Q26Speech13RoadblockFlow` (596 B, `falta=1 sobra=1 otro=1`),
   `ProcessUpdate__12CARSFX_Turbo` (672 B, `falta=2 sobra=2`) y
   `EXI2_WriteN` (432 B, `falta=3 sobra=3`) — merece un `fndiff` cada uno antes
   de gastar formas de fuente.
2. **`-da` sobre el fichero SUELTO como herramienta de diagnóstico.** Muchas
   unidades de SourceList tienen ficheros que compilan solos con los cflags de
   la unidad y reproducen la función **byte a byte** (verificado aquí:
   `FnStatelessQ.cpp`, 0,4 s contra 4 s). Los volcados de GCC 2.95 salen en
   `%TEMP%` con base `<temporal>.tmp.<pase>` y hay que copiarlos por
   `mtime` justo después de compilar. Es la diferencia entre adivinar una forma
   de fuente y leer la insn que sobra. Ojo: `EcstasyEx.cpp` **no** compila solo
   (depende de símbolos de la SourceList).
3. **Añadir a `triaje.py` una columna con la diferencia de TAMAÑO.** Hoy hay que
   cruzarla con `fncmp` a mano, y es justo la que separa «sobra una instrucción»
   de «sobra un registro».

---

## 7. Verificación

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEcstasy Speed/Indep/SourceLists/zEagl4Anim
  -> 2 ok, 0 fallidas
python scripts/fncmp.py Speed/Indep/SourceLists/zEcstasy
  -> 4 de 539 distintas, 5.180 B   (IGUAL que a la entrada)
python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  -> 3 de 318 distintas, 3.528 B   (entrada: 4 y 5.008 B)
python scripts/fndiff.py zEagl4Anim EvalSQT__...8BoneMask
  -> target=100.0%  ours=100.0%  size=1480/1480
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim   -> 315 ok, 0 FALLA
python scripts/audit.py Speed/Indep/SourceLists/zEcstasy     -> 0 FALLA
python scripts/lcfix.py --check
  -> todas las entradas @lc estan al dia
```

Además, censo SHA-256 **función a función** del `.o` de `zEcstasy` antes y
después: **646 funciones, 0 con bytes distintos**. Ninguna función de las dos
unidades empeora. Bancos y artefactos reproducibles en `scratchpad/r46_ecs/`
(`bench.py`, `elf.py`, `ver.py`, `t_sq*.py`, `t_dl*.py`, `t_ep*.py`, `t_hf.py`,
`t_es.py`, `micro/`, `rtl/`).
