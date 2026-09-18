# Ronda 18 — LibSN y los drivers

Grupo: `LibSN/metrotrk`, `LibSN/steering`,
`Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver`.

## 0. La asignación partía de cifras caducadas

La tabla del encargo daba `metrotrk` al 0 %, `steering` al 36,9 % y
`DebuggerDriver` al 16,4 % con 5.008 B pendientes. Medido con
`python scripts/measure.py -o antes_r18_libsn.json …` sobre los objetos reales
recién compilados (`antes_r18_libsn.json`):

| unidad | exacto | total | % | pendiente |
|---|---:|---:|---:|---:|
| LibSN/metrotrk | 148 B / 7 fn | 12.136 B / 107 fn | 1,22 % | 11.988 B |
| LibSN/steering | 3.668 B / 24 fn | 8.760 B / 36 fn | 41,87 % | 5.092 B |
| OdemuExi2/DebuggerDriver | 5.184 B / 28 fn | 5.988 B / 31 fn | 86,58 % | 804 B |

Total del grupo: **9.000 / 26.884 B = 33,48 %**, 59 funciones al 100 %.

`DebuggerDriver` no tiene 5.008 B pendientes sino **804 B**: jferr lo cerró en
las fases 5 a 10 (`scratchpad/phase5..phase10_debugger_*`). Lo que queda son
tres funciones y las tres son problemas de planificación de epílogo:
`EXI2_Poll` 264 B / 99,288 %, `EXI2_WriteN` 432 B / 94,444 % y
`AmcEXISetExiCallback` 108 B / 85,185 %. Las fases 7 y 8 documentan **doce
ensayos descartados** sobre exactamente esas tres (helpers static-inline,
retorno común, temporales del latch, permutación de argumentos del helper,
modelos de procesador 601/603e/604/7400/8240, y GC/1.2.5 completo). No repito
ese barrido.

## 1. Identidades

* ELF original `orig/GOWE69/NFSMWRELEASE.ELF`, SHA256
  `6ee16d55d92ad1b337a9b655989c32b2976f95e7c64e148cf83f482c613f79f7`
  (el mismo que fija `scratchpad/phase14_ppc2d2_plan.md`).
* `build/GOWE69/obj/LibSN/metrotrk.o` — objetivo, 107 STT_FUNC / 12.136 B,
  rango `.text` 0x8030EA7C..0x80311BE4.
* `build/GOWE69/obj/LibSN/steering.o` — objetivo, 36 STT_FUNC / 8.760 B.
* `build/GOWE69/obj/…/DebuggerDriver.o` — objetivo, 31 STT_FUNC / 5.988 B.

## 2. El hallazgo que decide el reparto: metrotrk no es C

**`LibSN/metrotrk` no es código de compilador. Es ensamblador escrito a mano.**
La prueba no es una impresión: son cuatro medidas independientes.

**(a) Censo por convención de llamada.** Clasificando las 99 funciones
pendientes de `build/GOWE69/asm/LibSN/metrotrk.s` según si leen un registro no
volátil sin haberlo salvado antes, y si usan instrucciones de supervisor:

| clase | fn | bytes |
|---|---:|---:|
| ABI privada (lee no-volátil sin salvarlo) | 27 | 5.588 |
| ABI privada + marco | 7 | 2.528 |
| supervisor / SPR (`rfi`, `mtmsr`, `mtspr`) | 9 | 1.800 |
| marco «normal» | 15 | 1.336 |
| hoja sin marco | 42 | 736 |

**9.916 B de 11.988 (82,7 %) no son expresables en C legal.** El ejemplo canónico
es `fn_8030EA80` (120 B): empieza con `li r6,1 ; stw r6,0x6ec(r31)` sin prólogo
ninguno — r31 es el contexto del depurador que entra vivo desde el llamante.

**(b) La clase «marco normal» tampoco es de compilador.** Ocho de esas quince
guardan LR por **r4**, no por r0:

    fn_8031152C:  mflr r4 ; stw r4,0x4(r1) ; stwu r1,-0x8(r1) …
                  … addi r1,r1,0x8 ; lwz r4,0x4(r1) ; mtlr r4 ; blr
    fn_8031147C:  … addi r1,r1,0x8 ; lwz r4,0xc(r1) …   (epílogo distinto)

