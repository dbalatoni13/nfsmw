# Iter3, 2026-09-08: vmbase cerrada, +380 B exactos

Base HEAD `8853122e`, fuente vigente copiada byte a byte en
`scratchpad/codex_20260908_iter3_vmbase/baseline.c`. Se conserva la mezcla
historica de saltos de linea fuera del cuerpo y de la nueva nota de cierre.
No se modifica ninguna otra funcion, cabecera compartida, configuracion o split.

## Cierre

`__VMBASESetupExceptionHandlers`: **380 B, 95 instrucciones, 100 %**.
La unidad pasa de 29/30 a **30/30 funciones exactas, 3.180 B**. El incremento
de codigo matched es solo 380 B; el linked no cambia por cerrar instrucciones.

Los cuatro desplazamientos branch1..4 ya estaban bien reconstruidos. El
problema restante era la cadena que fabrica las instrucciones de salto: el
ELF usa r0, pero las formas C agotadas asignaban r5/r6. MW admite productores
inline reales sin sustituir la funcion entera por ensamblador:

- Saltos de retorno: `neg r0,branchN; clrlwi r0,r0,6; oris r0,r0,0x4800;
  stw r0,0(p)`. Tanto branchN como p estan definidos en C antes del bloque.
  Equivale exactamente a `0x48000000 | ((-branchN) & 0x03ffffff)` y su store.
- Saltos de entrada: `oris r0,branchN,0x4800; mr instruction,r0`, con
  `instruction` declarada register. MW fusiona el mr con la salida: no emite
  ninguna copia adicional. Los dos stores a los vectores siguen escritos en C.

No hay cantidades sin inicializar, consumidores inventados, ASM vacio ni
metadatos que oculten diferencias. Los flushes, sync/isync/icbi, carga de la
instruccion original y restauracion del contexto conservan su orden y bytes.
Los handlers de excepcion preexistentes en ASM no se tocaron.

## Tres pruebas privadas, sin volver a las permutaciones agotadas

| copia | tamano | similitud | filas distintas |
|---|---:|---:|---:|
| baseline | 380 | 99,1579 % | 12 |
| backpatch | 380 | 99,789474 % | 4 |
| vectorpatch | 388 | 95,89474 % | 15 |
| instruction_slot | 380 | **100 %** | **0** |

backpatch cierra las dos cadenas de retorno. vectorpatch intenta hacer tambien
los stores de entrada en ASM mediante una base de vectores C definida: impide
compartir la base con los argumentos de flush y anade dos lis. Se descarta.
instruction_slot usa el puente mr para devolver el productor a una variable
C y mantiene la informacion de direccion de los stores; es la unica retenida.

## Verificacion

`audit_shadow.py production` comprueba:

1. SHA-256 inmutables de la copia fuente/objeto anterior, configure, splits y
   build.ninja; no se actualizan los hashes previos para hacer pasar el gate.
2. Mismo inventario de 30 funciones; unica diferencia de medida
   `__VMBASESetupExceptionHandlers` 99,1579 -> 100, con 380 B constantes.
3. Bytes de .text identicos fuera de esos 380 B; todas las secciones de datos
   y todas las secciones crudas de relocacion runtime permanecen identicas.
4. Fuente byte-identica fuera del cuerpo y nueva nota. El cuerpo integrado
   coincide con la sombra auditada, permitiendo solo diferencias de EOL.
5. Auditoria contra el ELF original de las 30 funciones, sin ninguna
   sustitucion de direcciones o alias: SetupExceptionHandlers da 18 relocs OK.

Un segundo agente repitio el gate sobre produccion y reviso expresamente la
semantica y definicion de cada operando MW. La compilacion real selecciono
exactamente `build/GOWE69/src/LibSN/vmbase.o`: 1 OK, 0 fallidas.

Reproduccion (el modo sin argumento es solo preintegracion y ahora debe fallar
su guarda de fuente intacta; no confundirlo con el modo vigente):

```text
python scratchpad/codex_20260908_iter3_vmbase/audit_shadow.py production
python scripts/fncmp.py LibSN/vmbase
python scripts/audit.py LibSN/vmbase
```

La viabilidad de linked se prueba por separado con un enlace completo privado;
no se cambia Matching sin autorizacion especifica.
