# r35-web1 — Investigacion en internet

Fecha: 7-sep-2026. Grupo `web1`. No he tocado `src/`, `config/` ni `configure.py`,
no he compilado y no he hecho commits.

---

## Resumen en cinco lineas

1. **Hay un hallazgo que vale 5.180 B y una promocion entera**: un fork de upstream
   tiene `_bOutput` **exacto**, la unica funcion imperfecta de nuestro `zBWare`. §1.
2. **Barrido el ecosistema entero de forks** (18 forks, 7.611 commits, 128 ramas):
   **eso es lo unico** que alguien tiene y nosotros no. §2.
3. **El frente de AttribSys que me encargaste no esta fuera, esta dentro**: las 83
   cabeceras generadas ya existen en el arbol, `tools/attrib_generator.py` es
   identico al de upstream, y lo unico que falta es su fichero de entrada. §3.
4. **`BuildMessageTable` se regenera del volcado DWARF que ya tenemos.** No hace
   falta internet para eso. §4.
5. **Frentes muertos, con medida**: `EmotionManager`, EAGL/SDK de EA, fugas de
   fuente de EA, prototipos con simbolos. §7.

---

## 1. VERIFICADO — `_bOutput` exacto en `matt-lacerda/nfsmw`, rama `zbware-complete`

Es el hallazgo de esta ronda. **Nuestro `zBWare` tiene 239 funciones y una sola
imperfecta**, y es exactamente la que ese fork cerro.

Nuestro estado (`build/GOWE69/report.json`, leido, no recompilado):

```
main/Speed/Indep/SourceLists/zBWare   total=35772  matched=30592  (-5180)
  _bOutput__FP11bOutputInfoPCcP13__va_list_tag   5180 B   99.9830 %
```

Como `matched_code` es todo-o-nada, **esa unica funcion se lleva los 5.180 B**, y
`zBWare` es la unidad entera.

### La fuente

- Repo: <https://github.com/matt-lacerda/nfsmw> (fork de `dbalatoni13/nfsmw`)
- Rama: `zbware-complete`, **95 commits por delante** de `dbalatoni13/main`,
  ultimo commit del **6-sep-2026** (ayer).
- Fichero: `src/Speed/Indep/bWare/Src/bPrintf.cpp`, estado final en `e8ef2cf18`.
- Commit que lo cierra: `39c8091bc` — *"99.9%: match bOutput instructions and
  normalized DWARF"* (4-sep-2026), toca 26 lineas de `bPrintf.cpp`.
- **Licencia: CC0 1.0 Universal** (fichero `LICENSE`, primera linea literal:
  `CC0 1.0 Universal`), heredada de upstream. Dominio publico: reutilizable sin
  atribucion obligatoria.

### Por que aplica a nosotros y no es un espejismo

- **Mismo objetivo, misma version.** Su log dice: `main.dol` SHA-1
  `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. Es el nuestro
  (`config/GOWE69/build.sha1` y `config/GOWE69/config.yml:4`).
- **Mismo recuento de funciones.** Ellos: *"239/239 functions"*. Nosotros: 239.
- **Mismo tamano de cuerpo.** Ellos: *"`_bOutput` is **5180B, exactly the
  target**"*. Nosotros: 5180 B.
- **El riesgo de cabecera es nulo.** Diferencia entre su `bPrintf.hpp` y el
  nuestro: nosotros tenemos una declaracion de mas
  (`void bPrintfSetLocaleInfo(char, char, char);`), ellos ninguna de menos.

### Aviso honesto — somos complementarios, no ellos mejores

**Su unidad entera esta peor que la nuestra.** Su propio log lo dice:

> "Whole-unit `.text` is **99.89210%**, not exact. Remaining code-screening entries
> are `bList::Sort` (94.53488%), `bFMod` (97.77778%), `bASin` (97.08334%), `bATan`
> (97.37805%), `bStrNCmp` (99.09091%), and `bStrNICmp` (97.77778%)"

Esas seis estan **al 100 % en nuestro arbol**. Su proyecto global va por
**39,99 % fuzzy / 34,58 % matched** (su propio baseline del 3-sep), nosotros por
97,97 %. **Lo que hay que traer es `_bOutput` y nada mas** — no fusionar la rama.

Tambien tienen `bFunkServer.cpp` (45 lineas) y `bIspPlat.cpp` (11 lineas) que
nosotros no tenemos, y mas lineas en `bMemory.cpp` (+72), `bMath.cpp` (+30),
`bList.cpp` (+12); pero nuestro `.text` ya casa sin ellos, asi que es curiosidad,
no trabajo.

### Reproducirlo

```
git clone --depth 1 -b zbware-complete --single-branch \
    https://github.com/matt-lacerda/nfsmw.git /tmp/mlbware
