# r54-epcalc — la ranura huérfana de `epCalculate`: **NO es memoria secundaria**

**Titular.** El experimento que la r52 y la r53 dejaron escrito está ejecutado y su
hipótesis **queda refutada con medida**: los 8 B que sobran en el marco **no son
`secondary_memlocs[]` de ningún modo**. Y de paso cae el mapa de pila con el que se
trabajaba desde la r46: **el objetivo NO tiene ranura huérfana** — lo que se leía como
«el objetivo tiene una y nosotros dos» es **relleno de alineación** que la aritmética de
`rs6000_stack_info` explica al byte.

`fncmp` de zEcstasy **idéntico antes y después**: 3 funciones / 4.384 B
(`epCalculate` 164 insn, `UpdatePlatInfo` 27, `eProject` 13). Único fichero tocado:
`src/Speed/GameCube/Src/Ecstasy/EcstasyEx.cpp`, y **sólo un bloque de comentario, con el
mismo número de líneas** (la hipótesis rancia de la r47 estaba escrita ahí arriba y ya
mandaba a la gente al sitio equivocado). Cero `asm` puestos, cero deuda.

---

## 1. El mapa de pila, corregido — y el objetivo no tiene hueco

`rs6000_stack_info` (`config/rs6000/rs6000.c:3890-4080`) con `abi == ABI_V4`,
`RS6000_SAVE_AREA = 8` (`config/rs6000/sysv4.h:501`), `BIGGEST_ALIGNMENT = 64`
(`sysv4.h:598`), `ABI_STACK_BOUNDARY = 64` (EABI):

    fp_size   = 8*18 = 144      gp_size = 4*18 = 72      cr_size = 4
    save_size = RS6000_ALIGN(144+72+4, 8) = 224          <- +4 de relleno
    fpmem_p   = regs_ever_live[76] = 1  ->  fpmem_size = 8, y como
                (main_save_offset - 8) % 8 != 0, fpmem_size += 4  ->  12

| tramo | OBJETIVO (marco 0x170) | NUESTRO (marco 0x178) |
|---|---|---|
| `fixed_size` (back chain + LR) | 0x000-0x007 | 0x000-0x007 |
| **`vars_size`** (las 16 ranuras) | **0x008-0x07f = 120 B** | **0x008-0x087 = 128 B** |
| relleno (`save_size` +4, `total_size` +4) | 0x080-0x087 | 0x088-0x08f |
| `fpmem` (conversión int↔float) | 0x088-0x08f (+4 de ajuste) | 0x090-0x097 (+4) |
| CR (`cr_save_offset = -220`) | 0x094 | 0x09c |
| `stmw r14` / `psq_st f14..f31` | 0x098 / 0x0e0 | 0x0a0 / 0x0e8 |

Comprobado contra el objeto: `stwu -0x170` vs `-0x178`, y las 49 filas del `fndiff` que
son **exactamente** el desplazamiento +8 (prólogo, epílogo y los tres accesos a `fpmem`).

> **Corrección de la r46/r52/r53**: los 0x080-0x087 del objetivo son **relleno**, no una
> ranura. Toda la diferencia es `get_frame_size()`: **120 en el objetivo, 128 en nosotros**.

### 1.1 Las «ranuras de 4 B con hueco» eran ranuras de 8 B leídas en +4

Las 16 ranuras que `alter_reg` (`reload1.c:2466`) reserva llenan `frame_offset` 0..120
**sin un solo hueco**, y la estructura no es la que se venía suponiendo:

| ranuras | modo | tamaño | dirección |
|---|---|---|---|
| 0x008, 0x00c | `SF` | 4 | `fo + 8` |
| 0x010…0x040, 0x050, 0x058, 0x060 | `PS` | 8 | `fo + 8` |
| **0x04c, 0x06c, 0x074, 0x07c** | **`SF` paradójica** | **8** | **`fo + 4 + 8`** |

Las cuatro últimas son pseudos `SFmode` con una referencia `(subreg:PS …)`: en `alter_reg`
`total_size = MAX(inherent, reg_max_ref_width) = 8`, `align = -1`, y `assign_stack_local`
aplica la **corrección big-endian** `bigend_correction = size - GET_MODE_SIZE(mode) = 4`
(`function.c:837`). Por eso 0x048/0x068/0x070/0x078 *parecían* huecos de 4 B y no lo son.