Ningún compilador salva LR por r4, y menos aún cambia la forma del epílogo
entre dos funciones consecutivas del mismo fichero. `fn_80311440` incluye
además `li r3,6 ; mtctr r3 ; .L: bdnz .L` — un bucle de retardo vacío.

**(c) La forma de las direcciones.** Todo el bloque materializa direcciones con
`lis rX,sym@h ; ori rX,rX,sym@l` (reubicaciones ADDR16_HI/ADDR16_LO). Compilé el
mismo fuente con las siete versiones de Metrowerks y con ProDG 3.9.3: **ninguna
emite @h/@l para un símbolo**; todas dan `lis @ha ; addi @l`. `@h/@l` es el
idioma del ensamblador, no de un compilador. (Probé mwcc 1.2.5n sobre
`fn_80310DB4` — sale `lis @ha`/`addi @l` y además un `crclr 6` de varargs que el
original no tiene: `scratchpad/r18libsn_probe/`.)

**(d) No hay DWARF.** `symbols/debug_lines.txt` tiene **cero** líneas en
0x8030EA7C..0x80311BE4. Por tanto `fuse.py` y `plan.py` —el método que el
encargo recomienda— **no tienen nada que fundir en esta unidad**. (Aviso: las
otras dos unidades del grupo tampoco tienen DWARF, así que este cuarto punto
por sí solo no distingue «ensamblador» de «biblioteca sin depuración»; los tres
anteriores sí.)

**Consecuencia.** Con la regla vigente («escribir ensamblador está prohibido»),
de los 11.988 B de metrotrk son alcanzables como mucho ~2.000 B, y 96 de esos
bytes son las 24 ranuras PC* automodificables que la fase 7 prohíbe
explícitamente contar como funciones C. Los 148 B ya cerrados por jferr lo
están **mediante `asm("lis %0,X@h\nori %0,%0,X@l")`**, es decir emitiendo dos de
las cuatro instrucciones de cada hoja desde ensamblador en línea. No lo he
tocado —está confirmado y auditado por su ronda 9— pero conviene que quien
decida sepa que ese patrón es justo el que el brief clasifica como prohibido.

**El camino sancionado ya existe y no lo puedo tomar yo**: `src/LibSN/pro2D4.s`
se registró esta misma sesión como `Object(Matching, "pro2D4.tmp",
source="LibSN/pro2D4.s")` (`configure.py:1445`) y se compila con la regla `as`,
igual que `LibSN/crt0.s` (`configure.py:1360`). Mover `metrotrk` a
`metrotrk.s` es exactamente el mismo cambio y abriría los 11.988 B enteros —
pero toca `configure.py`, que el brief me veta. **Queda como recomendación.**

## 3. Coordinación con ppc2D2: dos símbolos que no toco

`scratchpad/phase14_ppc2d2_plan.md` nombra dos funciones que viven en
**metrotrk**, no en ppc2D2:

* `fn_8031161C` / `SNDVDReadSync_next`, 0x8031161C, 104 B — «r3 destino, r4
  bytes; resultado ignorado».
* `SNSync`, 0x80311468, 20 B — «sondea el bit cero hasta que se limpia».

Ambas están en mi lista de candidatas por tamaño y ambas son de las quince de
«marco normal». **No las escribo.** El plan de phase14 dice que no las
reconstruye ni les da prototipo nuevo, así que definir su cuerpo en
`metrotrk.c` no cambiaría `ppc2D2.o`; aun así el encargo pide parar y avisar, y
eso hago.

## 4. steering: dónde está el trabajo real

Doce funciones pendientes, 5.092 B. Separadas por naturaleza del fallo:

| función | bytes | % | naturaleza |
|---|---:|---:|---|
| Effect_Update | 992 | 99,839 | transposición r6↔r7, 6 instrucciones |
| SimThread_Step | 924 | 92,355 | mezcla |
| Effect_UpdateEffect | 808 | 69,079 | forma real |
| HandleTriggers | 588 | 94,898 | mezcla |
| SimThread_Init | 324 | 90,741 | mezcla |
| Effect_Init | 276 | 29,232 | memset en línea + izado de constantes |
| CookValues | 260 | 95,769 | transposición r5↔r6 + un `extsb` |
| VDevice_RecalcGammaTable | 216 | 95,907 | marco 0x30 vs 0x20 + r30↔r31 |
| LGDownloadForceEffect | 204 | 99,510 | transposición r3↔r7, 4 instrucciones |
| Effect_PerformEnvelope | 196 | 93,265 | mezcla |
| VDevice_DownloadEffect | 188 | 99,362 | transposición r30↔r31, 6 instrucciones |
| VDevice_GetFreeEffect | 116 | 77,069 | forma real: falta una recarga y el orden del OR |

`libsn7_audio_resume_20260904_notes.md` deja ya descartadas, con cifra:
`Effect_Update` (orden de declaración y formas del resultado de la división),
`LGDownloadForceEffect` (booleano de validez materializado — **regresó**) y
`VDevice_DownloadEffect` (doce variantes de orden/inicialización de locales →
98,511 % o 99,362 %, nunca 100 %).

### Barrido de compilador y banderas (sólo medición, nada aplicado)

`scratchpad/r18libsn_sweep.py`, objeto a scratchpad, sin tocar `configure.py`:

* Niveles con GC/2.7: `-O4,s`/`-O4` con `-inline off` es **el mejor** de la
  rejilla (3.668 B, 92,703 % ponderado). `-O4,p` cae a 1.772 B; `-O3` y por
  debajo se desploman a 20 B. Las banderas actuales son correctas.
* Versiones con `-O4,s -inline off`: 1.3.2/1.3.2r/2.0/2.5/2.6/2.7 dan las mismas
  24 funciones y 3.668 B. **GC/2.0p1 da 94,051 % ponderado frente a 92,703 %**,
  sin regresar ninguna: sube `Effect_Init` de 29,232 → 69,275 % y
  `SimThread_Init` de 90,741 → 93,086 %. 1.2.5n y la serie 3.0a* se hunden.
* **No cierra ninguna función**, así que no es un cambio que valga por sí solo;
  queda anotado como pista de procedencia, y cambiar el compilador sería tocar
  `configure.py`.

## 5. Qué NO he probado

* **No he tocado `ppc2D2`** ni ninguno de los símbolos del plan de phase14.
* No he vuelto a barrer las tres pendientes de `DebuggerDriver`: las fases 7 y 8
  ya gastaron doce ensayos ahí y no tengo hipótesis nueva sobre el epílogo.
* No he intentado ninguna de las 34 funciones de ABI privada de metrotrk ni las
  9 de supervisor: no hay forma legal en C de expresarlas.
* El censo del apartado 2(a) es una clasificación **estática por lectura de
  registros no volátiles**; no es una prueba de que las 15 «marco normal» sean
  todas de compilador — de hecho 2(b) demuestra que ocho no lo son.
* El barrido de banderas cubrió `-O0..-O4`, `-O4,s`, `-O4,p` × `-inline
  off/auto` × 14 versiones de mwcc. **No** cubrió `-sdata/-sdata2`, `-schedule`,
  `-fp_contract off`, `-use_lmw_stmw off` ni `-char signed`.
* No he ejecutado nada: todas las afirmaciones son sobre instrucciones y bytes.

## 6. Ensayos

Todos con `scratchpad/r18libsn_try.py` (compila la unidad real a scratchpad y
mide con objdiff-cli; ~10 s por vuelta). Cifra = `match_percent` de la función.

### metrotrk — `fn_8030F4B4`, 20 B (lo único que cierra)

Partida 0 % (ausente). Es la única hoja del bloque privado con convención C
ordinaria: r3 es un puntero de verdad, su arista de retroceso apunta a la
entrada de la función (no hay prólogo), y nada más entra vivo.

    c31: do/while, u32 pending                  -> 96,000 %  (temporal en r0, no r4)
    c32: carga y máscara en dos sentencias      -> 96,000 %  idem
    c33: for(;;) con break explícito            -> 96,000 %  idem
    c34: while ((pending = *reg & 0x14) == 0)   -> 96,000 %  idem, con int con signo
    c35: dos locales, raw y pending             -> 96,000 %  idem
    c36: register unsigned int pending asm("r4") -> 100 %    <- se queda

