# El procedimiento

Cómo se encuentra lo que falta. `docs/METODO.md` dice **qué** cuenta y cómo se
mide; este documento dice **cómo se trabaja** para subir esa cifra sin hacer
trampas. Es el procedimiento con el que PS2 pasó en dos días del 24,7 / 26,7 %
al 28,1 / 32,3 %, sin un solo andamio y sin mover un byte del DOL de GameCube.

## La idea

**Se busca la causa común, no la función.** Cientos de funciones casi casadas
suelen colgar de muy pocas causas: una máscara de instrucción, un miembro de más
en una cabecera, una alineación que falta, un flag mal puesto. Arreglar una
causa cierra decenas o cientos de funciones de golpe; perseguir funciones de una
en una no termina nunca.

La segunda idea es igual de importante: **no se adivina nada que el binario pueda
decir.** Ni la máscara de una instrucción, ni el tamaño de una base, ni qué hacía
el prototipo en una rama. Hay oráculos para todo eso y se consultan antes de
tocar el código.

## El ciclo

### 1. Medir las cinco versiones

```bash
python scripts/pctall.py
```

La línea de cada versión tiene que decir **«0 objetos no compilan»**; si no, la
cifra no vale (ver `docs/METODO.md`). Esta es la base contra la que se juzga
todo lo demás, y se repite después de cada cambio.

### 2. Censo: dónde está el hueco

```bash
python scripts/censohueco.py
```

Da, por unidad, los bytes de funciones al 80-100 % (valen su tamaño entero,
porque una función al 99,99 % aporta cero). La columna que manda es
**«GC al 100»**: de esas funciones casi casadas en PS2, cuántas están al 100 % en
GameCube.

- Si son **casi todas**, la fuente es buena y lo que falla es algo de la
  plataforma — compilador, layout, flags. Ahí hay una causa común esperando.
  Así se vio que de 259 near-miss de zAI en PS2, 243 casaban en GameCube.
- Si son pocas, el problema es la forma del código en esa función, y es trabajo
  de una en una.

### 3. Los diffs de las unidades con más hueco

```bash
python scripts/diffunidad.py SLES-53558-A124 zAI zFe2 zWorld zSpeech
```

Deja el diff instrucción a instrucción en `build/diffs/<V>/`.

### 4. Leer los patrones que valen, y solo esos

objdiff clasifica cada diferencia. **Dos tipos dicen algo; tres no:**

| tipo | qué es | ¿sirve? |
|---|---|---|
| `DIFF_OP_MISMATCH` | misma posición, otro código de operación | **sí** |
| `DIFF_ARG_MISMATCH` | misma posición, otro operando | **sí** |
| `DIFF_REPLACE` / `INSERT` / `DELETE` | resultado de **alinear** dos secuencias de distinta longitud | **no**: son síntoma |

Cuando el cuerpo de una función tiene distinta longitud, la cola queda
desplazada y el epílogo —lleno de `ld`— sale como `REPLACE` fila a fila. En la
r81 eso produjo un falso patrón de 460 funciones que parecía un fallo de
compilador; contado con `objdump` sobre los dos objetos, no existía.

Las dos herramientas que leen los tipos buenos:

```bash
python scripts/tabla_op.py build/diffs/SLES-53558-A124/*.json
python scripts/desplazamientos.py build/diffs/SLES-53558-A124/*.json
```

**`tabla_op.py`** da la tabla «lo que emitimos → lo que emite el original» de
los códigos de operación, con su consistencia. Una correspondencia del 99-100 %
es una corrección directa, y la da el binario, no el nombre de la rutina:

```
vmul.xyzw   -> vmul.xyz    238   99 %
vmulax.xyzw -> vmulax.x    179   99 %
vsub.xyzw   -> vsub.xyz    133  100 %
```

El 1 % que no cuadra casi nunca es ruido: suele ser **otra rutina** con otra
forma (aquí, la distancia en el plano xz con `vmul.xz`). Y un mismo mnemónico
puede llevar máscaras distintas según la rutina: `vmulax` es `.x` en el
producto escalar y `.xyz` en la multiplicación matriz × vector.

**`desplazamientos.py`** busca cargas que solo cambian en el desplazamiento
(`lw v0, 0x58(s0)` contra `0x68(s0)`). Eso es un **fallo de layout**: un miembro
mal colocado tumba a todas las funciones que tocan la clase. Agrupado, da las
familias de clases afectadas y cuánto se desvían.

### 5. Confirmar la causa con un oráculo

Un patrón dice **dónde**; el oráculo dice **por qué**. Antes de cambiar nada:

- **Layout de una clase** → `scripts/sonda.py`. Se le pregunta al compilador el
  tamaño, la alineación y el desplazamiento de cada base y miembro, con los
  flags reales de la versión, y se compara con el volcado del original
  (`symbols/PS2/PS2_types.nothpp`). Así, en vez de razonar por qué los
  `CDAction*` iban 16 bytes cortos, la sonda dijo que `CameraAI::Action` medía
  0x18 contra 0x30 y que `UTL::COM::Object` tenía alineación 4 contra 16.
- **Qué inline expande el original** → `scripts/missinline.py`.
- **Locales, registros, orden de inlines** → `scripts/dwarf1.py` (GameCube) y
  `scripts/mdebug.py` (PS2).
