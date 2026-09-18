# r36d — zWorld / zWorld2 (5 funciones, 8.416 B)

Base verificada al empezar con `build_direct` + `fndiff`, y al terminar:

```
                                        BASE                    AHORA
 2980 zWorld2  HolePunchAvoidables      98,133 %  55 difs       igual
 2908 zWorld   RenderFlaresOnCar        98,618 %  18 difs       igual   (2908/2912)
  876 zWorld   UpdateWheelYRenderOffset 99,384 %   7 difs       igual   (876/872)
  836 zWorld   CullParts                99,426 %   5 difs       igual   (836/832)
  816 zWorld2  InitAtSegment            99,412 %  21 difs   ->  99,559 %  15 difs
```

`pctsnap --cmp` sobre las dos unidades (939 funciones): **1 mejora, 0 empeoran**.
`lcfix.py --check` limpio. Dos ficheros tocados —`WRoadNetwork.cpp` (3 líneas de
código y comentarios) y `CarRender.cpp` (**sólo comentarios**)—. Sin commits.

---

## 0. Lo que se lleva la ronda

1. **Palanca nueva y reutilizable: la CANTIDAD FANTASMA.** Un par de `asm`
   extendidos **no volátiles** sobre una *variable de registro local* fabrica un
   rango de vida en el registro que quieras **sin emitir una sola instrucción**.
   Es exactamente lo que la r36c buscó sin encontrar («una cantidad más, que se
   lleva r28 y no emite ni una instrucción») y cerró el racimo entero de
   r26/r27/r28 de `InitAtSegment`. §1
2. `InitAtSegment` pasa de **21 a 15 diferencias** y de 99,412 a **99,559 %**,
   con el tamaño exacto (816/816) y **sin mover ninguna otra función**. §1
3. **La regla de cierre de la cantidad fantasma**, que costó una medida falsa:
   el segundo `asm` tiene que consumir **además** un valor que se use después.
   Si sólo toca la propia variable, DCE se lleva la pareja entera y el efecto es
   **cero, sin aviso**. §1.2
4. **Por qué el racimo que queda en `InitAtSegment` no se arregla bloqueando
   registros**, con la demostración: los dos rangos se solapan, así que cualquier
   bloqueo se los come a los dos. Hace falta invertir la PRIORIDAD de
   `local_alloc`. §2
5. **El permutador ciego encuentra ese racimo y hay que RECHAZARLO**: la variante
   ganadora (14 difs) falsea el orden del original, y `lmap` lo prueba al byte.
   Es el aviso de `nfsmw-permutador-guiado` cumpliéndose. §3
6. **`HolePunchAvoidables`: el diagnóstico queda cerrado.** `regmap` lo declara
   permutación pura (85 locales iguales, 4 registros), el racimo cr2/cr3 es
   `allocno_compare` empatando y la única diferencia ESTRUCTURAL que queda es la
   reasociación, con la medida al día. §4
7. Once vedas nuevas medidas (§6).

---

## 1. La cantidad fantasma

### 1.1 El problema, y la receta

La r36c dejó escrito el diagnóstico exacto y sin solución:

> «El objetivo reparte r31, r30, r29, `(r28 vacío)`, r27, r26, r25, r24 — o sea
> que su `local_alloc` tenía **una cantidad más** que la nuestra, una que se
> lleva r28 y no emite ni una instrucción. No he encontrado con qué fabricarla.»

Se fabrica así:

```c
{
    register int guard asm("r28");
    asm("" : "=r"(guard));            // la define
    ...                               // el rango que quieres ocupar
    float endOffset = profile->GetRawLaneOffset(lane);
    asm("" : "+f"(endOffset) : "r"(guard));   // la consume, junto a un valor VIVO
    ...
}
```

Los dos `asm` tienen plantilla vacía y salida, así que **no son volátiles** y no
son barrera de planificación (que es lo que hundió los seis intentos de clobber
de la r36c, entre 36 y 63 diferencias). El único efecto es que r28 está vivo
entre los dos puntos. Coste: **cero bytes**.

