# Auditoría 2026-09-14: ASM, match pendiente y plan a 100 % sin andamios

Objetivo del encargo: 100 % matched code sin andamios ASM, en orden
ASM→C real, después 100 % match y finalmente 100 % linked. Todo lo afirmado
aquí está medido hoy; donde reutilizo la revisión r68/r71 la cito en vez de
repetirla. Reglas vigentes: canary anti-slop del repo, gates (`cleanup_gate`,
`final_gate`, `fncmp`, `lcfix --check`, DOL SHA
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`), y no romper funciones exactas
para quitar barreras.

## A. Estado medido hoy (no heredado)

- Código: **3.914.928 / 3.946.048 = 99,211365 %** (report regenerado).
  OJO: −356 B frente a los 3.915.284 de la revisión de esta mañana; pendiente
  identificar qué tanda los movió (candidatos: fixes r71e/portabilidad).
- Funciones: **18.405 / 18.432**; unmatched contados por script: **23**
  (la revisión informa 25 cuerpos + pad; la diferencia es criterio de conteo,
  no bytes nuevos).
- Linked: 1.264.380 B (32,04 %), unidades 525/619.
- Unidades pendientes reales: 18 SourceLists + `steering` + `madidct` (+
  `auto_*`, `prodg_fixes`, pad `.init` 20 B: contabilidad, no código).

## B. Andamiajes ASM: inventario y backlog de retirada

Base: censo oficial `scratchpad/codex_20260914_asm_instructions/census.json`
(3.679 ficheros, 1.557 ocurrencias con categoría/alcance). Mi conteo bruto
(331 ficheros, 1.759 sitios) coincide en orden de magnitud; el censo manda.
Cruce propio (script `Temp/opencode/audit_asm.py`) de barreras/pines/inline
de alcance proyecto contra `report.json`:

- **En funciones al 100 %: 137 ocurrencias.** Clasificación por plantilla:
  - `BARRIER-empty` (35, casi todo `vfprintf.c`): candidatas a retirar.
  - `PIN` (23: `vfprintf`, `FSasync`, `criticalpath` VP6…): candidatas.
  - `OTHER-inspect` (57: `FSasync lis`, bloques `asm{}` steering/VMMapping…):
    revisar una a una.
  - `HW/SDK-like` (22: `mfspr/mtspr GQR`, bucles paired-single VP6 con
    `mtctr/ps_*`, init `OSInitFastCast`): casi seguro ORIGINAL (Gekko/ABI/
    caché). **No tocar sin prueba de que es parche** (la revisión lo exige).
- 216 sin mapear a función (heurística de envolvente falla en macros/
  plantillas/multi-línea): el backlog real es mayor que 137; hace falta
  segundo pase con `objdump`/símbolos o revisión manual por fichero.
- En las 25 pendientes los andamios son load-bearing documentados (r36-r68):
  sólo se tocan con C alternativo que re-cierre (medido), nunca a pelo.

Conflicto explícito a resolver por el usuario: quitar ASM de una función al
100 % casi siempre la rompe (<100 %). La doctrina vigente (no romper exactas)
manda **retirar + re-cerrar**; si no re-cierra, se documenta y se conserva.
Quitar por quitar va contra el 100 %.

Backlog propuesto (fase 1), por fichero: `vfprintf.c` (35+2+…), `FSasync.c`,
`ppc2D2.c`, `steering.c` (bloques `asm{}`: clasificar original vs parche),
`VMMapping.c`, `criticalpath.c` (pines VP6: verificar original codec vs
parche), `vfwpbdll_if.c` (GQR: casi seguro original, auditar y excluir).

## C. Funciones y unidades a medias (verificado)

Las 25 de la revisión + pad 20 B. Sin cambios salvo la deriva de −356 B
(punto A). Diez unidades code-perfect para linked posterior: zAI, zEAXSound,
zFe, zFe2, zGameplay, zLua, zMisc, zPhysics, zPlatform, zSpeech (+ 72 auto +
`prodg_fixes` con tratamiento separado).

## D. Herramientas y evidencias NUEVAS localizadas hoy

| Recurso | Dónde | Para qué sirve |
|---|---|---|
| `scripts/dwarf1.py` (r70b) | scripts/ | Oráculo DWARF-1 del ELF GC (92 MB): nombres/tipos/ubicación de locales, params, inlines. Formato verificado contra `orig/prodg/NGC_GNU_SRC` (dwarfout.c de SN). **Clave para layouts (epCalculate) y ties (static init).** |
| `scripts/mdebug.py` | scripts/ | Debug ECOFF/MIPS del build **PS2 A124** (`orig/SLES-53558-A124/NFS.ELF` 91,9 MB + `NFS.MAP` 2,7 MB): params, bloques, líneas, rutas en orden inline. Estructura compartida con GC. |
| `scripts/x360ref.py` + `docs/X360_EXTRACTION.md` | scripts/, docs/, `orig/EUROPEGERMILESTONE/` (`NFS.exe` descifrado) + `tools/scratch/x360_*` | Gemelas X360 (21-oct-2005) como oráculo de estructura (r71c: métrica + sonda). |
| `scratchpad/codex_20260914_ghidra_export/` | scratchpad/ | **18.442 funciones** con `.c` + `.asm.txt` + `.vars.tsv` (variables Ghidra incluidas). Cubre los 25 muros. |
| `scripts/pubnames.py`, `aranges.py`, `reladdr.py`, `frontcoste.py`, `deadstr.py`, `censorancios.py` | scripts/ | Utilidades DWARF/medición nuevas (14/09 y 10-11/09). |
| Bancos rápidos | `scripts/agent_ecs_upi.py` (eLightE 2 s), `Temp/opencode/w2calc.py` (epCalculate 2 s), mini-TU q4phys (2,3 s), `Temp/opencode/w2smp.py` (SetMemoryPoolSize) | Iteración sin SourceList entera. |
| Míos (Temp) | `framecheck.py` (stwu por función), `audit_asm.py` + `asm_audit.json/txt`, `ExportFrameVars.java` | Framecheck reutilizable; el resto, auditoría de hoy. |
| **BAJA**: Ghidra 11.4 borrado del Escritorio (solo queda 12.1.2 + proyecto `NFSMW.gpr` en Temp + proyecto `NFSMW_DWARF` huérfano de 370 MB + CSVs). Sin 11.4 no hay consultas DWARF1-variables nuevas; la extensión se fue con él. Reinstalar = descargar snapshot DEV (disponibilidad incierta) + extensión 11.3.1. |

## E. Verificación multi-versión

- GC: `report.json` + gates + DOL SHA (vía oficial).
- X360: `x360ref.py` operativo sobre base descifrada (ver r71c); sin `configure.py EUROPEGERMILESTONE` automático (falla `xex split`, reportable).
- PS2: `mdebug.py` operativo sobre A124; sin build PS2 configurado (solo estructura como oráculo, no match).

## F. PLAN (orden estricto del encargo)

**FASE 1 — ASM → C real (sin romper exactas).**
1. Completar el triaje de las 137 (+216 sin mapear): clasificar original/HW
   vs parche con la regla de la revisión (SDK/arranque/ABI/especiales/caché/
   paired-single se auditan y excluyen). Salida: backlog nominal por función.
2. Retirar por fichero (vfprintf → FSasync → ppc2D2 → steering/VMMapping →
   criticalpath), cada una con re-cierre medido (objeto + `fncmp` + reporte);
   lo que no re-cierre se documenta y se conserva (doctrina vigente).
3. En las 25 pendientes: ningún retiro sin C alternativo que re-cierre.
4. Gate `cleanup_gate` + DOL SHA tras cada tanda.

**FASE 2 — match 100 % (duro → fácil), cada muro con su oráculo nuevo.**
1. `epCalculate` (frame + 155 filas): `dwarf1.py` para el orden real de
   locales (cierra a la vez frame y rotación FPR si el layout difiere).
2. `static_init` zCamera (6 filas): `dwarf1.py` vidas de `HydraulicsLookAngle`
   / `12000.0` (la única vía viva de la revisión).
3. `eProject` / `GenerateRoadNoise` / `Effect_Init`: gemelas X360
   (`x360ref.py`) como mapa de estructura + DWARF.
4. Ties con número exacto (`HolePunch` live 491, `InitAtSegment` 212≤6,
   `Defrag` 222≤47, `CookValues`, `SetMemoryPoolSize` 2 filas): reintentar
   SÓLO con palanca nueva deducida de `dwarf1.py`/`.greg`/X360, nunca
   barridos repetidos (todos vedados con cifra).
5. `IdctColumn`/`IdctRow`, `Tire`, resto steering/zCamera: procedencia
   (implementación original, macros, flags) vía PS2 `mdebug.py` (estructura
   compartida) y X360; `IdctRow` sigue sin fantasma (base 496 B).
6. Cada cierre: `fncmp` de la TU + bytes/datos/literales/relocs + `lcfix`
   + reporte + enlace privado con DOL original. Lo fuzzy no cuenta.

**FASE 3 — linked 100 %.** Sólo con fase 2 asentada: las diez unidades
code-perfect, luego `auto_*`/`prodg_fixes` con tratamiento separado, padding
20 B con corrección formal (nunca maquillado). Promoción sólo con enlace
privado + DOL original.

## G. Incidencias abiertas de hoy
- Deriva −356 B de código matched desde la revisión de la mañana: identificar
  tanda responsable antes de medir progreso.
- 216 ocurrencias ASM sin mapear a función: segundo pase pendiente.
- Ghidra 11.4 + extensión DWARF1 eliminados: decidir reinstalación (bloquea
  consultas de variables; mitiga `dwarf1.py`, que no necesita Ghidra).
