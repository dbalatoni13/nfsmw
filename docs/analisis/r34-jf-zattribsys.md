# `zAttribSys` es la promoción más cercana del proyecto: le faltan 464 B de dato

Triaje de las 33 SourceLists por distancia a promocionar (Δ`.text` + Σ|Δdatos|).
De las 29 sin promocionar, **`zAttribSys` es la primera**, y por bastante.

## El `.text` ya NO estorba — probado

    objetivo 196 funciones, nuestro 199
    funciones con TAMAÑO distinto: 0        <- las 196 comunes casan una a una
    sólo en el nuestro: 3 (156 B)          Attrib::Database::Get / operator delete / IsInitialized
    sólo en el objetivo: 0

Las tres no existen en el ELF original (0 de 3), no las referencia **ninguna
reubicación** de la unidad, y el volcado DWARF las da como `static inline`. Son
copias fuera de línea que GCC 2.9 emite en `finish_file` y que el original no
emitía.

**Y no hay que quitarlas: el enlazador se las lleva.** Enlazado el proyecto con
nuestro `zAttribSys.o`, el `.text` del ELF mide **3.804.440 B, exactamente igual
que el enlace base**, y ninguno de los tres símbolos aparece en la salida.
`LDFLAGS` es `-strip-unused-data -keep config/GOWE69/keep.lst`, y con eso basta.

O sea que lo único que separa a `zAttribSys` de `DOL OK` es el dato.

## El dato: 464 B, y 394 son veinte cadenas

    .bss     objetivo    40  nuestro    40   IGUAL
    .data    objetivo    96  nuestro    48   -48
    .rodata  objetivo  1176  nuestro   760   -416   (1ª diferencia +0x5C: el prefijo YA está)

El objetivo tiene 47 cadenas en `.rodata` y nosotros 29. **Faltan 20 (394 B) y no
sobra ninguna.** Este es el orden del objetivo, que es la receta; `*` es lo que
falta:

    +0x0060   Attrib::Attribute        +0x0190   Attrib::layout
    +0x0074   Attrib::Instance         +0x01A0   Attrib::attribute_data
    +0x0088   Attrib::Definition       +0x01B8 * Attrib::ClassTable
    +0x009C   Attrib::Class            +0x01CC * Attrib::TypeTable
    +0x00AC   Attrib::Database         +0x01E0 * Attrib::TypeDescPtrVec
    +0x00C0   Attrib::TypeDesc         +0x01F8 * Attrib::CollectionList
    +0x00D4   Attrib::RefSpec          +0x0210 * Attrib::ClassList
    +0x00E4   Attrib::Blob             +0x0224 * Attrib::DatabasePrivate
    +0x00F4 * Attrib::Array            +0x023C   STL
    +0x0104 * Attrib::Node             +0x0240   Attrib::DatabaseExportPolicy
    +0x0114 * Attrib::HashMapTable     +0x0260   Attrib::ClassExportPolicy
    +0x012C * Attrib::HashMap          +0x027C   Attrib::CollectionExportPolicy
    +0x013C * Attrib::ExportManager    +0x029C   %s_%08x
    +0x0154 * Attrib::Vault            +0x02A4   Attrib::ExportPolicyPair
    +0x0164 * Attrib::Collection       +0x02C0 * Attrib::Vault::DataBlock
    +0x0178 * Attrib::ClassPrivate     +0x02DC   Attrib::DataBlocks
                                       +0x02F0   Attrib::AssetIDs
                                       +0x0304 * Attrib::TweakableAttribute
                                       +0x0328 * EA.Reflection.Text
                                       +0x033C * Attrib.StringKey
                                       +0x0350 * %s\%s\%s\%s\%d\%s\[]:
                                       +0x0368 * AttribSys.gamecube
                                       +0x0428   Attrib::DatabaseLoadData
                                       +0x0444   Attrib::ClassLoadData
                                       +0x045C   Attrib::CollectionLoadData

(El salto de `+0x0368` a `+0x0428` son ~180 B que no son cadena.)

## Y la corrección a un cambio mío: la r33 se llevó SEIS de esas veinte

El commit `916962fd` («once etiquetas espurias fuera») puso a `NULL` estas seis,
que están en la lista de arriba:

    Attrib::ExportManager · Attrib::Vault      (AttribLoadAndGo.h)
    Attrib::HashMapTable                       (AttribHashMap.h)
    Attrib::Collection · Attrib::ClassPrivate · Attrib::DatabasePrivate  (AttribPrivate.h)

Aquel cambio **no era gratuito y no era erróneo**: nuestras cabeceras internaban
19 etiquetas **por cada TU** y el original 8, y quitarlas valió −232 B exactos en
las cuatro unidades medidas con `main.dol` intacto. El problema es que era una
solución de brocha gorda a un síntoma.

**El mecanismo real**: la cadena aparece si y sólo si el TU emite el `operator
new`/`delete` de esa clase. En el original, las clases privadas de AttribSys
(`Vault`, `ClassPrivate`, `DatabasePrivate`, `ClassTable`, `TypeTable`,
`CollectionList`, `ClassList`…) sólo se emiten en `zAttribSys`; en los otros 12
TU que incluyen `AttribPrivate.h` no. Nuestro `USE_ATTRIB_ALLOC` interna
incondicionalmente con el `if (ENABLE_IN_MILESTONE)`, así que la cadena salía en
los trece.

**La forma buena es local al TU**, y es el patrón que el propio brief de la r33
recomienda para tocar una cabecera compartida con radio de explosión **cero por
construcción**:

    // en la cabecera
    #ifdef ATTRIB_TAGS_AQUI
        return Alloc(bytes, "Attrib::Collection");
    #else
        return Alloc(bytes, NULL);
    #endif

    // y `#define ATTRIB_TAGS_AQUI` sólo en SourceLists/zAttribSys.cpp

Las otras catorce **nunca se escribieron**: ninguno de los veinte literales
existe hoy en el árbol (comprobado uno a uno con `grep -F`).

## Cómo verificarlo

1. `datacmp.py zAttribSys`: `.rodata` tiene que ir de 760 a 1.176 B y la primera
   diferencia moverse de `+0x5C` a `+0x0428` o más allá.
2. Las otras 12 unidades que incluyen `AttribPrivate.h` **no deben moverse ni un
   byte**: compilarlas a un directorio aparte y comparar tamaños de sección.
3. `trypromo.py zAttribSys`, y luego el DOL con su marca de tiempo.
4. Quedarán `-48 B` de `.data` y ~22 B de `.rodata` que no son cadena.