Resultado, medido con `fndiff` sobre la unidad completa:

| | difs | fuzzy | tamaño |
|---|---|---|---|
| base r36c | 21 | 99,412 % | 816/816 |
| **con la cantidad fantasma** | **15** | **99,559 %** | **816/816** |

Caen de golpe las seis filas del racimo (103, 107, 131, 152, 157, 172):
`fProfiles@ha` pasa de r28 a **r27** y la constante `0x4330` de r27 a **r26**,
que es el reparto del objetivo. Gana en las tres medidas a la vez.

### 1.2 La regla de cierre (una medida falsa la delató)

**El segundo `asm` debe consumir además un valor que se use más abajo.** Con
`asm("" : "+r"(guard))` a secas la salida no se usa nunca, DCE se lleva ese `asm`,
y entonces la salida del primero tampoco se usa y se lleva la pareja completa: el
objeto sale **byte a byte idéntico a la base** y parece que la palanca «no vale».
Lo medí sin darme cuenta en `UpdateWheelYRenderOffset` (7 difs antes y después,
exactamente los mismos). En `InitAtSegment` funcionó porque `endOffset` se usa en
la línea siguiente.

### 1.3 Dónde vale y dónde no (tres extrapolaciones, dos negativas)

Probé la palanca en las otras tres funciones. **La regla que sale es la
dirección de la presión**:

| función | qué quiere el objetivo | la cantidad fantasma |
|---|---|---|
| `InitAtSegment` | **más** presión (una cantidad más que nosotros) | **cierra el racimo** |
| `UpdateWheelYRenderOffset` | **más** presión (dos pseudos `@ha` del 0,0f vivos) | va en la dirección buena pero **se come el registro** que el segundo `@ha` necesita: arregla las filas 85/97/98 y rompe nueve del preencabezado (10 difs) |
| `RenderFlaresOnCar` | **menos** presión (un solo `@ha` izado 330 filas) | dirección contraria, ni se intenta |
| `CullParts` | otra cosa (una decisión de CSE, no de reparto) | manda `Polarity` a r10, no a r0 (9 difs) |

O sea: **la cantidad fantasma sólo paga cuando al objetivo le sobra una cantidad
respecto a nosotros.** Se sabe antes de probar, contando registros preservados
usados en cada lado (`grep -oE '\br(2[4-9]|3[01])\b'` sobre los dos `.s`).

---

## 2. Lo que queda en `InitAtSegment` y por qué no se bloquea

Quedan 15 filas en dos racimos:

```
 12,17,20  mulli r8 / lwz r10 / add r25,r10,r8   <- coste del pin asm("r25") de segment
 87,88,91,100,102,106,110,116..120  fNodes<->@ha del 0,0f: r10 <-> r11   (12 filas)
```

El racimo grande es **una inversión de prioridad de `local_alloc`**, no un
bloqueo. Los dos rangos son:

```
fNodes        filas 87..120   (5 referencias)
LC599@ha      filas 88..91    (2 referencias)   <- el 0,0f de SetLaneOffset
```

**Se solapan.** Cualquier registro que bloquees en la ventana 88-91 se lo quitas
a los dos a la vez, así que ni la cantidad fantasma ni un clobber pueden hacer
que uno tome r10 y el otro r11: hay que conseguir que `local_alloc` reparta
**fNodes primero**. En nuestra compilación reparte antes el corto (`LC599@ha`),
que es lo que dicta una prioridad por densidad `n_refs/longitud`.

Aviso para quien lo retome: `local_alloc` corre **después de `sched1`**, así que
los rangos que decide no son los del `.s` final (que ya pasó por `sched2`).
Razonar sobre el ensamblador de salida induce a error; el volcado bueno es
`cc1plus -dl` / `-dg` (receta en `r36c-wld.md` §2).

---

## 3. El permutador ciego: encuentra el racimo y hay que rechazarlo

Lancé el permutador **ciego** (`regmap` sólo señala la local del pin, que es
intencionada, así que el guiado iría en contra — es la recomendación de la r36c).

