# r71: el oráculo contra las near-miss — Initialize 14→8 filas y barrido completo

Ronda con `dwarf1.py` (el .debug del GC final) como oráculo ANTES de escribir
variantes, sobre las near-miss del brief. **DynamicLoader::Initialize baja de 14
a 8 filas divergentes (99,64 → 99,74 %)** con la palanca de fuente correcta; dos
funciones de la lista resultan YA cerradas; el resto queda nombrado con registro
y prioridad. Herramientas nuevas: el oráculo apuntado a NUESTRO .o (diff de
fichas DWARF objetivo/nuestro) y un banco rápido reutilizable.

## 1. Initialize (zEagl4Anim, 2.352 B): 14 → 12 → 8 filas — LA PALANCA

r64 midió en micro que `(S*)(i*sizeof(S) + (unsigned)p)` voltea el `add` a
índice-primero, pero no lo aplicó (y el micro decía que el lwzx no se movía).
Aplicado a la fuente REAL funciona y arrastra el lwzx plegado:

- **V1** `sheader = (ELFSectionHeader*)(i*sizeof(ELFSectionHeader) +
  (unsigned)p)` (en vez de `&reinterpret_cast<...>(p)[i]`): 14→12. Voltea el
  `add r30,r11,r10` (fila 343) Y el `lwzx` de sh_name con el add plegado (369).
- **V4** la misma forma en el bucle de symtab: `sym = (ELF32_Sym*)
  (i*sizeof(ELF32_Sym) + (unsigned)pHP->symtab)`: 12→8. Voltea las 4 filas del
  cluster `lwzx/add/stwx` de (i*16 + symtab) (499/500/511/541).
- Neutras/peores: V2 (las 27 indexaciones de p) NEUTRA; W1
  (`strcpy(&s[nameLength] + 1)`) PEOR (10); W2 (reordenar declaraciones del
  bloque STRTAB al orden del oráculo: t, typebuf, type_separator) IDENTICA — el
  pseudo nace en la ASIGNACION, no en la declaracion.
- **Las 8 que quedan**: swap t↔nameLength en r29/r30. El oráculo da el árbol de
  bloques exacto: target t=r29, type_separator=r30, nameLength=r30 (comparte
  ventana muerta); nosotros t=r30 (fusionado en la qty de type_separator al
  nacer donde este muere), nameLength=r29. Con las cifras de qty: nameLength
  (5 refs/8, pri 12500) deberia ganar r30 en los dos builds; en el nuestro la
  qty {type_separator,t} (5/12, pri 8333) vive hasta 424 y CONFLICTA con
  nameLength [423..430] → la empuja a r29. La palanca seria evitar que t se
  fusione con la qty de type_separator (que en el target NO se fusiona): sin
  forma conocida.

## 2. Herramientas (tools/scratch/vedas_oraculo/)

- **`dw1ours.py <unidad> <fn>`** — `dwarf1.py` apuntado a `build/GOWE69/src/...o`
  (nuestro). El `diff` de fichas contra `python scripts/dwarf1.py fn <fn>` da
  TODAS las diferencias estructurales (locales que faltan, registros, params de
  inlines, árbol de bloques) en un vistazo. ES la herramienta con la que se
  encontro V1/V4 y el árbol de bloques de Initialize.
- **`bench.py <cpp> [UNIT=] [DL=1] [EXTRA_INC=dir;dir] [REFRESH=1]`** — banco
  suelto de un .cpp de una SourceList: copia editable `bench_<name>.cpp` (NO se
  pisa en corridas posteriores; REFRESH=1 la regenera — BUG corregido hoy: la
  primera version la pisaba y las medidas de variantes median el BASELINE),
  `dumpbench.py` deja `bench_main.i.lreg/.greg`, **`measure1.py <sym> [UNIT=]`**
  cuenta filas divergentes contra el .o objetivo. ~8 s por iteracion.
- **`rows.py <unidad> <sym>`** — filas divergentes contra el build (SCRATCH propio).
- `eP.py`/`epCalcgreg.py`/`rfocgreg.py` — extractores de dispositions/allocnos
  (plantillas para otras funciones).

## 3. eProject (zEcstasy, 268 B, 93,97 %, 14 filas) — la mecánica completa

Leído el ASIGNADOR exacto en `orig/prodg/.../gcc`:

