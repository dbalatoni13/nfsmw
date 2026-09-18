# Ronda 35 — instrucciones comunes

## LEE PRIMERO `brief-r34.md`, `brief-r33.md` y `brief-r32.md` — siguen vigentes

Aquí va **sólo lo que la r34 cambió**, y cambió mucho.

---

## 0. El frente de enlace está cerrado por fuente

Al empezar la r34 había **42 símbolos indefinidos** en 7 SourceLists. Quedan
**12, y ninguno tiene arreglo de fuente**: son cuatro rangos de `splits.txt` mal
atribuidos, con la evidencia en `r34-jf-splits-propuesta.md`.

Los 30 cerrados salieron de tres patrones, todos reutilizables:

1. **Definiciones que sencillamente faltaban** (`EAGL4Anim::MatrixMultiply`, las
   diez de zEAXSound, las siete de zFeOverlay). La cabecera suele traer la
   dirección en un comentario al lado; `symbols.txt` da tamaño y sección, y el
   objeto extraído da el valor.
2. **Etiquetas del pool referenciadas y no emitidas**: se escriben como **dato en
   `asm()`**, no en C++. Definirlas en C++ deja que GCC pliegue el valor y cambia
   el código; en `asm()` el compilador no ve definición, cada uso sigue siendo
   referencia externa y el `.text` no se mueve ni un byte.
3. **La virtual que era pura** — ver §1.

## 1. La virtual que era pura: patrón nuevo, y la herramienta que lo cierra

Una función virtual **declarada no-pura y sin definir en ningún sitio** deja a la
clase **sin «key method»**, y entonces GCC 2.9 **no emite su vtable en ninguna
unidad de traducción**. El error habla de una vtable, no de la función.

Salieron tres (`IListener::OnCollision`, `ITrafficCenter::GetTrafficBasis`,
`AIAvoidable::OnUpdateAvoidable`) y **`scripts/ghostref.py` dice ahora 0**: el
frente está cerrado, no quedan más.

Se comprueba **sin compilar**: el símbolo no está en el ELF **y** —esto es lo que
prueba— la ranura de la vtable en el objeto extraído tiene una reubicación a
`__pure_virtual`. La ausencia sola no vale: el ELF define 24.857 símbolos y
`symbols.txt` tiene más líneas.

**Antes de poner `= 0`**: comprobar que todas las derivadas la sobreescriben, y
medir el radio de explosión compilando **las 33 SourceLists a un directorio
aparte** (no toca los objetos que usan los demás agentes).

## 2. El `.previous` — el detalle que cuesta una hora

Todo bloque `asm()` que emita datos **tiene que cerrar con un `.previous`**. Sin
él la sección se queda en `.rodata`, GCC sigue emitiendo funciones sin un `.text`
explícito, y el `.4byte .L_LC-.L_text_b` de `.section .line` cruza secciones.
`ngcas` corta con «Required relocation cant be represented in ELF format» y
señala **90.000 líneas más abajo, en otro fichero**.

Y el sitio del bloque importa: **el prefijo de bWare/STL va DELANTE de todos los
`#include`**. Detrás, GCC 2.9 lo emite al **final** de la `.rodata`. Moverlo
valió más que escribirlo: `zRender` y `zMiscSmall` pasaron de 6,4 % y 10,8 % a
**100 %** de `.rodata` idéntica.

## 3. `rangechk.py` y `ghostref.py`: dos herramientas nuevas

**`scripts/rangechk.py`** — el ELF trae **577 `STT_FILE`**, y sus nombres SON las
SourceLists (`zGameplay.cpp`, `zLua.cpp`…): **el original se compilaba con esta
misma estructura**. Enfrenta esa verdad con `splits.txt` de una pasada.

- `--text`: los **398 `gcc2_compiled.`** dan el inicio EXACTO del `.text` de cada
  TU. Resultado: **sólo 4 rangos de 534 se tragan una TU ajena**, o sea que el
  `.text` de `splits.txt` está bien y **el trabajo que queda está en los datos**.
- `--huecos`: dónde puede estar un límite mal puesto.
- Límite: los **globales no llevan atribución** (van detrás de los locales en el
  `symtab`), y son justo los que caen en los huecos. Ahí el ELF **acota** pero no
  fija; para fijar, contar **quién referencia cada `lbl_`** en los extraídos.

