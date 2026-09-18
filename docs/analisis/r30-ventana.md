# Ronda 30 — ventana: aplicación del paquete y verificación del DOL

Estado al cerrar: **97,974 % matched**, **18.337 funciones al 100 %**,
**`linked` 12,56 % (453 de 604 unidades)**, `main.dol` byte a byte
(`9619ba57c9919f95f7f2ac951a2166a3517f91e3`, 4.541.888 B).

| | antes | ahora |
|---|---|---|
| matched | 97,96 % | **97,974 %** |
| funciones al 100 % | 18.330 | **18.337** |
| `linked` | 12,18 % (443/599) | **12,56 % (453/604)** |
| `.data` que falta en el árbol | 3.546 B | **3.114 B** |

Dos commits: `ba39e201` (las diez promociones) y `97996ea0` (las dos fronteras).

---

## 1. Las diez promociones (15.280 B)

`csis`, `postproc`, `pathreal`, `gc_device`, `sstfxlev`, `satospkr`, `itoa`,
`quantize`, `sstvol`, `svol`. `audit.py` limpio en las diez (0 FALLA).

Aplicado tal cual el §6 de `r30-lk.md`: 2 `add_relocations` en `config.yml`,
2 símbolos partidos en `symbols.txt`, 16 líneas de `splits.txt`, 2 entradas de
`keep.lst`, el borrado de las diez etiquetas de `maddeca.cpp`, y en
`configure.py` las banderas y las diez marcas `Matching`.

**Una decisión de aplicación que el informe dejaba abierta.** El §2.1 proponía
`-fno-implement-inlines` «en los cflags de `realcore`, `path`, `csis`,
`realmemcard` y `rcmp`», pero **`csis` no es una biblioteca**: `csis.cpp` vive
dentro de la lib `misc`, con 272 objetos que el barrido **no** cubre. Va por
`Object(...)`; las otras cuatro, a nivel de biblioteca —comprobado antes que
**las cuatro son C++ puro** (0 ficheros `.c`), que es lo que hace segura una
bandera de C++ en el bloque entero—.

Los comodines del §6 («10 añadir, 5 quitar, 599 → 604») **no se tocan a mano**:
los genera el troceado a partir de `splits.txt`. Salieron los 604 solos.

## 2. El fallo que costó el primer enlace: `keep.lst` no era dependencia

El primer enlace dio el DOL **32 B corto y sin fallar nada**. La cadena:

1. `quantize.c` promocionada deja de emitir el bloque de cinco dobles de
   `lbl_804119A8` (0x28 B), que pasa al comodín `auto_05_804119A8_rodata`.
2. `keep.lst` seguía diciendo `quantize.o:lbl_804119A8`. Lo corregí…
3. …y **no cambió nada**, porque **el fichero de `-keep` no estaba en
   `implicit` de la arista de enlace**: ninja no volvió a enlazar.

Lo delató la fecha: `main.elf` era **más viejo** que el `keep.lst` que acababa
de tocar. Es el mismo fallo que ya tuvo el troceado con `splits.txt`, y el
síntoma es idéntico y mudo. Arreglado en `tools/project.py`: cada ruta de
`-keep` entra en `ld_implicit`.

**Receta de diagnóstico, que funcionó en tres minutos** (vale para cualquier
DOL roto por tamaño):

- `len()` del DOL contra el original → **cuánto**;
- la **tabla de secciones** de la cabecera del DOL → **qué sección** encoge (y
  qué secciones bajan de dirección por arrastre);
- las **rachas de bytes distintos** dentro de esa sección → la primera racha
  larga marca el sitio. **Las rachas de UN byte que aparecen antes son punteros
  corridos, no la causa**: aquí dos punteros a `.data` con un solo byte de
  diferencia (`…80` → `…60`) estaban 200 kB por delante del agujero real.

## 3. Las dos fronteras de `.data` (§5 de `r30-da.md`)

Aplicadas y verificadas: **el efecto medido es exactamente el previsto**
(`zFe` −568 → **−144**, `zFe2` +312 → **−112**).

Y una trampa nueva que el informe no podía prever, porque sólo aparece al
aplicar: **`dtk` renombra el relleno según dónde caiga**.

    gap_<sec>_<dir>_<nombre>    relleno ENTRE dos símbolos
    pad_<sec>_<dir>_<nombre>    relleno al PRINCIPIO (o al final) del rango

Al subir la frontera de `zEAXSound2` de 0x80417D88 a 0x80417DA8, sus 132 B de
relleno pasan a ser **el primer elemento** de la unidad y se renombran
`gap_06_80417DA8_data` → `pad_06_80417DA8_data`. La entrada de `keep.lst` deja
de casar y `-strip-unused-data` se los lleva: **DOL 128 B corto, enlace sin
error**. Van dos veces en la misma ventana que una entrada rancia de `keep.lst`
rompe el DOL en silencio.

De ahí **`scripts/keepchk.py`**: comprueba que cada entrada `objeto:símbolo` de
`keep.lst` existe de verdad en su objeto. Hoy da **758 ok, 19 rancias** — las 19
inocuas (sus símbolos ya no existen en ningún sitio; 14 son de `zEAXSound2` y
2 de `asd2`, que no tiene configuración). **Pásalo siempre que muevas una
frontera.**

## 4. Lo que queda para la r31

- **El tercer desajuste de frontera que el §5.3 apunta y no resuelve**:
  `_13MemoryCardImp.gEntryType` (0x8041BBB8, `.data` de zFe) y `gSaveType0..2`
  (0x80472F1C, `.bss` de zFe) contra las funciones `…__13MemoryCardImp` en
  0x8027217C, dentro del `.text` de **zPlatform**. Los dos rangos no pueden ser
  ciertos con un solo TU.
- **Las 10 SourceLists donde `-fno-implement-inlines` MEJORA** siguen sin la
  bandera. No compra promociones (el `.text` de más no llega al DOL), pero
  `zAttribSys` pasa a 48.776 = 48.776 **con cero símbolos de más**.
- Quedan **3.114 B de `.data`** que faltan en el árbol.
