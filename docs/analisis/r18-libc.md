# Ronda 18 — grupo `src/libc/` (middleware, nunca repartido)

## 0. Estado REAL medido al empezar, no el del brief

El brief daba `vfprintf` 6,2 %, `vfprintf_1` 3,5 %, `itoa` 18,9 % y las cuatro
de `libm` a 0 %. Eso era una foto vieja: el árbol de trabajo ya traía el
resultado de las tandas 5-10 del 2026-09-04. Medido de verdad, tras
`python scripts/build_direct.py` de las siete unidades:

    python scripts/measure.py -o antes_r18_libc.json libc/vfprintf libc/vfprintf_1 \
        libc/k_rem_pio2 libc/kf_rem_pio2 libc/e_pow libc/ef_pow libc/itoa
    -> 7 unidades  14468/22612 B  63,9837 %  10 funciones al 100 %

| unidad | función | objetivo | nuestro | objdiff | measure |
|---|---|---:|---:|---:|---|
| libc/vfprintf | `_vfwrite`,`vfprintf`,`_vfprintf_r`,`add_separators` | 6948 | 6948 | 100 ×4 | **cerrada** |
| libc/vfprintf_1 | `_vfwrite`,`_vfiprintf_r` | 5972 | 5972 | 100 ×2 | **cerrada** |
| libc/itoa | `strround`,`strrev`,`itoa`,`fftoa` | 1548 | 1548 | 100 ×4 | **cerrada** |
| libc/e_pow | `pow` | 2000 | 2000 | 98,250 % | 0 B |
| libc/ef_pow | `powf` | 1780 | 1784 | 96,897 % | 0 B |
| libc/k_rem_pio2 | `__kernel_rem_pio2` | 2184 | 2208 | 95,027 % | 0 B |
| libc/kf_rem_pio2 | `__kernel_rem_pio2f` | 2180 | 2212 | 94,028 % | 0 B |

Lo pendiente de verdad son **8144 B en cuatro funciones de `libm`**, todas
near-miss altas, no código sin escribir. El «premio gordo» de 12.280 B de
`vfprintf`/`vfiprintf` ya lo cerró la tanda 8 y sólo faltaba **probarlo**.

## 1. Identidad

| función | dirección ELF | tamaño | instrucciones |
|---|---|---:|---:|
| `pow` | `.text:0x8031B558` | 0x7D0 = 2000 B | 500 |
| `powf` | `.text:0x8031CEC0` | 0x6F4 = 1780 B | 445 |
| `__kernel_rem_pio2f` | `.text:0x8031EC3C` | 0x884 = 2180 B | 545 |
| `__kernel_rem_pio2` | `.text:0x8039C84C` | 0x888 = 2184 B | 546 |

ELF original: `orig/GOWE69/NFSMWRELEASE.ELF`. No me apoyo en el SHA256 de
ningún objeto objetivo suelto: la prueba resuelve contra el ELF.

## 2. Censo de llamantes — llamadas `bl` DIRECTAS

Barrido de las secciones ejecutables del ELF buscando `bl` cuyo destino sea la
dirección de la función. **Es un censo de llamadas directas: no prueba que no
exista un puntero a función** hacia ninguna de ellas (y `powf` casi seguro lo
tiene, porque `CARP::ExpressionEvaluator` la mete en una tabla).

- `pow`: 2 `bl`, desde 1 función (`VDevice_RecalcGammaTable`).
- `powf`: 9 `bl`, desde 9 funciones (`ePowf__Fff`, `AIActionRace::Update`,
  `Chassis::DoAerodynamics`, tres `SuspensionX::DoWheelForces`,
  `CarRenderConn::UpdateEngineAnimation`, `CARP::ExpressionEvaluator`,
  `CARP::powf`).
- `__kernel_rem_pio2`: 1 `bl`, desde `__ieee754_rem_pio2`.
- `__kernel_rem_pio2f`: 1 `bl`, desde `__ieee754_rem_pio2f`.

## 3. Llamados y su contrato

- `pow` llama a `fabs`, `sqrt` y `scalbn(double,int)`. Los dos primeros están
  aliasados con `__asm__("fabs")`/`__asm__("sqrt")` para que el compilador NO
  los expanda en línea; `scalbn` es una `bl` normal.
- `powf` llama a `fabsf`, `sqrtf`, `scalbnf(float,int)`, mismo esquema.
- `__kernel_rem_pio2` llama a `scalbn` y `floor`; `__kernel_rem_pio2f` a
  `scalbnf` y `floorf`. Consumen r3..r8 (x, y, e0, nx, prec, ipio2), devuelven r3.
- Todos leen sus constantes de `.sdata` por `@sda21` (r13), no de `.rodata`.

## 4. La causa raíz común: el sesgo int->double en `.rodata`

