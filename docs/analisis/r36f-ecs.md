# r36f — zEcstasy: `EmitterSystem::Render` cerrada al 100 %, y la palanca nueva es *retrasar*, no adelantar

Encargo: la cola de `zEcstasy`, empezando por `EmitterSystem::Render` (696 B a
dos instrucciones transpuestas), con el diagnóstico de la r36e escrito en la
fuente y la sugerencia de «quitar o fundir los dos `asm`».

**Resultado: `Render__13EmitterSystemP5eView` está al 100,0 %.** 696 B que ya
cuentan, y la unidad pasa de 5 funciones distintas a 4, de **5.876 a 5.180 B**
pendientes (`matched_code` 140.008 → **140.704** de 145.884). Era un muro de
siete rondas.

    python scripts/fndiff.py Speed/Indep/SourceLists/zEcstasy Render__13EmitterSystemP5eView
    # Render__13EmitterSystemP5eView  target=100.0%  ours=100.0%  size=696/696

`pctsnap --cmp` sobre las 539 funciones, contra el estado limpio de HEAD
(recompilado a propósito para tener el «antes» de verdad):

    EMPEORAN: ninguna
    MEJORAN: 1 funciones
        +1.218 pp  zEcstasy  Render__13EmitterSystemP5eView  98.7816 -> 100.0000  696 B

`fncmp` antes / después: 5 funciones y 5.876 B → **4 funciones y 5.180 B**.
`lcfix.py --check`: «todas las entradas @lc están al día». Cuatro ficheros
tocados, los cuatro en mi territorio: `EmitterSystem.cpp` (el cambio real) y
`EcstasyEx.cpp`, `eLightE.cpp`, `eMathE.cpp` (**sólo comentarios** con las vedas
nuevas; verificado con `pctsnap`, que sí ve un cambio de contenido a igual
tamaño). **Sin commit.** No he tocado `configure.py`, `config/GOWE69/*` ni
`splits.txt` — `git status --porcelain config/` sale vacío.

**~167 medidas nuevas**, todas en ejes que ninguna ronda anterior había tocado.

---

## 1. `EmitterSystem::Render`: la r36e miraba al lado equivocado del reloj

### 1.1 El diagnóstico heredado era exacto; la conclusión, no

La r36e dejó la función a 4 filas y una transposición, con el `.sched2` leído:

    t=12  858  %6=[%30+0xc]        <- lfs f6, world_size
    t=13  ready={861}              <- asm("" : : "f"(world_size))
    t=14  ready={977, 863}         <- el `ori r24,r24,2` y la barrera de ranura
    t=15  ready={878,881,884,...}  <- las OCHO cargas de xbasis

y concluía: «las cargas dependen de 863, así que no salen antes de t=15 y el
`ori` sale en t=14 pase lo que pase; para casar harían falta las dos cosas con
UN solo ciclo de `asm`». De ahí el encargo: **fundir o quitar los dos `asm`**.
Eso está medido y es un callejón (fundirlos da 27 filas / 84,99 %).

**La salida es la contraria y está en el mismo volcado.** No hay que ADELANTAR
las cargas: hay que **RETRASAR EL `ori` UN CICLO**. Si el `ori` deja de estar
listo en t=14 y pasa a t=15, los dos —`ori` y primera carga— compiten en el
mismo ciclo y `rank_for_schedule` emite antes la carga, que es **exactamente el
orden del objetivo**.

Y se consigue sin añadir un `asm`: **metiéndole el operando a la barrera que ya
está**.

```c
asm("" : "+m"(xbasis), "+r"(sprite_hack_flags));
```

| variante | fuzzy | filas | eq | tamaño |
|---|---|---|---|---|
| base r36e | 98,781610 | 4 | 171 | 696/696 |
| `+ "+r"(sprite_hack_flags)` | 99,540230 | 14 | 160 | 696/696 |

Las 14 filas **asustan y son la señal buena**: el horario ya es el del objetivo
(`lfs f6` · `lfs f9,0x98(r1)` · `ori` · `lfs f8` · `lfs f7`, fila por fila) y lo
único que queda es una **rotación limpia de cuatro registros**:

| | objetivo | nuestro |
|---|---|---|
| `sprite_hack_flags` | r24 | r27 |
| iterador `grp` | r25 | r24 |
| centinela de la lista de emisores | r26 | r25 |
| `&vposition` (dirección de pila) | r27 | r26 |

Las tres formas de crear la dependencia dan **la misma medida exacta**
(99,540230 % / 14 filas / eq 160 / 696 B): `"+r"(sprite_hack_flags)`
(dependencia de dato), `: "r"(sprite_hack_flags)` (anti-dependencia, WAR sobre
el `ori`, que reescribe r24) y la misma con `"+m"(ybasis)` en vez de `xbasis`. Añadir un `asm` aparte para lo mismo da 23 filas: **tiene que
ser un operando de la barrera que ya existe**, porque un `asm` más gasta otro
ciclo entero.

### 1.2 La regla de cardinalidad, y con ella el 100 %

De los cuatro registros de la rotación, `sprite_hack_flags` es **el único
pinchable** (los otros tres son una dirección de pila, el centinela y el
iterador). La regla del brief —«ante una rotación de N registros, pinchar UNO
arrastra a los demás»— funciona al primer intento:

```c
register unsigned int sprite_hack_flags asm("r24") = 0;
```

| pin | fuzzy | filas | eq | tamaño |
|---|---|---|---|---|
| **r24** | **100,000000** | **0** | **174** | **696/696** |
| r25 | 99,712650 | 8 | 166 | 696/696 |
| r26 | 99,626434 | 11 | 163 | 696/696 |
| r27 (= sin pin) | 99,540230 | 14 | 160 | 696/696 |

La curva es monótona en el sentido de la rotación: cuanto más lejos del registro
del objetivo, más filas. **Y el pin SOLO, sin la dependencia, da los 4 de la
base**: es el orden del brief («si el pin empeora, es síntoma: arregla antes la
primera diferencia que no sea de registro») en su versión positiva — el pin no
hace nada hasta que el horario está bien.

### 1.3 Las seis piezas y ninguna sobra

Medido quitando una cada vez sobre el ganador:

| se quita | filas | tamaño |
|---|---|---|
| — (ganador) | **0** | 696/696 |
| el `"+r"(sprite_hack_flags)` | 4 | 696/696 |
| el pin `r24` | 14 | 696/696 |
| la barrera de ranura `"+m"(xbasis)` | 13 | 696/696 |
| la barrera de `world_size` | 9 | 696/696 |
| el pin `fr6` de `world_size` | 31 | 696/696 |
| la cantidad fantasma en r19 | 25 | **688**/696 |

Seis constructos apilados de cinco rondas distintas, y el último vale 4 filas.

---

## 2. Reglas nuevas que se lleva el árbol

1. **Cuando dos insns compiten por el mismo ciclo, se puede atacar por los dos
   lados, y el barato es RETRASAR la que va pronto**, no adelantar la que va
   tarde. Se hace metiendo un operando (entrada o salida, da igual) en un `asm`
   **que ya esté** en el bloque: cero bytes y cero ciclos nuevos. Es la primera
   vez que el proyecto gana un desempate de `sched2` sin gastar una ranura.
2. **Un `asm` nuevo cuesta un ciclo; un operando más en un `asm` que ya está, no.**
   La misma dependencia puesta en un `asm` aparte da 23 filas donde el operando
   da 14.
3. **Un salto de filas HACIA ARRIBA después de arreglar el horario es la señal
   buena, no el fracaso**: 4 → 14 filas fue el paso previo al 100 %. Lo que hay
   que mirar no es el número sino de qué TIPO son las filas nuevas (aquí, todas
   de registro y en una rotación cerrada).
4. **La rotación se cierra pinchando el único miembro pinchable**, y el resto —
   direcciones de pila, iteradores, centinelas — se arrastra solo. Confirma la
   cardinalidad del brief con una rotación de cuatro (la del brief era de N
   genérico).
5. **La cantidad fantasma se puede descartar SIN COMPILAR**: si el censo de
   registros de los dos listados es idéntico, no hay hueco asimétrico y la
   palanca no tiene precondición. Ahorró 18 compilaciones en
   `GenerateHorizonFog`.
