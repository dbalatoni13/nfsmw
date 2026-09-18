# Ronda 19 — grupo `libc/` (libm): las cinco unidades a cero

## 0. Resultado

    python scripts/measure.py --cmp base_r19_libm.json despues_r19_libm.json
    +9012 B, +5 funciones, 5 unidades cambian
          +868 B  libc/e_rem_pio2      0 -> 868
         +1780 B  libc/ef_pow          0 -> 1780
         +2000 B  libc/e_pow           0 -> 2000
         +2180 B  libc/kf_rem_pio2     0 -> 2180
         +2184 B  libc/k_rem_pio2      0 -> 2184

Y **dos unidades mas fuera del encargo**, encontradas contando el hallazgo en
todo `libc/` (`despues_r19_libm_extra.json`):

         +532 B  libc/kf_tan   `__kernel_tanf`  97,71 -> 100 %
         +640 B  libc/sf_log   `logf`           90,09 -> 100 %

**Total: 10.184 B en 7 funciones.** Las siete pasan `scripts/audit.py` y estan
congeladas con `scripts/frozen.py cong`.

## 1. La causa raiz que la ronda 18 no encontro: `-msafe-sda`

`ngccc` 3.9.3 (SN ProDG) tiene opciones **propias de SN** que no estan en el gcc
2.95 de serie y que no aparecen en `cc1 --help`. Salen de la tabla de
`target_switches` dentro de `cc1.exe`:

    sndata=  safe-sda  fast-cast  align-section-names  ps-float  ps-nodf  ...

**`-msafe-sda` manda el POOL DE CONSTANTES del compilador a `.sdata` y lo
direcciona con `@sda21`**, que es exactamente donde lo tiene el original:

    sin -msafe-sda              con -msafe-sda
    .section ".rodata"          .section ".sdata","aw"
    lis  r11,.LC0@ha            lfd  f0,.LC0@sda21(0)
    lfd  f0,.LC0@l(r11)

Esto explica de golpe **las tres cosas** que la ronda 18 daba por muro:

1. el sesgo `0x4330000080000000` de cada conversion `(double)int` costaba
   **+4 B** (`pow` 1 conversion, `powf` 1, `__kernel_rem_pio2` 6,
   `__kernel_rem_pio2f` 6, `__ieee754_rem_pio2` 2);
2. por que **todas** las constantes de fdlibm del objetivo estan en `.sdata`
   (804FEDC0-804FF8AC) y no en `.rodata`: son entradas del pool, no variables;
3. por que hubo que inventar el truco `SDA_DOUBLE` de `fdlibm.h` — con
   `-msafe-sda` un `static const double` de fdlibm ya sale donde debe.

**Es por objeto, no por biblioteca.** Medido compilando las 63 unidades de
`libc/` con y sin el flag y contando bytes al 100 % (sin tocar `build/`):
aplicarlo a toda la biblioteca **cuesta -15.544 B** (`vfprintf` -6128,
`vfprintf_1` -5760, `math_support` -1464, `itoa` -1256, `strtod2` -812,
`mbtowc_r` -796, `fopen` -324, `libgcc2_4` -176) y **gana 1172 B** en `kf_tan`
y `sf_log`. Va en `extra_cflags` de las siete unidades y de ninguna mas.

## 2. El segundo flag: `-mstrict-align` y el orden de operandos del `lfsx`

`powf` indexa tres arrays de 2 floats (`bp`, `dp_h`, `dp_l`). El objetivo emite
`lfsx fD, rBase, rIndice` y nosotros `lfsx fD, rIndice, rBase` — la regla de
`EXPAND_SUM` («el sumando constante va el ultimo») ya descrita en el PLAYBOOK.

El discriminante **no es la forma de la fuente** (probadas 12) **ni el modo de
direccionamiento**: es el **modo del TIPO del array**. `float[2]` mide 64 bits,
`layout_type` le da `DImode` y sale `indice, base`; `float[3]` (96 bits) no
tiene modo entero, se queda `BLKmode` y sale `base, indice`. **`-mstrict-align`
fuerza `BLKmode` tambien en el de 8 B** (`TYPE_ALIGN < TYPE_SIZE`), y con eso
`powf` cierra sin tocar el dato. Reproducido en 10 lineas:

    static const float a2[2] __attribute__((section(".sdata2"))) = {1.0, 1.5};
    float ha(float x, int k) { return x - a2[k]; }
    -msafe-sda                 -> lfsx 0,3,9   (indice, base)
    -msafe-sda -mstrict-align  -> lfsx 0,9,3   (base, indice)   <- objetivo

`-mstrict-align` es neutro en las otras seis unidades; solo lo llevan las cinco
del encargo (por uniformidad del grupo `pow`/`rem_pio2`), y `kf_tan`/`sf_log`
llevan **solo** `-msafe-sda`.

## 3. Ensayos numerados

