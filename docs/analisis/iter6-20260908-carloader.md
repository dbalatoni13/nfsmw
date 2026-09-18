# Iter6: SetMemoryPoolSize y el store real

Resultado: **ningun cambio de produccion, +0 B / +0 funciones**.

Se releen r36f-wld, r37-world, r37-root-ensayos y r40-world antes de la prueba.
Las barreras de argumento, el retorno ABI y las dependencias vacias ya estaban
agotadas. La hipotesis nueva es expresar el store PPC real con un operando
de memoria de salida y usar el nombre del pool como entrada adicional, para
forzar que su direccion exista antes del store. No se escribe un ordinal LC
en ASM: la cadena sigue siendo el literal C original, con el mismo pool.

Solo se compilaron SourceLists completas privadas que sustituyen el include
de CarLoader.cpp por una copia; el resto de includes/flags sigue intacto.
La base privada reproduce el objeto real en codigo, datos y relocaciones.

| sombra | bytes | similitud | filas | efecto |
|---|---:|---:|---:|---|
| base | 304 | 97,36842 % | 2 | stw/addi intercambiados |
| store | 308 | 93,42105 % | 5 | store real correcto, nombre en r9 temprano y mr r6,r9 extra |
| store_r6 | 300 | 93,35526 % | 6 | nombre fijado al argumento, desaparece mr r0,r3 y store usa r3 |
| copy_store | 304 | 94,73684 % | 4 | copia real a r0 restaura tamano/store, lis/addi siguen adelantados |

El productor base fue `asm("stw %1,%0" : "=m"(CarLoaderMemoryPoolNumber)
: "r"(pool_num), "r"(pool_name))`, seguido de bInitMemoryPool con pool_num
como argumento real. La tercera forma produce stored_pool_num mediante un
`mr` real con entrada inicializada antes del store; no lee ningun registro
indefinido. El pin r0 no cruza llamadas. Ninguna forma cierra simultaneamente
el orden del nombre y de los argumentos; no se retiene el parcial.

Gate: `python scratchpad/codex_20260908_iter6_carloader/audit.py`, PASS.
Comprueba 846 cuerpos fuente (845 vecinos), 582 entradas mapeadas (577 al
100 %), datos/BSS/constructores y 11582 relocaciones normalizadas. Al variar
el candidato +/-4 B cambian campos REL24/REL14 en funciones ajenas: solo se
enmascaran sus campos reubicables y se comprueban por separado tipo, offset
de instruccion, propietario y destino (simbolo/offset), no se confunden esos
cambios de direccion con diferencias de instrucciones. La base privada y
el objeto real si tienen todos los cuerpos crudos identicos.

Para el candidato se conserva el multiconjunto de relocaciones; los offsets
internos y el orden del codigo cambian de forma esperada en sombras fallidas,
no se certifican como exactos. Ninguna otra metrica por funcion cambia.
Los nombres y tamanos de todos los cuerpos fuente se mantienen, salvo el
tamano del candidato de cada prueba.

Fuente real SHA-256:
`99d4f03ea355c5423428c95073a9c9b866635be8899c4762be4c0a715add7383`.
Objeto real SHA-256:
`45e1452f0fdcfa4306a8edcbf3cd3e293deccd7ecc776c0f7e0db068c216c333`.

Los artefactos estan en `scratchpad/codex_20260908_iter6_carloader/`. El primer
auditor hacia busquedas lineales sobre relocaciones debug en cada funcion;
se paro solo ese proceso propio identificado y se sustituyo la busqueda por
indices de relocaciones/rangos. El gate final termino sin tocar produccion.