diff -u src/Speed/Indep/bWare/Src/bPrintf.cpp \
        /tmp/mlbware/src/Speed/Indep/bWare/Src/bPrintf.cpp
```

Salen **22 hunks / 517 lineas**. `_bOutput` empieza en nuestra linea 166, asi que
**19 de los 22 hunks (de `@@ -162` en adelante) son el cuerpo de `_bOutput`** y son
los que hay que mirar. Los **tres primeros no** (`@@ -5`, `@@ -25`, `@@ -112`) y
**no hay que tocarlos**:

- Los dos primeros son el mismo problema que nosotros ya resolvimos —el
  `crclr cr1eq` delante del `bl memset`, que obliga a declarar `memset` variadico—
  con distinta ortografia. Nosotros:
  `extern "C" void *bp_memset(void *dst, ...) __asm__("memset");`
  Ellos: `extern void *bMemSetVA(void *, int, unsigned long, ...) asm("memset");`
  **Los dos arboles llegaron a ese mecanismo por separado**, lo cual lo confirma.
  El nuestro ya casa: no se cambia.
- El tercero es el cuerpo de `bSPrintf`/`bPrintf`, que tenemos al 100 %.

### Los mecanismos, que valen aparte del codigo

Su `docs/zbware_task.md` (2.517 lineas) es un log de hallazgos de calidad alta.
Cinco cosas reutilizables fuera de `zBWare`, citadas literalmente:

1. **La ruta al valor decide si una local sobrevive.** Es el mecanismo que cerro
   sus seis residuos de DWARF de golpe:
   > "**A local dies when its definition is a copy.** GCC's expander writes a
   > computed expression *directly into* the variable's own pseudo, so `x = a & b`
   > keeps `x` alive. But when the value arrives from somewhere that already holds
   > it [...] the definition is a copy insn, and `regmove` deletes it"

   Corolario medido: `bStringHash(s) % 2048` y `& 0x7ff` compilan al **mismo
   `clrlwi`**, pero solo el modulo deja localizacion DWARF, porque
   `expand_divmod` calcula en un temporal y copia.

2. **Un bloque lexico vacio no existe para GCC.**
   > "**GCC only emits a DWARF lexical block for a scope that declares
   > something.** Five spellings of an empty compound statement (bare `{}`, a
   > wrapped `if`, `if/else`, `do{}while(0)`, a nested empty block) all produced
   > nothing. An `extern` declaration resolves it"

3. **El `cmplw` muerto es huella de cross-jumping**, no ruido: comparacion sin
   salto = dos bloques identicos fusionados **despues** de reload, o sea que la
   variable tenia dos definiciones durante la asignacion.

4. **Trampa de medida** — nos afecta directamente:
   > "Comparing a function's bytes against dtk's reference object reports two
   > differing bytes in the `beq` displacement [...] Compare a variant against the
   > *baseline build*, not against the reference object"

5. **Cuando los bytes ya casan, una diferencia de registro en DWARF solo puede
   ser un nombre.** Cerraron `bDistToLine` y `CleanupExpandedSlotPools` con
   renombrados puros despues de que cuatro variantes de "desempate de asignacion"
   fallaran y dos empeoraran el match.

---

## 2. VERIFICADO — barrido completo del ecosistema de forks: no hay nada mas

Metodo: clon *blobless* de upstream + `git fetch` de los 14 forks con actividad
(**7.611 commits, 128 ramas**), y cruce automatico de los mensajes de commit
contra **nuestras 57 funciones imperfectas** de SourceLists y las 20 unidades de
biblioteca con codigo pendiente.

**Resultado: cero coincidencias a nuestro favor salvo `_bOutput`.** Cada vez que
un fork nombra una de nuestras funciones pendientes, su porcentaje es **peor**:

| funcion nuestra | nosotros | el mejor fork |
|---|---:|---:|
| `RenderFlaresOnCar` (zWorld) | 98,62 % | 71,3 % |
| `HolePunchAvoidables` (zWorld2) | 95,46 % | 84,2 % (implementada al 67,9 %) |
| `UpdateAllAvoidables` (zAI) | 99,60 % | 72,7 % |
| `ActualReadJoystickData` (zPlatform) | 99,24 % | 90,1 % |
| `TrackStreamer::GetLoadingPriority` | 97,18 % | 90,7 % |
| `FindMatchTime` (zEagl4Anim) | 97,36 % | 87,5 % |
| `SubTitler::GetElapsedTime` (zFe) | 95,52 % | 65,5 % |

Y las **unicas** reclamaciones de "100 %" ajenas en todo el historico son
funciones que **ya tenemos al 100 %** (`GetDesiredRainIntensity`, la tanda de
`Speech Observer Assess*`, `SetCameraMatrix`, `sndfxbus`, `SNDSTRM_getprogvol`…),
mas la de `zBWare` de §1.

### Lo que si conviene saber del mapa

- `dbalatoni13/nfsmw`: **CC0-1.0**, 165 estrellas, 18 forks, `main` empujado el
  5-sep-2026. En **decomp.dev** (<https://decomp.dev/dbalatoni13/nfsmw/GOWE69>)
  la version GOWE69 figura al **41,87 % total y 5,02 % linked**. Nuestra ventaja
  esta intacta desde la r32.
- **Hay ramas de fork que upstream no tiene** y que la auditoria de la r32 no
  cubrio (solo miro las 11 ramas de upstream). Por si vuelve a hacer falta:
  `matt-lacerda/zbware-complete` (95), `JohnDeved/zFE2` (1.343),
  `JohnDeved/zWorld2` (994), `zGameplay` (711), `zTrack` (554),
  `zPhysicsBehaviors` (468), `zEagl4Anim` (385), `zAI` (335), `zattribsys` (87),
  `RealCA/test` (3.481), `m3cinat/{cameramover,zcamfix,zcam-draft}`,
  `NoxieNya/{zBWare-functions,zAnim-*,zFE-solving}`. **Todas revisadas por
  mensaje de commit; ninguna aporta**, pero las de JohnDeved son de marzo-2026 y
  las de m3cinat tocan `zCamera`, que es nuestra peor unidad (11.928 B).
- `Sidihidi/pr/matching-fixes` **es tuyo**: contiene commits firmados `jferr`
  (p. ej. `a11810418`, 17-ago-2026). No es material externo.

---

## 3. VERIFICADO — AttribSys: el frente no esta fuera, esta dentro

Tu encargo pedia "el contenido de las cabeceras `Attrib::Gen::<clase>`: que
clases, que campos, en que orden". **Ya lo tenemos, y mejor que nadie en la web.**

- `src/Speed/Indep/Src/Generated/AttribSys/Classes/` tiene **83 ficheros**, 60
  clases mas sus `_hash`. Estan **todas** las que `r35-jf-attribgen.md` marca como
  FALTAN (`audioimpact`, `audioscrape`, `milestonetypes`, `speech`,
  `trafficpattern`, `visuallook*`…).
- `tools/attrib_generator.py` es **byte a byte identico al de upstream/main**
  (solo difiere en CRLF).
- Nuestras cabeceras incluyen **claves de clase, hashes por campo, `_LayoutStruct`
  con offsets, los cuatro accesores por campo y los `TypeOf_`**. La mejor version
  publica (§6) tiene solo el `LayoutStruct` y un accesor.

**Contraprueba independiente del orden de campos**: la decompilacion de fisica de
MW para **PC/MSVC** de Brawltendo trae `nos.h` con el orden
`NOS_DISENGAGE, TORQUE_BOOST, FLOW_RATE, RECHARGE_MIN, NOS_CAPACITY,
RECHARGE_MAX, RECHARGE_MAX_SPEED, RECHARGE_MIN_SPEED`, **identico al nuestro**.
Dos plataformas y dos compiladores distintos coinciden: el orden esta bien.

### Lo unico que falta de verdad, y donde conseguirlo

`attrib_generator.py` se invoca asi (linea 363, literal):

```
Expected usage: {0} <vault yml file> <vault string file> <output directory>
```

- El **`vault string file`** lo tenemos: `symbols/vlt.txt`, 13.216 lineas
  `0xHASH - Nombre`.
- El **`vault yml file`** (con `Classes[].Name`, `LayoutSize`, y `Offset` por
  campo) **no esta en el arbol, ni en upstream, ni en ninguna rama de ningun
  fork**. Comprobado con `git log --all --diff-filter=A -- "*info.yml"` sobre los
  7.611 commits: nunca se ha commiteado. Upstream lo genero una vez y lo tiro.

**Si alguna vez hay que regenerar o corregir una clase**, la via es reconstruir
ese YAML desde el vault del propio juego con **VaultLib** (§6): es MIT, lee
AttribSys nativamente y **tiene lector big-endian**
(`VaultLib.Core/VaultLoadingWrapper.cs:27` → `new BigEndianBinaryReader(...)`,
`Database.cs:178` → `bool isBigEndian = byteOrder == ByteOrder.Big`). Ojo: su
perfil de MW es *"Basic profile for **PC 32-bit** NFS Most Wanted"*
(`Attribulator.Plugins.SpeedProfiles/MostWantedProfile.cs`), asi que para
GameCube habria que anadir un perfil; pero como MW es 32 bits en las dos
plataformas, **el vault de PC ya da el mismo esquema de clases y campos**.

Corroboracion de que ese es el linaje correcto: VaultLib registra para MW el tipo
`"Attrib::StringKey"` (`VaultLib.Support.MostWanted/ModuleDef.cs`), y tiene
`VaultLib.Core/Types/EA/Reflection/Text.cs`. Son **dos de las veinte cadenas** que
`r34-jf-zattribsys.md` lista en la `.rodata` del `zAttribSys` original
(`Attrib.StringKey` en `+0x033C`, `EA.Reflection.Text` en `+0x0328`).

**Conclusion**: el `#include` que falta por SourceList (los 118 que faltan / 154
que sobran de `r35-jf-attribgen.md`) es un problema **local de colocacion**, no de
contenido, y no hay nada en internet que lo resuelva.