Las cinco formas de fuente dan **la misma instrucción** y el mismo tamaño
(20 B, `beq` a la entrada incluido); sólo cambia el registro. GCC 2.9 elige r0
para un temporal que muere dentro de una hoja. La ligadura de registro es de
último recurso, está comentada en la fuente diciendo qué se barrió, y **pasa
`python scripts/audit.py LibSN/metrotrk`** (8/8 ok, bytes contra el ELF
original, 1 rama y 1 reubicación comprobadas).

No escribí `fn_8030EFD4`, `fn_803113B8` ni `fn_803113BC` (4 B cada una, un
`blr` suelto). Son funciones de verdad en el split, y `void f(void) {}` daría
100 % — pero son exactamente el tipo de relleno que la fase 7 de jferr prohíbe
contar como decompilación. **12 B que dejo sobre la mesa a propósito.**

### steering — `VDevice_RecalcGammaTable`, 216 B

    c16: declarar `u8 *p` antes que `s32 i`     -> 96,833 %
    c17: asignar g antes que p                  -> 95,907 %  (sin cambio)
    c18: quitar el puntero p, indexar el array  -> 99,796 %  <- el hallazgo
    c19: sin la local v, cast en cada rama      -> 68,463 %
    c20: v declarada dentro del bucle           -> 99,796 %
    c21: valor absoluto y signo aparte          -> 17,241 %
    c22: ternario en vez de if/else             -> 99,796 %
    c23: local n compartida por las dos ramas   -> 99,796 %
    c24: negar v en una sentencia separada      -> 97,037 %
    c25: 0.6666667f literal, sin la local g     -> 97,574 %
    c26: while en vez de for                    -> 99,796 %
    c27: cast explícito (u8)(s32)v              -> 99,796 %

**c18 es correcto y no cierra.** Con él las 54 instrucciones y los registros
coinciden **todos**; lo único que queda es el marco: el objetivo usa **una**
ranura de 8 B en 0x8(r1) para las tres conversiones (dos int→double, una
double→int) y marco 0x20; nosotros usamos tres ranuras (0x8, 0x10, 0x18) y
marco 0x30. Siete formas distintas dan la misma asignación de ranuras, y
tampoco la mueven `-opt lifetimes` / `-opt nolifetimes` ni ninguna de las seis
versiones de mwcc que compilan bien esta unidad.

**Veda:** barrida la sentencia `*p = (u8)v; p++` frente a
`g_iGammaLookup[i] = (u8)v`, la posición de la declaración de `v`, la forma del
bucle, el ternario, la local intermedia y el cast doble. **No** se ha probado
declarar `pow` con otro prototipo ni tocar el orden de las dos ramas.

El parche de c18, para que la próxima ronda no lo redescubra:

```diff
-    s32 i;
-    u8 *p;
-    float v;
-    float g;
-
-    p = g_iGammaLookup;
-    g = 0.6666667f;
+    s32 i;
+    float v;
+    float g;
+
+    g = 0.6666667f;
     for (i = 0; i < LG_GAMMA_SIZE; i++) {
         ...
-        *p = (u8)v;
-        p++;
+        g_iGammaLookup[i] = (u8)v;
```

### steering — `Effect_Update`, 992 B

Partida 99,839 % con **seis** instrucciones distintas: cuatro son la
transposición r6↔r7 de `start`/`duration` y dos son de forma real
(`divw r0,r3,r6` frente a `divw r3,r3,r7`, y `add r3,r7,r0` frente a
`add r3,r3,r6`).

    c2:  n = e->e.u.ramp.start + n            -> 99,718 %
    c3:  v = start + (n*m + h)/d, expresión   -> 99,718 %  (invierte n*m + h)
    c4:  n*=m; n+=h; v = start + n/d          -> 99,758 %  (formas ya correctas)
    c5:  local s para ramp.start              -> 99,718 %
    c6:  n /= d; v = start + n                -> 99,718 %
    c7:  v = n/d + start                      -> 99,758 %
    c8:  c4 con `int d` declarada la primera  -> 99,819 %  <- la mejor forma
    c9:  sin la local m                       -> 98,427 %
    c10: sin la local d                       -> 98,871 %
    c11: h calculada dentro del if            -> 97,714 %
    c12: usar las locales t/round/v del ámbito de función -> 97,056 %
    c13: c12 con v intermedia                 -> 97,077 %
    c14: orden de declaración != orden de asignación -> 99,758 %
    c15: declaración d, m, n, h               -> 99,819 %  (= c8)