Las cuatro fallan por lo MISMO, y `fdlibm.h` ya lo tenía documentado y resuelto
sólo para `sf_log10.c`: la conversión nativa `(double)algún_int` de este ngcc
materializa su constante privada `0x4330000080000000` en `.rodata`, lo que
obliga a `lis rN,[.rodata]@ha` + `lfd @l(rN)` — **dos** instrucciones —
mientras el original la tiene en `.sdata` y la lee con **una**,
`lfd fN, lbl_XXXX@sda21`. Verificado byte a byte en el ELF:

    804FEF90 [.sdata] 4330000080000000   <- pow      (entre cp y mone)
    804FF1D0 [.sdata] 4330000080000000   <- powf     (tras cp + 4 B de relleno)
    804FF388 [.sdata] 4330000080000000   <- __kernel_rem_pio2f
    804FF838 [.sdata] 4330000080000000   <- __kernel_rem_pio2

Cada conversión cuesta **+4 B**. Es el frente, no una anécdota de asignador.

## 5. Ensayos numerados

Cada uno con su cifra medida sobre el objeto real (`build_direct.py` + objdiff
3.8.0, `function_reloc_diffs=none`, `ppc.calculatePoolRelocations=false`).

    c1  e_pow: SDA_DOUBLE(i2d_bias) + I2D_STORE con `double i2dtmp` local del
        bloque `else`                          98,250 -> 93,454 %  (2000->1992 B)
        el marco encoge 8 B: la local nombrada ocupa 0x30 y empuja la ranura
        de reload a 0x38; el objetivo tiene hueco en 0x38 y conversion en 0x40.
    c2  igual pero `volatile double i2dtmp`    93,454 %   identico, misma pila
    c3  i2dtmp en un bloque interno justo en el punto de la conversion
                                               93,454 %   identico, misma pila
    c4  helper `static __inline__ double i2d(int)`
                                               93,454 %   identico, misma pila
    c5  `huge` como MEMORIA VIVA (sin `const` en la declaracion de uso)
                                               98,250 -> 98,340 %  <- SE QUEDA
        recupera la pareja `beq/b` del desbordamiento: triage pasa de
        `faltan 53, sobran 53, 4 SUST, li-1 mr+1 b+1 lis-1` a
        `faltan 51, sobran 52, 1 SUST, li-1 mr+1 lis-1`.
    c6  k_rem_pio2: i2d manual con `double cvt` detras de los arrays
                                               95,027 -> 94,092 %  (2208->2192 B)
        marco 0x280 -> 0x298: cvt coge 0x238 y los temporales del compilador se
        van a 0x248, ademas de gastar un r18 salvado mas. Revertido.
    c7  k_rem_pio2: el scratch como `q[20]` ampliando `double q[21]`
                                               95,027 -> 78,284 %  (2208->2220 B)
        el array se direcciona por base (r25 = r1+0x198), asi que `q[20]` sale
        como `0xa0(r25)` y no como `(r1)`: no es la misma forma. Revertido.
    c8  barrido de flags SOLO en el scratchpad (sin tocar configure.py), con
        `scratchpad/r18libc_flags.py`: `-G 8`, `-msdata=eabi -G 8`,
        `-msdata=sysv -G 8`, `-msdata=data -G 8`
                                               98,340 % en los cuatro, IDENTICO
        el `.rodata` del objeto sigue midiendo 8 B y siendo exactamente el
        sesgo. Ningun `-msdata=` ni `-G` mueve el POOL del compilador.

Se queda **c5** y nada mas. `despues_r18_libc.json` = `antes_r18_libc.json`:
**+0 B, +0 funciones**. c5 sube el porcentaje y quita una diferencia real, pero
`matched_code` es todo-o-nada y `pow` sigue sin cerrar.

## 6. La veda, con su causa

**No insistir en la conversion int->double manual en estas cuatro funciones.**
La tecnica de `sf_log10.c` (declarar el sesgo con `SDA_DOUBLE` y escribir la
conversion a mano) **solo cierra cuando la ranura de conversion del objetivo se
puede alcanzar con una local NOMBRADA**, es decir cuando no hay delante ninguna
ranura de temporal del compilador ni de memoria secundaria de reload — GCC 2.9
las asigna despues de todas las locales nombradas. Mapas de pila medidos (hoy
IDENTICOS entre objetivo y nuestro en las cuatro, luego el marco no es el
problema):

    sf_log10  log10f  0x8 palabra float NOMBRADA, 0x10 conversion  -> CIERRA
    e_pow     pow     0x8..0x28 nombradas, 0x30 reload, 0x38 hueco, 0x40 conv
    ef_pow    powf    0x8 reload (51 refs), 0x10 conversion
    k_rem_pio2        0x8..0x237 arrays, 0x238 conv+reload, 0x240 temporal
    kf_rem_pio2       0xa8.., 0x150 conv+reload, 0x15c/0x160 temporales