Suma: `4+4 + 10·8 + 4·8 = 120`. **Cuadra al byte con el `vars_size` del objetivo.**
Y las 16 corresponden, una a una y en orden ascendente de pseudo, a las 16 más bajas de
las 24 con `reg_renumber < 0` del `.greg`
(`112 114 120 123 127 129 131 133 135 139 164 166 197 258 505 508`), que es el orden de la
pasada inicial `for (i…) alter_reg (i, -1)` de `reload1.c:839`.

> Es decir: **la ranura que sobra se reserva DESPUÉS de las 16**, en `fo = 120`, y mide 8 B.

---

## 2. El instrumento: una sonda que reserva `secondary_memlocs[modo]`

`get_secondary_mem` (`reload.c:667-733`) reserva **una ranura por MODO y por función**
(`secondary_memlocs[]` sólo se limpia en `reload1.c:665`, antes del bucle). Eso se puede
provocar desde C, y **su dirección lee `frame_offset` exacto**:

```c
{ float _p; __asm__ __volatile__("fsubs %0,%1,%1" : "=f"(_p) : "f"(alpha_rng));
            __asm__ __volatile__("" : "+r"(_p)); }     /* fuerza FPR -> GPR */
```

**Calibrado en una mini-TU** (`t2.cpp`, cflags de zEcstasy):

| función | sondas | marco | ranuras |
|---|---|---:|---|
| `pp1` | 1 × PS | 16 | una, `8(r1)` |
| `pp2` | 2 × PS | 16 | **la misma** |
| `pp3` | 3 × PS | 16 | **la misma** |
| `mix` | 1 × PS + 1 × SF | 24 | **dos**: PS en `8(r1)`, SF en `16(r1)` |

Una por modo, confirmado. **Aviso**: la sonda tiene que forzar de verdad el tránsito
FPR→GPR. `__asm__("":"+r"(hack_scale))` **no vale**: el valor ya estaba derramado en
`0x6c` y reload lo lee con un `lwz` de esa ranura (+1 instrucción, cero marco). Es una
sonda nula. De ahí el `"=f"` previo.

---

## 3. La medida que refuta la hipótesis

Sonda inyectada tras `sn_ps zero = __builtin_ps_sub(lc2b, lc2b);` (banco de **1,4 s**:
`EcstasyEx.cpp` suelto con `-include` de los `extern` de `testc0…teste3`; reproduce el
marco 0x178 y las 518 instrucciones de la unidad entera):

| variante | marco | insns | ranura nueva |
|---|---:|---:|---|
| base | 0x178 | 518 | — |
| **sonda SF** | **0x180** | 522 | `stfs/lwz` en **0x88** |
| **sonda PS** | **0x180** | 521 | `psq_st/lwz` en **0x88** |
| **sonda DF** | **0x180** | 523 | `stfd/lwz` en **0x88** + `lwz 0x8c` |
| sonda SF + sonda PS | 0x188 | 525 | SF en 0x88 (4 B) **y** PS en 0x90 |

Las tres estrenan ranura **en 0x88**, o sea `frame_offset = 128` **antes** de la sonda
(la SF alinea a 4: si `fo` hubiera sido 124 habría caído en 0x84, y si 120 en 0x80).

> **Por tanto: `secondary_memlocs[SF]`, `secondary_memlocs[PS]` y `secondary_memlocs[DF]`
> NO estaban reservados**, y la huérfana ocupa `fo` 120..127 — 8 B, alineada a 8.

### 3.1 Y `DImode` tampoco

Los únicos modos del cuerpo (censo del `.greg`): `SI 496`, `PS 418`, `CC 98`, `SF 92`,
`DF 15`, `QI 12`, `HI 6`, `DI 2`. `get_secondary_mem` ensancha lo que mida menos de una
palabra (QI/HI → SI), así que **de 8 B sólo hay `PS`, `DF` y `DI`**. Los dos primeros
están descartados arriba, y `DI` cae por construcción:

* las dos únicas apariciones de `DImode` son `(reg:DI 76 fpmem)` en los insns 2678 y 2679
  (`*fix_truncdfsi2_store` / `*fix_truncdfsi2_load`);
* en `config/rs6000/rs6000.md:5011` y `:5039` ese `(reg:DI 76)` está **escrito a fuego en
  el patrón, no es `match_operand`** (y en `:4990`/`:5000` sólo como `clobber`) → `find_reloads` no le empuja nunca una recarga;