**c8/c15 dejan CERO diferencias de forma**: las nueve instrucciones del bloque
de rampa son idénticas y `d` cae ya en r6 como el objetivo; sólo quedan
`start` y `n` intercambiadas entre r3 y r7. Es decir, **c8 es más cerca de la
fuente original que la línea base aunque su porcentaje sea 0,02 pp menor** —
justo la trampa que el brief describe. No se retiene porque no cierra.

**Veda:** barridas quince formas de la sentencia
`n /= d; n += start;` y del bloque de locales de la rama EF_RAMP. La hipótesis
«el original usa las locales `t`/`round`/`v` del ámbito de función, como hace
la rama EF_PERIODIC» queda **descartada con cifra** (97,06 %).

### steering — `Effect_UpdateEffect`, 808 B — un fallo de tipo real

Nuestro `u32 dir;` hace que `DEG2BYTE(dir)` emita **`divwu`** donde el original
emite **`divw`** (dos sitios). Los casos 1 y 2 usan el campo directamente, que
promociona a `int`, y ahí sí sale `divw`.

    c28: s32 dir                              -> 66,500 %  (el divwu SÍ pasa a divw)
    c29: s32 dir, at, ft                      -> 66,500 %
    c30: caso 0 sin las cinco locales         -> 65,817 %

El porcentaje **baja** aunque la instrucción se corrige: la alineación de
objdiff se desplaza. Es el segundo ejemplo de la trampa «el porcentaje no mide
avance». La causa de fondo del 69 % no es el tipo: el caso 0 carga cinco campos
por adelantado (`dir/at/ft/al/fl`), llega a **r12** por presión de registros y
el original los intercala. No he encontrado la forma que reproduce el
intercalado; queda como el mayor bloque de trabajo real de la unidad.

### steering — barrido de mnemónicos: dónde está de verdad el trabajo

Contando, por función pendiente, las diferencias en las que cambia **el
mnemónico** (no sólo el registro) e ignorando destinos de rama:

| función | bytes | % | mnem | sólo registro |
|---|---:|---:|---:|---:|
| Effect_Update | 992 | 99,839 | **0** | 6 |
| SimThread_Step | 924 | 92,355 | 15 | 72 |
| Effect_UpdateEffect | 808 | 69,079 | 62 | 58 |
| HandleTriggers | 588 | 94,898 | **4** | 50 |
| SimThread_Init | 324 | 90,741 | 8 | 4 |
| Effect_Init | 276 | 29,232 | 50 | 20 |
| CookValues | 260 | 95,769 | 2 | 12 |
| VDevice_RecalcGammaTable | 216 | 95,907 | 2 → **0 con c18** | 19 |
| LGDownloadForceEffect | 204 | 99,510 | **0** | 5 |
| Effect_PerformEnvelope | 196 | 93,265 | 1 | 30 |
| VDevice_DownloadEffect | 188 | 99,362 | **0** | 6 |
| VDevice_GetFreeEffect | 116 | 77,069 | 6 | 5 |

**1.384 B (Effect_Update + LGDownloadForceEffect + VDevice_DownloadEffect) no
tienen ni una sola diferencia de mnemónico**: son transposiciones limpias de
dos registros. Ninguna de las catorce versiones de mwcc las cierra (tabla en
§4), ninguna bandera las mueve, y jferr ya gastó quince ensayos de orden de
locales en ellas. **Es un frente de asignación de registros de mwcc, no de
fuente**, y conviene tratarlo como tal antes de gastar otra ronda en formas.

