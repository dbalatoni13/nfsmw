# r64c -- Censo de `.o` rancios entre las unidades promocionadas

Encargo: si el `.o` de `zSim` llevaba rondas rancio y tapaba una regresion,
**cuantas de las 525 unidades promocionadas estan igual?**

Respuesta corta: **525 medidas, 0 rancias.** `zSim` era la unica, y a la hora de
cerrar este censo ya estaba arreglada por el agente que iba en paralelo.
La conclusion util no es el cero: es **por que ninguno de los controles de hoy
podia haberlo cazado**, y cual si puede (1 min 36 s).

---

## 1. Metodo

Ninguna medida toca el arbol. Para cada unidad promocionada:

1. Se saca su edge de `build.ninja` con `build_direct.parse_units()` --los
   mismos cflags, el mismo toolchain, el mismo `section_rename`.
2. Se **recompila a un directorio del scratchpad**, no encima del `.o`. Asi no
   hay que guardar copias ni restaurar nada, y `build_direct.py` no puede
   mentir dejando el `.o` viejo: el fichero que comparo lo acabo de crear yo.
3. Se comparan los **digests por seccion** de los dos ELF (lector propio de
   ELF32-BE), no el sha1 del fichero. Se ignoran `.line`, `.debug*`,
   `.comment`, `.stab*` y `.note`: cambian por cosas que no llegan al DOL.
   Se comparan si `.text`, `.rodata`, `.data`, `.sdata*`, `.ctors`, **sus
   `.rela.*`** y `.symtab`.

Herramienta instalada: **`scripts/censorancios.py`**
(`python scripts/censorancios.py` = las 525; acepta `SL`, `NOSL` o nombres
sueltos; deja el detalle en `build/GOWE69/censorancios_all.json`).

### Las tres unidades de ensamblador

`crt0`, `metrotrk` y `pro2D4` no salen de `parse_units()`: su regla es `as`, no
`prodg`. El script las trata aparte (`powerpc-eabi-as` + `dtk elf fixup`) para
que el censo sea de 525 y no de 522.

---

## 2. Controles (la medida vale lo que valen sus controles)

**Falsos positivos -- ninguno.** Las 525 unidades recompiladas a una ruta
distinta salieron **identicas byte a byte, incluidas `.line` y `.debug`**. O
sea: el compilado es determinista y la ruta de salida no entra en el `.o`. Si
el comparador hubiera sido ruidoso, se habria visto aqui.

**Falsos negativos -- el comparador ve las diferencias.** Control con `zDebug`
recompilada con `-O2` en vez de `-O1`:

```
CONTROL -O2: no-debug distintas = ['.rela.debug', '.rela.debug_pubnames',
                                   '.rela.debug_srcinfo', '.rela.text',
                                   '.symtab', '.text']
```

**Repeticion.** Tres pasadas completas independientes (SL+NOSL por separado,
luego dos completas), las tres con 0 rancias.

**El censo no se lo debe a la reconstruccion de hoy.** A las 00:52-01:02 se
reconstruyeron 78 de las promocionadas. Las otras **443 no se tocaron hoy** y
aun asi recompilan identicas; la mas antigua es `mtdecppc`, del **7 de
septiembre** --cuatro dias en disco sin desviarse de su fuente.

| edad del `.o` promocionado | unidades |
|---|---|
| reconstruido hoy (00:52-01:02) | 78 |
| 1-3 dias | 77 |
| 3-10 dias | 366 |
| **no reconstruido hoy** | **443** |

---

## 3. Resultado

```
promocionadas en el enlace : 525
medidas                    : 525   (522 prodg/mwcc + 3 asm)
RANCIAS                    :   0
no compilan                :   0
sin .o en disco            :   0
coste                      : 1 min 36 s de pared (6 hilos)
```

`zSim` entra en el censo y sale **ok**: su `.o` de las 01:19 corresponde a la
`QuickGame.cpp` que hay ahora en disco. Y el enlace base relanzado desde los
objetos de disco a las 01:20 da:

```
DOL relink AHORA  : 9619ba57c9919f95f7f2ac951a2166a3517f91e3
referencia        : 9619ba57c9919f95f7f2ac951a2166a3517f91e3
VEREDICTO: OK
```

Como no hay ninguna rancia, la pregunta "la version nueva rompe el DOL o solo
cambia?" no tiene sujeto. El unico caso era `zSim`, y ya esta cerrado.

**Caveat de fecha, porque el censo es una foto.** El agente de `zSim` seguia
trabajando mientras yo medía: `QuickGame.cpp` sigue modificada en el working
tree. La foto de `zSim` vale para las 01:26; si ese agente ha vuelto a editar
la fuente despues, `zSim` estara rancia otra vez --y es lo normal a mitad de
trabajo. Las otras 524 no las tocaba nadie. Un censo es valido hasta la
siguiente compilacion: por eso la recomendacion es correrlo **al cerrar la
ronda**, no al abrirla.