**Ronda 1, variante `move_stmt:move-3@13`: `SetLaneOffset(0.0f)` tres sentencias
más arriba, entre `fStartPos` y `fEndPos`.** Medido con `fndiff`: **14 difs**,
tamaño 816/816, y **el racimo de doce filas del `@ha` desaparece entero**. Es la
única variante que lo mueve de 380 probadas, y 690 más en profundidad 2 no
mejoraron sobre ella.

**Se rechaza.** `lmap` sobre el objetivo:

```
80301A28  stb r24, 0x2c3(r31)   WRoadNetwork.h:525
80301A2C  stb r24, 0x2c1(r31)
80301A30  stfs f0, 0x2c8(r31)   WRoadNetwork.h:530     <- SetLaneOffset
80301A34  stfs f0, 0x2c4(r31)
80301A38  stfs f0, 0x2cc(r31)
```

Los tres `stfs` del objetivo van **detrás** de los dos `stb`; el original tiene
`SetLaneOffset(0.0f)` al final, como nosotros. La variante los sube 17 filas y
mete 3 INSERT + 3 DELETE en su sitio. Es exactamente el aviso de
`nfsmw-permutador-guiado`: *«al aceptar una variante del permutador, exigir que
el orden siga casando con el mapa de líneas»*. Además el fuzzy **bajaba** 2,6
puntos (99,412 → 96,814) mientras las diferencias bajaban de 21 a 14, porque los
INSERT/DELETE puntúan cero.

Lo que sí deja es el **mecanismo**: la posición de la sentencia que crea el
pseudo del literal 0,0f decide el par r10/r11. Quien encuentre cómo invertir la
prioridad sin mover la sentencia se lleva las doce filas.

---

## 4. `HolePunchAvoidables` (2.980 B, 55 difs): diagnóstico cerrado

`regmap --all` da el veredicto más limpio que se le puede pedir a esta función:

```
MISMO conjunto de locales y mismo arbol de bloques; solo cambia el reparto en 4.
(85 iguales, 4 con registro distinto, 0 de ambito equivocado, 0 solo del
 original, 0 solo nuestras)

    avoidable_delta_offset     f10 -> f11
    offset_change              f10 -> f11
    avoidable_half_width        f9 -> f8
    hole_punch_safety_margin    f4 -> f6
```

En el original esas tres comparten f10 con `avoidable_offset`; nosotros las
partimos en f10 + f11.

### 4.1 El racimo cr2/cr3 es SÍNTOMA, no objetivo

Con el volcado `-dg` de `cc1plus` (las tres comparaciones izadas son allocnos
**globales**):

```
;; Register dispositions:  ... 334 in 71 ... 505 in 72 ... 901 in 70 ...
;; N regs to allocate:  ... 505 ... 334 901 ...
```

`505` = `is_traffic` → cr4(72), `334` = `is_racer` → cr3(71), `901` = `is_drag`
→ cr2(70), **en ese orden de prioridad**; el `REG_ALLOC_ORDER` de los CR
preservados es cr4, cr3, cr2. El objetivo reparte is_traffic, **is_drag**,
is_racer. Las dos que bailan tienen el **mismo `n_refs` (2)** y el **mismo rango
(todo el bucle, porque las dos están vivas en el arco de retroceso)**, así que
`allocno_compare` **empata** y lo que decide es el desempate del `qsort`. No hay
forma de tocarlo desde la fuente sin mover la llamada a `GRaceStatus::IsDragRace()`,
que es la veda de la r36b (−5,4 puntos). **Cae solo cuando cierre lo demás.**

### 4.2 La reasociación: remedida, y lo que revela

`approach_time * 0.2f * delta_offset` (la forma que da el árbol del objetivo)
sigue costando: **55 → 64 difs, 98,133 → 97,919 %**, tamaño igual 2980/2980.