Todos con `scripts/build_direct.py` + objdiff 3.8.0,
`function_reloc_diffs=none`, `ppc.calculatePoolRelocations=false`.
Estado de partida: `base_r19_libm.json`, 0/9012 B.

    c1  Las CINCO versiones de ProDG (3.5, 3.5b140, 3.7, 3.8.1, 3.9.3)
        compilando `double f(int i){return (double)i;}`
                                          las 5 ponen el sesgo en .rodata con
                                          `lis @ha` + `@l`. DESCARTADO: la
                                          pista mas prometedora de la r18 es
                                          negativa y queda cerrada con cifra.
    c2  Barrido de 30 flags de seccion sobre el mismo caso:
        -msdata=eabi|sysv|data|default x -G 0|2|4|6|8|12|16, -mno-eabi,
        -mminimal-toc, -mfull-toc, -mno-toc, -mcall-sysv|aixdesc|nt,
        -mstrict-align, -fno-common, -fdata-sections, -ffunction-sections,
        -fmerge-constants, -mrelocatable(-lib), -fpic
                                          todos .rodata salvo -mrelocatable y
                                          -fpic, que lo mandan a **.data** (y
                                          cambian el codigo). DESCARTADO.
    c3  `-msafe-sda` (sacado de la tabla de switches de cc1.exe)
                                          e_pow 98,250 -> 99,440
                                          ef_pow 96,897 -> 99,933
                                          k_rem_pio2 95,027 -> **100,000**
                                          kf_rem_pio2 94,028 -> 98,615
                                          e_rem_pio2 93,571 -> 96,590
                                          SE QUEDA.
    c4  Cambiar SDA_DOUBLE/SDA_FLOAT por `static const` de fdlibm en las cinco
                                          k_rem_pio2 100 (0 diffs) y
                                          kf_rem_pio2 mejor; pero e_pow
                                          99,440 -> 96,788 (`lfd +3`) y ef_pow
                                          99,933 -> 98,303 (`lfs +3`): con el
                                          literal plegado, CSE se queda la
                                          constante en un registro y el
                                          objetivo la RECARGA. SE QUEDA SOLO
                                          en k_rem_pio2 y kf_rem_pio2.
    c5  ef_pow: 12 grafias del indice (`k[bp]`, `*(bp+k)`, `*(&bp[0]+k)`,
        cast a puntero, indice unsigned/long, `(*(const float(*)[4])bp)[k]`,
        array sin `__attribute__((section))`, array no `static`, array no
        `const`, tam explicito)           ninguna invierte el `lfsx`;
                                          `*(bp+k)` y `k[bp]` ademas mueven el
                                          `li` de la base. DESCARTADAS.
    c6  ef_pow: arrays de 3 y 4 elementos  **100,000 %**, pero cambia el DATO
                                          (12/16 B por array contra 8 en el
                                          ELF). DESCARTADO por falsear datos —
                                          util solo como diagnostico: el modo
                                          del tipo es la palanca.
    c7  `-mstrict-align` sobre c3          ef_pow 99,933 -> **100,000**, las
                                          otras cuatro sin cambio. SE QUEDA.
    c8  e_pow: `huge` como memoria viva (`static double`, sin `const`)
                                          99,440 -> 99,760: recupera la pareja
                                          `beq`/`b` del desbordamiento. Con
                                          `extern const` (RTX_UNCHANGING) gcse
                                          iza el `lfd huge` por encima del
                                          `beq` e invierte a `bne`. SE QUEDA.
    c9  e_pow: `xnegative` escrito dos veces como en fdlibm en vez de una
        variable                          99,440 -> 99,680: CSE calcula
                                          `(unsigned)hx>>31` una vez y guarda
                                          copia (`mr r5,r9`), que es el `mr`
                                          que le sobraba al objetivo.
                                          SE QUEDA. c8+c9 = **100,000 %**.
    c10 e_rem_pio2: el bloque |x|<3pi/4 con `y[0]`/`y[1]` repetidos dentro de
        cada rama (fdlibm) en vez de una `w` comun
                                          96,590 -> **100,000 %**. Con `w`,
                                          GCC reusa f1 (el argumento ya muerto)
                                          y el reparto de FPR no casa; con las
                                          colas duplicadas las funde el
                                          cross-jumping como en el original.
                                          SE QUEDA.
    c11 kf_rem_pio2: escalares `static const` (c4) + `PIo2` en `.sdata2`
                                          98,615 -> **100,000 %**. Solo uno de
                                          los dos cambios no basta: `const`
                                          solo deja 5 diffs (`PIo2` fuera de
                                          small data) y `.sdata2` solo deja 55.
                                          SE QUEDA.
    c12 Contar el hallazgo en las 63 unidades de `libc/` (compilando al
        scratchpad, sin tocar `build/GOWE69/src`)
                                          +1172 B: `kf_tan` 97,71 -> 100 y
                                          `sf_log` 90,09 -> 100, **los dos con
                                          `-msafe-sda` solo**. `ef_rem_pio2`
                                          90,31 -> 92,97 y `sf_expm1` 91,57 ->
                                          93,48 mejoran y NO cierran.
                                          Aplicar el flag a toda la biblioteca
                                          costaria -15.544 B. SE QUEDA por
                                          objeto.

