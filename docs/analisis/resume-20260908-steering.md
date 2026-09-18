# VDevice_RecalcGammaTable cerrada: 216 B (2026-09-08)

Base real: `8853122e` con los cambios locales R47/R48. Se conserva el cierre
local de 188 B de `VDevice_DownloadEffect` y todas las anotaciones previas.
Se modifica exclusivamente el cuerpo de `VDevice_RecalcGammaTable` y se agrega
una nota de reconstruccion. No hay cambios de cabeceras, flags ni splits.

## Resultado integrado

```
# VDevice_RecalcGammaTable target=100.0% ours=100.0% size=216/216
```

54/54 instrucciones, incluido el marco de 0x20 y los tres usos de la ranura
en r1+8. Auditoria del ELF: 216 B, 3 ramas, 14 reubicaciones, 10 accesos a
literales verificados por contenido (los lfd comparan 8 bytes completos).

La unidad pasa de 29 a **30 funciones exactas de 36**, +216 B. Las otras
seis pendientes siguen iguales; los 188 B de DownloadEffect son trabajo
local anterior, no progreso de esta tanda. La unidad sigue NonMatching:
**no se suman bytes linked** ni se afirma paridad de todos sus datos.

## Por que funciona

R20/R46 descartaron correctamente las conversiones C implicitas: MWCC asigna
una ranura distinta a cada conversion y ningun pragma/version probado las
fusionaba. Pero eso no descarta almacenamiento explicito de fuente.

Una union local de 8 B representa la unica ranura del original. Las dos
conversiones de entero construyen el double con las mitades `0x43300000` y
`integer ^ 0x80000000`; se resta el sesgo exacto 4503601774854144.0. Al final
la misma union recibe el resultado de fctiwz y se lee su palabra baja.

La union sola no basta: `(float)(double-double)` emite `fsub; frsp`, dos
instrucciones donde el objetivo tiene una `fsubs`. Dos bloques MW inline ASM
emiten esas dos fsubs; un tercero emite fctiwz, para poder reutilizar la misma
ranura sin generar otro temporal implicito. **Solo tres de las 54
instrucciones se expresan en ASM**; el bucle, ramas, llamadas, almacenamiento
y operaciones restantes siguen siendo C.

Los operandos se nombran con variables `register` ordinarias, no se fijan
registros fisicos. Todos los valores se definen antes de consumirse. No se
afirma recuperar la sintaxis perdida: es una reconstruccion exacta del
comportamiento de maquina, documentada en la fuente.

## Tres ensayos causales, sin barrido

| forma | bytes | fuzzy | resultado |
|---|---:|---:|---|
| base R48 | 216 | 99,796295 % | marco 0x30; tres ranuras |
| union + fctiwz | 224 | 93,14815 % | marco correcto; sobran dos frsp |
| union + dos fsubs + fctiwz | 216 | 99,44444 % | solo intercambio r0/r3 en rama negativa |
| escribir palabra baja antes de alta tambien en rama negativa | 216 | **100 %** | exacta |

La tercera forma corrige el primer uso de los dos enteros temporales; MWCC
asigna r3 al entero y r0 al high-word sin ningun pin. El scheduling vuelve a
colocar primero el store de la palabra alta, exactamente como en el objetivo.

## Verificacion de no regresion

`scratchpad/codex_20260908_resume_steering/audit_shadow.py production`:

- Las 36 funciones conservan tamanos y porcentajes salvo el cierre de 216 B.
- Todos los bytes de `.text` fuera de la funcion son identicos.
- Todas las reubicaciones runtime mantienen offset, tipo, addend e identidad
  de destino, incluidas las de las seis funciones aun no exactas.
- Se conservan secciones y tamanos; `.data` es identica. Los 40 B de `.sdata2`
  reordenan literales anonimos: el inventario completo de valores y tamanos
  es identico, y se verifica cada referencia por su contenido.
- Auditoria original ELF de las 30 funciones exactas: PASS. Para los dos
  estaticos numerados de GetFreeEffect se valida explicitamente la identidad
  antes/despues y sus declaraciones originales LOCAL de 4 B; no se presupone
  que la distribucion completa de `.sbss` coincida al enlazar esta unidad.
- Fuente de produccion recompilada con el target unico
  `build/GOWE69/src/LibSN/steering.o`; no se ejecuto ninja.
- CRLF puro conservado. No se edita ni elimina el trabajo local previo.

La prueba sombra `union_order` y la de produccion pasan el mismo gate.
Snapshots completos, los dos negativos y copias de la fuente/objeto iniciales
quedan bajo el mismo directorio de scratch. El censo global y DOL de la tanda
se documentan en `resume-20260908-cierre.md`.

## Control posterior: la copia de Effect_PerformEnvelope sigue fusionada

Un unico ensayo adicional en sombra sustituye la asignacion inicial de `out`
por `asm { mr out, mag }`, con ambos operandos `register` y `mag` inicializado.
MWCC reconoce/coalesce tambien esa copia ASM: sigue en 192/196 B y
93,265305 %, sin el mr del objetivo. No se integra. El cierre de las
conversiones no implica que una instruccion de copia fuerce dos registros
distintos; no repetir este ensayo como una barrera que supuestamente no se ha
probado. Artefactos: `envelope_copy.c/.o/.json` en el mismo scratch.
