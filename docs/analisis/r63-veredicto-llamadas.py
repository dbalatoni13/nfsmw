# -*- coding: utf-8 -*-
# Se ejecuta desde la RAIZ del repo:  python docs/analisis/<este fichero>
import sys, os, struct, collections, bisect, random
exec(open('docs/analisis/r63-veredicto-elfo.py', encoding='utf8').read())
# -*- coding: utf-8 -*-
"""llamadas2.py -- cuenta INSTRUCCIONES bl (opcode 18, LK=1) por funcion y resuelve
el destino, en el objeto EXTRAIDO del original y en el NUESTRO. Sin Ghidra."""
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


def calls(path, pref):
    o=Obj(path); fs=o.funcs()
    m=[x for x in fs if x['name'].startswith(pref)]
    if not m: return None,None
    f=m[0]; a,b=f['value'],f['value']+f['size']
    sec=o.sec('.text'); data=o.b[sec['off']:sec['off']+sec['size']]
    rel={}
    for off,si,typ,add in o.relocs('.text'):
        if typ==10: rel[off]=(o.syms[si],add)
    lo=[x['value'] for x in fs]
    c=collections.Counter(); n=0
    for off in range(a,b,4):
        w=struct.unpack_from('>I',data,off)[0]
        if (w>>26)==19 and (w&1)==1:             # bclrl / bcctrl: llamada INDIRECTA
            xo=(w>>1)&0x3ff
            c['<indirecta blrl>' if xo==16 else '<indirecta bctrl>']+=1
            continue
        if (w>>26)!=18 or (w&1)!=1: continue     # bl = I-form, LK=1
        n+=1
        nm=None
        if off in rel:
            sy,add=rel[off]
            if sy['name'] and sy['typ']!=3: nm=sy['name']
            else:
                i=bisect.bisect_right(lo,add)-1
                if 0<=i<len(fs) and add<fs[i]['value']+fs[i]['size']: nm=fs[i]['name']
        else:
            d=w&0x03fffffc
            if d&0x02000000: d-=0x04000000
            t=off+d
            i=bisect.bisect_right(lo,t)-1
            if 0<=i<len(fs) and t<fs[i]['value']+fs[i]['size']: nm=fs[i]['name']
        c[nm or '???']+=1
    return f['size'], c

print('%-44s %6s %6s %5s %5s  %s'%('funcion','tam_o','tam_n','bl_o','bl_n','veredicto'))
ok=dif=0
for unit,fn in L:
    so,co=calls('build/GOWE69/obj/%s.o'%unit, fn)
    sn,cn=calls('build/GOWE69/src/%s.o'%unit, fn)
    if co is None or cn is None:
        print('%-44s NO ENCONTRADA'%fn[:44]); continue
    d=[k for k in set(co)|set(cn) if co[k]!=cn[k]]
    v='IGUAL' if not d else 'DIFIERE'
    ok+= (not d); dif+= bool(d)
    print('%-44s %6d %6d %5d %5d  %s'%(fn[:44],so,sn,sum(co.values()),sum(cn.values()),v))
    for k in sorted(d): print('      %-52s obj %d / nue %d'%(k[:52],co[k],cn[k]))
print('\nIGUAL %d  DIFIERE %d'%(ok,dif))
