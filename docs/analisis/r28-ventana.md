# Ronda 28 — ventana: el frente real, cuantificado

## 0. Lo que vendí en las rondas 27 y 28, y que NO existe

Dije que había **~303.000 B de `.text` de crédito falso bloqueando la
promoción** de las SourceLists. **Es falso**, y lo desmontaron dos agentes por
separado el mismo día:

1. **Este `cc1plus` SÍ emite `.gnu.linkonce.t.*`.** Lo que lo apaga es
   **`-gdwarf+`** (cualquier `-g` ≥ 2: DWARF-1 necesita un `.text` contiguo).
   ProDG 3.5 y 3.7 lo emiten incluso con `-gdwarf+`; 3.8.1 y 3.9.3 no.
2. **Y da igual, porque `ngcld -strip-unused-data` hace ese mismo trabajo**:
   los mismos dos objetos dan 252 B sin la bandera y **184 B con ella**, y
   **184 B** con `linkonce`. Idéntico.
3. **El `.text` de más NO llega al DOL.** Sustituyendo nuestro `zLua.o` —con sus
   +18.908 B— el `.text` enlazado da **3.804.440 B, exactamente el del
   original**, y tres parejas A/B dan **el mismo sha1 con y sin el exceso**.
4. Y la pieza que lo cierra: **el original se compiló él mismo con `-gdwarf+`**
   (su volcado lleva los atributos GNU de DWARF-1), así que `linkonce` **no
   puede** ser como él deduplicaba: se quedó **una copia de cada una, todas
   dentro del rango de `zAI`**, la primera SourceList del enlace.

**El error fue mío y es de método: medí un delta de tamaño y lo llamé «crédito
falso» y «bloqueo» sin comprobar que llegara al DOL.** La comprobación que
faltaba era de una línea — sustituir el objeto en el enlace y mirar el sha1.

Las dos banderas quedan descartadas **con cifra**: `-fno-implement-inlines` quita
43.488 B pero **rompe 32 funciones y arregla 0**; `-fno-implicit-templates` quita
346.516 B (se pasa en 28.620) y rompe **1.207**.

## 1. EL FRENTE REAL, y es diez veces más pequeño

Lo que mueve el DOL es **`.rodata`/`.data`/`.sdata`**, y ahí las 32 SourceLists
**van CORTAS**: les faltan constantes que el original sí emite.

```
TOTAL .rodata:  faltan 32.472 B  ·  sobran 11.400 B  ·  neto -21.072
TOTAL .data  :  neto -6.004 B
```

Las peores, por `.rodata` que falta:

| unidad | d.rodata | d.data | d.text (irrelevante) |
|---|---|---|---|
| `zFe2` | **−4.792** | −580 | +13.388 |
| `zWorld` | **−3.952** | +148 | +11.660 |
| `zFe` | **−2.568** | −1.548 | +14.572 |
| `zGameplay` | **−2.368** | −344 | +13.616 |
| `zEAXSound` | −1.704 | −912 | +24.080 |
| `zPlatform` | −1.664 | −56 | +7.184 |
| `zMain` | −1.600 | −160 | +20.188 |
| `zTrack` | −1.520 | −480 | +5.008 |
| `zFoundation` | −1.320 | −220 | +348 |

**Eso es contenido que se puede escribir**: literales, tablas y constantes que el
original emite y nosotros no. Y `zFoundation` es el caso limpio para empezar —
**+348 B de `.text` y −1.320 de `.rodata`**: su código casi casa y lo que le
falta son datos.

**Para la ronda 29 la pregunta es: ¿qué símbolo de `.rodata` tiene el objetivo en
el rango de cada unidad que nosotros no emitimos?** El ELF original trae **577
símbolos `STT_FILE`** con sus locales detrás, que es exactamente esa atribución.

## 2. Herramientas nuevas en `scripts/`

- **`seccdiff.py`** — tamaño de cada sección contra el objeto extraído, **con la
  distinción SourceList/biblioteca**, que es la mitad del valor: en bibliotecas
  el exceso es inocuo (`mtx.c` promociona con +5.376 B y el DOL casa).
- **`extrasym.py`** — qué símbolos sobran, separados en **familia A** (existen en
  el original en otra unidad: instanciaciones duplicadas, 1.226 símbolos) y
  **familia B** (no existen en ningún sitio, 2.758).

## 3. Trampas de método de esta ronda

- **Comparar cuerpos de función en crudo miente**: hacía parecer que
  `-fno-implement-inlines` perturbaba **1.095** funciones cuando perturba **32**.
  **Los campos de `bl` y de reubicación los parchea el enlazador**: hay que
  taparlos antes de comparar.
- **El volcado DWARF tiene 59 funciones SIN CUERPO** (`ERROR: Failed to process
  tag … (GlobalSubroutine)`). **«No aparece en el volcado» no significa «el
  original no la tenía».**
- **`alloc.py` no puede decir nada de un pseudo que no llega a `global_alloc`.**
  En `TerrainVelocityNoise` sus cinco pseudos los asigna `local_alloc`, así que
  la tarea que yo serví no existía. Es la tercera corrección encadenada sobre esa
  misma función.