6. **`insn:TI` en el volcado `-dR` marca el comienzo de ciclo.** Dos insns
   consecutivas donde la primera lleva `:TI` y la segunda no están en el MISMO
   ciclo, y entonces el orden lo decide sólo `INSN_PRIORITY`. Es una lectura
   directa, sin necesidad de la visualización por ciclos.

---

## 3. `GenerateHorizonFogDisplayList` (796 B, 2 filas): la palanca nueva NO vale, y por dos razones medidas

Lo primero que hice al cerrar `Render` fue traerle a esta función la palanca
nueva, porque es el mismo tipo de problema (dos insns en el mismo ciclo,
`srawi` = `i/2` con prioridad 12 contra `andi.` = `i&1` con prioridad 2).
**No se puede**, y las dos razones son estructurales:

**(a) El `srawi` no se puede retrasar sin emitir un `mr`.** Escribe r0 y su
entrada es el `add` intermedio de la división con signo (`srwi`, `add`,
`srawi`), que **no tiene nombre en C**. La anti-dependencia que ganó en `Render`
—`asm("" : "+r"(V) : "r"(multiple))` delante de la división— sí mueve código
pero destroza el reparto:

| variante | fuzzy | filas | tamaño |
|---|---|---|---|
| base | 98,994970 | 2 | 796/796 |
| `"+r"(current_index) : "r"(multiple)` | 96,678390 | 84 | 796 |
| `"+r"(color_base) : "r"(multiple)` | 96,904526 | 79 | 796 |
| `"+r"(verts_per_strip) : "r"(multiple)` | 95,597990 | 82 | **804** |
| volátil `asm("" : : "r"(multiple))` | 96,829150 | 82 | 796 |
| control `"+r"(current_index)` solo | 97,437190 | 25 | 796 |
| `"+f"(grid_pointX) : "r"(multiple)` | — | 2 | **objeto idéntico** (DCE) |
| `"+r"(multiple)` delante | — | 2 | **objeto idéntico** (DCE) |
| `int k = i; asm("":"+r"(k)); multiple = k/2;` | 98,040200 | 8 | **800** |

**(b) Subir `prio(andi.)` con una cadena de `asm` que SÍ dependa de él rompe la
fusión de `combine`.** La r36e colgó la cadena de un *fantasma* —que no es
dependiente de nada, y por eso 1, 2, 4 y 8 eslabones daban el mismo objeto—;
`priority()` de haifa propaga por `INSN_DEPEND`, así que la cadena tiene que
colgar del VALOR `i & 1`. Pero materializar `int odd = i & 1;` y darle un uso
más impide que `combine` funda el AND con la comparación:

    objetivo   andi. r11, r31, 0x1                    (una insn)
    con cadena clrlwi r0, r31, 31  +  cmpwi r0, 0x0   (dos)

| cadena | fuzzy | filas | tamaño |
|---|---|---|---|
| `int odd = i & 1;` sin cadena (control) | 98,994970 | 2 | **objeto idéntico** |
| 1 / 2 / 4 eslabones | 98,190956 | 4 | **800** |
| 8 | 97,989950 | 9 | 800 |
| 12 / 16 / 24 | 97,286430 | 10 | 800 |

La saturación en 12 confirma que la propagación existe (a diferencia de la
cadena colgada del fantasma), pero llega tarde y ya cuesta 4 B.

**La cantidad fantasma queda descartada por PRECONDICIÓN, sin compilar**: el
censo de registros de los dos listados es idéntico —r15..r31 usados con hueco
sólo en r14, y f0-f3, f11-f13, f20-f31 en los dos—, o sea que no hay hueco
asimétrico que regalar.

**Y el mecanismo queda probado por RTL, no por la visualización**: en el volcado
`-dR` la insn 216 (`ashiftrt`, el `srawi`) lleva `insn:TI` y la 231 (el
`extzvsi_internal1`, el `andi.`) **no**, o sea que las dos se emiten en el MISMO
ciclo y lo único que las ordena es `INSN_PRIORITY` (12 contra 2). Con
`INSN_DEPEND(231) = {el salto}` esa prioridad no puede subir sin añadir código.
**18 medidas nuevas**, todas anotadas en `EcstasyEx.cpp`.

