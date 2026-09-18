# El residuo de estripado: medido, y es pequeño. El hueco es otra cosa

## La hipótesis, y lo que da al medirla

`-strip-unused-data` deja los últimos `size % 8` bytes de cada símbolo muerto, así
que `"Attrib::Array\0"` (14 B) sobrevive como `"Array\0"` (6 B). De ahí salía la
sospecha de que **parte de los bytes que nos faltan en `.rodata` no son dato sino
restos** de cosas que el original emitía y nosotros no escribimos.

`scripts/residuo.py` lo mide: construye el vocabulario de todas las cadenas del
árbol y busca, en la `.rodata` del objetivo, cadenas `S` que sean el final exacto
de una `T` más larga con `len(T) % 8 == len(S)`.

    TOTAL: 918 B de resto en el objetivo, 821 B de cadenas que no emitimos

**La hipótesis es cierta pero pequeña: ~2 % del hueco.** Y hay que leerla con
pinzas por los dos lados:

- **Falsos positivos**: `'%s'`, `'ff'`, `'TH'` son cadenas legítimas que por
  casualidad son el final de otra. En `zFeOverlay`, 98 de los 98 «restos» son de
  este tipo.
- **Falsos negativos por construcción**: el método sólo encuentra el resto si la
  cadena completa existe en algún sitio del árbol. El caso que más importaba —la
  cadena que no escribimos nunca— es **invisible** para esta medida.

Así que 821 B es una cota inferior floja. No merece más trabajo: el hueco está en
otro sitio, y esa medida sí sale limpia.

## Lo que sí explica el hueco: cadenas ausentes, y son casi todo

Descomponiendo el hueco de `.rodata` unidad por unidad:

    TOTAL hueco .rodata 19.624 B; cadenas ausentes 22.746 B (116 %)

Pasa del 100 % porque también emitimos cadenas que el objetivo no tiene, y el
hueco es el neto. Pero el diagnóstico es inequívoco: **lo que falta son cadenas**,
no tablas ni constantes.

Y son de cuatro clases bien distintas:

**1. Nombres de ámbito de profiling.** En `zEAXSound`:

    'm_pNFSMixMaster->ProcessMixMap(t)'   'm_pStateMgr[n]->ProcessUpdate()'
    'm_pSTICH_Playback->Update(t)'        'GetFrontEnd()->Update()'
    'Speech::Manager::Update()'           'm_pEAXSND8Wrapper->Update()'

Son **el texto de la sentencia que se mide**, o sea una macro que estringa su
argumento. En el árbol hay **104 marcadores `"TODO"`** puestos donde iba una
cadena real: **75 en `ProfileNode`** y el resto en nombres de asignación de
`bMalloc`. Cada uno cuesta doble: falta la cadena buena y sobra el `"TODO"`.

Ojo, son dos casos distintos y sólo uno es mecánico:

- `ProfileNode profile_node("TODO", 0);` al principio de una función
  (`AICopManager::OnTask`, por ejemplo). La cadena hay que sacarla del objetivo.
- Macros que envuelven una llamada y estringan la expresión. **Esos sitios no
  existen en nuestro árbol**: hay que reconstruirlos.

**2. Nombres de asignación de memoria.** `'AEMSMGR: async bank load buffer'`,
`'AUD:PF MUSIC Stream Channel'`, `'CarLoaderDefrag but with a really long debug
name!!'`. Van como argumento de `bMalloc(size, nombre, __LINE__, pool)`.

**3. Tablas de nombres de enumerado.** En `zWorld`, la tabla `PERF_PART_*`
completa: `'PERF_PART_BR_CROSS_DRILLED_AND_SLOTTED_ROTORS'`,
`'PERF_PART_SU_PERFORMANCE_SPRINGS_AND_SHOCKS'`... y en `zAnim` los
`'WAC_START_FLAG_RANDOM_S...'`.

**4. Las `Attrib::Gen::<clase>`**, que ya tienen su lista completa en
`r35-jf-attribgen.md` (faltan 118, sobran 154).

## Por qué esto cambia el plan

`linkdelta.py` dejó dicho que **20 de 28 SourceLists tienen el `.text` a delta 0**
y que lo que bloquea es `.rodata`. Esto dice **de qué está hecho ese `.rodata`**:
cadenas de depuración y de instrumentación que la decompilación descartó por no
afectar al código.

Es trabajo mecánico y verificable —la cadena buena está en el objeto extraído, en
orden— y es el camino más corto a que esas veinte unidades enlacen.
