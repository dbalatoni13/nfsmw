# Ronda 34 — instrucciones comunes

Estado: **97,974 % matched**, 18.337 funciones, **`linked` 13,43 %** (459 de 607),
`main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).

## LEE PRIMERO `docs/analisis/brief-r33.md` y `brief-r32.md` — siguen vigentes enteros

Aquí va **sólo lo que la r33 cambió**.

---

## 0. Cuatro SourceLists grandes están bloqueadas SÓLO POR DATOS

`zFe2` (250.732 B), `zFe` (174.200), `zSpeech` (178.888) y `zPhysics` (146.124)
**ya enlazan**: `trypromo.py` da `DOL ROTO`, no `ENLACE FALLA`. Sus `.text` salen
del mismo tamaño y el mismo desplazamiento. **750.000 B esperando `.rodata`.**

Y el dato que ordena la ronda: **36 unidades tienen ya el `.text` perfecto y
siguen sin enlazar** (672.640 B). Llegar al 100 % de match no las movería. **Sólo
el 20 % del código no enlazado está bloqueado por match.**

## 1. `trypromo.py` antes de todo, y el ENLACE BASE entre cambio y cambio

Regla nueva de la r33, y cazó rangos que trocean sin una queja y rompen el DOL:
**el enlace base cuesta 2,6 s y separa «rango correcto» de «rango expresable»**.
Va entre rango y rango, antes de promocionar nada.

Y **si algo falla, comprueba la base antes de acusar a tu cambio**: en la r33 dos
agentes se encontraron el árbol roto por un tercero.

**Trampa que me costó una hora**: cuando `ninja` falla, **`main.dol` se queda en
disco con el hash bueno del build anterior**. Comprobar el sha1 sin mirar si el
enlace corrió da `OK` siempre. **Mira la marca de tiempo del fichero.**

## 2. AttribSys: aplicado, y el frente sigue abierto

Nuestras cabeceras internaban **19 etiquetas `Attrib::*` por TU y el original
8**, en otro orden. Ya está corregido (−232 B exactos en las cuatro unidades
medidas, `zDynamics` intacta, `main.dol` byte a byte).

**Lo que queda es la otra mitad, y la encontraron dos agentes por separado**: lo
que falta en `zFe`/`zSpeech`/`zPhysics` son **cadenas `Attrib::Gen::<clase>` de
cabeceras GENERADAS de AttribSys que el TU original incluía y el nuestro no**
(22-48 % del hueco), más un racimo compartido (`WorldBodyConn`, `Pkt_Body_*`,
`MAudioReflection`…) que falta **en las tres a la vez**.

## 3. El estripado: mídelo bien

`-strip-unused-data` quita **`size & ~7`** y deja los `size % 8` finales **más el
relleno de alineación**. La estimación por reubicaciones **se queda corta** (872 B
frente a 2.624 reales en `zFe`): la medida buena es **emparejar por nombre
nuestros símbolos con los del ELF ya enlazado y leer el desplazamiento
acumulado**.

Se recupera forzando en `keep.lst` **sólo** los símbolos de `.rodata` **muertos,
locales y sin reubicaciones dentro** — así el `.text` queda intacto; el
`keep.lst` a lo bruto de la r32 movía `.text` +128/+3.232 B. **`keep.lst` no
admite comodines**, medido.

## 4. El orden del `.text`: el protocolo

**`symbols/debug_lines.txt` da GRATIS el orden de parseo del original** — el
fichero:línea de cada función del bloque diferido, leído por dirección. El
nuestro sale del `.ii` preprocesado. Cotejarlos dice qué `#include` reordenar,
**en el `.cpp`, no en la SourceList**. Reordenar diez `#include` de
`Simulation.cpp` valió **45 funciones**.

Y para tocar una cabecera compartida sin romper nada: **`#ifndef GUARDA` en la
cabecera + `#define` en la SourceList** = radio de explosión **cero por
construcción**.

## 5. Vedas nuevas de la r33 — no las reabras

- **Las vtables duplicadas NO bloquean**: `gc_blockcalculator` promociona con
  `DOL OK` emitiendo sus 96 B duplicados.
- **Un rango que empieza en `4 mod 8` no se puede expresar**, y el `align:4` por
  rango no lo arregla.
- **Devolverle el pool al compilador deja CORTO**: sólo genera el 61 % del pool
  del objetivo. La receta de la r32 vale para `zDynamics`/`zMission`, **no** para
  `zMisc`/`zEcstasy`.
- **Renombrar un símbolo al UID de hoy hace enlazar y cuesta −624 B y −16
  funciones**, porque objdiff empareja por nombre. La forma buena son **dos
  etiquetas en la misma dirección**, y el `.type ...,@function` **no es
  opcional**: sin él objdiff aborta y la unidad entera deja de medirse.
- **`alloc.py` acierta el orden de `global_alloc` pero NO el registro.** Sus
  cifras no son una receta (probado y refutado en `pathnode`).

## 6. Método

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra; si no cierra, **revierte** y anota la veda
  diciendo **qué sentencia barriste**.
- **Base y medida SEGUIDAS**, y la base tomada deshaciendo **sólo tus trozos**
  (`git checkout` no vale con otros agentes editando).
- Detalle en `docs/analisis/r34-<grupo>.md`. **No commits.**
- **Finales de línea**: `VehicleDB.cpp` tiene 1.418 CRLF + 222 LF mezclados y un
  parche en modo texto borra 1.418 bytes **en silencio**. La herramienta `Edit`
  normaliza a CRLF. Parchea en binario.

## 7. Prohibido

- **Escribir ensamblador de instrucciones.** Emitir **datos** sí es legítimo.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`: PROPONER con verificación**;
  puedes editarlos para medir si los dejas como estaban.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).

## Convivencia

Scratchpad `c34<grupo>_`. Otros tres agentes en el mismo `build/`: objeto suelto,
nunca `ninja` completo. Vigila el disco y borra tus temporales al acabar.