- **`REG_ALLOC_ORDER` (rs6000.h:932)**: los float se reparten en orden
  **f0, f13, f12, f11, f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, f31...f14**.
  f0 primero y LUEGO DESCENDENTE desde f13. Explica TODO el baile f0/f13 de la
  r48 (halfVP2 con pri máxima cae en f13 porque la qty {eye.x,clipX} ya tenía
  f0 en la ventana solapada) y por qué x2 (pri 12727) lo volteó.
- Los floats los asigna LOCAL-alloc: fusión por muere-uno-nace-otro, pase de
  SUGERENCIAS (`qty_phys_copy_sugg`) ANTES del pase por prioridad, ventanas
  falsas ±2 insns con `flag_schedule_insns_after_reload`.
- La veda queda con causa exacta: cerrar (a) exige subir n_refs de la qty
  {vp[2],halfVP2} de 5 a 7 — sin asm no hay forma conocida de añadir 2 refs
  sin emitir código.

## 4. epCalculate (zEcstasy, 2.072 B, 155 filas) — la reapertura de r68, respondida

r68 la condicionó a "el layout original de locales (DWARF vars)". `dw1ours.py`
lo da ahora: **el layout nombrado CASA AL BYTE** (specular_power +8 ...
specular_a +124, los 10 doubles de stack, todos los f-regs nombrados). Diferencias
DWARF reales: solo la rotación dcrg f3→f5, dcba f2→f4, scba f4→f13, l1 f7→f5,
zero f31→f30. El +8 del frame (0x178 vs 0x170) NO es un local nombrado: es un
slot TEMP anónimo (el double por bits del bucle: 0x88 objetivo, 0x90 nuestro).
La palanca no está en declaraciones ni locales — veda.

## 5. RenderFlaresOnCar (zWorld, 2.908 B, 98,05 %, 62 filas)

Diff de fichas: TRES diferencias DWARF, ninguna toca .text: los "locales"
ShaperLightsCharacters/Backup del original van `(sin AT_location)` y el frame es
0xf8 en ambos (no caben 2×136 B en stack: emisión del compilador); el ctor
ProfileNode con 2 args (cuerpo vacío, cero código); `address` de
eFrameMallocMatrix optimizado fuera allá / r4 aquí (las filas 30-47 ya casan).

El .text real: rotación {position r16→r15, flashHeadlights r15→r14} + cascada
(mfcr r14→r9+stack, orden de stw de cr, hoist del lis). Cabeza: el pseudo de
`position` (84: 6 refs/607, pri 197) se lleva r16; en el objetivo algo con pri
>197 ocupa r16 y nadie lo lista (temp anónimo). Medida nueva (con el banco
arreglado): `base_headlight_intensity = 1.0f` → `= lbl_8040AD04` en las dos
ramas EMPEORA (94 filas, 2884 B) — r49 negative confirmada y extendida.

## 6. Resto del barrido (cifras de hoy)

| función | filas | qué queda |
|---|---:|---|
| EvalState (zEagl4Anim, 456 B) | 18 | árbol de inlines del oráculo = el nuestro; rotación de temps sin nombre; loop.c no iza el offset (r50/r62 en pie) |
| DefragmentPool (zWorld, 684 B) | ~15 | rotación r21↔r22/r17↔r16/r27↔r25 pura |
| InitAtSegment (zWorld2, 816 B) | 12 | swap r10↔r11 puro |
| HolePunchAvoidables (zWorld2) | 4 | veda r68 (live de is_drag/is_racer) |
| TrackCar::Update (zCamera, 992 B) | 99,63 % | vedas R1-R3 (r70b §2: f0/f13 muertas) |
| __static_init (zCamera, 3.604 B) | 99,84 % | veda r50 asignador |
| **UpdateWheelYRenderOffset** | **0** | **YA CERRADA a nivel de instrucciones** |
| **TerrainVelocityNoise** | **100 %** | **YA CERRADA** |

## 7. Conclusión

Las near-miss vivas del brief son casi todas la MISMA familia: permutación de
registros de temps sin nombre, con el empate roto por nº de pseudo/orden de
expansión, y las formas que cambian el orden también mueven posiciones. HOY la
excepción fue Initialize: ahí el oráculo (líneas del original + árbol de
bloques) señaló que el objetivo indexaba con el ÍNDICE primero, y la forma
entera de r64 — desestimada por su micro — transferida a la fuente real cerró
6 filas. Receta para quien siga: `dw1ours.py` diff de fichas primero (2 min),
luego banco con measure1.py, nunca confiar en micros fuera de contexto.
