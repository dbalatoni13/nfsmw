# r76 — Despineo del lote zAI + zPhysics

Encargo: dejar **cero pines y cero barreras** en
`src/Speed/Indep/Src/AI/Common/AIPursuit.cpp` y
`src/Speed/Indep/Src/Physics/Common/PhysicsObject.cpp`, con el criterio del
usuario: *antes código real con menos porcentaje que un 100 % falso sostenido
por un `asm`*. Rama `agent/vedas-oraculo-r2`, base `bf3d0dc0`.

**Resultado: los dos ficheros quedan LIMPIOS.** Se retiran un pin, una barrera
y **dos `__extension__({ … })`** (la extensión de GCC que sostenía cada
andamio: eso tampoco compila fuera de GCC). El precio son 2.852 B que pasan de
«100 %» a 99,2–99,7 % **reales**, con el mecanismo de lo que falta medido en el
RTL, no supuesto.

---

## 0. Tabla del lote

| función | unidad | B | andamio ANTES | % ANTES | andamio DESPUÉS | % DESPUÉS | filas |
|---|---|---:|---|---:|---|---:|---:|
| `AIPursuit::AssignClosestOffsets` | zAI | 1.684 | 1 barrera de 2 operandos dentro de un `__extension__({…})` | 100 **FALSO** | **ninguno** | 99,66746 | 8 |
| `PhysicsObject::PhysicsObject(const char*, const char*, SimableType, HSIMABLE, WUID)` | zPhysics | 1.436 | 1 pin `bodyOwner` r3 dentro de un `({…})` | 100 **FALSO** | **ninguno** | 99,24234 | 5 |

Censo de `scripts/censoasm.py` sobre los dos ficheros: **PIN 0, BARRERA 0**.
El otro ctor del fichero (`PhysicsObject(const Attrib::Instance&, …)`, 1.412 B)
sigue al **100 %** y nunca tuvo andamio.

Sellos `sha1` de los `.o` (tres compilaciones seguidas, idénticas):
`zAI 1190f64c9a5d21f8` · `zPhysics ead4a58e309396e0`.
Las dos unidades son `NonMatching`: el DOL enlaza el objeto extraído y no
puede romperse por esto.

### Corrección al censo r76

`docs/analisis/r76-censo-pines.md` §5 imputa el pin de `PhysicsObject.cpp:75`
a `PhysicsObject::PhysicsObject` de **1.412 B**. Es la **sobrecarga
equivocada** — exactamente la trampa que el propio censo describe en su §4
(«las sobrecargas engañan al informe»). La línea 75 está en el ctor de
`const char *`, que son **1.436 B**
(`__13PhysicsObjectPCcT111SimableTypeP10HSIMABLE__Ui`). El de 1.412 B
(`…RCQ26Attrib8Instance…`) no tiene andamio y casa.

---

## 1. `AIPursuit::AssignClosestOffsets` — 1.684 B

### Qué había

```c
} while (0 < (__extension__({ int next = copsToAssignOffsets - 1;
               copsToAssignOffsets = next;
               __asm__("" : "+r"(copsToAssignOffsets) : "r"(next)); next; })));
```

Dos extensiones de GCC en una línea: la expresión-sentencia y la barrera de
dos operandos.

### El oráculo

`python scripts/dwarf1.py fn AssignClosestOffsets` (rango
`0x80031760..0x80031DF4`) da como locales de la función: `numRows` r25,
`numCols` r26, `copOffsetDistance` `[r1+8]`, `copOffsetMaximums` `[r1+32]`,
`INDEX_ASSIGNED` (**loc vacía**: optimizada fuera) y `copsToAssignOffsets`
**r9**. **No hay ninguna local `next`.** El temporal que el objetivo tiene en
r29 es un **temporal del compilador**, así que una fuente fiel no lo puede
nombrar: la expresión-sentencia era invención nuestra tanto como la barrera.

### El argumento que cierra la familia de formas

El cuerpo del `do` tiene **tres `continue` que pertenecen a este bucle**
(`:1017`, `:1030` y el del bloque `{ int j; … }`). Cualquier forma que baje el
decremento **al cuerpo** —`x--;` al final más `while (x > 0)`, o un `for (;;)`
con `if (--x <= 0) break;`— se lo salta en cada `continue` y es un **bucle
infinito**. Luego el decremento tiene que ir en la condición, y ahí sólo queda
una familia de formas. (Las variantes «decremento en el cuerpo» que midieron
la r36c y la r67 son, además de peores, **incorrectas**.)

### Barrido r76 (10 formas, ninguna con `asm` ni `__extension__`)

Banco: `scratchpad/r76_aiphys/tu_ai.cpp`, TU reducida = `zAI.cpp` truncado tras
el `#include` de `AIPursuit.cpp`. **Control**: con el andamio puesto reproduce
la función **byte a byte** (1.684/1.684 B, 100,00000 %, 0 filas). 27 s por
forma contra los ~3 min de la unidad entera.

