# Las cadenas `Attrib::Gen::*` que falta y sobra en cada SourceList

Generado leyendo los objetos EXTRAIDOS (el objetivo) y los nuestros, y comparando
las cadenas `Attrib::Gen::<clase>` de `.rodata`/`.data`/`.over`.

**La lista de las que FALTAN es la lista de cabeceras generadas de AttribSys que
hay que incluir en ese TU** (`dat2` lo dejo probado en la r34: no solo anaden
bytes, RECOLOCAN el pool). **Las que SOBRAN son cabeceras que incluimos de mas.**

Total: **faltan 118, sobran 154**.

```
zMain
   FALTAN (29): aud_moment_strm, aud_stitch_loop, audioimpact, audioscrape, brakes, chopperspecs, damagespecs, emitteruv, explosion, fecooling, frontend, fuelcell_effect, fuelcell_emitter, infractions, junkman, light_flares_cg, lightmaterials, lightshaders, milestonetypes, music, ocean, speech, system, timeofdaylighting, trafficpattern, visuallookeffect, visuallooktransition, visualrgbtweaker, world
   SOBRAN ( 0): -
zAnim
   FALTAN ( 9): audioimpact, audioscrape, audiosystem, camerainfo, ecar, effects, engineaudio, milestonetypes, speech
   SOBRAN ( 8): chassis, emitterdata, emittergroup, induction, nos, presetride, tires, transmission
zTrack
   FALTAN (11): aivehicle, audioimpact, audioscrape, audiosystem, effects, engineaudio, milestonetypes, pursuitescalation, pursuitlevels, pursuitsupport, speech
   SOBRAN ( 5): chassis, induction, nos, tires, transmission
zLua
   FALTAN ( 8): audioimpact, audioscrape, camerainfo, ecar, effects, milestonetypes, speechtune, world
   SOBRAN ( 8): chassis, emitterdata, emittergroup, induction, nos, presetride, tires, transmission
zWorld
   FALTAN ( 9): audioimpact, audioscrape, audiosystem, engineaudio, frontend, fuelcell_effect, milestonetypes, speech, world
   SOBRAN ( 6): chassis, induction, nos, presetride, smackable, transmission
zPlatform
   FALTAN ( 7): audioimpact, audioscrape, camerainfo, ecar, effects, milestonetypes, speech
   SOBRAN ( 6): chassis, induction, nos, presetride, tires, transmission
zGameplay
   FALTAN ( 7): audioimpact, audioscrape, camerainfo, ecar, effects, speechtune, world
   SOBRAN ( 6): chassis, induction, nos, presetride, tires, transmission
zCamera
   FALTAN ( 6): audioimpact, audioscrape, audiosystem, engineaudio, milestonetypes, speech
   SOBRAN ( 7): emitterdata, emittergroup, induction, nos, presetride, tires, transmission
zSpeech
   FALTAN ( 1): aud_moment_strm
   SOBRAN (11): acceltrans, chassis, emitterdata, emittergroup, induction, nos, presetride, shiftpattern, tires, transmission, turbosfx
zEAXSound2
   FALTAN ( 4): audioscrape, camerainfo, ecar, milestonetypes
   SOBRAN ( 8): chassis, emitterdata, emittergroup, induction, nos, presetride, tires, transmission
zWorld2
   FALTAN ( 5): audioscrape, milestonetypes, rigidbodyspecs, smackable, speech
   SOBRAN ( 6): chassis, engine, induction, nos, tires, transmission
zSim
   FALTAN ( 3): audioimpact, audioscrape, milestonetypes
   SOBRAN ( 8): chassis, emitterdata, emittergroup, induction, nos, presetride, tires, transmission
zFeOverlay
   FALTAN ( 5): audioimpact, audioscrape, effects, milestonetypes, speech
   SOBRAN ( 6): chassis, induction, nos, presetride, tires, transmission
zFe
   FALTAN ( 0): -
   SOBRAN (11): acceltrans, chassis, emitterdata, emittergroup, induction, nos, presetride, shiftpattern, tires, transmission, turbosfx
zAI
   FALTAN ( 6): audioimpact, audioscrape, audiosystem, effects, milestonetypes, world
   SOBRAN ( 5): emitterdata, emittergroup, induction, presetride, transmission
zFe2
   FALTAN ( 0): -
   SOBRAN (10): acceltrans, chassis, emitterdata, emittergroup, induction, nos, shiftpattern, tires, transmission, turbosfx
zEAXSound
   FALTAN ( 2): milestonetypes, smackable
   SOBRAN ( 8): chassis, emitterdata, emittergroup, induction, nos, presetride, tires, transmission
zRender
   FALTAN ( 0): -
   SOBRAN ( 9): chassis, effects, engine, induction, nos, presetride, pvehicle, tires, transmission
zPhysicsBehaviors
   FALTAN ( 6): audioimpact, audioscrape, audiosystem, milestonetypes, speech, world
   SOBRAN ( 3): emitterdata, emittergroup, presetride
zMiscSmall
   FALTAN ( 0): -
   SOBRAN ( 8): chassis, engine, induction, nos, pvehicle, simsurface, tires, transmission
zMisc
   FALTAN ( 0): -
   SOBRAN ( 7): chassis, induction, nos, presetride, smackable, tires, transmission
zEcstasy
   FALTAN ( 0): -
   SOBRAN ( 6): chassis, induction, nos, presetride, tires, transmission
zPhysics
   FALTAN ( 0): -
   SOBRAN ( 2): emitterdata, emittergroup
zDebug
   FALTAN ( 0): -
   SOBRAN ( 0): -
```

## Y la contraprueba del mapa de PS2

El `NFS.MAP` del alpha de PS2 (`orig/SLES-53558-A124/`) lista por objeto los
simbolos emitidos, y ahi salen los `ClassKey__Q36Attrib3Gen<clase>`. Su lista es
siempre un **subconjunto exacto** de la de GameCube (verificado en seis unidades):
PS2 dice las clases **usadas**, GameCube las **parseadas** --el
`USE_ATTRIB_ALLOC` interna la cadena aunque la rama este muerta--. Sirve para
saber cual se usa de verdad, no para la lista de `#include`.
