# r54 — cierre: la ronda de diagnóstico entrega dos mecanismos y cero bytes

2026-09-09. Siete agentes. **El brief pedía un mecanismo, no bytes, y eso es exactamente lo
que ha salido.** Ninguna promoción, ninguna función cerrada, y aun así es la ronda que
desbloquea el bloque A.

## Progreso oficial verificado

| medida | antes r54 | después r54 | delta |
|---|---:|---:|---:|
| funciones exactas | 18.403 / 18.432 | 18.403 / 18.432 | 0 |
| unidades completas | 518 / 616 | 518 / 616 | 0 |
| `dolwhere zMain` | 17.269 B | **7.961 B** | **−9.308 B** |
| `zGameModes` .rodata | +104 (suelo) | **−744** | desbloqueada |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, 4.541.888 B, verificado por el jefe
enlazando los 616 objetos **después** de la re-extracción de `frontera` y otra vez al
cerrar. `checksplits`, `prefijochk` y `lcfix --check` limpios.

---

## Los dos mecanismos

### 1. La posición la decide el PUNTO DE USO dentro de una función

`forense1` y `forense2`, por vías distintas y **ciegas entre sí** — que era el diseño de la
ronda —, llegaron al mismo sitio con el fuente de GCC 2.95.3 delante.

`finish_file` (`cp/decl2.c:3643`) drena **colas distintas por turnos**: el cuerpo de
plantilla sale de `pending_templates`, la vtable de `walk_globals`. Y `instantiate_decl`
(`cp/pt.c:9439-9461`) instancia una plantilla *inline* en el punto del parseo **si y sólo si
su primer uso está dentro de una función** (`nested = in_function_p()`, `pt.c:9308`).

**No lo decide la declaración, ni la vtable, ni el `#include`.** Por eso el
`#include ActionQueue.h` de la r53 emparejó las vtables y dejó el destructor 9.204 B tarde:
actuaba sobre la cola que no era.

La palanca son dos líneas y cuesta `.text +0` porque el enlazador estripa la `static` muerta.
En `zMain`: `dolwhere` 17.269 → **7.961 B**, símbolos descolocados 183 → 88, `fncmp` 0/1.380
y `linkdelta` IGUAL antes y después. **Es un andamio** y está marcado como tal.

Y la medida que faltaba para saber si esto es lo que bloquea una unidad: **`reorden.py`**,
que recoloca nuestros bytes en la secuencia del objetivo enmascarando el campo de
reubicación. En `zMain` da **0 de 39.617 palabras** — no queda contenido que escribir. En
`zFe2` dice que **no** está igual, que es el control que la hace útil.

### 2. El cambio de dueño, que no medía nada

`control`. Un símbolo que **nuestro** objeto define y el original de esa unidad no, y que
otro objeto también define, se lo lleva el enlazador. `linkdelta` no lo ve (el total no
cambia), `permorden` tampoco (compara dentro de la unidad), `fncmp` tampoco (la función casa
al 100 %).

Se mide en **dos restas**: lo que crece la sección enlazada menos lo que crece nuestra
aportación. Ya está en `movidos.py`.

`clear__..._List_base<WGridManagedDynamicElem,...>`, **120 B**, lo define un solo objeto
original (`obj/zSim.o`) y **cinco** de los nuestros. Verificado en las tablas de símbolos, y
hay un segundo de la misma familia (`...WGridNodeElem...`, original en `obj/zAI.o`).

---

## Y `control` refuta la premisa de su propia ronda

**`zMain` no es representativa: es la más limpia de las trece por un factor de 20 a 76.** Lo
que encontraron los forenses vale **159.776 B, no 1.652.576**. Histograma distinto,
proporción de secciones distinta, símbolo dominante análogo sólo en una.

Es el resultado más incómodo de la ronda y el que más cambia el reparto de la r55.

---

## Lo que se cierra con prueba

- **`frontera`**: no era la frontera, era **una entrada de `keep.lst` que faltaba**. Probado
  por ablación: los hashes de mis dos ensayos fallidos se reproducen **dígito a dígito**
  quitando una línea cada vez. `keepchk2` detecta entradas fantasma, **no entradas que
  faltan** — hueco de herramienta, anotado.
