# Ronda 33 — instrucciones comunes

Estado: **97,974 % matched**, 18.337 funciones, **`linked` 13,30 %** (456 de 604),
**Game Code 0,88 %** (4 de 33), `main.dol: OK`
(`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).

## LEE PRIMERO `docs/analisis/brief-r32.md`, `docs/HERRAMIENTAS.md` y el informe de tu predecesor

El brief de la r32 sigue vigente entero. Aquí va **sólo lo que la r32 ha
cambiado**.

---

## 0. Ya hay tres SourceLists dentro, y la receta está probada

`zDebug` (r31), **`zDynamics` y `zMission`** (r32). Y **`zFe`, `zSpeech` y
`zPhysics` ya ENLAZAN** — 499.212 B a un paso.

**`trypromo.py` ANTES de gastar un ensayo en codegen.** Es la regla que más ha
pagado: en la r32 los cinco agentes la aplicaron y tres descubrieron que su
unidad tenía otro bloqueo antes de tocar una función.

## 1. La receta del pool, probada dos veces (zDynamics y zMission)

**BORRAR el pool escrito a mano, dejar que `cc1plus` lo emita donde ya lo emite,
y escribir a mano SÓLO lo que el compilador no genera** (las cadenas con el
`__FILE__` de EA y el dato muerto).

Y **la veda de la r31 que decía lo contrario era falsa**: contado sobre los 604
objetos extraídos y los 523 nuestros, **cero referencias externas** a esas
etiquetas. La vía inversa (`extern const float lbl_…`) **no escala**: el pool de
GCC 2.9 es **por función**, así que la misma constante tiene etiqueta distinta en
cada una, y una cabecera compartida no puede referenciar una etiqueta distinta
por llamante.

Tres mecanismos de colocación, medidos:

1. **Un `asm()` de ámbito de fichero ENTRE dos funciones** de un `.cpp` incluido
   se emite **entre sus dos pools**.
2. **Para llegar DETRÁS de las vtables**: un **estático de plantilla instanciado
   por un puntero `static` sin usar** se emite en la última vuelta de
   `finish_file`. El puntero no cuesta un byte. La instanciación **explícita** no
   vale.
3. **`.text` al final de un `asm()` DESINCRONIZA a GCC**: la siguiente constante
   de pool sale sin `.section .rodata` y cae en `.text`. **Cierra con
   `.previous`.**

Herramienta: **`scripts/lcmap.py`** — el mapa `$LC` ↔ `lbl_` por reubicaciones
insn a insn con cotejo de bytes. En `zEcstasy` da 551 pares con **2 CONFLICTO**:
ahí un mapa posicional sería el error silencioso.

## 2. `inline` no es una optimización, es una POSICIÓN

GCC 2.9 emite todo lo inline en `finish_file`, al final de la unidad. Con eso se
colocaron **218 de 405 funciones** y `zFoundation` quedó con el **orden
perfecto**. El bloque diferido se ordena por **orden de parse de la DEFINICIÓN de
la clase**, las vtables salen **al revés**, y `finish_file` corre **en pasadas**.

**`scripts/textorder.py`** compara el orden **sin enlazar**. Aviso de lectura:
su columna «descolocadas» cuenta también el **deslizamiento** que provoca una
función corta — `filesys` daba «58 de 75» y en realidad su orden es perfecto y
sólo le falta una instrucción. **Léela siempre con el delta de tamaño al lado.**

## 3. `-strip-unused-data` no borra el símbolo muerto entero

**Le quita `size & ~7` y deja los `size % 8` bytes finales.** Verificado byte a
byte: `"Attrib::Array\0"` (14 B) sobrevive como `"Array\0"`, `"Attrib::RefSpec\0"`
(16 B) desaparece, `"STL\0"` (4 B) no se toca. Por eso el delta del DOL llega a
ser **cuatro veces** el delta del objeto, y por eso **4.864 de los 7.920 B que
«faltan» en `.rodata` son daño de estripado**, no dato ausente.

Y el `.bss` de GCC 2.9 tiene **TRES GRUPOS** (estáticos locales / objetos con
constructor / definiciones tentativas diferidas); el orden lo decide el grupo, no
el fichero.

## 4. Reclamar un rango de `splits.txt`: DE UNO EN UNO

Un lote de ocho rangos rompió el troceado y hubo que revertirlo entero. Dos modos
de fallo, ninguno relacionado con que el rango sea correcto:

- **Un rango no puede acabar dentro de un símbolo.** `claimrange.py` calcula el
  final con el último símbolo que emite **nuestro** objeto, y el del objetivo
  puede ser mayor.
- **Los rangos de datos FIJAN EL ORDEN DE ENLACE y pueden hacerlo cíclico.**
  Sirve además para detectar el anclaje débil: el que sobra es el que ya salía
  «anclaje NO único».

**`trypromo.py` y `keepchk.py` entre rango y rango.** En bloque, el primer fallo
esconde a los demás.

## 5. Dos ejes cerrados que NO hay que reabrir

- **`filesys/AddToQueue`**: 8 formas de fuente + 10 banderas + 5 de iterador.
  `-fno-cse-follow-jumps` da el tamaño exacto y **pierde 14 funciones en
  realcore**.
- **`pathnode/PATHI_nextnode`**: `alloc.py` da los allocnos y el umbral exacto de
  `n_refs`, y **subirlo NO produce el intercambio** (+1 ref sube 0,13 pp, +2 y +3
  bajan a 90,52 %). **Acertar el orden de `global_alloc` no basta para acertar el
  registro**: no leas sus cifras como una receta.

## 6. Método (igual que la r32)

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra; si no cierra, **revierte** y anota la veda
  diciendo **qué sentencia barriste**.
- **Base y medida SEGUIDAS.** Si algo falla, comprueba la base antes de acusar a
  tu cambio: en la r32 dos agentes se encontraron el enlace roto por un tercero.
- Detalle en `docs/analisis/r33-<grupo>.md`. **No commits.**

## 7. Prohibido

- **Escribir ensamblador de instrucciones.** Emitir **datos** sí es legítimo.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`: PROPONER con verificación**
  (`trypromo.py` + DOL); puedes editarlos para medir si los dejas como estaban.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).

## Convivencia

Scratchpad con tu prefijo `c33<grupo>_`. Hay otros cuatro agentes en el mismo
`build/`: mide con objeto suelto (`build_direct.py`/`mn_repro.py`), nunca con un
`ninja` completo. **Vigila el disco** (17 GB) y borra tus temporales al acabar.
