# Ronda 15 — instrucciones comunes

Estado: **93,14% matched**, 17.956 funciones, `main.dol: OK`, `linked` 7,56%.
En las 33 SourceLists quedan **168.748 B**: **89.632 B con delta de mnemónicos**
(código que falta o sobra) y **79.116 B de muro del asignador**.

La ronda 14 estrenó esa partición y cerró **+9.272 B y 12 funciones** con un
tercio del terreno vetado. Sigue igual: tu encargo trae **dos listas** y la de
MURO es una prohibición, no una sugerencia. Si crees que algo está mal
clasificado, **demuéstralo con el asm y dilo** — en la 14 se reclasificaron seis
funciones en los dos sentidos y eso valió más que los bytes.

**El signo del delta es `OBJETIVO − NUESTRO`: positivo = nos FALTA, negativo = nos
SOBRA.** Verificado midiendo. Leerlo al revés manda en dirección contraria.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Antes de tocar nada

1. `python scripts/measure.py -o antes_r15_<grupo>.json <tus unidades>`; al
   terminar **escribe en `despues_r15_<grupo>.json`**, nunca en `despues.json`.
2. **`python scripts/pctsnap.py -o pct_antes_<grupo>.json <tus unidades>`** y al
   final `--cmp`. `measure.py` **no ve una regresión de near-miss**: una función
   puede caer del 95,5% al 75,7% sin mover un byte, y ya dejó código malo en HEAD.
3. `auditecho.py` para tu delta al día, `lmap.py` para la línea de fuente del
   original instrucción a instrucción, `dwblocks.py` para los rangos DWARF
   normalizados, `fuse.py` para la lista de mem-init.

## Trampas de herramienta — todas medidas, todas han costado dinero

| | |
|---|---|
| **`bench.py`** | puede dar un **100% FALSO** (sólo compara tamaños) y su columna «bytes» imprime **el tamaño del OBJETIVO**, no el tuyo. Verifica cada cierre contra el `.o` de la unidad |
| **`measure.py`** | **NO compila**: pasa `build_direct.py` antes o mide los `.o` viejos y da `+0 B` en silencio |
| **el `.o` rancio** | con varios agentes sobre el mismo `build/`, un `.o` puede haberse compilado con los cambios de OTRO. **Bórralo antes de medir** |
| **barridos propios** | si no compruebas el **código de salida del compilador**, un fallo de compilación se lee como «IGUAL». Y `str.replace` sin ancla única parchea la ocurrencia equivocada. **Ficheros CRLF** (`Geometry.cpp`) hacen que las anclas con `\n` casen cero veces |
| **una medida vacía** | si tu barrido dice «no cambia nada», comprueba que la medida **no está vacía**: me pasó auditando barreras y comparaba dos cadenas vacías |
| `regmap` | empareja mal cuando hay **sobrecargas** (`Tire::UpdateLoaded`, `FnRawLinearChannel::Eval`): comprueba cuántas hay. `--list` **no** regenera el volcado |
| `dwbody` | coge `cands[-1]` del glob → volcado rancio tras recompilar |
| `litcheck` | **da falsos positivos en el lado del OBJETIVO**: no ve los `lfs fN,0xNNNN(r10)` del truco `mflr/mtctr` ni los `lbl_` que declaramos `extern` con `__asm__` |
| `permuter` | cerró **cero** funciones en las rondas 13 y 14. Remide su ganador con objdiff siempre |

## Lo aprendido en la ronda 14 — todo con cifra

**De C++, y por eso vale en cualquier compilador:**
- **`delete p;` NO es `p->~T(); ::operator delete(p);`** — el primero evalúa el operando una sola vez en un temporal; el segundo **relee el miembro** tras el destructor inline. **784 B.**
- **Un local nombrado antes de una llamada con parámetro de clase POR VALOR cuesta una copia de más.** `StringKey f(x); g(f)` hace dos copias de 16 B; `g(x)` hace una. **+6,3 pp.** El `const` **no** la elide.
- **`const` en un parámetro POR VALOR no es nunca una pista de codegen**: da código byte a byte idéntico, sólo cambia el DWARF.

**De cross-jumping y switch:**
- **El bloque `return X` inline se lo come el cross-jumping si el ÚLTIMO bloque antes del epílogo es idéntico.** Una función que acaba en `return false` pierde sus `return false` intermedios; una que acaba en `return true` los conserva. **280 B.**
- **El `return false` que sobrevive es el que NO es el último bloque**: mételo en un `else` y deja `return true` al final. **228 B.**
- **`case A: case B:` con cuerpo compartido = en el original son DOS cases con el cuerpo duplicado.** Un solo `code_label` impide que se fundan las colas de despacho. **756 B.**
- **La última hoja del árbol de switch se invierte si su case es el PRIMERO de la fuente**, y esa inversión impide la fusión. Va en los dos sentidos. **1.204 B.**

**De registros y literales:**
- **La misma expresión escrita DENTRO de un `case` se pliega en los desplazamientos del `lwz`; escrita en el bloque que DOMINA al `switch`, sobrevive.** **1.640 B.**
- **`local_alloc` contra `global_alloc`**: la rama tiene que sobrevivir a `delete_trivially_dead_insns`, y sólo sobrevive si el bloque **reasigna la variable que se testea** (`if (x) x = 0` → r31; `if (x) y = 1` → r30).
- **El literal en una VARIABLE, no en el literal**: si el objetivo mantiene una constante en un salvado a través de llamadas, dale una variable — y **el sitio de la asignación decide cuántas cargas salen** (arriba una, en cada rama una por rama).
- **`lhz`+`extsh` en vez de `lha` no es un cast, son DOS VARIABLES**: con `static_cast` GCC pliega el `extsh`; con una variable no puede.
- **Cabecera que miente**: `UMath::Sub(const Vector4&,const Vector4&,Vector4&)` **no existía** y era el 7% de una función. Antes de pelear con el asignador, comprueba que el inline que el DWARF expande **existe**.

**De lectura del DWARF:**
- **Un desajuste del árbol de bloques puede valer CERO, y la hermana al 100% lo prueba**: dos hermanas casaban al 100% con exactamente los mismos bloques ausentes. Mira eso antes de perseguirlo.
- **El volcado sube una declaración de bloque interior al ámbito de función** cuando el bloque no emite rango propio: no lo leas como posición.
- **El volcado INVIERTE los declaradores de una misma sentencia**; las sentencias separadas no.

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada.** Una función al 99,99% aporta cero.
- **Trabajo a medias REGRESA.** Si no cierra, revierte y anota la veda **diciendo qué sentencia barriste**, no cuántas formas.
- **NO escribas ensamblador a mano ni claves registros.** Eso no cierra una función, la falsifica; se revirtió un caso de 456 B. *(Los seis `__asm__("")` que quedan en el árbol están auditados uno a uno y **los seis sostienen código**: no los toques sin medir.)*
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`. Si una unidad queda LIMPIA en `promote.py`, **dímelo y pásale `scripts/trypromo.py`**, que prueba la promoción enlazando de verdad sin tocar el árbol.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y `UVectorMath.hpp` (**este último debe conservar 2**). *(Hay un port a Android vivo que compila código real del árbol.)*
- **Cabecera compartida = mídela en TODAS las unidades que la incluyen.** El listón de la 14 fue un A/B por objetos sobre **las 589 unidades**.
- **El scratchpad es compartido**: nombre único a tus helpers y a tus `.json`.
