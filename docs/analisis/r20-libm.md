# Ronda 20 — grupo `libc/` (libm): las cuatro que quedaban

## 0. Resultado

    python scripts/build_direct.py ef_rem_pio2 sf_expm1 e_exp sf_exp
    python scripts/measure.py --cmp base_r20_libm.json despues_r20_libm.json
    +1652 B, +2 funciones, 2 unidades cambian
          +804 B  +1 fns  libc/sf_expm1      0 -> 804
          +848 B  +1 fns  libc/ef_rem_pio2   0 -> 848

    python scripts/measure.py libc/ef_rem_pio2 libc/sf_expm1 libc/e_exp libc/sf_exp
    4 unidades  2844/2844 B  100.0000%  4 funciones al 100%

Las cuatro pasan `scripts/audit.py` (dos pasadas) y estan congeladas.

| unidad | antes | ahora | B |
|---|---|---|---|
| `libc/ef_rem_pio2` | 96,509 % | **100 %** | 848 |
| `libc/sf_expm1` | 94,876 % | **100 %** | 804 |
| `libc/e_exp` | 100 % (con 2 pines) | **100 %** sin pines | 636 |
| `libc/sf_exp` | 100 % (con 2 pines) | **100 %** sin pines | 556 |

**El encabezado del encargo estaba rancio**: decia las cuatro a 0 %, pero
`base_r20_libm.json` ya traia 1192/2844 B — `e_exp` y `sf_exp` los habia cerrado
la tanda anterior (commits `24450c44` y `b7f658a3`), y `configure.py` ya tenia
los `extra_cflags` de las cuatro en `HEAD`. **No he tocado `configure.py`.**

Y algo que hay que decir: **las cuatro han quedado sin una sola linea de asm ni
un solo pin de registro**. Las muletas anteriores (`__asm__("" :: "r"(y))`,
barreras `# efpos`/`# efneg`, `register double __asm__("fr10")`) no eran
necesarias: eran el sintoma de que el DATO estaba mal.

## 1. La llave: donde acaban las variables y empieza el POOL

La r19 encontro que `-msafe-sda` manda el pool de constantes del compilador a
`.sdata`. Lo que faltaba era la consecuencia: **en el objetivo, buena parte de
las constantes de fdlibm NO son variables, son literales que el compilador
pliega en su pool** — y por eso estan en `.sdata`.

Se lee en el ELF sin adivinar nada. La entrada del sesgo `i2d`
(`4330000080000000`) mide 8 B y necesita alineacion de 8, asi que **parte el
bloque de constantes en dos y deja un hueco de 4 B**. Todo lo que hay *antes*
del hueco esta en orden de DECLARACION (son variables); todo lo que hay
*despues* esta en orden de **PRIMER USO** (son entradas del pool):

    sf_expm1, .sdata 804FF288:
      288 mone  28C o_threshold  290 pinf  294 tiny  298 zero
      29C ln2_hi  2A0 ln2_lo  2A4 nln2_lo  2A8 invln2      <- variables
      2AC half  2B0 mhalf                                  <- pool (1er uso)
      2B4 HUECO  2B8 sesgo i2d (8 B)                       <- pool
      2C0 huge  2C4 Q5  2C8 Q4  2CC Q3  2D0 Q2  2D4 Q1
      2D8 one  2DC three  2E0 six  2E4 mquarter  2E8 mtwo  <- pool

El orden `Q5, Q4, Q3, Q2, Q1, one` es el que delata el mecanismo: es el orden de
expansion de `one + hxs*(Q1 + hxs*(Q2 + hxs*(Q3 + hxs*(Q4 + hxs*Q5))))`, que GCC
evalua **de dentro afuera**. Ninguna lista de declaraciones sale asi.

Consecuencia para el codigo: una constante `SDA_FLOAT` es un pseudo largo
(`extern const` -> MEM RTX_UNCHANGING) que compite en `allocno_compare`; un
literal plegado nace donde se usa. Cambia el reparto de FPR entero.

## 2. Ensayos numerados

Todos con `scripts/build_direct.py` + objdiff 3.8.0,
`function_reloc_diffs=none`, `ppc.calculatePoolRelocations=false`.