**`scripts/ghostref.py`** — lo que referenciamos y el original no tenía.

## 4. Lo más rentable que queda, por orden

`promodist.py` da la foto: **69 unidades sin promocionar, y 31 tienen el `.text`
YA PERFECTO** (573.468 B). El trabajo es de datos, no de match.

1. **`zFEng`** (71.460 B) — 0 funciones imperfectas, **orden perfecto desde la
   r34**, y sólo `rodata−648 data−76`.
2. **`zAttribSys`** (48.776 B) — 0 imperfectas, `rodata−416 data−48`, y **la
   receta exacta está escrita** en `r34-jf-zattribsys.md`: veinte cadenas con su
   desplazamiento. Ojo, el `.text` NO estorba aunque emitamos 3 funciones de más:
   **el enlazador se las lleva**, medido.
3. **`zAnim`** (42.292 B) y **`zFoundation`** (36.000 B) — 0 imperfectas, deltas
   de dato de menos de 1,4 kB.
4. **`zMain`** (159.776 B) y **`zSim`** (96.400 B) — 0 imperfectas; a `zSim` le
   quedan 21 funciones descolocadas (tres reales, ver `r34-ord3.md` §5.2).
5. **Las cabeceras generadas de AttribSys.** `dat2` lo dejó probado: **la lista
   de cadenas `Attrib::Gen::*` que faltan ES la lista de cabeceras que hay que
   incluir**, y no sólo añaden bytes, **recolocan el pool** (`zSpeech` 41,0 →
   71,3 % de `.rodata` idéntica). En todo el árbol faltan 3.976 B y sobran 3.470.
6. **`BuildMessageTable`**: las cadenas que faltan **en las cuatro grandes a la
   vez** son nombres de campo que en el original vivían **dentro de
   `Generated/Messages/*.h`** y nosotros tenemos en `EventDefs.cpp`. Toca las 33.
7. **El relleno de alineación de los bloques a mano.** No lo reproducimos: en
   `zMisc` eran 14 huecos a cero de menos de 8 B, y con sus `.balign` pasó de
   36,9 % a **94,7 %**. Hay herramienta en el scratchpad de `pre2`.
8. **`gc_interface`**: `asd2` **no es una unidad**, son los `_GLOBAL_.I/.D` de
   `gc_interface.cpp`. Fundiéndolo, el enlace pasa de FALLA a **0 errores**.

## 5. Vedas de la r33 que la r34 ROMPIÓ — no las cites

- **Las vtables NO deciden dónde va una función.** Caso mínimo con `cc1plus`:
  salen en bloque, en orden inverso de definición, **antes que todas** las
  funciones. El racimo de `zSim` que la r33 vetó «por ranuras de vtable» era un
  destructor implícito, y `zMisc` cerró **sin tocar una sola vtable**.
- **Mecanismo nuevo, y muy rentable**: un `~X` cuyo `fichero:línea` en
  `debug_lines` es un `.cpp` mientras la clase vive en una cabecera **no está
  declarado en el original**: es sintetizado, y GCC 2.9 lo emite **por delante**
  de todas las inline de esa clase.
- **La causa más rentable del desorden son las cabeceras que incluyen de más**:
  `ResourceLoader.hpp` y `bFile.hpp` incluyen `driver.h` sin usarlo, y sólo por
  eso 13 funciones de `zMisc` se parseaban 14 ficheros antes.

## 6. Método — igual que la r34

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra; si no cierra, **revierte** y anota la veda
  diciendo **qué sentencia barriste**.
- **Base y medida SEGUIDAS.** Y **si algo falla, mira la base antes de acusar a
  tu cambio**.
- **`main.dol` se queda con el hash bueno cuando `ninja` falla**: mira la **marca
  de tiempo**.
- Detalle en `docs/analisis/r35-<grupo>.md`. **No commits.**
- **Parchea en binario** los ficheros con finales de línea mezclados.

## 7. Prohibido

- **Escribir ensamblador de instrucciones.** Emitir **datos** sí es legítimo.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`: PROPONER con verificación.**
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **Borra tus temporales.** El scratchpad llegó a **2,2 GB** en la r34, y el
  disco lleno se lee como «la variante no cambia nada».
