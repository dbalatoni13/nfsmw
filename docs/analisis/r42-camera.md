# r42 - cache de size() en una cabecera sombra

2026-09-08. Base r41 final. Ninguna fuente, cabecera ni objeto de produccion
modificado. Solo se escriben artefactos en `scratchpad/codex_r42_camera/`.

## Hipotesis y prueba

Se retomo la propuesta concreta de r36c y de la shortlist r41 para
`_Storage<CameraAI::Director*,2>` en zCamera: objetivo y fuente de 1.156 B,
98,69896 %, 22 filas distintas. El objetivo conserva el resultado de `size()`
en r4 alrededor del bucle de crecimiento; la fuente recarga mSize al entrar
y evita una copia en el latch. Ambas diferencias se compensan en tamano.

Se copio UTLVector.h a una ruta de includes privada. El compilador recibe
los flags reales y unicamente antepone esa ruta para el ensayo; no se altera
ningun flag guardado, fichero de configuracion, SourceList o header real.
La compilacion control con la cabecera identica confirma que el arnes no
modifica ningun byte runtime ni identidad de simbolo del objeto de produccion.

En la copia, SOLO en la rama `srcIt == 0`, se cambio:

```cpp
reserve(minSize);
size_type curSize = size();
if (minSize > curSize) {
    do {
        push_back();
        curSize = size();
    } while (minSize > curSize);
}
```

La cache se refresca despues de push_back; no se supone que las llamadas
virtuales carezcan de efectos ni se sustituye size() por un incremento
independiente. No se anaden barreras, pines ni valores sin inicializar.

## Resultado negativo, sin integracion

**1.156 B / 98,69896 %, neutro.** Los 587 registros de funciones mantienen
exactamente sus instrucciones y relocaciones, descartando solo metadatos de
linea/puntuacion. Mas fuerte: el auditor ELF confirma que TODAS las secciones
alloc, todas las relocaciones runtime, la tabla de simbolos cruda y sus nombres
son identicos entre objeto real inicial, control sombra y variante c1.

Nombrar este resultado no basta para preservar un pseudo distinto tras las
optimizaciones. No se propone integrar el cambio ni solicitar permiso para
el header. No se compilan los otros consumidores porque el ensayo no cierra
su propia candidata y no saldra de scratch. Si otra receta cerrase en el
futuro, seguiria siendo obligatorio comprobar todos los consumidores y pedir
autorizacion para cualquier edicion real de UTLVector.h.

La cabecera y el objeto reales siguen byte por byte iguales a las copias
iniciales, incluidos finales de linea. No es necesario restaurarlos ni
recompilarlos: nunca se tocaron.

## Reproduccion

Artefactos: `UTLVector_before.h`, `include/.../UTLVector.h`, `baseline.o`,
`c1.o`, `baseline.json`, `c1.json`, `trial.py` y `audit.py` dentro de
`scratchpad/codex_r42_camera/`. La copia en include conserva la variante c1.

```text
python scratchpad/codex_r42_camera/trial.py c1
python scratchpad/codex_r42_camera/audit.py
```

El baseline ya guardado fue compilado antes de editar la copia de la cabecera;
no ejecutar `trial.py baseline` sobre la variante c1 para recrearlo. Para
reproducir ese control debe usarse la copia original UTLVector_before.h.
