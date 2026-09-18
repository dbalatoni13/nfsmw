# -*- coding: utf-8 -*-
# Se ejecuta desde la RAIZ del repo:  python docs/analisis/<este fichero>
import sys, os, struct, collections, bisect, random
exec(open('docs/analisis/r63-veredicto-elfo.py', encoding='utf8').read())
# -*- coding: utf-8 -*-
"""perfil2.py -- igual que perfil.py pero ALINEANDO las dos secuencias (LCS sobre la
palabra con los campos de registro enmascarados). Separa: identicas, diferencia solo
de REGISTRO, y diferencia ESTRUCTURAL (instruccion de mas / de menos / otra)."""
from difflib import SequenceMatcher
L = [
 ('Speed/Indep/SourceLists/zCamera','Update__8ICEMoverf'),
 ('Speed/Indep/SourceLists/zCamera','__static_initialization_and_destruction_0'),
 ('Speed/Indep/SourceLists/zWorld2','HolePunchAvoidables__8WRoadNav'),
 ('Speed/Indep/SourceLists/zWorld','RenderFlaresOnCar__13CarRenderInfo'),
 ('Speed/Indep/SourceLists/zEagl4Anim','Initialize__Q25EAGL413DynamicLoader'),
 ('Speed/Indep/SourceLists/zEcstasy','epCalculateLocalDirectionalPOS16'),
 ('Speed/Indep/SourceLists/zEcstasy','UpdatePlatInfo__27eLightMaterialPlatInterface'),
 ('Speed/Indep/SourceLists/zPlatform','ActualReadJoystickData__Fv'),
 ('Speed/Indep/SourceLists/zEAXSound2','GenerateRoadNoise__16CARSFX_RoadNoise'),
 ('Speed/Indep/SourceLists/zCamera','Update__19TrackCarCameraMoverf'),
 ('LibSN/steering','SimThread_Step'),
 ('Speed/Indep/SourceLists/zWorld','UpdateWheelYRenderOffset__13CarRenderInfo'),
 ('Speed/Indep/SourceLists/zPhysicsBehaviors','UpdateLoaded__Q217SuspensionTraffic4Tire'),
 ('Speed/Indep/SourceLists/zWorld2','InitAtSegment__8WRoadNav'),
 ('Packages/vp6/1.0.6/source/decode/gc/criticalpath','VP6_PredictFilteredBlock'),
 ('Speed/Indep/SourceLists/zTrack','GetLoadingPriority__13TrackStreamer'),
 ('Speed/Indep/SourceLists/zWorld','DefragmentPool__9CarLoader'),
 ('egami/rcmp/dev/source/decoder/cmn/madidct','IdctColumn'),
 ('Speed/Indep/SourceLists/zSpeech','Setup__Q26Speech13RoadblockFlow'),
 ('LibSN/steering','HandleTriggers'),
 ('egami/rcmp/dev/source/decoder/cmn/madidct','IdctRow'),
 ('Speed/Indep/SourceLists/zEagl4Anim','EvalState__Q29EAGL4Anim14FnRawStateChan'),
 ('LibSN/steering','SimThread_Init'),
 ('Speed/Indep/SourceLists/zWorld','SetMemoryPoolSize__9CarLoaderi'),
 ('LibSN/steering','Effect_Init'),
 ('Speed/Indep/SourceLists/zEcstasy','eProject__FfffPA3_fPfN44'),
 ('LibSN/steering','CookValues'),
 ('LibSN/steering','Effect_PerformEnvelope'),
]


def mask(w):
    op=w>>26
    if op in (31,19,63,59): return (w & 0xfc0007fe)
    if op in (16,18):       return (w & 0xfc000003)
    return (w & 0xfc00ffff)

def words(path,pref):
    o=Obj(path); fs=o.funcs()
    m=[x for x in fs if x['name'].startswith(pref)]
    if not m: return None
    f=m[0]; sec=o.sec('.text'); d=o.b[sec['off']:sec['off']+sec['size']]
    return [struct.unpack_from('>I',d,x)[0] for x in range(f['value'],f['value']+f['size'],4)]

print('%-44s %5s %5s %5s %5s %5s  %s'%('funcion','insn','ident','reg','estr','%estr','veredicto'))
solo=[]; estr=[]
for unit,fn in L:
    wa=words('build/GOWE69/obj/%s.o'%unit,fn); wb=words('build/GOWE69/src/%s.o'%unit,fn)
    if wa is None or wb is None: print(fn,'NO'); continue
    ma=[mask(x) for x in wa]; mb=[mask(x) for x in wb]
    sm=SequenceMatcher(None,ma,mb,autojunk=False)
    ident=reg=0; est=0
    for tag,i1,i2,j1,j2 in sm.get_opcodes():
        if tag=='equal':
            for k in range(i2-i1):
                if wa[i1+k]==wb[j1+k]: ident+=1
                else: reg+=1
        else:
            est+=max(i2-i1,j2-j1)
    n=len(wa)
    v='SOLO REGISTROS' if est==0 else 'estructura'
    (solo if est==0 else estr).append(fn)
    print('%-44s %5d %5d %5d %5d %5.1f  %s'%(fn[:44],n,ident,reg,est,100.0*est/n,v))
print('\nSOLO REGISTROS/PLANIFICACION: %d de %d'%(len(solo),len(L)))
print('con diferencia estructural  : %d'%len(estr))