Lo que queda NO es una forma de fuente: es que el pool de constantes del
compilador salga en `.sdata` con `@sda21` en vez de en `.rodata` con
`lis @ha` + `@l`. Cuesta exactamente **+4 B por conversion**:

| funcion | exceso | `lis [.rodata]@ha` de mas | resto |
|---|---:|---:|---|
| `pow` | +4 B | 1 | `mr+1`, `li-1` (se compensan) |
| `powf` | +4 B | 1 | permutacion de registros r7/r8, f8/f9/f10 |
| `__kernel_rem_pio2` | +24 B | **6** | nada mas: quitarlos deja el tamano exacto |
| `__kernel_rem_pio2f` | +32 B | **6** | +2 instrucciones de otra cosa |

Es el mismo diagnostico que ya dejo escrito la cabecera de `k_rem_pio2.c`.
`cc1.exe` 3.9.3 **si** acepta `-msdata=none|sysv|eabi|data` y `-G`, y **ninguno
mueve el pool** (c8). Queda pendiente, y no lo he probado: mirar si el original
se compilo con otra version de ProDG cuyo `rs6000_select_rtx_section` mande los
literales a `.sdata`, cosa que ademas explicaria por que `static const double`
tuvo que emularse con el truco `SDA_DOUBLE` de `fdlibm.h`.

## 7. La prueba de lo que SI esta cerrado

`scripts/audit.py` no termina de auditar tres de las diez funciones: corta con
`FALLA` en la primera reubicacion cuyo simbolo no se llama igual en los dos
lados y no es un `$LC`. En estas unidades pasa por tres motivos:

1. nuestro pool es **una seccion con addend** (`[.rodata]+0x70`) y el del
   objetivo un `lbl_8040FC28`;
2. el mismo estatico local con otro mangling, y el mangling del troceador
   **lleva la direccion dentro** (`blanks` contra `blanks.29_8040FBB8`);
3. el troceador no reconstruyo la reubicacion y el objetivo trae el inmediato
   absoluto ya resuelto (`lis r11, 0x804b` contra `lis r11, str@ha`).

`scratchpad/r18libc_prove.py` hace las MISMAS comprobaciones que `audit.py`
(numero de instrucciones, destino **relativo** y condicion/CR de cada rama,
tipo y addend de cada reubicacion, texto exacto del resto) y ademas resuelve
esos tres casos comparando **el valor de los bytes**, enmascarando las palabras
que en nuestro `.o` todavia llevan reubicacion. Resultado:

    libc/vfprintf    _vfprintf_r    PROBADA  6128 B, 343 ramas,  98 relocs (54 por valor)
                     _vfwrite       PROBADA   212 B,   7 ramas,  18 relocs (16 por valor)
                     add_separators PROBADA   392 B,  22 ramas,   1 reloc
                     vfprintf       PROBADA   216 B,  15 ramas,   2 relocs
    libc/vfprintf_1  _vfiprintf_r   PROBADA  5760 B, 318 ramas,  86 relocs (46 por valor)
                     _vfwrite       PROBADA   212 B,   7 ramas,  18 relocs (16 por valor)
    libc/itoa        fftoa          PROBADA  1256 B,  56 ramas,  56 relocs (52 por valor)
                     itoa           PROBADA   124 B,   4 ramas,   3 relocs
                     strrev         PROBADA    76 B,   3 ramas,   0 relocs
                     strround       PROBADA    92 B,   5 ramas,   0 relocs

**10 funciones, 14.468 B probadas, sin una sola reubicacion sin cotejar.**

No he tocado `scripts/audit.py`: otro agente la estaba editando en el arbol
durante esta tanda (su copia de trabajo ya arregla el `KeyError: 'address'` de
los simbolos en desplazamiento 0). Los tres huecos de arriba siguen ahi y
convendria portarlos.

`scripts/frozen.py` **no existe en el arbol**, asi que no he podido congelar
nada con `frozen.py cong`. Los diffs completos quedan en
`scratchpad/r18libc_pr_*.json`.

## 8. Que NO he probado

- Cambiar cflags de verdad: el barrido de `-msdata`/`-G` de c8 se hizo
  compilando al scratchpad; `configure.py` y `config/GOWE69/*` estan intactos.
- Otras versiones de ProDG (3.5, 3.7, 3.8.1 estan en `build/compilers/`) para
  ver si alguna manda el pool a `.sdata`.
- El `permuter`: el diagnostico apunta a una seccion de datos, no a una
  permutacion de registros.
- La permutacion de registros que le queda a `powf` (r7/r8, f8/f9/f10) y las
  ~150 filas de `__kernel_rem_pio2f`: el exceso de instrucciones las tapa;
  primero hay que quitar los `lis`.
- El censo de llamantes no cubre punteros a funcion ni tablas de datos.
- Ejecucion en hardware o emulador de nada de esto.