* y `DImode` no cabe en `FLOAT_REGS` (`"f"` con `long long` da *impossible register
  constraint*), así que `SECONDARY_MEMORY_NEEDED(x, FLOAT_REGS, DI)` no puede darse.

**Control independiente**: quitando *las dos* conversiones int↔float (`*(int*)pt` y
`(int)t1`) desaparecen `DF` y `DI` de la función entera y **sigue habiendo 8 B huérfanos**
(sonda SF y sonda PS caen las dos en 0x90 = `fo` 136, con las ranuras reales acabando en
128). Con `DF`/`DI` inexistentes y `SF`/`PS` descartados por sonda, en esa variante la
huérfana **no puede** ser memoria secundaria.

> **VEDA (con su desigualdad).** La huérfana no es `secondary_memlocs[M]` para ningún
> `M`: para todo modo de 8 B presente en la función, o la sonda estrena ranura *por
> encima* de la huérfana (SF, PS, DF) o el modo no admite recarga (DI). Buscar la
> «sentencia con dos conversiones vivas» de la r47 es tiempo perdido.

---

## 4. Lo que sí es, con las líneas de `reload1.c`

Sólo quedan tres sitios que reserven pila después de la pasada inicial de `alter_reg`
(`caller-save` está descartado: la función **no tiene ni una llamada**):

* `reload1.c:921` — `assign_stack_local (BLKmode, 0, 0)`: sólo redondea a
  `BIGGEST_ALIGNMENT = 8`, y 120 ya es múltiplo de 8. **No crece.**
* `reload1.c:979` — ranura nueva para un pseudo cuya `reg_equiv_memory_loc` deja de ser
  direccionable. Queda referenciada.
* **`reload1.c:4012` — `finish_spills` → `alter_reg (i, reg_old_renumber[i])`** para los
  pseudos que reload echa a la calle **a mitad de camino**, cuando `find_reload_regs`
  requisa un registro duro como registro de recarga (`reload1.c:2368`, las líneas
  `Spilling reg N.` del volcado). Ésta es la única que cae **al final** del marco.

Y el que la deja sin dueño:

```c
/* reload1.c:6878-6883, en emit_reload_insns */
  /* If these are the only uses of the pseudo reg,
     pretend for GDB it lives in the reload reg we used.  */
  if (REG_N_DEATHS (REGNO (old)) == 1 && REG_N_SETS (REGNO (old)) == 1)
    { reg_renumber[REGNO (old)] = REGNO (reload_reg_rtx[j]);
      alter_reg (REGNO (old), -1); }
```

(y su gemelo en `delete_output_reload`, `reload1.c:8134-8136`). El pseudo **ya tenía
ranura**; reload decide que con un solo `set` y una sola muerte se queda en el propio
registro de recarga, le cambia `reg_renumber` y **la ranura no la referencia nadie**.

**La huella encaja con el volcado.** Registros duros requisados como registros de
recarga, contados en el `.greg` de los dos horarios:

| build | `Spilling reg` | registros FP requisados |
|---|---:|---|
| base (con `sched1`) | 118 | **f0 f3 f6 f8 f11 f12 f13** (7) |
| `-fno-schedule-insns` | 140 | f0 f12 f13 (3) |

`sched1` no aumenta el número de derrames — de hecho hay menos eventos — pero **mete cuatro
registros de coma flotante más en el juego de recarga**, y son justo los que alojan pseudos
`PS` de 8 B. El primer insn que fuerza cada uno (mapeado a línea con las notas del `.greg`):
f11 ← insn 1295 (~1797, `vl = ps_mul(vxy,vxy)`), f12 ← insn 2108 (~1298, el
`t0*pow_scale + pow_bias` de `my_fpow`), f8 ← insn 1810 (~1841, `ndotl2`), f6 y f3 ←
insn 1736 (~1833, `ssrg`/`ndotl01`).

Con `-fno-schedule-insns` el marco es 0x170 pero **el juego de derrames es otro**
(14 ranuras que acaban en `fo` 116, no las 16 del objetivo): no reproduce el objetivo, sólo
esquiva la huérfana. Y ya está medido que no se puede poner (r36d: 0 mejoras, 39 empeoran).

---

## 5. Negativos nuevos de esta ronda

