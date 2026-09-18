# Ronda 36 — instrucciones comunes

Estado: **98,1043 % matched**, 18.337 funciones, 617 unidades, `main.dol`
verificado byte a byte. **`undlist` da 1 símbolo indefinido en todo el proyecto.**

## LEE PRIMERO `brief-r35.md`, y de la r35 estos informes según tu encargo

`r35-att.md` (el pool escrito a mano), `r35-ord4.md` (orden), `r35-gen.md`
(dato de las cuatro grandes), `r35-spl.md` (rangos), y los `r35-jf-*.md`.

---

## 0. La medida cambió: usa `linkdelta.py`, no los tamaños del objeto

`promodist.py` y `datacmp.py` comparan tamaños de sección **del objeto**, y eso
engaña: el enlazador lleva `-strip-unused-data` y **se lleva todo lo que no se
referencia, código incluido**. `zLua` tiene el `.text` del objeto +18.908 B y
enlazado sale **+0**.

**`scripts/linkdelta.py`** enlaza el proyecto sustituyendo una unidad cada vez y
compara contra el enlace base. Con esa medida, **20 de 28 SourceLists tienen el
`.text` a delta 0** y lo que bloquea es `.rodata`.

Y **`scripts/stripmap.py`** (de `att`) separa el daño de estripado del dato
ausente: `+keep == 0` el dato ya está entero y sólo fallan los NOMBRES;
`+keep == test` es dato ausente puro; `+keep > 0` a la unidad le SOBRA dato.
**En 19 de 25 unidades el estripado pesa más que el dato ausente.**

## 1. El mecanismo que cierra el dato: el pool escrito a mano

`keep.lst` fuerza el dato muerto con el nombre que le da `dtk` en el objeto
**extraído** (`$LC2151483872`); el nuestro se llama `$LC60`, la entrada no casa
con nada y `-strip-unused-data` se lleva `size & ~7` de cada cadena. **`keepchk.py`
NO lo ve**, porque valida contra el objeto que hoy está en el enlace.

La receta, probada en `zAttribSys` (`.rodata` 1176/1176 al **100 %**):

1. `scripts/genrodata.py <unidad> <ini> <fin>` genera el `asm()` del rango con los
   nombres de `dtk`. Aborta si eliges mal los límites, que es lo que quieres.
2. El bloque va **delante de todos los `#include`** de la SourceList.
3. Hay que **impedir que `cc1plus` interne esas mismas cadenas**, o emitirá su
   copia detrás. El patrón es una guarda por unidad (`ZATTRIBSYS_HAND_POOL`) y,
   para las cadenas VIVAS, el renombrado `asm()` de GCC:
   `extern const char X[] asm("$LC...");`
4. **Veda: no vale para constantes numéricas.** El pool de flotantes se emite en
   RTL, no como literal.

Mecanismo relacionado y muy rentable: **GCC 2.9 emite el literal de una función
que NO llega a emitir**. Una sola `inline` sin usar, nombrando las cadenas en el
orden del objetivo, llevó una `.rodata` del 13,3 % al 87,4 %.

## 2. De qué está hecho el hueco de `.rodata`

Medido: **el hueco son CADENAS AUSENTES** (22.746 B contra 19.624 B de hueco
neto; pasa del 100 % porque también emitimos cadenas que el objetivo no tiene).
Cuatro clases:

1. **Nombres de ámbito de profiling**, que son el *texto de la sentencia medida*
   (`'m_pNFSMixMaster->ProcessMixMap(t)'`, `'GetFrontEnd()->Update()'`). Hay
   **104 marcadores `"TODO"`** en el árbol donde iba la cadena real, 75 en
   `ProfileNode` y el resto en nombres de `bMalloc`.
2. **Nombres de asignación de memoria** (`'AUD:PF MUSIC Stream Channel'`).
3. **Tablas de nombres de enumerado** (la `PERF_PART_*` entera en `zWorld`).
4. **Las `Attrib::Gen::<clase>`**: lista exacta por unidad en
   `r35-jf-attribgen.md` (faltan 118, sobran 154).

## 3. Herramientas nuevas de la r35

- `linkdelta.py` — distancia real al enlace, sección por sección.
- `stripmap.py` — estripado contra dato ausente.
- `datadiff.py` — diff byte a byte con resincronización: dice **qué** falta.
- `genrodata.py` — genera el `asm()` de un rango con los nombres de `dtk`.
- `dwarfmap.py` — de quién es cada dato, según el DWARF. `--limites` da los
  límites de `splits.txt` a corregir con su intervalo.
- `stripped.py` — qué emitía el original y el enlazador descartó (marcador
  `0xFFFFFFFF` en el DWARF).
- `ps2map.py` — el mapa del alpha de PS2. `--tres` cruza los tres órdenes y dice
  cuáles de nuestras funciones descolocadas **corrobora PS2**.
- `rangechk.py`, `ghostref.py`, `residuo.py`, `undlist.py`.

## 4. El fuente del compilador está en el árbol

**`orig/prodg/NGC_GNU_SRC/`** tiene el front-end de C++ de **nuestro compilador
exacto** — `version.c:135` forma `"2.95.3 SN BUILD v<x>.<y> for Nintendo
Gamecube"`, que es el `AT_producer` del DWARF del juego. `gcc/cp/decl2.c` trae
`finish_file` en la línea 3643, y están `class.c`, `method.c`, `varasm.c`.

**El orden de emisión ya no se deduce: se lee.** Y diferenciando contra el GCC
2.95.3 de serie salen las modificaciones de SN, que son las que no se adivinan.

Lo que **no** está: `ngcld` y `ngcas` son propiedad de SN. La aritmética
`size & ~7` del estripado y el marcador `0xFFFFFFFF` siguen siendo sólo medida
nuestra.

## 5. Vedas de la r35 — no las reabras

- **`order:` de `splits.txt` no rompe los ciclos.** Existe y va en la línea de la
  unidad, pero la detección corre sobre el grafo derivado. Tres ensayos, incluido
  uno con el ciclo reducido a tres unidades con nombre: idéntico las tres veces.
- **`datacmp.py` mide POSICIÓN, no orden**: meter 16 B correctos en medio lo
  hunde. Para orden, LCS.
- **`DOL OK` no está a 4 kB de las cuatro grandes**: `trypromo` compara el sha1
  del DOL entero, y `zFe`/`zFe2`/`zPhysics`/`zSpeech` tienen funciones por debajo
  del 100 %. Por dato solo no promocionan.
- **La posición de la declaración de un destructor no mueve su ranura** (medido:
  `.text` byte a byte idéntico).

## 6. Método

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra; si no cierra, **revierte** y anota la veda
  diciendo **qué sentencia barriste**.
- **Base y medida SEGUIDAS.** Si algo falla, **mira la base antes de acusar a tu
  cambio**: en la r35 la base se rompió tres veces por otros agentes.
- **`main.dol` se queda con el hash bueno cuando `ninja` falla**: mira la marca de
  tiempo.
- **Al copiar ficheros, actualiza su marca de tiempo** (`shutil.copy2` conserva la
  del origen y `ninja` no recompila).
- `build_direct.py <unidad>` para objetos sueltos; **nunca un `ninja` completo**.
- Informe en `docs/analisis/r36-<grupo>.md`. **No commits.**

## 7. Prohibido

- **Escribir ensamblador de instrucciones.** Emitir **datos** sí es legítimo.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`: PROPONER con verificación**,
  y dejar el paquete aparte en el scratchpad.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp`.
- **Borra tus temporales.**