---

## 4. VERIFICADO — `BuildMessageTable`: se regenera del DWARF que ya tenemos

`tools/message_yaml_generator.py` (linea 8, literal):

```
python message_yaml_generator.py <input_dwarf_dump.txt> <output.yml>
```

Es decir: **la entrada es el volcado DWARF**, que ya tenemos
(`symbols/mw_dwarfdump.nothpp`). Y `tools/message_class_generator.py` lo convierte
en las cabeceras (`Usage: {} <messages.yml> <output_dir>`).

Tenemos **66 cabeceras** en `src/Speed/Indep/Src/Generated/Messages/`, entre ellas
`MGeneric.h` y `MAudioReflection.h`, los dos nombres de tu encargo. Los cuerpos de
`BuildMessageTable` viven hoy en `src/Speed/Indep/Src/Generated/Events/EventDefs.cpp`
(p. ej. linea 1038).

**El frente es local y esta desbloqueado**: regenerar el YAML del DWARF y volcar
los cuerpos a las `M*.h` es una pasada de herramienta, no una busqueda. No hay
nada publico sobre `LuaMessageDeliveryInfo` ni sobre `Generated/Messages` — cero
resultados en toda la web.

---

## 5. INDICIO — las fuentes de ProDG para GameCube (requiere tu permiso explicito)

