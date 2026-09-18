# r40 - CarLoader: retorno ABI y orden de argumentos

2026-09-07. Territorio exclusivo de coordinacion:
`src/Speed/Indep/Src/World/CarLoader.cpp`, unidad `zWorld`.
Leidos el brief r36f, r37-world, r37-root-ensayos y el cierre r39.

## Resultado

**No se retiene ningun cambio.** `SetMemoryPoolSize__9CarLoaderi` conserva
304 B / 97,36842 %, con el par `stw` / `addi` intercambiado. CarLoader queda
restaurado byte por byte, incluido CRLF; no se toco TrackStreamer ni su cambio
local previo. Tampoco se reabrieron los ciclos de `DefragmentPool`.

## Hipotesis nueva y ensayos

La combinacion r39 de argumento ABI real y dependencia vacia sugeria atacar
el retorno de `bGetFreeMemoryPoolNum`: r3 es a la vez el retorno y el primer
argumento de `bInitMemoryPool`. La base materializa ademas una copia en r0
para el store global. Los pines de esta tanda no cruzan ninguna llamada entre
su definicion y su ultimo uso; todos los valores estan inicializados. No se
introdujo ASM con instrucciones ni cantidades fantasma.

| ensayo | fuente respecto al anterior | medida |
|---|---|---:|
| C1 | retorno real `pool_num` fijado a r3, dependencia `+r`, store global y argumento usan ese valor | 300 B / 98,61842 % |
| C2 | copia real `stored_pool_num` a r0; segunda dependencia `+r(stored):r(pool_num)` | 304 B / 94,73684 % |
| C3 | fusionar ambas dependencias en `+r(stored),+r(pool_num)` | 304 B / 97,36842 % |
| C4 | C3 mas nombre local Cars y `+m(global):r(nombre)` antes del store | 304 B / 97,36842 % |
| C5 | C4 cambia esa ultima salida por `+r(stored):r(nombre)` | 308 B / 95,26316 % |
| C6 | C5 mas nombre fijado a r6, locales de memoria/tamano y dependencias de esos argumentos | 304 B / 94,73684 % |
| C7 | C6 añade `+m(MemoryPoolSize)` a la primera dependencia de retorno/copia | 304 B / 97,210526 % |

C1 corrige el orden `addi`/`stw`, pero elimina `mr r0,r3` y guarda desde r3:
no es un cierre aunque suba la similitud. C2 conserva la copia, pero adelanta
la carga de tamano y vuelve a invertir el store. C3 recupera exactamente las
instrucciones de la base. C4 sigue siendo neutra: no constituye evidencia de
una nueva dependencia efectiva. C5 hace efectivo el orden nombre/store, a
costa de materializar el nombre en r9 y añadir `mr r6,r9`.

La cadena adicional C6 es, tras la primera dependencia de C3:

```cpp
int pool_size = this->MemoryPoolSize;
void *pool_mem = this->MemoryPoolMem;
asm("" : "+r"(pool_mem) : "r"(pool_size));
register const char *pool_name asm("r6") = "Cars";
asm("" : "+r"(pool_name) : "r"(pool_mem), "r"(pool_size));
asm("" : "+r"(stored_pool_num) : "r"(pool_name));
CarLoaderMemoryPoolNumber = stored_pool_num;
bInitMemoryPool(pool_num, pool_mem, pool_size, pool_name);
```

C6 evita la instruccion extra, pero intercala `lis` entre las dos cargas y
retrasa `mr r0,r3` hasta justo antes del store. C7 recupera el `mr` temprano,
pero intercambia las cargas de memoria/tamano y conserva el `lis` intercalado.
Es la misma clase de efecto que r37-R4: los pines ABI no resuelven el conflicto
de scheduling al combinarlo con estas dependencias. No repetir C1-C7 como un
nuevo barrido. Hace falta evidencia RTL adicional para controlar el `lo_sum`
sin cambiar la prioridad de las otras preparaciones.

## Verificacion

- Compilacion directa solo de `build/GOWE69/src/Speed/Indep/SourceLists/zWorld.o`.
- `fndiff` final: 304 B / 97,36842 %; ningun ensayo parcial queda aplicado.
- `codex_r40_world_audit.py`: JSON COMPLETO anterior/posterior identico, ambos
  lados incluidos; 2.883 simbolos fuente, 23 secciones, datos, instrucciones,
  alineaciones, reubicaciones y metadatos de comparacion sin cambios.
- 582 funciones medidas, las mismas 577 exactas; auditor estandar correcto.
- Fuente y copia inicial con SHA-1 identico:
  `a4ae1631423519dceccf5bf1aebbd7bd1f42cb0f`.
- `lcfix.py zWorld --check` correcto; diff de CarLoader vacio.

Artefactos: `scratchpad/codex_r40_world_{before,after,c1,c2,c3,c4,c5,c6,c7}.json`,
`scratchpad/codex_r40_CarLoader_before.cpp` y
`scratchpad/codex_r40_world_audit.py`.
