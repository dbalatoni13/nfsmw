# ENVIRO_AEMS: control aislado de la barrera r47-c25 (2026-09-08)

## Resultado

La regresión de `InitSFX__16CARSFX_RoadNoise` está causada exactamente por las
dos líneas locales r47-c25 añadidas tras `SetFilter_Effects_Dry_FX` en
`ENVIRO_AEMS.h`:

```cpp
// r47-c25 (P6): barrera de memoria tras Dry_FX.
__asm__ volatile("" : : : "memory");
```

Una única variante en sombra las elimina y conserva intactos el resto de la
cabecera, `Wet_FX=-1` y todos los cambios locales. Compilada con el toolchain,
flags y renombrado exactos de `zEAXSound2`, produce:

| función | control actual | sin barrera |
|---|---:|---:|
| `InitSFX__16CARSFX_RoadNoise` | 444 B, 92,34234 % | **444 B, 100 %** |
| `Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi` | 392 B, 87,45918 % | 392 B, 99,64286 % |

El inventario permanece en 1112/1112 símbolos de función y 930 comparables;
las exactas pasan de 926 a 927. Sólo esas dos métricas cambian. Este resultado
restaura un cierre de 444 B que ya existía en HEAD/r46: no se contabiliza como
ganancia nueva de esta auditoría.

## Control del wrapper

La inclusión anticipada de la copia de cabecera podría, en principio, cambiar
inlines o pools. Por eso se compiló también un wrapper de control que contiene
la cabecera local sin modificar y después la SourceList absoluta. Frente al
objeto fuente real de `zEAXSound2`, son byte a byte iguales `.text`, `.rodata`,
`.data`, `.bss`, `.ctors` y todas sus secciones de reubicación crudas.

La única diferencia de nombres del `symtab` se limita a dos símbolos privados
que el contador interno de ProDG renombra al entrar por el wrapper:

| objeto real | control | índice | identidad cruda |
|---|---|---:|---|
| `k.22888` | `k.23410` | 355 | `.bss+0x10`, 4 B, LOCAL/STT_NOTYPE |
| `_.tmp_2.22889` | `_.tmp_2.23411` | 356 | `.bss+0x14`, 4 B, LOCAL/STT_NOTYPE |

Hay siete usos de esos nombres. En cada uno coinciden offset, tipo, addend e
índice de símbolo; la propia `.rela.text` cruda es idéntica. El gate no ignora
nombres de símbolos globalmente: afirma expresamente que no existe ningún otro
par distinto.

## Auditoría de impacto

Los dos objetos sombra tienen idénticos tamaño y layout de todas sus funciones.
Entre control y variante cambian 117 posiciones de byte de `.text` y cuatro
registros de reubicación resueltos; todos caen dentro de los rangos de
`InitSFX` o `Play`. `.rodata`, `.data`, `.bss`, `.ctors` y sus reubicaciones no
cambian.

Objdiff serializa 25 registros de función distintos. Al eliminar solamente el
campo DWARF `line_number`, 23 vuelven a ser idénticos: quitar dos líneas de la
cabecera desplaza en -2 la procedencia de inlines posteriores. Los únicos dos
registros con una diferencia de código real son `InitSFX` y `Play`.

El auditor contra el ELF original confirma:

```text
BindToData__14GinsuSynthDataPv                       ok   340 B,  7 ramas, 12 relocs, 4 literales
InitSFX__16CARSFX_RoadNoise                          ok   444 B,  7 ramas, 20 relocs, 0 literales
MsgBarrier__18CARSFX_PreColWooshRC16MAudioReflection ok   140 B,  4 ramas,  9 relocs, 4 literales
MsgPlayMiscSound__10SFX_CommonRC10MMiscSound         ok   780 B, 25 ramas, 64 relocs, 0 literales
```

Así quedan protegidos los cierres solicitados de 780, 140 y 340 B. Los hashes
de las fuentes, objetos y configuración de producción guardados antes del
ensayo siguen iguales.

## Consumidores y procedencia

La búsqueda completa encuentra exactamente dos construcciones
`new Csis::FX_ROADNOISE(...)`, ambas en `CARSFX_Roadnoise.cpp`: la de
`InitSFX` y la de `Play`. La construcción `FX_ROADNOISE_TRANS` es otra clase y
no contiene la barrera. `zEAXSound2.cpp` incluye tanto `CARSFX_Roadnoise.cpp`
como `ENVIRO_AEMS.cpp`; es el único consumidor que emite esas dos
instanciaciones. `zEAXSound.cpp` sólo referencia los cuatro globals AEMS desde
`EAXAemsManager.cpp` y no emite ninguna función de road-noise ni constructor.

No existe nota en `docs/analisis` que sustente conservar r47-c25; antes de crear
estos artefactos, la búsqueda del identificador sólo encontraba el comentario
en la propia cabecera.

## Reproducción y propuesta

Los artefactos están aislados en
`scratchpad/codex_20260908_resume_enviro`. El gate completo es:

```text
python scratchpad/codex_20260908_resume_enviro/prepare.py
python scratchpad/codex_20260908_resume_enviro/build.py
python scratchpad/codex_20260908_resume_enviro/audit_variant.py
python scratchpad/codex_20260908_resume_enviro/audit.py
```

La propuesta acotada, todavía **no aplicada**, es retirar exclusivamente esas
dos líneas r47-c25 de la cabecera real. Deben conservarse `Wet_FX=-1` y todos
los demás cambios locales. Después de autorización corresponde recompilar los
consumidores, repetir los dos auditores, censo global y SHA del DOL.

Nota de reproduccion: `prepare.py` es para preparar un namespace nuevo y
rechaza directorios ya existentes; no repetirlo sobre estos artefactos. Con
las sombras disponibles bastan `audit.py` y `audit_variant.py`. `audit.py`
protege deliberadamente los hashes de la produccion anterior: despues de una
integracion autorizada se necesita un gate de produccion que compare contra
esas copias y la sombra exacta, no cambiar los hashes antiguos para hacerlos
pasar artificialmente.