---

## 4. `UpdatePlatInfo` (2.044 B, 28 filas): 86 medidas en los dos ejes que faltaban, todas negativas

El encargo mandaba atacar el SOLAPE: «`diffuse_rng_a` no puede coger f5 porque
f5 está ocupado mientras vive, y en el objetivo esos dos rangos NO se solapan».
Hecho, por cuatro vías nuevas.

**4.1. Pines LIMPIOS del ciclo de tres, uno solo y sobre la base sin combinar.**
El brief avisa de que de medir pines encima de otra cosa salió una veda falsa;
la r36c/d/e siempre los midió combinados. Los nueve solos:

| pin | fr1 | fr5 | fr31 |
|---|---|---|---|
| `diffuse_rng_a` | 38 | 96 (2.040 B) | 32 |
| `envmap_min_g` | 82 | 247 (2.052 B) | 68 |
| `envmap_max_g` | 69 | 119 | 67 |

Ninguno baja de 28. La veda no era falsa.

**4.2. Nuestro propio pin es quien ocupa f5 — y quitarlo tampoco vale.** La
fuente lleva `register float envmap_min_scale asm("fr5")` desde la r36b, y un
registro duro conflictúa con todo pseudo vivo a su través: **por ahí se le
prohíbe f5 a `diffuse_rng_a`**. Nadie había barrido el registro de ESE pin.
Barrido completo (17) más «quitarlo y pinchar `rng_a` en su sitio» (10):

| pin de `envmap_min_scale` | fr5 (base) | fr4 | fr31 | fr3 | fr0 | fr6 | fr1/fr2 | fr7 | fr8 | fr9-12 | sin pin |
|---|---|---|---|---|---|---|---|---|---|---|---|
| filas | **28** | 35 | 35 | 39 | 43 | 42 | 46 | 55 | 60 | 74-83 | 60 |

Y el dato que cierra la vía: **en las 27 variantes `diffuse_rng_a` NO llega
nunca a f5** — se queda en f1 o f2 —, medido con una sonda directa sobre el
`fsubs fX, f24, f7` del objeto. El pin de fr5 no es el bloqueo; es el mejor
reparto disponible.

**4.3. Atar el `fsubs` por debajo del bloque envmap.** Bajar la sentencia sola
da **objeto idéntico** (reconfirma que `sched1` la vuelve a subir), y las
barreras de PLANIFICACIÓN que sí cortan a `sched1` lo destrozan: `asm volatile`
delante 288 filas, detrás 289, tras el bloque envmap 133; con barrera selectiva
sobre el operando, 30 y 75; con el pin `fr5` de `rng_a` encima, de 114 a 268.
16 medidas.

**4.4. SEGUNDA SALIDA en los tres `asm` que ya están** —la forma exacta que
cerró `Render`; la r36e sólo barrió ENTRADAS—. `"+f"(envmap_power), "+f"(V)`
×1/×3 sobre 10 locales, 21 medidas: la mejor es 28 (idéntica a la base, con
`diffuse_min_a`) y el resto va de 31 a 161.

**Lo único que baja de 28 y NO lo aplico.** Mover `envmap_power = 6.0f;` dentro
del `case 0x68E97F75` da **27 filas / 99,344420 %**. Lo que arregla es una fila
del bloque de literales por casualidad, y **reordena el pool de constantes**
—cambia la numeración `$LC`—, que es justo lo que el brief prohíbe hacer sin
pasar por `keep.lst`. Queda anotado en la fuente.

**Sobre la única diferencia que no es de registro**, para quien siga: está en un
solo bloque básico, el del `case 0x68E97F75...0xDD90E320` (tres constantes y
nada más):

    objetivo  lis r11,6.0 | lfs f2,(r11) | lis r9,0.22 | lis r11,0.86 | lfs f7 | lfs f5
    nuestro   lis r9,6.0  | lis r11,0.22 | lis r10,0.86 | lfs f2 | lfs f7 | lfs f1

El objetivo **reutiliza r11** porque carga la primera constante en cuanto tiene
su `@ha`; nosotros levantamos los tres `@ha` primero y gastamos tres GPR. Las
otras ~21 filas son el ciclo f1→f31→f5→f1 y nada más.