## 4. La prueba

`scripts/audit.py` cortaba con `FALLA` en la primera reubicacion cuyo simbolo
se llama distinto en los dos lados y no es un `$LC` — que aqui es **todas**,
porque el pool del compilador se referencia como `[.sdata]+0x38` y el troceador
le puso nombre propio (`lbl_804FF838`) a cada entrada. Es el hueco 1 y 2 que ya
anoto la r18.

Lo he portado a `scripts/audit.py`, **restringido para no debilitar la
auditoria**: la comparacion por VALOR solo se admite cuando el simbolo del
objetivo es un `lbl_XXXXXXXX` (dato anonimo del troceador, cuya identidad ES su
valor), el tipo de reubicacion apunta a dato (no a codigo) y los dos lados
resuelven a bytes concretos. El ancho lo manda **la instruccion** (`lfs`->4,
`lfd`->8), no el `size:` de symbols.txt, que es una estimacion del troceador y
se come el relleno (`lbl_804FF208` es un float y le pone `size:0x8`).

Contraste contra la version de `git HEAD` en 22 unidades: **ninguna empeora** y
mejoran seis.

    libc/k_rem_pio2   FALLA -> ok  2184 B,  79 ramas, 110 relocs, 24 literales
    libc/kf_rem_pio2  FALLA -> ok  2180 B,  80 ramas, 111 relocs, 24 literales
    libc/e_pow        FALLA -> ok  2000 B,  75 ramas, 128 relocs, 47 literales
    libc/ef_pow       FALLA -> ok  1780 B,  64 ramas, 113 relocs, 43 literales
    libc/e_rem_pio2   FALLA -> ok   868 B,  24 ramas,  49 relocs, 21 literales
    libc/kf_tan             ok   532 B,  12 ramas,  34 relocs,  8 literales
    libc/sf_log             ok   640 B,  20 ramas,  47 relocs, 27 literales
    -- de propina, funciones que YA casaban y no se podian probar:
    libc/sf_log10     FALLA -> ok   268 B (log10f)
    libc/vfprintf     FALLA -> ok  6128 B (_vfprintf_r, 441 relocs)
    libc/vfprintf_1   FALLA -> ok  5760 B (_vfiprintf_r, 404 relocs)
    -- identicas al original: zLua (536 ok / 1 FALLA), zAttribSys (196 ok),
       ppc2D2 (9 ok), k_cos, e_acos, strlen, memcpy, ef_atan2, sf_tan, s_sin,
       e_sqrt, libgcc2, zMemcard.

`frozen.py chk` da «identico al congelado» en las siete.

## 5. Que NO he probado

- **Promocion a `linked`.** Las siete siguen `NonMatching` en `configure.py`
  (solo les he anadido `extra_cflags`). `obj.completed` es lo que hace que el
  objeto **se enlace** en el DOL, y `fdlibm.h` ya avisa de que el objeto
  extraido exporta las constantes con el nombre del troceador
  (`lbl_804FF838`, `bp_80500390`): con enlace interno salen LOCAL y el enlace
  puede fallar aunque el codigo case. Eso es trabajo de `trypromo.py`/
  `promote.py` y **no lo he tocado**. Tampoco he construido el DOL.
- **`-msafe-sda` fuera de `libc/`.** No lo he contado en `snd`, `path`,
  `LibSN` ni en las SourceLists. Aviso de que el pool del CODIGO DE JUEGO si
  esta en `.rodata` en el original (`vfprintf` lee `lbl_8040FC28`, que cae en
  `.rodata`), asi que no es un flag global; pero las bibliotecas matematicas de
  otras rutas merecen la misma cuenta que c12.
- Los otros switches de SN que salieron en la tabla y no he explorado:
  **`-msndata=N`** (seccion `.sdata.snda`), **`-mfast-cast`**,
  **`-malign-section-names`**, `-mno-safe-sda`, `-mps-nodf`. `-mfast-cast` no
  mueve el pool (c2) pero no he mirado que hace con las conversiones.
- **`ef_rem_pio2` (848 B) y `sf_expm1` (804 B)**, que con `-msafe-sda` suben a
  92,97 % y 93,48 % y no cierran: no he buscado la forma de fuente que les
  falta. Son los dos candidatos inmediatos.
- El censo de llamantes de la r18 (solo `bl` directas) sigue sin cubrir
  punteros a funcion ni tablas de datos.
- Ejecucion en hardware o emulador de nada de esto.

## 6. Nota de concurrencia

Durante la tanda **`configure.py` volvio a su version de `HEAD` una vez con mis
cambios ya escritos** (otro proceso regenero `build.ninja` en medio). Reaplicado
y verificado despues con `grep msafe-sda build.ninja` (7 ocurrencias) mas
`build_direct.py` + `frozen.py chk`. Si alguien regenera desde una copia vieja
de `configure.py`, las siete unidades vuelven a 0 B sin que nada avise: el
delator es que `build.ninja` deje de tener las 7 lineas con `-msafe-sda`.
