# El GCC de SN contra el 2.95.3 de serie: qué tocaron exactamente

Bajado `gcc-2.95.3.tar.gz` de `ftp.gnu.org` (12.911.721 B) y extraído en
`orig/prodg/stock/` — ignorado por git, igual que el de SN. Las dos versiones se
identifican solas:

    de serie   version.c:1    "2.95.3 20010315 (release)"
    de SN      version.c:135  "2.95.3 SN BUILD v<x>.<y> for Nintendo Gamecube"

## Tamaño de la desviación

    fichero        lineas SN  de serie  hunks    +/-
    cp/decl2.c        5412      5436      27   +148/-170
    cp/class.c        5448      5753      12   +109/-364
    cp/method.c       2487      2666      13    +80/-236
    varasm.c          4681      4490      20   +245/-56
    toplev.c          6015      5739      29   +341/-93
    cp/pt.c           9918      9876      10   +146/-102
    cp/decl.c        15280     15165      33   +503/-390

Buena parte es ruido (`Removed PROTO macro use`, 62 veces, porque «*was causing
VisualAssist to spin its wheels*»). Lo que importa está abajo.

## `finish_file`: sólo dos cambios, y ninguno mueve el orden

El orden que documenté en `r36-jf-finish-file.md` **se mantiene**. SN sólo:

1. **Quita** el paso `walk_globals(vlist_ctor_wrapper_p, finish_vlist_ctor_wrapper, ...)`
   que el 2.95.3 de serie tiene entre las vtables y los objetos de duración
   estática. Un paso menos, pero no reordena los demás.
2. **Añade** al final, con nombre y todo:

       // SN-Phil: retro-fix from GCC3, ensure statics get debug data
       if (pending_statics)
         check_global_declarations (&VARRAY_TREE (pending_statics, 0),
                                    pending_statics_used);

**Ese segundo cambio explica un mecanismo del que veníamos tirando a ciegas**: es
la razón de que el DWARF conserve entradas de estáticos que el enlazador
descartó, que es de donde sale el marcador `0xFFFFFFFF` que usa `stripped.py`.
No era una rareza: es una retro-corrección deliberada de SN para que los
estáticos tuvieran información de depuración.

## El historial de revisiones es un registro de cada desviación

Los ficheros de SN llevan bloques `$History:` **escritos por su propio
ingeniero**, con fecha y descripción. Es literalmente la lista de lo que
cambiaron. Las entradas que tocan cosas nuestras:

**El COMMON de los estáticos de clase, y el vaivén completo:**

    Version 9   "Remove COMMON from uninitialised class static members.
                 They now go in bss."
    Version 10  "Pulled static class memebers mod. STL expects the GNU way
                 of doing things."

O sea que lo quitaron y **lo volvieron a poner**. Por eso nuestro build emite
COMMON para `LuaRuntime *LuaRuntime::mObj;` sin inicializador, que es justo lo
que medí al investigar el límite `.data` de zGameplay/zLua. El comportamiento es
el de serie, no una rareza local.

**Datos y secciones:**

    "Added support for FP consts in small data sections"   x4
    "Added PS mode float consts"                           x3
    "SDA attribute mods" · "Fixed LCOM not checking SDA attribute"
    "Changed to new 'safe-sda' options"                    (el -msafe-sda que usamos)
    "Option for alignment based section names"             x3
    "byte bool switch"                                     x8
    "switch table threshold feature"
    "No static dtors mod"                                  (el -fno-static-dtors que usamos)
    "force link once flag" · "Debug builds use 'weak' rather than 'linkonce'"

Lo de **`byte bool switch`** conviene retenerlo: medimos en `zFeOverlay` que aquí
`bool` ocupa **4 B** (`bool[11]` = 0x2C exactos), y resulta que hay una opción del
compilador que lo cambia.

## Cuatro atributos propios que no estamos usando

`c-common.c:481`, con el comentario `// SN-Phil: ADDR ATTR MODS`:

    __attribute__((address(<constante>)))    coloca un global en una direccion fija
    __attribute__((sda))  /  ((nosda))       control del small data area
    __attribute__((snda))
    __attribute__((noalias))

El de `address` acepta sólo una constante entera (`c-common.c:786-806`: da
`"requested address is not a constant"` si no lo es, y `"address attribute cannot
be specified for local variables"` para locales).

**No lo he probado**, y hay que hacerlo antes de venderlo: colocar una variable
en dirección absoluta puede emitirla como símbolo absoluto en vez de relativo a
sección, y eso rompería la disposición en vez de arreglarla. Pero si funciona
como parece, es una alternativa directa al pool escrito a mano para el dato que
hoy no sabemos colocar.

## Lo que sigue sin estar

`ngcld` y `ngcas` son propiedad de SN y **no vienen** en la entrega GPL (su README
lo dice: sólo `cpp`, `cc1`, `cc1plus`, make, sed, sh y `libgcc`). La aritmética
`size & ~7` del estripado sigue siendo sólo medida nuestra.