| forma | B | % | filas |
|---|---:|---:|---:|
| **`--x > 0`** | **1.680** | **99,66746** | **8** |
| `0 < --x` | 1.680 | 99,66746 | 8 |
| `(x -= 1) > 0` | 1.680 | 99,66746 | 8 |
| `x -= 1, x > 0` (coma) | 1.680 | 99,66746 | 8 |
| `(x = x - 1) > 0` | 1.680 | 99,66746 | 8 |
| `--x >= 1` | 1.680 | 99,66746 | 8 |
| `--x != 0` | 1.680 | 99,65558 | 9 |
| `--x` (a secas) | 1.680 | 99,65558 | 9 |
| `x-- > 1` | 1.688 | 99,05938 | 11 |
| `x > 1 ? --x, 1 : 0` | 1.696 | 97,96912 | 15 |

Las seis primeras dan el **mismo objeto**. Se queda la natural,
`--copsToAssignOffsets > 0`, confirmada luego en la unidad entera:
**1.680/1.684 B, 99,66746 %**.

### Las 8 filas que quedan son DOS cosas

```
 260 lis  r28, -1.0f@ha        | lis  r29, $LC1470@ha
 263 subi r29, r9, 0x1         | subi r28, r9, 0x1
 271 lfs  f12, -1.0f@l(r28)    | lfs  f12, $LC1470@l(r29)      (y 295, 314, 352)
 388 mr   r9, r29              | mr.  r9, r28
 389 cmpwi r29, 0x0            | —
```

- **a) SEIS filas: permutación limpia r28↔r29.** El objetivo deja el `@ha` del
  literal `-1.0f` en r28 y el temporal `x-1` en r29; nosotros al revés. `regmap`
  da reparto idéntico: los dos pseudos son temporales, no hay variable que
  tocar. El andamio la volteaba subiendo `n_refs` del temporal con su operando
  de **entrada** — palanca de `asm`, no de fuente.
- **b) DOS filas y CUATRO BYTES.** El objetivo emite `mr r9,r29` + `cmpwi r29,0`
  y nosotros el `PARALLEL` `mr. r9,r28`. Es `combine.c:1699` (*arithmetic
  operation and set the condition code*): el operando del compare **es** i2dest
  de la copia, así que combine mete i2src dentro del compare. **Somos una
  instrucción más cortos que el objetivo.**

---

## 2. `PhysicsObject::PhysicsObject(const char*, …)` — 1.436 B

### Qué había

```c
IBody(({ register UTL::COM::Object *bodyOwner asm("r3") = this; bodyOwner; })),
```

### El oráculo

`dwarf1.py fn __13PhysicsObjectPCcT111SimableTypeP10HSIMABLE__Ui`: parámetros
`this` r31, `attributeClass` r17, `attribName` r14, `objType` r16, `owner` r15,
`wuid` r18. El `INLINE IBody` trae `this` **optimizado fuera** y `owner` = r31.
**No existe ninguna local `bodyOwner`.** Y el árbol de inlines
(ISimable → _IHandle/IUnknown/Instanceable, IBody, IAttachable, GarbageNode,
Container, WWorldPos, Behaviors, Attachments, UCrc32) es **idéntico** al del
otro ctor, que casa al 100 % con `IBody(this)` pelado.

### El mecanismo, leído en el RTL (no supuesto)

Banco: `scratchpad/r76_aiphys/tu_phys.cpp`, TU reducida = `zPhysics.cpp`
truncado tras el `#include` de `PhysicsObject.cpp`. **Control**: reproduce la
función byte a byte. 6 s por forma.

`IUnknown::IUnknown` (`UCOM.h:93`) hace `_mCOMObject = owner;` y acto seguido
`_mCOMObject->_mInterfaces.Add(handle, this)`. **El segundo RELEE el miembro.**
El volcado `.lreg` lo enseña sin ambigüedad:

```
(insn/i 420 … (set (reg:SI 3 r3)
        (mem/s:SI (plus:SI (reg/v/u:SI 82) (const_int 60 [0x3c])) 0)))
```

es decir un `lwz r3,0x3c(r31)` de verdad. `reload_cse` lo convierte después en
`mr r3,r31`, pero **sched1 ya le ha puesto una dependencia VERDADERA sobre el
`stw r31,0x3c(r31)` con coste 2**; la traza de `-fsched-verbose-2` lo dice
literalmente:

```
;;  --> scheduling insn <<<417>>> on unit lsu
;;  dependences resolved: insn 420 into queue with cost=2
;;  Ready-->Q: insn 420: queued for 2 cycles.
```

Con esa dependencia el `mr` **no puede subir**. El objetivo lo tiene arriba:

```
objetivo :  stw r30,0x40(r31) | mr r3,r31 | stw r31,0x3c(r31) | addi r5,r31,0x3c
nuestro  :  stw r31,0x3c(r31) | addi r5,r31,0x3c | stw r30,0x40(r31) | … | mr r3,r31
```

### El hallazgo incómodo

**El objetivo tiene los DOS ctors planificados distinto con el MISMO código.**
El ctor de `Attrib::Instance` del original trae exactamente *nuestro* orden
(`stw 0x3c / addi r5 / stw 0x40 / addi r29 / mr r3`) — o sea, con la relectura
dentro— y el de `const char *` no. Difeados instrucción a instrucción, los dos
ctors del original sólo se diferencian en los movimientos de parámetro
(`stmw r14` contra `stmw r16`, 6 parámetros contra 4) y en esta permutación de
cuatro instrucciones. **La palanca no está en este inicializador**: está en si
CSE se come o no la relectura de `_mCOMObject`, y eso no se decide en
`PhysicsObject.cpp`.

### Barrido r76 (7 formas)

| forma | ventana |
|---|---|
| `IBody(this)` | `stw 31,60 \| addi 5,31,60 \| stw 30,64 \| addi 29,31,68 \| mr 3,31` |
| `IBody((UTL::COM::Object *)this)` | idéntica |
| `IBody(static_cast<UTL::COM::Object *>(this))` | idéntica |
| `IBody(this)` escrito el **primero** de la lista | idéntica |
| `mOwner` asignado en el cuerpo | idéntica |
| `Sim::Object(10 + 3)` | idéntica |
| `mWorldID` asignado en el cuerpo | idéntica |

**Las siete dan el mismo objeto.** No hay forma de fuente en este ctor que
mueva la planificación: se queda `IBody(this)`, que es lo que el DWARF
describe y lo que el ctor hermano usa para casar.

---

## 3. Portabilidad (sonda X360)

`python tools/scratch/x360_sonda/sonda.py <fichero>`

| fichero | clase | primer bloqueo |
|---|---|---|
| `Src/Physics/Common/PhysicsObject.cpp` | **OTRO** | `C2653 '_STL' : is not a class or namespace name` (`PhysicsObject.cpp:30`, `_STL::remove`) — error de C++ del árbol, **ya no es `asm`** |
| `Src/AI/Common/AIPursuit.cpp` | **GCCASM**, pero **no por este fichero** | `SpeechManagerLite.h:37` — `extern EventHistory gManagerGlobalHistory __asm__("…")`, o sea la familia **ALIAS** de cabecera compartida, uno de los 6 grupos del §3 del censo. Dentro de `AIPursuit.cpp` no queda ni un `asm`. |

Los dos ficheros salen del grupo «bloqueado por andamio». `PhysicsObject.cpp`
figuraba en los 16 mixtos del censo (PIN + error C++): retirado el pin, sólo le
queda el error C++, que es otro frente.

---

## 4. Lo que queda en AI/ y Physics/, y no es mío

`censoasm.py` sobre todo el árbol, filtrado a `**/AI/**` y `**/Physics/**`:
queda **un** andamio vivo,

```
BARRERA  src/Speed/Indep/Src/Physics/Dynamics/Geometry.cpp:432
         __asm__("" : "+r"(a_lp) : : "memory");
```

que es de **zDynamics** (`zDynamics.cpp:38`), no de zAI ni de zPhysics —
la barrera de `Dynamics::Collision::Geometry::SphereVsBox` (1.896 B) del §5 del
censo. Fuera del encargo: no se ha tocado.

---

## 5. Herramientas nuevas (en `scratchpad/r76_aiphys/`, sin commitear)

| fichero | qué hace |
|---|---|
| `tu_ai.cpp` / `probe_ai.py` | TU reducida de zAI + sonda que compila a un `.o` PRIVADO y lo diffea contra el extraído. 27 s por forma, reproduce la función byte a byte. |
| `tu_phys.cpp` / `probe_phys.py` | lo mismo para zPhysics. 6 s por forma. |
| `sweep_ai.py` / `sweep_phys.py` | barrido de variantes: sustituyen, miden y **restauran siempre** el fichero. |
| `sweep.py` | barrido contra la unidad ENTERA, con sello `sha1` del `.o`. |

**Aviso de método reutilizable**: la TU reducida de `zAI` **no** se puede
construir comentando los `#include` anteriores (`AIPursuit.cpp` necesita
`VehicleClass` y demás de los `.cpp` de arriba); hay que **truncar** tras el
`#include` que interesa y dejar los anteriores. En `zPhysics` sí valen las dos
formas, pero truncar es obligatorio igual porque `IMPLEMENT_SINGLETON(
Smackable::Manager)` va después. Y `schedtrace` con `-fsched-verbose-2` se cae
con la unidad entera (bug conocido) y **no** se cae con la TU truncada: ahí es
donde salió la traza de `cost=2`.