Pero lo que hay que mirar es el racimo de las filas 619-648, **la única
diferencia ESTRUCTURAL de la función**: con la reasociación deja de tener
INSERT/DELETE y se convierte en una **permutación pura de flotantes de scratch**
(f4/f5/f7/f8/f9/f10). `fmuls f31,f31,f7` cae en su fila 639 y
`fmadds f1,f31,fX,f15` en la 642, las dos del objetivo. Quien la retome, que
parta de ahí y ataque la permutación, no la estructura.

La cantidad fantasma **no vale aquí**: `fr11` es scratch de todo el bloque y
bloquearlo cuesta 73 difs (rango largo) o 75 (rango corto).

---

## 5. Las otras tres, al día

- **`RenderFlaresOnCar`** (2.908 B, 18 difs, 2908/2912): sin cambios. El objetivo
  iza **un** pseudo `@ha` de `lbl_8040AD04` en la fila 123 y lo mantiene 330
  instrucciones; nosotros materializamos dos. Nos sobra un `lis`, o sea que el
  objetivo tiene **menos** presión que nosotros: la cantidad fantasma va en la
  dirección contraria y no se prueba.
- **`UpdateWheelYRenderOffset`** (876 B, 7 difs, 876/872): sin cambios. Ver §1.3
  y las dos vedas nuevas. Confirmado con `lmap`: el `@ha` que el objetivo duplica
  es el de la comparación del **width** (línea 3278 del original, r16), mientras
  el ternario del `<= 0.0f` (3248) y el del **radius** (3288) comparten r19.
- **`CullParts`** (836 B, 5 difs, 836/832): sin cambios, pero con un hallazgo
  parcial. **`register int polarity asm("r0")` SÍ mete `Polarity` en r0** y casa
  las tres filas 95/97/144 —era el diagnóstico de la r36b y ahora está probado—,
  pero GCC **reserva r0 para toda la vida de la variable de registro local**, así
  que la carga de `NumPlanes` ya no puede reusarlo (que es justo lo que hace el
  objetivo) y encima `n = 0` sale como `mr r31,r0`. Neto 5 → 9 difs. **La forma
  que falta es un pin de ÁMBITO CORTO**, que muera justo detrás del `cmpwi`; no
  he encontrado cómo escribirlo sin romper el `else if`.

---

## 6. Vedas nuevas, medidas

Todas con `build_direct` + `fndiff` sobre la unidad completa.

| función | ensayo | resultado |
|---|---|---|
| `InitAtSegment` | permutador ciego: `SetLaneOffset(0.0f)` 3 sentencias arriba | 14 difs / 96,814 % / 816 B — **rechazada por `lmap`** (§3) |
| `InitAtSegment` | ídem 2 sentencias arriba (entre `fEndPos` y `fToLaneInd`) | 21 difs, igual que la base |
| `InitAtSegment` | `float zero=0.0f; asm("":"+f"(zero))` tras `fStartPos` + `SetLaneOffset(zero)` al final | **28 difs**, 99,152 % |
| `InitAtSegment` | `asm("" : "+r"(segment))` detrás del pin de r25 | neutro (15 difs) |
| `InitAtSegment` | cantidad fantasma con el rango ampliado hasta antes de `fStartPos` | neutro (15 difs) |
| `HolePunchAvoidables` | `approach_time * 0.2f * delta_offset` | 55 → **64 difs**, 97,919 % |
| `HolePunchAvoidables` | cantidad fantasma en `fr11`, rango `avoidable_delta_offset`→`extra_width` | **73 difs**, 97,742 % |
| `HolePunchAvoidables` | ídem con rango corto (hasta la barrera de `offset_change`) | **75 difs**, 97,729 % |
| `UpdateWheelYRenderOffset` | `float zero_w=0.0f; asm("":"+f"(zero_w))` en el width | 7 → **10 difs**, 872 B |
| `UpdateWheelYRenderOffset` | cantidad fantasma en r16 cerrada con `asm("":"+r"(guard))` | **idéntico a la base** (DCE, §1.2) |
| `UpdateWheelYRenderOffset` | ídem anclada a `desired_width` | 7 → **10 difs**, 872 B |
| `CullParts` | `register int polarity asm("r0")` | 5 → **9 difs**, 98,421 % |
| `CullParts` | ídem + `register int n asm("r31")` | **85,086 %**: r31 es el marco, sale `slwi r20,r1,4` |
| `CullParts` | cantidad fantasma en r11 anclada a `plane_info` | 5 → **9 difs**, 99,330 % |

