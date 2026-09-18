# Brief de la ronda 52

Sustituye a `brief-r51.md`. Léelo entero antes de tocar nada.

## Estado

    python scripts/estado.py

**Es lo primero que tienes que correr.** No rehagas el censo.

    CODIGO   3.909.216 / 3.946.048 B   99,0666 %   faltan 32 funciones, 36.832 B
    DATOS    0 B de trabajo real
    ENLACE   518 / 616, techo REAL 544
             QUEDAN 27 unidades: 22 SourceLists + 5 de biblioteca (una imposible)

DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. El plan está en
`docs/ESTRATEGIA.md`.

## Lo que cambió en la r51, y cambia el trabajo

**`linked` pasó de 506 a 518**: doce unidades. El **SDK (99/99) y el STD
(109/109) están al 100 %**, y de la biblioteca sólo quedan cuatro reales
(`criticalpath`, `steering`, `madidct`, `inittmr`).

**`zSim` cerró: es la primera SourceList del frente.** Y su receta es lo más
importante que tienes, porque **doce SourceLists tienen el código al 100 % y
guardan 1,65 MB parados**: zAI 272 kB, zFe2 250 kB, zFe 174 kB, zMain 159 kB,
zEAXSound 151 kB, zPhysics 146 kB, zGameplay 141 kB, zFeOverlay 141 kB, zLua
94 kB, zMisc 78 kB, zAnim 42 kB, zGameModes.

### La receta de zSim, que es lo que hay que generalizar

1. **Mide el CONTENIDO, no los tamaños.** zSim tenía las nueve secciones del
   mismo tamaño que la base y aun así 2.172 B distintos. Se ve alineando la
   sección enlazada contra el DOL original con `difflib` y sacando el script de
   edición. En zSim eran **siete ediciones cuyos desplazamientos suman cero**,
   por eso toda comprobación de tamaños decía «idéntico».
2. **La función que reparte el pool es `varasm.c:3797 output_constant_pool`**, y
   escribe en **orden de CREACIÓN** (`force_const_mem`) — o sea **orden de
   sentencia, no de uso**. `varasm.c:3891 mark_constant_pool` tira la entrada
   que ninguna insn superviviente referencia.
3. **Un `asm()` de ámbito de fichero SIEMPRE cae delante de las vtables**,
   porque `finish_file` corre después del fichero. Para meter bytes en el
   SEGUNDO pool hay que poner el `asm()` dentro de una función.
4. **El orden de las vtables era una pista falsa**: las que `vtord` daba por
   descolocadas eran vtables muertas que el enlazador estripa. El fallo real era
   que el bloque entero iba 8 B tarde.
5. **Los andamios caducan — pero no todos.** En zSim la palanca grande fue
   QUITAR un relleno de 12 B de la r36c que sobraba por 16. Otro andamio de la
   misma ronda se re-midió y **seguía pagando**. Re-medir, no suponer.

## Reglas duras

1. **Nunca lances un `ninja` completo ni `configure.py`.** Usa
   `python scripts/build_direct.py <unidad>`. Hay otros agentes en el árbol.
2. **No hagas commits ni `git add`.**
3. **No toques `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`.**
   Proponlo con la medida; lo aplico yo en la ventana.
4. **Una cabecera compartida se propone, no se aplica** — y si la tocas para
   medir, **déjala compilable en todo momento**: en la r51 un agente dejó
   `EventSequencer.h` a medias 40 minutos y bloqueó a otros dos, que perdieron
   la ronda persiguiendo un error que no era suyo.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. **BORRA TUS VOLCADOS AL TERMINAR**, y no borres con comodines en el
   scratchpad: es compartido y en la r51 un agente se llevó por delante las
   copias de otro.
7. **Cero bytes con `asm` puesto es deuda.**
8. **Ninguna sonda de un solo uso en `scripts/`.** Van a tu scratchpad.
9. **Antes de construir una herramienta mira si existe**: `docs/HERRAMIENTAS.md`
   (generado, 106 entradas) y `docs/PLAYBOOK-MAPA.md` para el manual, que pesa
   442 kB y **no hay que leerlo entero**.
10. **Exígele a tu métrica un control que TENGA que cambiar.** En la r51 esto
    cazó dos barridos enteros que no habían compilado nada distinto.

## Trampas nuevas, todas medidas en la r51

- **Sombrear una cabecera con `-I` NO funciona si el include es RELATIVO.** Hay
  que copiar el **directorio entero**. El control que lo caza: mete un error de
  sintaxis en la sombra; si compila, la sombra no se usa.
- **El fuzzy manda al sitio equivocado**: 15 filas al 99,63 % contra 9 al
  98,27 %. Un `INSERT`/`DELETE` pesa mucho más que un operando distinto.
  **Cuenta filas con `fndiff`.**
- **El número de `$LC` NO es el orden de la `.rodata`.** Contrasta con bytes.
- **`prefijotu.py` lee el OBJETO y lo que manda es el ENLACE.** Una unidad con
  andamio de pool escrito a mano emite a propósito bytes que el estripado se
  lleva.
- **«Los símbolos de más cuestan cero» tiene excepción**: si otra unidad los
  referencia, cuestan. En `interface` costaban 15.206 B. Pasa `refs.py`.

## Dos palancas nuevas del catálogo

- **`__asm__("")` es un CORTE DE REGIÓN de `sched1`**, no sólo una barrera, y
  cuesta cero bytes. Llevó `UpdatePlatInfo` de 28 filas a 24.
  **Límite medido**: no puede ADELANTAR una insn de una sentencia posterior.
- **`__asm__("" : : : "rN")` mueve el reparto de ENTEROS** de `local_alloc`,
  ocupando el registro en `regs_live_at`. Hasta ahora el catálogo sólo tenía
  palancas para coma flotante.
- Y para Metrowerks (`steering`, `OdemuExi2`, `Packages`) el catálogo es OTRO:
  está en `r51-steer.md` §1. `PLAYBOOK.md` entero está escrito para GCC.

## El reparto

| agente | encargo |
|---|---|
| `rodata1` | **zPhysics** (`rodata−784`) y **zAnim** (`−1240`): las dos con el código al 100 % y el déficit más pequeño |
| `rodata2` | **zLua** (`−1656`, permorden 1) y **zMain** (`−1720`, permorden 19) |
| `cam` | las 5 de zCamera (10.812 B) — `TrackCar` ya en 6 filas y `_Storage` en 15 |
| `ecs` | las 4 de zEcstasy — `UpdatePlatInfo` está a **1 fila** |
| `world` | zWorld (4 fn) y zWorld2 (2 fn) — `HolePunchAvoidables` al 99,97 % |
| `varios` | zEagl4Anim (2), zPlatform (1), zTrack (1), zSpeech (1) |
| `crit` | `criticalpath` (1 fn) y `madidct` (2 fn al 46 %/50 %) |

## Lo que me quedo yo

- El paquete de `keep.lst` y los `$LC`: **cualquier literal que añadáis desplaza
  los `$LC` y deja rancias sus entradas**. Vosotros proponéis las líneas
  (`deadlink.py --keep`, `prefijotu.py --asm`), yo las aplico y paso `lcfix`.
- La coordinación de `splits.txt` que le falta a `steering` (40 B de `.bss`,
  40 de `.sdata2`, `g_bInitialized` fuera de rango). **Por eso `steering` no
  está en el reparto**: cerrar sus seis funciones no la promocionaría.
- La frontera `zGameModes`/`zGameplay`.