### `ef_rem_pio2` — 96,509 -> 100 % (848 B)

    c1  El bloque |x| < 3pi/4 con y[0]/y[1] REPETIDOS dentro de cada rama
        (fdlibm) en vez de una `w` comun
                                          96,509 -> **99,906 %**, 844 -> 848 B.
                                          Es la misma llave que cerro e_rem_pio2
                                          en la r19 (c10) y aqui nadie la habia
                                          probado. Con `w` comun es UN solo
                                          pseudo vivo en las dos ramas del +-1 Y
                                          en la del tamano medio (f11 en las
                                          tres); el objetivo tiene f12 en el +-1
                                          y f1 en la media, o sea DOS pseudos.
                                          El cross-jumping posterior a reload
                                          vuelve a fundir las colas.
    c2  Quitar las dos entradas vacias `__asm__("" :: "r"(y), "r"(y))`
                                          99,906 %, sin cambio. **La muleta del
                                          swap r30/r31 ya no hace falta**: la
                                          arreglaba c1. Fuera. Las barreras de
                                          0 B `# efpos`/`# efneg` cayeron con
                                          c1 por el mismo motivo.
    c3  `z = two8 * (z - tx[i])`           99,882 %. Peor. DESCARTADO.
    c4  Partir la sentencia en `z = z - tx[i]; z = z * two8;`
                                          99,858 %, 5 diffs. DESCARTADO.
    c5  `two8` como `static const float` (plegado al pool) en vez de SDA_FLOAT
                                          **100,000 %**. Como variable, el
                                          pseudo de `two8` nacia antes que el
                                          del sesgo i2d del bucle y le robaba
                                          f12.
    c6  Bucle `do { } while (++i < 2)`     99,906 %, igual. DESCARTADO.
    c7  `pio2_3` y `pio2_3t` tambien `static const`
                                          100,000 % y **la .sdata pasa a tener
                                          el mismo hueco de 4 B y el mismo orden
                                          que el original**. Pero GCC emite
                                          ademas las tres variables sin usar en
                                          `.sdata2`: 12 B que el objetivo no
                                          tiene.
    c8  Las tres como `#define` con sufijo `f` (literales puros)
                                          100,000 % y `.sdata2` vuelve a 920 B.
                                          **`.sdata` (56 B) y `.sdata2` (920 B)
                                          identicas byte a byte al ELF**
                                          (804FF350 y 8050042C). SE QUEDA.

### `sf_expm1` — 94,876 -> 100 % (804 B)

Partia con una veda de la r19 que decia «el repack de FPR no responde a la forma
de la fuente». **La veda era falsa**: no respondia a la forma del CODIGO porque
el problema estaba en los DATOS.

    d1  huge, Q5..Q1, one, three, six, mquarter, mtwo como `#define` (pool)
                                          94,876 -> **98,532 %**, 66 -> 20 diffs.
                                          Y la .sdata pasa a 104 B con el hueco
                                          y el sesgo en su sitio.
    d2  La cola con `int i` de bloque y GET/SET repetidos en las dos ramas,
        como fdlibm, en vez de factorizados detras del if/else
                                          98,532 -> **99,627 %**, 20 -> 14.
                                          Cierra el `add r0,r9,r0` /
                                          `add r9,r9,r0`: con la version
                                          factorizada el temporal reusaba el
                                          registro de `hx`.
    d3  `half` y `mhalf` tambien al pool   99,627 -> **99,925 %**, 14 -> 3.
                                          Cierra el intercambio half=f5 /
                                          c=f6. La .sdata NO se mueve: el pool
                                          los crea en el mismo sitio en que
                                          estaban como variables.
    d4  La reduccion entera de fdlibm
        `k = invln2*x + ((xsb==0)?half:mhalf); t = k;`
                                          97,562 %, 19 diffs. PEOR. DESCARTADO.
    d5  `t = invln2 * x` (orden de operandos)
                                          99,876 %. Peor. DESCARTADO.
    d6  Un local propio `u` para el producto: `u = x * invln2; k = u + half;`
                                          **100,000 %**. En fdlibm ese producto
                                          es un temporal anonimo dentro de
                                          `k = invln2*x + (...)`; metido en `t`
                                          --que vive de punta a punta de la
                                          funcion-- su allocno caia a f1, y el
                                          objetivo lo tiene en f11. SE QUEDA.
    d7  Sin temporal, multiplicacion duplicada en las dos ramas
                                          96,841 % y **792 B** (falta una
                                          instruccion). DESCARTADO.

`.sdata` (104 B) identica byte a byte al ELF (804FF288). No hay otra seccion de
datos.

### `e_exp` y `sf_exp` — 100 % ya, pero con el DATO mal y dos pines

    e1/f1  huge, one, P5..P1, two, twom1000/twom100 como `#define` (pool)
                                          siguen al 100 % Y **la .sdata deja de
                                          estar corrida una ranura**: pasa a ser
                                          identica al ELF (120 B en 804FF6E0,
                                          72 B en 804FF0C8).
    e2/f2  Quitar los dos pines de registro y la barrera de asm, dejando el
           `x = hi - lo;` pelado de fdlibm
                                          **100 %** las dos. Los pines
                                          (`register double __asm__("fr10")`,
                                          `__asm__("fr7")`) y el
                                          `__asm__ volatile("" : "+f"...)`
                                          sobraban en cuanto el pool esta bien.

Esto es el patron de «el 100 % que miente» **en los datos**: el codigo casaba
byte a byte y la `.sdata` del objeto estaba desplazada 8 B, porque el sesgo i2d
iba al final en vez de en medio.

## 3. La prueba

    scripts/audit.py libc/ef_rem_pio2   ok  848 B, 24 ramas, 49 relocs, 21 lit.
    scripts/audit.py libc/sf_expm1      ok  804 B, 34 ramas, 65 relocs, 31 lit.
    scripts/audit.py libc/e_exp         ok  636 B, 21 ramas, 45 relocs, 19 lit.
    scripts/audit.py libc/sf_exp        ok  556 B, 21 ramas, 45 relocs, 19 lit.