---

## 7. Contabilidad de `zWorld2`

Sigue como la dejó la r36c y **no se ha movido**: las dos unidades son
`NonMatching` en `configure.py`, así que nada de esta ronda puede tocar el DOL.

```
zWorld2   near-miss: HolePunchAvoidables (2980 B, 98,133) + InitAtSegment (816 B, 99,559)
          [.rodata-0]  4560/4168  (-392 B)
```

Miré el orden de parseo que sugería el reparto. `datadiff.py zWorld2` confirma lo
que decía la r36c —las cadenas están **las dos veces**, en otro orden
(`AIParams`, `RBComplexParams`, `Attrib::Gen::rigidbodyspecs`,
`Attrib::Gen::pursuitlevels`, las versiones `19.8.31` / `16.2.1` / `1.2.3`…)— y
además faltan constantes flotantes sueltas (`3f000000`, `41e00000 00000000
43300000 00000000 3f000000`). **No lo he tocado**, por dos razones: (a) es trabajo
de reordenar la emisión de literales de varias TU, con riesgo de `lcfix`, y (b)
**no paga esta ronda**: `zWorld2` sólo promociona si además cierran las DOS
funciones, y `HolePunchAvoidables` está a 55 diferencias. Cuando alguien cierre
`HolePunchAvoidables`, ese `.rodata` es lo único que queda para 300 kB
promocionables.

---

## 8. Qué queda y por dónde

1. **`InitAtSegment` (816 B, 15 difs)** — sigue siendo el mejor premio. El racimo
   de doce filas es una inversión de prioridad de `local_alloc` entre `fNodes` y
   el `@ha` del 0,0f (§2): **no** se ataca con bloqueos ni con el permutador (la
   variante que lo cierra falsea el orden). El camino es el volcado `-dl`/`-dg`
   de `cc1plus` mirando los rangos **post-`sched1`**, no el `.s`.
2. **`HolePunchAvoidables` (2.980 B, 55 difs)** — atacar la reasociación de §4.2
   y, desde ahí, la permutación de f4/f5/f7/f8/f9/f10 que `regmap` ya nombra
   (4 locales, veredicto de permutación pura). El cr2/cr3 no se toca.
3. **`CullParts` (836 B, 5 difs)** — falta un pin de r0 de **ámbito corto** (§5).
4. **`RenderFlaresOnCar` / `UpdateWheelYRenderOffset`** — siguen siendo el mismo
   mecanismo (`-fforce-addr`: cuántos pseudos `@ha` viven a la vez) y siguen sin
   palanca.

**Fuera de mi territorio, lo que vale para cualquiera:**

- **La cantidad fantasma (§1)**: `register T g asm("rN"); asm("":"=r"(g)); … ;
  asm("":"+X"(algo_vivo) : "r"(g));`. Cero bytes, no es barrera de planificación,
  y ocupa el registro que le digas durante el rango que le digas. **Sólo paga si
  al objetivo le SOBRA una cantidad respecto a nosotros**, y eso se cuenta en un
  minuto comparando los registros preservados de los dos `.s`. Es la palanca que
  faltaba junto a la barrera selectiva de la r36b.
- **La trampa de DCE (§1.2)**: si el `asm` de cierre no toca un valor vivo, la
  pareja desaparece y el objeto sale **idéntico**, no peor. Un ensayo que dé el
  mismo número EXACTO de diferencias que la base es sospechoso de esto.
- **Rangos post-`sched1` (§2)**: `local_alloc` no ve el orden del `.s` final. Es
  la razón por la que muchos diagnósticos de reparto leídos sobre el ensamblador
  «no cuadran con la fórmula».
