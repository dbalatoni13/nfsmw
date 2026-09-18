# Ronda 17 — instrucciones comunes

Estado: **93,61% matched**, 17.969 funciones, `main.dol: OK`, `linked` 7,56%.
La 16 cerró **+15.580 B y 9 funciones** y rompió una serie que venía cayendo.

## Lo que la 16 enseñó, y es lo primero que tienes que leer

**Media ronda 16 se fue en corregir mis herramientas, y ahí estuvo el
rendimiento.** Tres reglas que yo repartía como firmes cayeron con medida, y una
de ellas hizo que ocho agentes recibieran **126.192 B de trabajo que no existía**.
Lo cuento para que apliques el mismo criterio: **si un dato mío no cuadra con lo
que ves en el asm, mídelo y dímelo.** Eso vale más que los bytes.

- `triage.py` iba por la v2 y contaba las filas INSERT/DELETE en crudo. **objdiff
  alinea sobre el TEXTO EXACTO**: un tramo que solo difiere en un registro no
  empareja, y cualquier instrucción idéntica de dentro hace de ancla y lo parte en
  DELETE + INSERT. Una instrucción desplazada contaba como **2N filas**.
- La regla del **bloque fundido** («un address con N líneas son N sentencias»)
  estaba mal enunciada: eso vale **solo** si el address es punto de unión **y** las
  líneas de más emiten código en otro sitio. Lo normal es que una sentencia que no
  emite nada empuje su nota a la instrucción siguiente. Entre el 5% y el 17% de
  los addresses llevan 2+ líneas.
- La regla **«delta solo de copias = muro»** iba 11/11 y tiene contraejemplo:
  `UpdateCameraMovers` cerró con 1.264 B siendo «solo copias». Es **pista, no veto**.

## Cómo leer tu lista (`python scripts/triage.py <tus unidades>`)

    faltan N, sobran M, de ellas K SUST

**`SUST` son sustituciones: la misma fila con otro opcode.** `faltan 4, sobran 4,
de ellas 4 SUST` es **una sola diferencia repetida cuatro veces**, no ocho. Sin
ese número repartí un lead imposible sobre las gemelas de `SAP.h`.
`dmax` separa el jitter del planificador (≤20) del bloque movido de verdad (>60).

Mapa al día: **61 funciones y 70.880 B accionables**, 91 de muro (78.108 B).

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Antes de tocar nada

1. `measure.py -o antes_r17_<grupo>.json <unidades>` y al terminar `despues_r17_…`.
2. **`pctsnap.py` antes y después.** `measure.py` no ve una regresión de near-miss.
3. **Compila tu `.o` al scratchpad, no al `build/` compartido.**
4. **Usa el banco de pruebas**: `scripts/mn_repro.py` compila **un `.cpp` suelto**
   con los cflags exactos de su unidad sacados de `build.ninja`; `mn_diff.py` lo
   enfrenta al `.o` objetivo; `mn_forms.py` parchea/mide/restaura una lista de
   formas (aguanta los finales de línea mixtos); `mn_sweepflags.py` barre 28
   juegos de flags. **Itera en 4-6 s** en vez de recompilar la SourceList.

## Patrones nuevos de la 16 — todos con cifra

**De lectura (los tres cerraron funciones que el diff daba por muro):**
- **El destino de la rama dicta el anidamiento**: `fila = (destino−inicio_fn)/4`. Si los dos saltos de un `||` van a la fila del test *siguiente* y no a la del cuerpo, la condición es un `&&` con las comparaciones invertidas **y el bloque de detrás es hermano, no anidado**. **976 B.**
- **El hueco de retorno del DWARF nombra la sentencia**: un inline que devuelve agregado compartiendo hueco con otra local, más `lmap` dando *una* línea donde tú tienes seis sentencias → la forma es **inicialización**, con las demás declaradas después. **1.100 B.**
- **La lista de inlines del DWARF es una checklist**: si trae un `operator*=` que no tienes, **falta esa sentencia** aunque se pliegue a nada. `vEyeDirection *= 1.0f;` arregló **el tamaño del marco**. Parte de **3.776 B**.

**De fuente:**
- **Una local sin inicializar cuya única escritura es salida de `asm` lleva `= 0.0f` en el original.** En `bAbs()`: **5.212 B**. **No generaliza**: en `bMin`/`bMax` cuesta −10.636 B.
- **`T *p = &objetoDePila;` usado SOLO en la primera llamada** → 96,02 → 99,83%. Usarlo en todos los accesos la hunde a 94%. *(Es el contrario del `T *p = this;`, que hay que quitar.)*
- **`x = x && cond;`** frente a `if (x) { x=false; if (c) x=true; }`. Firma: `cmpwi rX,0` ; `li rX,0` ; `beq` — el reset ejecuta **antes** del salto.
- **`c ? f(A) : f(B)` y no `f(c ? A : B)`.** *(Veta agotada: 99 apariciones, las 99 al 100%.)*
- **`do/while` con guarda `if` → `for`**: si el objetivo hace `li r5,0` + `mr r7,r5` donde tú emites **dos** `li`.
- **`return x;` final → `goto done;`**: el `return` final se fusiona con los `return false` de arriba.
- **GCC pliega `a * K * b` a `a * (b * K)`**: el orden de operandos del `fmuls` te da la asociatividad de la fuente.

## Negativos de la 16 — no los repitas

- **Nada en la fuente impide izar el literal de retorno: lo impide que r3 esté ocupado.** Cierra `TestToLicensed`, `Play__9EAXCommon` y `AssignAudioStreamHandle` como asignador. 8 formas en repro, todas dan la nuestra.
- **La técnica del cflag no aplica en zEAXSound** (21 flags, ninguna mejora) **ni en zEcstasy** (12 flags) **ni en `DrawAmbientShadow`** (9 sondas).
- **El permutador lleva cuatro rondas sin cerrar nada.**
- **`zcmp2` miente con permutación global de registros**: una variante subió de 162/275 a 225/275 y con objdiff **bajaba** 1,5 pp. Barre con zcmp2, **cierra con objdiff**.
- **Fuentes falsas que dan el tamaño exacto**: `worldHeight` sin inicializador en `Place` y la unificación de símbolos de `0.0f` en `RenderFlaresOnCar`. **El tamaño exacto no es la medida; `faltan/sobran` sí.**

## Trampas de herramienta

| | |
|---|---|
| **`bench.py`** | puede dar un **100% FALSO**; su columna «bytes» es la del OBJETIVO |
| **`measure.py`** | **no compila**: pasa `build_direct.py` antes |
| **el `.o` rancio** | bórralo antes de medir |
| **disco lleno** | `No space left on device` de cc1plus **se lee como «la variante no cambia nada»**. Si un barrido entero sale neutro, comprueba el espacio y el código de salida |
| **finales de línea** | mixtos **por línea**, no por fichero. Usa `\r?\n` |
| `regmap` | empareja mal con sobrecargas; «SOLO NUESTRA» no es «sobra» |
| `litcheck` | falsos positivos en el lado del OBJETIVO |

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada.** Una función al 99,99% aporta cero.
- **Si no cierra, revierte y anota la veda diciendo QUÉ SENTENCIA barriste.**
- **NO inventes locales ni claves registros para cuadrar un número.**
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y `UVectorMath.hpp` (**dos bloques**). Hay un port vivo.
- **Cabecera compartida = A/B por objetos sobre TODAS las unidades que la incluyen.** El listón de la 16 fue eso sobre las 32 compilables.
- **El scratchpad es compartido**: nombre único a tus helpers y a tus `.json`.