`HandleTriggers` es la mejor pieza siguiente: 588 B, y sus cuatro diferencias
de mnemónico son **el mismo `mr` sobrante repetido cuatro veces** (nuestro
objeto mide 604 B, el objetivo 588). Nuestro `_mx` de AUTOCAL cae en **r0** y
hay que copiar; en el original cae en el registro donde ya vivía la CSE de
`cal->max.left` del `if` exterior. La macro AUTOCAL **no** se toca: la usa
también `HandlePedals`, que está al 100 %.

## 7. Resultado medido

`antes_r18_libsn.json` → `despues_r18_libsn.json`, con los tres objetos reales
recompilados por `build_direct.py` antes de cada medida:

    antes:    9.000 / 26.884 B  33,4772 %  59 funciones al 100 %
    despues:  9.020 / 26.884 B  33,5516 %  60 funciones al 100 %

    LibSN/metrotrk            148 B /  7 fn  ->  168 B /  8 fn   (+20 B, +1)
    LibSN/steering           3.668 B / 24 fn -> 3.668 B / 24 fn   (sin cambio)
    OdemuExi2/DebuggerDriver 5.184 B / 28 fn -> 5.184 B / 28 fn   (sin cambio)

**+20 B y +1 función.** Único fichero modificado: `src/LibSN/metrotrk.c`.
`git diff --check` correcto. `python scripts/audit.py LibSN/metrotrk` pasa con
las ocho funciones. No se tocaron cabeceras, banderas, `configure.py`,
`config/GOWE69/*`, `splits.txt`, mapeos ni ninguna otra unidad.

El objeto que entra en el enlace del DOL es `build/GOWE69/obj/LibSN/metrotrk.o`
—el original—, no el nuestro (línea 19789 de `build.ninja`); la referencia a
`build/GOWE69/src/LibSN/metrotrk.o` de la línea 20327 pertenece al *phony*
`all_source`. El cambio no puede alterar el DOL.

**`scripts/frozen.py` no existe en el árbol.** El brief lo da por hecho; no
está, así que la congelación se hace con las instantáneas
`scratchpad/r18libsn_metrotrk_antes.json` y `despues_r18_libsn.json`, más
`audit.py`, que es la prueba fuerte.

## 8. Aviso sobre `audit.py`: 18 FALLA falsas en dos unidades

`python scripts/audit.py LibSN/steering` y
`… OdemuExi2/src/DebuggerDriver` devuelven **rc=1**, con dieciocho líneas del
tipo:

    AmcEXIDeselect   FALLA: reubicacion a otro simbolo: exi_804FF598 contra exi
    VDevice_Init     FALLA: reubicacion a otro simbolo: g_bGammaInitialized_804FF8C4 contra g_bGammaInitialized

**No son fallos.** El objeto original bautiza sus locales anónimas como
`<nombre>_<dirección>` y nuestra fuente usa `<nombre>` a secas. `symbols.txt` lo
confirma para las diecisiete que llevan nombre:

    exi                 = .sdata:0x804FF598   <-> exi_804FF598
    g_bGammaInitialized = .sbss:0x804FF8C4    <-> g_bGammaInitialized_804FF8C4
    DBGCallback         = .sbss:0x804FFC4C    <-> DBGCallback_804FFC4C

La decimoctava (`LGInit`, `lbl_804FF8C0` contra `g_bInitialized`) es del mismo
tipo pero al revés: nuestra fuente le pone nombre a una etiqueta que el
original deja anónima. `g_bInitialized` es `static int` en `steering.c` y no
aparece en `symbols.txt`, así que **su dirección no queda demostrada aquí**;
las otras diecisiete sí.

Esto no lo he introducido yo: las dos unidades están intactas y estas FALLA
estaban antes de mi cambio. Es el mismo problema que jferr resolvió a mano en
`libsn7_audio_resume_20260904_notes.md` («audit-only normalization»), y que
`scripts/audit.py` no lleva. **Mientras no se normalice el sufijo `_<addr>`,
`audit.py` no puede usarse como criterio de aceptación en unidades con datos
locales anónimos** — dará rc=1 sobre trabajo correcto. Es el mismo género que
«el 100 % que miente», pero en el otro sentido: aquí miente el rojo.
