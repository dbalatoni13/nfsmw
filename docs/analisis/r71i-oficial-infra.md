# r71i: INFRAESTRUCTURA DEL REPO OFICIAL — qué tienen y qué nos falta

Fuentes: `.github/workflows/build.yml` + `.devcontainer/` del repo
dbalatoni13/nfsmw (commit cae30b5).

## Lo que tiene el oficial

### Matriz multi-versión en CI (5 versiones)

| versión | plataforma | estado allí |
|---|---|---|
| GOWE69 | GameCube | la principal (con report DWARF en cada push) |
| EUROPEGERMILESTONE | X360 (prototipo) | compila |
| SLES-53558-A124 | PS2 alpha | compila (con deps PS2 extra) |
| SLUS-21351 | PS2 retail (Black Ed.) | compila |
| SPEED_EXE_1_3 | PC | compila |

**Implicación**: el repo oficial YA tiene las 5 versiones operativas —
nuestro configure.py solo tiene 3 (GOWE69, EUROPEGERMILESTONE, SLES-53558-A124).
Faltan SLUS-21351 y SPEED_EXE_1_3.

### Pipeline de CI (build.yml)

1. Contenedor privado `ghcr.io/dbalatoni13/nfs-gc-build:main` con los
   binarios originales DENTRO (`cp -R /orig .`) — legal: cada corrida usa
   la copia del contenedor, no del repo
2. binutils por arquitectura: `/mips_binutils` (PS2) o `/ppc_binutils` (GC/X360)
3. `python configure.py --map --version <V> --binutils <B> --compilers /compilers`
4. `ninja all_source progress build/<V>/report.json`
5. Para GOWE69 en push: dump DWARF con `dtk dwarf dump` + split + reporte

### Devcontainer

- Base: `mcr.microsoft.com/devcontainers/cpp:2-debian13`
- Python venv con: **splat64** (troceador MIPS/PS2), **spimdisasm**,
  **rabbitizer** (desensambladores MIPS), **pyelftools**, **pygfxd**,
  **objdiff CLI** (de encounter/objdiff — la MISMA herramienta que usamos)
- Extension VS Code: `nfsmw-match-annotations.vsix`

## Qué significa para nosotros

1. **Nuestra configure.py ya soporta 3 de las 5 versiones** — el oficial
   confirma que el enfoque (una configure.py, versiones en lista, plataformas
   con toolchains distintos) es el correcto y solo faltan 2.

2. **SLUS-21351 (PS2 retail)**: ya extraimos el ELF de la Black Edition ISO
   (r71b: stripped, calibrador). El oficial lo COMPILA — significa que tiene
   un split/config para la versión retail. Su .MAP puede estar en el repo
   (público), y eso nos daria los simbolos del build FINAL de PS2.

3. **SPEED_EXE_1_3 (PC)**: version PC — nunca la habiamos tocado. El oficial
   la compila, asi que el codigo indep/ ya casa multi-plataforma alli.

4. **El pipeline CI con report por version** es exactamente lo que
   necesitamos para el objetivo multi-version: cada push valida las 5.

5. **splat64 + spimdisasm + rabbitizer**: herramientas PS2 que NO usamos
   (nuestro enfoque PS2 fue mdebug/EER via Ghidra). splat64 es el troceador
   estandar — si el oficial lo usa para PS2, nos conviene igual para cuando
   cerremos el PS2.

## Accion derivada (no urgente, pero valiosa)

- **Añadir SLUS-21351 a configure.py**: crear `config/SLUS-21351/` con el
  splits.txt del PS2 retail (necesitamos los rangos del oficial o generarlos
  con splat64 + el .MAP que ya tenemos del A124 como aproximacion)
- **Añadir SPEED_EXE_1_3**: extraer el exe PC y montar config (el oficial
  tiene los rangos)
- **Montar el contenedor**: con el Dockerfile del devcontainer como base
  (es publico) + nuestros compiladores ya in-tree, un contenedor propio
  para CI

## Riesgo legal

El contenedor oficial es PRIVADO (los binarios del juego dentro). Nosotros
no podemos distribuirlo — pero el Dockerfile del devcontainer es público y
GPL: se puede usar como base sin los binarios, aportando cada cual los suyos
(igual que ya hacemos con orig/).