Dos pasadas, identicas. Congeladas con `frozen.py cong`:

    libc/ef_rem_pio2  a5c9d059cf2b5e0e     libc/e_exp   8cb1c37ef797ef6f
    libc/sf_expm1     bec9360ff6afd1fb     libc/sf_exp  8c8ec513acdfbf8b

**Y ademas**, comparacion byte a byte de las secciones de datos contra el ELF
original (`scratchpad/r20libm/cmpdata.py`), que `audit.py` no hace porque
compara reubicaciones por valor, no la seccion entera:

    ef_rem_pio2  .sdata  56 B  IGUAL   .sdata2 920 B  IGUAL
    sf_expm1     .sdata 104 B  IGUAL   (no tiene otra)
    e_exp        .sdata 120 B  IGUAL   .sdata2  48 B  IGUAL
    sf_exp       .sdata  72 B  IGUAL   .sdata2  24 B  IGUAL

## 4. Promocion: las cuatro rompen el DOL

Pedido explicitamente por el encargo, `scripts/trypromo.py`, una a una y las
cuatro juntas:

    libc/ef_rem_pio2                             DOL ROTO (5fe358831bae)
    libc/sf_expm1                                DOL ROTO (88ae6d2fbfaf)
    libc/e_exp                                   DOL ROTO (a6e7c095f51a)
    libc/sf_exp                                  DOL ROTO (c6b3b174cf28)
    las cuatro juntas                            DOL ROTO (1883b04064b9)

Enlaza, pero el DOL sale distinto. Es el mismo muro que las siete de la r19, y
ahora se entiende por que: **nuestros objetos LLEVAN la `.sdata` de esas
constantes**, que en el original la aportan los objetos de datos que troceo el
extractor. Promocionar duplica o desplaza ese bloque. **No he promocionado
nada** y `configure.py` sigue como en `HEAD`.

## 5. El hallazgo se extiende: 4 de las 7 unidades de la r19 tienen el DATO mal

Contado, no extrapolado (`scratchpad/r20libm/scandata.py`, que localiza el
bloque por la direccion mas baja que codifica el nombre del troceador y compara
la `.sdata` entera contra el ELF):

    e_pow         272 B  base=804FEEF0  DISTINTO
    ef_pow        136 B  base=804FF180  DISTINTO
    e_rem_pio2     88 B  base=804FF7E0  DISTINTO
    sf_log         72 B  base=804FF110  DISTINTO
    k_rem_pio2     64 B  base=804FF838  IGUAL
    kf_rem_pio2    40 B  base=804FF388  IGUAL
    kf_tan         32 B  base=804FF330  IGUAL
    sf_log10       40 B  base=804FF158  IGUAL

Las tres `IGUAL` de la r19 son justo las que llevaron `static const` (c4/c11).
El patron de las cuatro `DISTINTO` es el mismo que tenian `e_exp`/`sf_exp`:

    sf_log  +00..+0C iguales, y a partir de ahi TODO corrido 8 B porque el
            sesgo i2d va en +0x40 y en el original va en +0x10.
    e_rem_pio2  el orden entero es otro: el objetivo tiene
            zero, pio2_1, pio2_1t, pio2_2, pio2_2t, invpio2, half, SESGO,
            pio2_3, pio2_3t, two24 (primer uso), y nosotros la lista de
            declaraciones con el pool al final.

**No las he tocado**: no son mi encargo, estan congeladas por la r19 y el arbol
es compartido. El codigo de las cuatro sigue al 100 %; lo que esta mal es el
dato, y se arregla con el mismo `#define` (medido aqui cuatro veces).

## 6. Que NO he probado

- **Arreglar el dato de `e_pow`, `ef_pow`, `e_rem_pio2` y `sf_log`.** Medido que
  esta mal, no corregido. Son ~568 B de `.sdata` y la receta esta cerrada.
- **Los pines de registro de otras unidades**: aqui han caido dos veces en
  cuanto el pool estaba bien. No he barrido `libc/` buscando mas pines.
- **`-msafe-sda` fuera de `libc/`** (sigue igual que en la r19), y los otros
  switches de SN (`-msndata=N`, `-mfast-cast`, `-malign-section-names`).
- **La duplicacion en `.sdata2`** que provoca `static const` (c7): no he mirado
  si hay unidades ya cerradas que la arrastren.
- **Promocion**: solo la medida de `trypromo.py`. No he construido el DOL ni he
  mirado `promote.py`, ni he investigado si un `keep.lst` o un cambio en el
  troceado permitiria enlazar estas unidades.
- Ejecucion en hardware o emulador de nada de esto.

## 7. Nota de concurrencia

Una invocacion mia de `build_direct.py --help` recompilo **todas** las
SourceLists (el script no tiene `--help`: interpreta el argumento como filtro y
al no casar con nada, compila todo). No cambia ninguna fuente, pero puede haber
pisado compilaciones de otros agentes en marcha. Los cuatro `audit.py` se han
repetido en dos pasadas por si acaso, y dan lo mismo.