**Cobertura declarada, sin adornos.** Se han medido las **525 promocionadas**,
que es el 100 % de lo que el encargo pedia. **NO** se han medido los **94
objetos extraidos** (`build/GOWE69/obj/*.o`) que completan las 619 entradas del
enlace: salen de la extraccion y la regla 1 prohibia re-extraer. Es un hueco
real y esta abierto (ver seccion 5).

---

## 4. Por que nadie lo vio: el grafo de build no existe

Esto es lo que de verdad hay que llevarse.

- **`ninja` no corre desde el 9 de septiembre.** `.ninja_log` termina el
  `Sep 9 21:54`. Todo lo posterior lo ha compilado `build_direct.py`.
- **`build_direct.py` no mira fechas.** Compila lo que le nombras y nada mas.
  Si tocas una cabecera que comparten diez unidades y recompilas una, las otras
  nueve quedan rancias y **nada** en el pipeline se entera.
- **No hay depfiles.** De los 548 `.o` de `build/GOWE69/src` solo **3** tienen
  su `.d` al lado --y son justo las tres de ensamblador, que se hicieron con
  `as -MD`. Las reglas de `build.ninja` piden `depfile = $out.d`, pero como
  ninja no corre, los `.d` no se generan. El grafo de dependencias esta vacio.
- **Y hay una segunda trampa, mas gorda que la de los `.o`:**

```
build/GOWE69/main.dol   mtime Sep  9 21:54   sha1 9619ba57...  <-- "DOL OK"
build/GOWE69/main.elf   mtime Sep  9 21:54
```

  **El `main.dol` del arbol es un fichero de hace dos dias.** Cualquier control
  que compruebe "el DOL base" haciendole `sha1sum` a ese fichero dara `DOL OK`
  para siempre, pase lo que pase con los objetos. Durante toda la ventana en
  que `zSim` estuvo roto hoy, ese fichero seguia diciendo `9619ba57`. El unico
  veredicto valido es **relanzar el enlace** desde la lista de objetos de
  `build.ninja` (lo que hace `trypromo.py`).

Y por eso los cuatro controles de hoy no podian cazarlo:

| control | que mira | por que se le escapo |
|---|---|---|
| `lcfix.py` | valor de los `$LC` con directiva `@lcsrc` | solo ve las unidades **anotadas**; y de hecho **si** lo canto, pero canta sobre la fuente, no sobre el `.o` |
| `checksplits.py` | coherencia de `splits.txt` | no toca objetos |
| `prefijochk.py` | prefijo de la TU | no toca objetos |
| DOL base | el DOL | correcto **si se relinka**; ciego si se lee `build/GOWE69/main.dol` |

Ninguno compara `.o` contra fuente. Esa casilla estaba vacia.

---

## 5. Recomendacion de protocolo

**Al cerrar cada ronda, antes de promocionar y antes del veredicto del DOL:**

1. **`python scripts/censorancios.py`** -- 1 min 36 s, 525 unidades. Si sale
   algo distinto de `RANCIO 0`, la lectura de `DOL OK` de esa ronda **no vale**:
   hay al menos un `.o` que no corresponde a su fuente. Es el control que
   faltaba y es barato: cuesta menos que un solo `zMain`.
2. **El DOL base, siempre relinkando.** Prohibir el `sha1sum
   build/GOWE69/main.dol`. El juez es el enlace desde la lista de objetos de
   `build.ninja`; `trypromo.py` ya lo hace bien y el mismo codigo sirve para el
   base sin sustituciones.
3. **Barato y complementario:** al acabar una tanda, recompilar **todas** las
   unidades que incluyan una cabecera tocada en la tanda, no solo la que el
   agente estaba mirando. Mientras no vuelva a correr `ninja`, esa lista hay
   que sacarla a mano --o dejar que `censorancios.py` la encuentre a posteriori,
   que es mas fiable.
4. **Hueco abierto:** los 94 `build/GOWE69/obj/*.o` extraidos no los verifica
   nadie. Un control equivalente seria re-extraer a un temporal y comparar
   secciones; no se ha hecho aqui porque el encargo lo prohibia.

### El diagnostico de fondo

`zSim` no se rompio por un `.o` rancio. Se rompio por el `$LC526`, y `lcfix`
lo estaba cantando. Lo que hizo el `.o` rancio fue **tapar la rotura durante
varias rondas** haciendo que el enlace diera `DOL OK`. Es decir: la rancidez no
es una causa de regresiones, es un **anestesico**. Por eso el control tiene que
ir *antes* del DOL, no despues: sin el, el DOL no es un juez, es un testigo al
que le han cambiado las pruebas.

---

## 6. Estado del arbol al cerrar

Sin modificaciones. El censo compila siempre a un temporal, no se guardo ni
restauro ningun `.o`, y ningun fichero de `build/GOWE69/src` cambio de fecha
por mi (las de las 00:52-01:02 son las 78 de la reconstruccion previa; la de
las 01:19 es `zSim`, del otro agente). Lo unico que se anade al arbol es
`scripts/censorancios.py` --fichero nuevo, sin tracking, como el resto de
`scripts/agent_*.py`-- y los `build/GOWE69/censorancios_*.json`, que caen en
`build/` y estan ignorados.