Nuestro mayor agujero fuera de SourceLists es **`LibSN/steering`: 4.080 B de
8.760**, y es justo la rama en la que estas (`agent/mw-steering-r2`). Las diez
funciones que faltan son de volante con force-feedback:
`HandleTriggers`, `CookValues`, `VDevice_RecalcGammaTable`,
`VDevice_DownloadEffect`, `VDevice_GetFreeEffect`, `Effect_Init` (88,62 %),
`Effect_Update`, `Effect_PerformEnvelope`, `SimThread_Init`, `SimThread_Step`.
**Es codigo propietario de SN Systems, no newlib.**

Hay **dos** paquetes archivados publicamente que podrian contenerlo:

| item archive.org | fichero | tamano |
|---|---|---|
| `GameCubeSDK` | `SNSystems ProDG for GameCube + Sources +.NET and more.rar` | 91.315.577 B (87 MB) |
| `sn_sys_consoles_2` | `NGC/ProDGforNGCv393_Source_Code.zip` | 28.626.142 B (27,3 MB) |

- <https://archive.org/details/GameCubeSDK> — descripcion literal: *"This is a
  collection of GameCube SDKs put together by someone named Dragoon, and this was
  posted on ASSEMblerGames"*. SHA-1 del rar segun el indice: `179577b8befd…`.
