# Flujo completo de extracción del PKG PS3 (PS2-Classic, NFS MW USA)

Verificado end-to-end sobre `JYGTxOGqfQqdVzicuxNODNKWyOJJKTJukTQjDqTFJHOiuzWvkHxIBXWAQoBusKiCdmNLgWhebHquDLoyRorwNiZlYhhpFVLctUitz.pkg`
(CONTENT-ID `UP9000-NPUD21267_00` → **SLUS-21267**, NFS MW PS2 USA retail).

## Pipeline (todo en tools/scratch/, WSL para lo compilado)

1. **Extraer PKG** — [PyKG](https://github.com/AphelionWasTaken/PyKG) (GUI) reutilizado
   por módulo: `tools/scratch/pkg_cli.py` (runner CLI) → `pkg_out/SLUS21267/`.
   Contenido: `USRDIR/ISO.BIN.EDAT` (320B), `USRDIR/ISO.BIN.ENC` (3.7GB), CONFIG, CONTENT/*.dxt.

2. **RAP key** — de la db pkgi pública: `D7C19F648E01705F07962AD122C4EFAD`.

3. **Descifrar ISO.BIN.EDAT** — [make_npdata](https://github.com/ErikPshat/data_makenp)
   (compilado en WSL, `tools/scratch/makenp/Linux/`):
   ```
   make_npdata -d ISO.BIN.EDAT ISO.BIN <klic ceros> nfs.rap
   ```
   → contiene solo "SLUS-21267" (el título del disco, según PSDevWiki).
   **EL PASO CLAVE**: con `-v` revela `DECRYPTION KEY: 8FC6D8FD042818AD4CAA2780E1767077`
   (derivada del RAP) — ¡ESA es la klicensee del ENC, no el RAP directo!

4. **Descifrar ISO.BIN.ENC** — [ps2classic](https://github.com/sdkmap/PS2Classics)
   (compilado en WSL con `tools.c` y `-lcrypto`):
   ```
   ./ps2classic_bin d cex klic2.bin ISO.BIN.ENC image2.iso meta2.bin
   ```
   (klicensee va en ARCHIVO binario, no en línea). Sector 16 = `\x01CD001` ✓.

5. **ISO PS2 resultante**: root en sector **261** (los ISO de EA arrancan tarde —
   el LBA del root record del PVD, no 156+default). `SLUS_212.67` (4.9MB ELF MIPS)
   extraído a `tools/scratch/SLUS_212.67.elf`.

## Valor para el proyecto
- Es la **USA retail** — SIN símbolos (stripped). El Alpha 124 PAL (SLES-53558-A124,
  ya soportado con symbols/PS2) sigue siendo la fuente PS2 principal.
- Uso: tercera plataforma de referencia para cruzar funciones resistentes
  (MIPS + VU overlays .DVP.*), junto al X360 (docs/X360_EXTRACTION.md).
