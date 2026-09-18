# Extracción del build X360 (EUROPEGERMILESTONE, prototipo 21-oct-2005)

## Flujo completo verificado

1. **Object oficial** (README upstream): copiar `NfsMWEuropeGerMilestone.exe`
   (XEX2 devkit, uncompressed+encrypted, 10MB) de la carpeta del prototype a
   `orig/EUROPEGERMILESTONE/NfsMWEuropeGerMilestone.xex` (renombrado).

2. **Basefile descifrado** — el truco: `dtk v0.1.2` (la versión que exige la
   config X360; la moderna 1.8.31 ya no trae `xex`) falla con
   "control characters are not allowed" SI el path del xex es largo/anjdir.
   Solución: copiar el xex a un CWD plano con nombre corto:
   ```
   cd tools/scratch && cp ../../orig/EUROPEGERMILESTONE/NfsMWEuropeGerMilestone.xex t.xex
   ./dtk012.exe xex extract t.xex     # → NFS.exe (10.259.456 B, PE PPC en claro)
   ```
   El `NFS.exe` de la carpeta del prototype ES este mismo basefile (hash
   6d0729061ff1..., idéntico byte a byte).

3. **Secciones del basefile** (ImageBase 0x400000):
   .rdata va=0x600 · .pdata va=0xB9600 · **.text va=0xF0000 (8.413.696 B,
   raw@0xE7200)** · .data va=0x920000 · .reloc va=0xD00200 ...
   Extraído en `tools/scratch/x360_text.bin` (VA base 0x4F0000).

## Estado
- `xex extract`: OK (con path corto) · `xex info`: OK
- `xex split`: FALLA siempre ("control characters", bug v0.1.2 con estos
  builds — probado win/linux/WSL/ambos xex) → bloquea `configure.py
  EUROPEGERMILESTONE` automático. Reportable a dbalatoni13/nfsmw.
- Usable YA: el .text en claro permite desensamblar/ cruzar cualquier
  función X360↔GC con objdump (powerpc64) como referencia.

## Herramienta de cruzado

`scripts/x360ref.py` — localiza strings/constantes en el build X360 y vuelca
el desensamblado de la función:

```
python scripts/x360ref.py str "NFS Most Wanted"   # VA del string
python scripts/x360ref.py fn "ERROR:"             # función que lo usa + asm
python scripts/x360ref.py fnat 0x4f0070 40        # asm de una VA concreta
```

Desensamblado completo de referencia en `build/GOWE69/asm/X360_text.s`
(2.1M líneas; VAs del .text: 0x4F0000..0xCF61A4). Nota X360: los datos se
referencian vía TOC (r2), no lis/addi estático — localiza funciones por sus
strings/constantes, no al revés.