- El segundo ya lo senala `r35-web2.md` para otra cosa (el fuente de `ngcld`).

**Mi lectura honesta, para que no lo bajes con falsas esperanzas:** el manual
*Getting Started with ProDG for Nintendo GameCube* (v3.5, OCR publico en
`sn_sys_consoles_2/NGC/prodgngc_gs_v35_djvu.txt`, que si he leido) lista en
`.\ngc\lib\` solo binarios — `libc.a`, `libgcc.a`, `libm.a`, **`libsn.a` "SN
runtime library"** — y junto a ellos `copying`, `copying.lib`, `copying.newlib`,
*"GNU general public license files"*. Eso sugiere que **el zip "Source Code" de
27 MB es la entrega de cumplimiento GPL** (gcc / binutils / newlib), y que
`libsn.a` —codigo propio de SN, sin obligacion de fuente— **probablemente no
este**. El rar de 87 MB dice "+ Sources" en sentido amplio y es el unico
candidato real para `steering`.

**No he descargado ninguno de los dos.** Son ficheros grandes de un SDK
propietario; una descarga necesita tu visto bueno explicito. Si lo das, el de 87 MB
es el que hay que abrir, y el criterio de exito es un grep de
`VDevice_DownloadEffect` o `Effect_PerformEnvelope`.

---

## 6. Herramientas publicas del ecosistema NFS: que son, y su licencia

Solo por si en algun momento hace falta releer el vault. **No he copiado nada al
arbol.**

| repo | licencia | que es | para que sirve aqui |
|---|---|---|---|
| [NFSTools/VaultLib](https://github.com/NFSTools/VaultLib) | **MIT** (`LICENSE`: "MIT License / Copyright (c) 2019 NFS Tools & heyitsleo") | libreria C# que lee y escribe AttribSys (VLT) | la unica via limpia de regenerar el `vault yml` de §3. Tiene `VaultLib.Support.MostWanted/` y lector big-endian |
| [NFSTools/Attribulator](https://github.com/NFSTools/Attribulator) | **sin fichero LICENSE** (todos los derechos reservados de facto) | CLI que exporta la base VLT a texto/YAML | `Attribulator.Plugins.YAMLSupport`, `SpeedProfiles/MostWantedProfile.cs` |
| [NFSTools/vltedit](https://github.com/NFSTools/vltedit) | sin licencia; *"The original VLTEdit, by Arushan, in source-code form. Distributed with permission."* | el VLTEdit historico | referencia del formato, 310 kB |
| [Brawltendo/Most-Wanted-Vehicles-Decomp](https://github.com/Brawltendo/Most-Wanted-Vehicles-Decomp) | **sin fichero LICENSE**; README: *"Original code belongs to Electronic Arts"* | decomp casante de la fisica de MW **para PC/MSVC 7.1** | contraprueba del orden de campos (§3). **Nada mas: todo lo demas ya lo tenemos mejor** |

Sobre el ultimo, para que nadie lo persiga otra vez: sus 1.051 ficheros incluyen
`packages/snd/9/source/library/cmn/` y `Speed/Win32/Libs/STL/STLport-4.5/`
completos — **los dos ya estan en nuestro arbol**
(`src/Speed/Indep/Libs/snd/9/source/library/cmn/` y
`src/Speed/GameCube/Libs/stl/STLport-4.5/`). Y sus 8 cabeceras
`Generated/AttribSys/Classes/` son un subconjunto pobre de nuestras 83.

Citado en la busqueda pero **descartado por inutil**: `NFSTools/{Binary,GlobalLib}`
(editan GlobalA/GlobalB, **no** el vault: *"the Binary tool is not capable of
editing VLT files"*), `CiPH3R-88/Vaultinator`, `nlgxzef/EdLegacy`,
`OpenNFS/OpenNFS` y `jeff-1amstudios/OpenNFS1` (remakes de NFS 1-6, otro motor).

---

## 7. Lo que NO he encontrado — frentes que doy por cerrados

Esto vale tanto como lo anterior. **No volvais por aqui.**

- **`EmotionManager`**: **cero** resultados tecnicos en toda la web. Ninguna
  herramienta, wiki, foro ni repo lo nombra. Solo tenemos el nombre, y de la web
  no va a salir nada mas.
- **`bWare` como fuente publica de EA**: no existe. Toda mencion de `bMemory`,
  `bSlotPool`, `bChunk` en internet apunta **a este mismo proyecto** o a sus
  forks. No hay filtracion de fuente de EA de esa epoca que lo contenga.
- **EAGL / EAGL4Anim**: no hay SDK ni documentacion tecnica publica. Solo fichas
  enciclopedicas (MobyGames, PCGamingWiki, Miraheze) que dicen la version por
  juego —MW usa **EAGL 3**— y nada mas. `bartlomiejduda/EA-Graphics-Manager` es
  un visor de texturas FSH/SSH, no toca codigo.
- **`dbalatoni13/nfsug`** (decomp de NFS Underground GameCube, CC0, 30 estrellas):
  **310 ficheros y practicamente todos del `dolphinsdk`**. Cero ficheros con
  `EAGL`, `AttribSys`, `Generated` o `SourceLists` en la ruta. Nada que traer.
- **Prototipos con simbolos**: Hidden Palace tiene cinco builds de MW. **El unico
  con simbolos ya lo tenemos**: el *Sep 20, 2005 prototype* es
  *"Build name: Alpha 124"*, PS2 — nuestro `orig/SLES-53558-A124/`. Sus notas
  literales: *"NFS.ELF contains debug symbols for the build. NFS.MAP is a linker
  map that also contains debug symbols."* Los otros cuatro: Oct 12 (PS2,
  *"Beta 1.1"*, sin mencion de simbolos), Oct 8 (**Xbox**, *"Debug enabled"*),
  Oct 21 y Oct 21 07.42 (**Xbox 360**). **No hay ningun prototipo de GameCube.**
  Ademas, los ficheros de descarga de varias de esas paginas estan caidos
  (*"Error: The download file provided does not exist"*).
- **Buscadores de codigo por cadena literal**: `grep.app` esta detras de un
  *Vercel Security Checkpoint* y devuelve HTML, la API de `searchcode.com` no
  devuelve JSON valido, y la busqueda de codigo de GitHub exige autenticacion
  (`gh auth status` → *"You are not logged into any GitHub hosts"*). **No he
  podido hacer busqueda global por cadena**; lo he suplido clonando y grepeando
  en local, que para este dominio cubre lo mismo.

---

## Lo accionable, por rendimiento

1. **`_bOutput` de `matt-lacerda/nfsmw@zbware-complete`** → 5.180 B y `zBWare`
   entera. Es CC0 y el objetivo es el mismo `main.dol`. (§1)
2. **Los cinco mecanismos de su log de hallazgos** (§1), en especial el de "la
   ruta al valor" y el del bloque lexico vacio: aplican a las 57 funciones
   imperfectas del arbol, no solo a `zBWare`.
3. **Decidir sobre el rar de 87 MB de ProDG** (§5) — es la unica esperanza para
   los 4.080 B de `LibSN/steering`, y necesita tu autorizacion.
4. Si algun dia una cabecera `Attrib::Gen::*` sale mal: **VaultLib (MIT)** para
   reconstruir el `vault yml` (§3). Hasta entonces, ni tocarlo.

*Temporales del scratchpad (`c35web1_*`) borrados al terminar.*