* **El orden de las sentencias del bloque interno es INERTE.** Siete permutaciones
  (mover `rxy/rz` detrás de `ndotl2`, `dsba/ssrg/ssb` detrás de `ndotl2`, los dos `ndotl`
  antes de `rxy/rz`, `specvdotn/envvdotn` detrás de `rxy/rz`, `ndotl2` antes de `ndotl01`,
  `dsba/ssrg/ssb` antes de `rxy/rz`) dan **las 518 instrucciones, el marco 0x178 y las 16
  mismas ranuras**. `sched1` reconstruye el horario desde el grafo de dependencias y aquí
  no hay empates que el LUID pueda romper.
* `__asm__("":"+r"(x))` sobre una variable **ya derramada** no reserva nada (sonda nula).
  Cualquier sonda de memoria secundaria necesita forzar antes el valor a un FPR.
* Ablar **una sola** de las dos conversiones int↔float quita la huérfana… pero porque
  aparece un derrame real más (17 ranuras referenciadas, `fo` = 128). No es una vía.

---

## 6. Qué queda, y qué vale

* **49 de las 155 filas** del `fndiff` son literalmente el `+8` del marco. La cota de la
  r36d (~102 filas si el marco casa) queda confirmada por aritmética.
* La huérfana **no tiene palanca de fuente conocida**: la reserva depende de *qué registro
  duro requisa reload en un insn concreto* y de `REG_N_SETS==1 && REG_N_DEATHS==1` del
  pseudo que vivía ahí, dos cosas que **no aparecen en el código emitido**. Ninguna forma
  de fuente las nombra.
* **El siguiente paso acotado ya no es el marco.** Nuestras 16 ranuras y sus direcciones
  son **idénticas** a las del objetivo: el reparto *resultante* casa. Lo que no casa son
  las ~102 filas de código real, concentradas en 364-453 (75 filas, la región
  `my_fpow3`/`my_fpow`), donde el objetivo usa f12/f7 y nosotros f0/f5. **Si esas se
  arreglan, el juego de registros de recarga cambia y la huérfana se cae sola**; al revés
  no funciona, y esta ronda lo demuestra.

---

## 7. Método (reutilizable)

* **Banco de 1,4 s para `EcstasyEx.cpp` suelto**: `ngccc` con los cflags de zEcstasy más
  `-include` de un fichero con `extern float testc0[2], testc1[2], testl1..3, teste1..3;`
  (están definidos en `EcstasyE.cpp` y por eso la TU sola no compilaba). Los errores
  posteriores a la función (línea 3119) no impiden que se emita. Reproduce marco e
  instrucciones de la unidad entera (22 s → 1,4 s).
* **Leer `frame_offset` exacto** con la sonda SF: su dirección es `fo + 8`. Es el único
  instrumento del árbol que distingue «hay hueco de 4» de «hay hueco de 8» — el marco
  final los confunde porque `vars_size = RS6000_ALIGN(get_frame_size(), 8)`.
* **Reconstruir las ranuras con `assign_stack_local` a mano** (alineación del modo,
  `total_size = MAX(inherent, reg_max_ref_width)`, `align=-1` → `BIGGEST_ALIGNMENT`, y la
  corrección big-endian) antes de llamar «hueco» a nada.
* El `.greg` de este `cc1plus` **sí** trae la traza de reload (`Spilling for insn N.` /
  `Spilling reg R.`, `reload1.c:1946` y `:2368`) — la r52 escribió que no. Lo que no trae
  son las líneas «Reloads for insn»: `debug_reload_to_stream` (`reload.c:6686`) existe pero
  **sólo lo llama `debug_reload()` contra `stderr`**, que es un ayudante de gdb y no lo
  invoca ningún volcado. No es que `-da` no las escriba: **no se escriben nunca**.

## 8. `eProject`

**No tocada.** La veda de la r53 (§3) sigue en pie y su andamio `"+f"(halfVP2)` sigue
pagando 14 filas contra 23. Esta ronda se ha ido entera en `epCalculate`, que era el
encargo principal.

## 9. Limpieza

Volcados borrados de `scratchpad/rtl/` (`zEcstasy_cpp.*` y `EcstasyEx_cpp.*`, sólo los
míos; `f54cr_cpp.*` y `zWorld_cpp.*` intactos). El fichero de sonda
`src/.../EcstasyEx_probe54.cpp` lo crea y borra el banco en cada medida; comprobado que no
queda. Nada nuevo en `scripts/`.