- **Qué hacía el prototipo en una rama** → el desensamblado del objeto original
  (`build/<V>/obj/...`), con `build/mips_binutils/mips-linux-gnu-objdump.exe`.

Si el oráculo no puede decir cuál es la forma correcta, **no se escribe**. Se
anota y se sigue con otra cosa.

### 6. Arreglar lo mínimo

- **Solo lo imprescindible.** Si se cambian dos cosas, se prueba a quitar cada
  una: en la r81 alinear también `Factory` no cambiaba nada, y se quitó.
- **Línea a línea, comprobando el texto antes de tocarlo.** Nada de
  sustituciones a ciegas por patrón: `VU0_v4scale` escala los cuatro
  componentes y su `.xyzw` es correcto; un reemplazo global la habría roto.
- **Si una sonda puede confirmar el arreglo antes de compilar todo, se usa.**
  Con la alineación de `Object`, la sonda dio `Action` = 0x30 y el primer
  miembro de `CDActionDrive` en 0x60 —el original exacto— antes de medir nada.

### 7. Medir, verificar y commitear con la evidencia

```bash
python scripts/pctall.py
python -m ninja && sha1sum build/GOWE69/main.dol   # 9619ba57c9919f95f7f2ac951a2166a3517f91e3
```

Las **cinco** versiones, no solo la que se tocaba: un cambio de cabecera
compartida puede mover otra. El commit lleva la evidencia (la tabla, la sonda, el
antes y el después) y lo que se descartó y por qué.

## Cómo se escribe el arreglo sin hacer trampas

Las reglas de `docs/METODO.md` (cero `asm` nuestro, nada de pines ni barreras,
la lista de upstream) se aplican siempre. Además, estas formas han salido una y
otra vez:

- **Un miembro que el prototipo A124 no tiene** se guarda con
  `#ifndef EA_BUILD_A124`, y sus usos con **salida temprana**, nunca duplicando
  el cuerpo:

  ```c
  #ifndef EA_BUILD_A124
      if (!FEDatabase->bAutoSaveOverwriteConfirmed) {
          ...
          break;            /* antes era un else */
      }
  #endif
      GetMemcard()->DoAutoSave();
  ```

  Un `case` entero de un `switch` se guarda como bloque. Si un uso es una
  condición cuya forma en la A124 no se conoce, se mira en el binario; si no se
  puede saber, no se toca.

- **El inicializador de un miembro lleva la misma guarda que el miembro.** Un
  `mLastPursuit(nullptr)` guardado con otra condición que su declaración colaba
  de casualidad hasta que un define dejó de ser universal.

- **El `asm` del propio EA se corrige; el nuestro se quita.** Las rutinas VU0 de
  `UVectorMath.hpp` son código del original (en C no existe `vsub.xyz`), igual
  que el de `bMath.hpp` que conserva upstream. Corregirles la máscara para que
  digan lo mismo que el original es legítimo. Un pin de registro no lo es.

## Los fallos de medida valen tanto como los de código

Varias de las mayores subidas de la r81 no tocaron una línea de fuente. Eran
matches que ya existían y que la medida no veía:

| causa | efecto |
|---|---|
| `section_rename` solo se aplicaba a la regla de GameCube; en PS2 zFeOverlay salía con `.text` y el objetivo con `.over` | +414 funciones |
| SLUS (la versión de venta) se compilaba con los defines del prototipo | +268 |
| `symbols.json` de PC con nombres desmanglados de IDA o con mangling de GameCube, que con MSVC no casan nunca | +716 al tomar los de upstream |
| una etiqueta interna del compilador (`$L121558`) usada como nombre de función rompía el informe entero de PC | informe restaurado |

Por eso, ante una unidad entera al 0 % en una versión y bien en otra, lo primero
es sospechar de la medida: el nombre, la sección, los defines.

## Lo que casi engaña, y cómo se evita

- **Contar por nombre.** El censo de accesores vacíos contaba llamantes por
  nombre y juntaba todas las sobrecargas: anunciaba 110 accesores con llamantes y
  resultaron 3 que no bloqueaban nada. Hay que contar lo que el binario dice
  (`missinline.py`), no lo que sugiere el nombre.
- **El patrón que es síntoma.** Los `REPLACE`/`INSERT`/`DELETE` de un epílogo son
  alineación. Se desmiente contando en los objetos con `objdump`.
- **Extrapolar sin contar.** «El barrido de `unsigned long` a `u32` dio mucho en
  el frontend, hagámoslo en todo el árbol»: 802 en bruto, 82 en código
  compartido, unos 55 accionables. No era un frente.
- **Una medida que no reconstruye.** Para juzgar un cambio de símbolos hay que
  volver a trocear: con los objetos objetivo viejos, un nombre roto no da la
  cara.

## Resultado de este procedimiento en la r81

| versión | antes | después |
|---|---:|---:|
| PS2 EU (A124) | 24,72 % | **28,07 %** |
| PS2 US (venta) | 26,71 % | **32,33 %** |
| PC | 9,16 % | **9,99 %** |
| Xbox 360 | 1,89 % | 1,93 % |
| GameCube | 97,14 % | 97,14 % (DOL intacto) |

Las cinco con 0 objetos que no compilan, y ni un solo `asm` nuevo.
