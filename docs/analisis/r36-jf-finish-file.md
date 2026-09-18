# El orden de emisión, leído del compilador que construyó el juego

Ya no hay que deducirlo. `orig/prodg/NGC_GNU_SRC/NGC/gcc/cp/decl2.c` es el
front-end de C++ de **nuestro compilador exacto** —`version.c:135` forma
`"2.95.3 SN BUILD v<x>.<y> for Nintendo Gamecube"`, que es el `AT_producer` del
DWARF del juego— y `finish_file` está en la línea **3643**.

## El bucle, en orden

`finish_file` es literalmente un `do { ... } while (reconsider);`, y dentro de
cada vuelta el orden es éste:

    1.  instanciar las plantillas pendientes
    2.  walk_globals(sigtable_decl_p, ...)              tablas de firma
    3.  walk_globals(vtable_decl_p, finish_vtable_vardecl, ...)   VTABLES
    4.  objetos de duración estática -> funciones de inicialización
    5.  for i in saved_inlines: synthesize_method(decl)  METODOS SINTETIZADOS
    6.  marcar las funciones de manejo de excepciones
    7.  wrapup_global_declarations(saved_inlines, ...)   LAS INLINE DECLARADAS
    8.  for i in pending_statics: los estáticos de clase

Y `saved_inlines` se llena en **orden de parseo** (`decl2.c:2154`):

    VARRAY_TREE (saved_inlines, saved_inlines_used) = decl;
    ++saved_inlines_used;

`wrapup_global_declarations` (`toplev.c:2940`) recorre `for (i = 0; i < len; i++)`
**hacia delante**, o sea que respeta ese orden de parseo.

## Qué confirma, qué refina y qué corrige

**Confirma tres cosas que el proyecto había deducido midiendo:**

- **Las vtables salen antes que todas las funciones** (paso 3 contra pasos 5 y 7).
  Esto es lo que rompió la veda de la r33 que decía que las vtables decidían la
  colocación de las virtuales: no la deciden, salen en bloque y antes.
- **`finish_file` corre en pasadas.** El `while (reconsider)` es literal, y por eso
  los métodos de una clase pueden partirse en dos sitios lejanos.
- **Todo lo inline sale al final de la unidad**, que es lo que colocó 218 de 405
  funciones en la r32.

**Refina el mecanismo del destructor implícito** que `ord4` encontró en la r35 y
que valió 16 funciones en `zSim`. La causa exacta es que **son dos pasos
distintos**: el paso 5 recorre `saved_inlines` y llama a `synthesize_method`, que
**emite en el acto**; las inline *declaradas* no salen hasta el paso 7. Por eso un
método sintetizado —un `~X` que el original no declara— aparece **por delante** de
todas las inline declaradas de su clase, aunque en la fuente esté después.

El filtro del paso 5 dice exactamente cuáles son:

    DECL_ARTIFICIAL (decl) && ! DECL_INITIAL (decl) && TREE_USED (decl)
    && (! DECL_REALLY_EXTERN (decl) || DECL_INLINE (decl))

`DECL_ARTIFICIAL` = generado por el compilador, `!DECL_INITIAL` = sin cuerpo
escrito. O sea: **constructores, destructores y `operator=` implícitos, y sólo
si se usan**. Es la definición precisa del racimo que `ord4` cazó a mano.

**Y añade dos pasos que no teníamos:** las **tablas de firma** van antes que las
vtables (paso 2), y los **estáticos de clase** salen los **últimos** de cada
vuelta (paso 8, `pending_statics`). Ese paso 8 es la explicación de por qué las
cuatro estáticas de Lua acaban donde acaban, y de por qué `pending_statics` no
sigue el orden de las funciones.

## Lo que sigue sin fuente

`ngcld` y `ngcas` son propiedad de SN y **no vienen** en la entrega GPL: la
aritmética `size & ~7` del estripado y el marcador `0xFFFFFFFF` siguen siendo
sólo medida nuestra.

## Lo siguiente, si hace falta

Diferenciar este árbol contra el GCC 2.95.3 de serie para aislar las
modificaciones de SN. `decl2.c`, `varasm.c` y `toplev.c` llevan cabeceras de
revisión de SN, pero no he comprobado cuáles son cambios reales de código: hace
falta bajar el 2.95.3 original, y eso no lo he hecho.
