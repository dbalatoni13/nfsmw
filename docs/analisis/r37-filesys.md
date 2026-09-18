# r37 — cierre de FILEOPERATION::AddToQueue

Fecha: 2026-09-07. Base: `590dc4a0`. Unidad:
`Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys`.

## Resultado comprobado

`AddToQueue__13FILEOPERATION` pasa de 252 B / 96,82539 % a **252 B / 100 %**.
Son **+252 B exactos y +1 funcion**, no una mejora parcial de fuzzy.
`filesys` queda con **74/74 funciones exactas, 10.872 B de codigo**.

La fuente conserva el algoritmo, los locks, las actualizaciones de la cola y
la senal. No se cambian cabeceras, flags, configuracion, splits ni ensamblador
de instrucciones. Tampoco se anade relleno o un valor sin inicializar.

## Causa y solucion

La base r36e ya tenia el tamano y flujo correctos. El unico desfase era el
intercambio del desplazamiento de prioridad con la carga de `pending.Head()`.
Mover sentencias no bastaba: el planificador conservaba libertad para volver
a intercambiarlas.

Se separa el calculo en valores reales inicializados y se expresa esta cadena
mediante dependencias vacias selectivas:

`cargar id -> cargar Head -> desplazar prioridad -> enmascarar id`.

```cpp
unsigned int newpriority = GetPriority();
int newid = GetId();
__asm__("" : "+m"(device->pending) : "r"(newid));
current = device->pending.Head();
__asm__("" : "+r"(newpriority) : "r"(current));
newpriority <<= 24;
__asm__("" : "+r"(newid) : "r"(newpriority));
newoppriority = newpriority | (newid & 0x00FFFF00);
```

El desplazamiento usa un entero sin signo. Los asm no emiten instrucciones ni
modifican realmente los operandos; evitan transformaciones concretas del
compilador. Se mantienen las dos dependencias sobre `current` heredadas de r36e
que preservan la comprobacion de nulo redundante del original.

## Ensayos acotados

| ensayo | tamano / fuzzy | observacion |
|---|---:|---|
| base | 252 / 96,82539 % | 2 instrucciones intercambiadas |
| C1: temporales, Head temprano y dependencia prioridad/Head | 252 / 97,77778 % | 5 instrucciones; cambia reparto r0/r9 |
| C2: C1 con prioridad fijada a r9 | 252 / 98,09524 % | quedan mascara/desplazamiento intercambiados |
| C3: desplazamiento separado y dependencia id/prioridad | 252 / 99,650795 % | quedan cargas de id/Head intercambiadas |
| C4: dependencia de `pending` respecto a id antes del Head | 252 / 100 % | cierre |
| C5: retirar el pin r9 de C4 | 252 / 100 % | **version retenida**, mismo codigo sin registro fijo |

Los ensayos C1-C3 se midieron sobre el objeto real; el JSON de trabajo se
sobrescribio entre ellos. Se conservan instantaneas nombradas de base, C4 y C5.

## Auditoria

- Compilacion directa de **una sola unidad**, con los flags del proyecto.
- `audit.py`: 74 funciones verificadas contra el ELF original. AddToQueue:
  252 B, 8 ramas, 12 referencias/reubicaciones y ningun literal.
- `scratchpad/codex_r37_filesys_audit.py`: inventario de 150 simbolos y 26
  secciones; las otras 73 funciones, datos, alineaciones y referencias no
  cambian. Se excluye solo la informacion de depuracion, cuyo tamano cambia
  legitimamente al introducir locales y lineas de fuente.
- La misma auditoria compara C4 con C5: codigo, datos y referencias identicos
  al retirar el pin.

## Linked sigue pendiente, no confundir con codigo exacto

`promote.py` detecta diferencias anteriores a esta ronda:

- `.rodata`: fuente 616 B frente a 520 B extraidos.
- `.sdata`: fuente 4 B frente a 8 B extraidos.
- Nueve simbolos en distinto desplazamiento; varias vtables estan +96 B.

`trypromo.py` confirma **DOL ROTO (21c5daf704f3...)** al sustituir solamente
filesys en un enlace aislado. Por tanto **no se propone marcarla Matching
todavia**. El ensayo no toca `configure.py`, `config/`, ni el `main.dol` del
proyecto. Primero hay que resolver la disposicion de datos con evidencia.

## Artefactos reproducibles

En `scratchpad/`:

- `codex_r37_filesys_before.json`
- `codex_r37_filesys_c4.json`
- `codex_r37_filesys_after.json`
- `codex_r37_filesys_audit.py`

Ejecutar la auditoria propia y despues `scripts/audit.py` con el nombre
completo de unidad de la cabecera. El censo global de la ronda se documenta
por separado; este informe no atribuye a este parche diferencias de
contabilidad entre `measure.py` y el informe oficial.