- **`flares`**: la puerta de `scan_loop` **no puede disparar nunca** en PowerPC.
  `LEGITIMATE_LO_SUM_ADDRESS_P` exige `REG`, y `lo_sum (high` aparece **0 veces** en los
  siete volcados de 599 funciones. Doce cflags verificadas, cero mejoran nada.
- **`epcalc`**: **el objetivo no tiene ranura huérfana** — son relleno de alineación. Los
  «huecos» que perseguíamos desde la r46 son cuatro ranuras `SF` paradójicas leídas en +4 por
  la corrección big-endian de `assign_stack_local` (`function.c:837`).
- **`loaded`**: **cse2 deshace siempre la copia de PRE**, y es veda de todo el proyecto:
  `HIGH` cuesta **0** (`CONST_COSTS`) contra **1** del pseudo (`cse.c:519`). `0 < 1` en todo
  bloque y para toda fuente.

---

## Herramientas: tres correcciones y dos altas

- **`movidos.py` estaba mal y era mío.** Lo encontraron **tres agentes por separado**.
  Restaba como arrastre **la moda**, pero sólo mira símbolos que se movieron, así que cuando
  el arrastre real es 0 la moda no puede valer 0 nunca: cancelaba el mayor racimo de
  permutaciones reales. Ahora son **tres términos exactos**, y fuera los `$LC`, cuyo nombre
  colisiona entre objetos. **zGameModes 6.363 → 4; zLua 7.383 → 14.**
- **`dolwhere` cuenta rangos, no bytes** (extiende mientras difiera alguno de los 12
  siguientes): 183.693 donde hay 15.369 reales.
- **Altas**: `reorden.py` (contrafactual de orden) y `serie.py` (la tasa real por ronda).
- **Candidata pendiente**: `hicount.py`, de `flares`.

---

## Avisos de método que valen para todas las rondas

1. **`build/GOWE69/src/*.o` no es base estable con agentes en paralelo.** A `forense2` le
   saltó la cifra de 15.369 a 110.064 B porque otro recompiló `zMain.o`. El remedio es
   compilar desde `git show HEAD:` a un directorio privado — **y el objeto tiene que
   llamarse igual**, porque `keep.lst` empareja por basename y si no falla en silencio.
2. **`ngccc` emite `lis 30,$LC…` sin la `r`.** Un `grep 'lis r30'` da cero e induce a creer
   que el compilador nunca iza a preservado. Invalida búsquedas anteriores.
3. **`build_direct.py zMain` falla de forma intermitente sin error** y pasa al segundo
   intento.
4. **Dos errores de mi propio brief, corregidos por los agentes**: `-dc` es `.combine`, no
   `.cse` (`-ds`), y el pase que ve `scan_loop` es `gcse` (`-dG`), no `cse`.

---

## Lo que decide la r55

El brief traía la regla escrita: *si la r54 nombra el mecanismo, la r55 lo propaga; si no,
el bloque A deja de recibir agentes*. **Lo ha nombrado — dos veces —, así que se propaga.**

Con una corrección importante de `control`: propagarlo a `zMain` valía 159.776 B, no 1,65 MB.
El reparto de la r55 tiene que ir a las unidades donde el mecanismo **pesa**, y las medidas
que dicen cuáles son ya existen: `reorden.py` para «¿es orden o es contenido?» y las dos
restas de `movidos.py` para el cambio de dueño.

Dos objetivos individuales ya nombrados y con cifra:

- **`clear__..._List_base<WGridManagedDynamicElem,...>`** — 120 B que envenenan `zMisc`,
  `zPhysics` y `zGameplay` a la vez.
- **`ActualReadJoystickData`** (zPlatform, 1.588 B) — el único de 199 casos con **cero
  uniones** entre sus dos usos, o sea el único donde la cadena de `loaded` queda al alcance
  de `cse1`.

Y una causa de fuente que está a un `sed`: **`CarCustomize.hpp:686` define catorce globales
en una cabecera** en vez de declararlas, y `uiQRCarSelect.hpp` la propaga a `zAnim` y `zFe2`.