---

## 5. `eProject` (268 B, 14 filas): 33 medidas con la receta de `Render`

La receta que cerró `Render` dice: arregla PRIMERO la diferencia que no es de
registro. Aquí esa diferencia está localizada — filas 33-38: el objetivo hace
`lfs vp[0]` + `fadds` (el `vp[0] + halfVP2` de `*sx`) **antes** del `fmadds` de
`clipY`, y nosotros al revés. Nada la mueve:

- **Segundo operando en el `asm` que ya está** (la forma ganadora en `Render`):
  `"+f"(halfVP2), "+f"(V)` y `"+f"(halfVP2) : "f"(V)` sobre `halfVP3`, `clipX`,
  `clipY`, `clipZ` y `oneOverW`, ×1 y ×2 — 16 medidas, de **20 a 44 filas**, y
  cinco además engordan a 272 B. La mejor (entrada `oneOverW`) da 20.
- **Barrera de RANURA (`"+m"`)**, la cuarta familia, que la r36d no probó aquí,
  y barreras sueltas ENTRE `*sx` y `*sy` y DELANTE de `*sx` — 14 medidas:
  `"+m"(*sx)` 18/42, `"+m"(*sy)` 24/47, `"+f"(oneOverW)` 26, `"+f"(clipY)` 24
  (272 B); `"+f"(halfVP3)`, `"+f"(clipZ)` y el clobber de `r0` salen
  **idénticos** en los dos sitios (absorbidos).
- **Local propia para `vp[0]`/`vp[1]` con barrera** (la regla del brief sobre la
  local): 19 filas y objeto idéntico.

Nada baja de 14. Anotado en `eMathE.cpp`.

---

## 6. `epCalculateLocalDirectionalPOS16`: no la he tocado

El encargo la daba por acotada con 447 medidas de la r36d y así se queda.

---

## 7. Método y herramientas

- **Banco por fichero sin tocar el árbol** (`e36_var.py` + `e36_es.py` de la
  r36e, reutilizados tal cual; nuevos `f36_hf.py` para `EcstasyEx.cpp` y
  `f36_up.py`/`f36_ep.py` para `eLightE.cpp`/`eMathE.cpp`). Tiempos:
  `eLightE.cpp` **1,7 s**, `eMathE.cpp` 1,6 s, `EcstasyEx.cpp` 12 s,
  `EmitterSystem.cpp` 8 s. **Hay que lanzarlos con `cwd` en la raíz**:
  `build_direct.parse_units()` abre `build.ninja` por ruta relativa.
- `f36_sch.py`: vuelca el `.sched2` (`-dR -fsched-verbose=5`) de cualquier banco
  al scratchpad. Confirmado el aviso de la r36e: **el compilador termina con
  «Internal compiler error» DESPUÉS de volcar**, hay que ignorar el rc. El
  dump de esta unidad son 17 MB; la parte útil se corta por `;; Function`.
- `f36_up5.py`: **sonda de reparto** — en vez de leer 28 filas de diff, extrae
  del objeto qué registro lleva cada variable buscando su instrucción por los
  operandos que NO cambian (`fsubs fX, f24, f7`). Convierte un barrido de 27
  variantes en una tabla de una línea por variante.
- Verificación: `pctsnap` sobre las 539 funciones con el «antes» **recompilado
  desde HEAD a propósito** (`git checkout` del fichero, build, snapshot,
  restaurar, build, snapshot). Sin eso el «antes» habría sido el estado ya
  parcheado y la comparación no habría dicho nada.

## 8. Aviso para quien reparta

`lcfix.py` **no acepta unidad**: mira `keep.lst` entero y el argumento sobra
(`solo_check = '--check' in sys.argv`, así que `--check` se respeta igual). O
sea que `python scripts/lcfix.py zEcstasy --check` puede sacar desfases de
OTRAS unidades y parecer que son tuyos — a mí me sacó dos de `zTrack`, de otro
agente, que desaparecieron en cuanto ese agente recompiló. Lo que zanja de quién
es el desfase no es el nombre de la unidad sino
`git status --porcelain config/`, que aquí sale **vacío**.
