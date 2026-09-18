#include "pathi.h"

using namespace Path;

unsigned int PATHI_sampleoffset(int node)
{
    unsigned int offset = 0;
    int sampleindex;
    PATHFINDNODE *nodeinfo;

    if (node < 0 || node >= pfstate->pmap->numnodes)
        return 0;

    nodeinfo = PATHI_getnode(node);

    if (nodeinfo->index > 0)
    {
        sampleindex = nodeinfo->index - 1;
        offset = pfstate->psampleoffsets[sampleindex].offset;
    }

    return offset;
}

int PATHI_beatinfo(PATHTRACK *track, PATHBEATINFO *beatinfo)
{
    PATHFINDNODE *nodeinfo;
    PATHSTATUS status;

    if (track->node < 0)
        return -1;

    nodeinfo = PATHI_getnode(track->node);

    PATHI_status(track, &status);

    if (beatinfo)
    {
        beatinfo->beats = nodeinfo->beats;
        beatinfo->notes = nodeinfo->notes;
        beatinfo->playingbeat = status.playingbeat;
        beatinfo->beatduration = status.beatduration;
        beatinfo->barduration = status.barduration;
        beatinfo->nodeduration = status.nodeduration;
        beatinfo->timetonextbar = status.timeleftinbar;
        beatinfo->timetonextbeat = status.timeleftinbeat;
        beatinfo->timetonextnode = status.timeleftinnode;
    }

    return status.playingbeat;
}

int PATHI_calcwaitbeat(int every, int note, int offset, PATHBEATINFO *beatinfo)
{
    float scalar;
    float fevery;
    float foffset;
    int timeinbar;
    int firstsynchtime;
    int nextsynchtime;

    scalar = (float)beatinfo->notes / (float)note;
    fevery = (float)every;
    foffset = (float)offset;

    timeinbar = beatinfo->barduration - beatinfo->timetonextbar;

    firstsynchtime = (int)(scalar * foffset * (float)beatinfo->beatduration);
    scalar = scalar * fevery;

    nextsynchtime = firstsynchtime;
    if (nextsynchtime < timeinbar)
    {
        while (nextsynchtime < (int)beatinfo->barduration)
        {
            nextsynchtime += (int)(scalar * (float)beatinfo->beatduration);

            if (nextsynchtime > timeinbar)
                break;
        }
    }

    if (nextsynchtime > (int)beatinfo->barduration)
        nextsynchtime = beatinfo->barduration + firstsynchtime;

    return nextsynchtime - timeinbar;
}

int PATHI_choosesynchtime(int node, const PATHFINDNODE &entryinfo, const PATHBEATINFO &masterinfo,
                          unsigned int &waitms)
{
    PATHFINDNODE *nodeinfo;
    bool forcesynch;
    float beatlen;
    int nodebeat;
    int elapsedtime;
    int overbeatsleft;
    int overbeatsdone;

    if ((short)entryinfo.partID < 0 || node < 0)
        return 1;

    nodeinfo = PATHI_getnode(node);

    PATHFINDSAMPLE oversample = pfstate->psampleoffsets[nodeinfo->index - 1];

    forcesynch = nodeinfo->extra.beat.forcesynch;

    beatlen = (float)oversample.duration;
    if (nodeinfo->beats)
        beatlen = beatlen / (float)(nodeinfo->beats * nodeinfo->bars);

    if (forcesynch)
        beatlen = (float)masterinfo.beatduration;

    elapsedtime = masterinfo.nodeduration - masterinfo.timetonextnode;

    nodebeat = -1;

    overbeatsleft = (int)((float)masterinfo.timetonextnode / beatlen);
    overbeatsdone = (int)((float)elapsedtime / beatlen);

    switch (entryinfo.synch)
    {
    case 1:
        overbeatsdone++;
        waitms = (int)((float)overbeatsdone * beatlen) - elapsedtime;
        overbeatsdone = overbeatsdone % nodeinfo->beats;
        nodebeat = overbeatsdone + 1;
        break;

    case 2:
        waitms = masterinfo.timetonextnode % (int)((float)overbeatsleft * beatlen);
        nodebeat = nodeinfo->beats - (overbeatsleft % nodeinfo->beats) + 1;
        break;

    case 3:
        waitms = (int)((float)(overbeatsdone + 1) * beatlen) - elapsedtime;
        nodebeat = 1;
        break;
    }

    if (waitms)
        waitms = waitms + milliseconds;

    return nodebeat;
}

int PATHI_timeremaining(PATHTRACK *track)
{
    int timeremaining = 0;
    int activerequests = 0;

    {
        int i;

        for (i = 0;; i++)
        {
            int requesttime = track->trackimp->TimeRemaining(i);

            if (requesttime >= 0)
                timeremaining += requesttime;
            else
                break;
        }
    }

    return timeremaining;
}

int PATHI_pickclosestbranch(int numBranches, int control, PATHFINDBRANCH *branch)
{
    int below;
    int above;
    int distance;
    int closest;
    PATHFINDBRANCH *bestBranch = branch;
    int i;

    if (numBranches <= 0)
        return -1;

    closest = 127;

    for (i = 0; i < numBranches; i++)
    {
        below = branch[i].controlmin - control;
        above = branch[i].controlmax - control;

        if (below < 0)
            below = -below;

        if (above < 0)
            above = -above;

        distance = above;
        if (above > below)
            distance = below;

        if (closest > distance)
        {
            closest = distance;
            bestBranch = &branch[i];
        }
    }

    return bestBranch->dstnode;
}

/* UNSOLVED, 300 B contra 308 al 93,05 %. Con PATHI_calcwaitbeat (98,87 %) son
   las dos que le faltan a pathnode; cerrarlas promociona la unidad, 4.204 B.

   NO es desorden: textorder dice "4 de 12 descolocadas", pero todas las
   direcciones casan hasta esta funcion y a partir de ella todo va 8 B corrido.
   Es el patron de filesys --una funcion corta que desliza al resto--, asi que
   esa columna hay que leerla con el delta de tamano al lado.

   Es un INTERCAMBIO DE REGISTROS, y el DWARF lo nombra: el objetivo pone el
   parametro forreal en r31 y la local nextnode en r12; nosotros forreal en r12
   y nextnode en r6. Los 8 B que faltan son justo lo que el objetivo paga por
   usar un salvado (stw r31 mas marco 0x10 en vez de 0x8). Y r31 se lee UNA vez
   (cmpwi r31,0) con la unica llamada al final: forreal no cruza ninguna
   llamada, o sea que el objetivo gasta un salvado sin necesitarlo.

   ALLOC.PY sobre volcados RTL propios (cc1plus -dl -dg via lreg.dumps_for).
   De los 28 pseudos que llegan a global_alloc, los dos ultimos son estos:

     #26  pseudo 89   6 refs  live 130  pri 923  -> r6    nextnode
     #27  pseudo 84   2 refs  live  34  pri 588  -> r12   forreal

   y alloc.py confirma que la formula reproduce el orden del compilador. Para
   que forreal adelante a nextnode le hacen falta 4 refs
   (floor_log2(4)*4/34*10000 = 2352 > 923); con 3 se queda en 882.

   PROBADO Y REFUTADO: subiendo n_refs con asm selectivo sobre forreal --que
   cuenta como referencia sin emitir un byte-- una ref extra da 93,18 % y DOS O
   TRES lo bajan a 90,52 %; ninguna mueve los 300 B. La diferencia no es la
   prioridad relativa de esos dos: es el estado entero de la asignacion.

   Formas medidas y descartadas (todas 93,05 % / 300 B salvo donde se diga):
   nextnode declarada la primera, nextnode sin inicializador, forreal a una
   local propia, barrera total tras leerlo, i declarada despues de nextnode,
   forreal probado dentro del if externo, y +1/+2/+3 refs. EMPEORA invertir el
   && a forreal && nodeinfo->repeat > 0: 91,43 %.

   El eje del reparto queda cerrado desde la fuente. */
/* VEDA r36e, 300 B contra 308 al 93,05 %. El objetivo usa UN PRESERVADO
   MAS: guarda el tercer parametro en r31 (marco 0x10, `stw r31,0xc(r1)`) y
   nosotros lo dejamos en r12, un volatil (marco 0x8). Ademas conserva
   `pfstate` en r6 y nosotros lo recargamos.
   Con una barrera de solo lectura sobre `forreal` al final --que lo mantiene
   vivo y obliga al preservado-- se llega al TAMANO EXACTO, 308/308 y 95,88 %,
   y quedan solo DOS filas reales: falta el `mr r6,r9` que copia pfstate y
   sobra un `lwz r8,0x0(r7)`. Pero cero bytes es cero bytes y no se deja
   puesta: seria deuda de asm.
   Barrido y medido: lectura de `forreal` (308, 2 filas), de `forreal` mas
   `pfstate` (312), de `control` (316), barrera sobre `pfstate` o sobre `track`
   detras de su uso (312 las dos), las dos barreras al final (312), y la
   cantidad fantasma con `register int guard asm("r31")` (300, neutra).
   O sea que la palanca del preservado esta encontrada y lo que falta es la
   copia de pfstate, que ninguna de las siete formas trae.

   R46. Reproducida la base de r36e (barrera de solo lectura sobre `forreal` al
   final: 308/308 B, 95,88312 %) y AVANZADA con el pin que el DWARF pide:
   `register int nextnode asm("r12") = -1;` sube a 96,207794 % --308/308-- y
   deja SOLO la copia de pfstate: sobra nuestro `lwz r9,_4Path.pfstate@sda21`
   (indice 25) y falta el `mr r6,r9` del objetivo (indice 10), mas los dos usos
   que cuelgan de el. El pin SOLO, sin la barrera, da 300 B / 93,376625 %.
   Medido y neutro (todo 96,207794 % sobre la base con barrera+pin):
   `Path::pfstate` explicito, `pfstate_agg[0]` para el acceso a `track`, y una
   local `PATHTRACK **tracks = pfstate->track`. Intercambiar las sentencias
   `branches`/`track` no mueve NADA (93,05195 % sin barrera).
   CFLAGS descartados sobre esa base --ninguno cambia el 96,207794 %--:
   `-fforce-mem`, `-fgcse`, `-fcse-skip-blocks`. Y son NOCIVOS:
   `-fno-cse-follow-jumps` 316 B / 94,8442 % y ademas rompe `PATHI_seeknextnode`;
   `-fno-schedule-insns2` 92,3117 % y tambien rompe `seeknextnode`;
   `-fno-strength-reduce` rompe CUATRO funciones de la unidad. Los cflags de
   `path` estan bien.
   La copia de pfstate nace dentro del inline `PATHI_getnode` de `pathi.h`
   (cabecera COMPARTIDA, fuera de territorio): el objetivo saca de CSE una
   COPIA para la tercera referencia a `Path::pfstate` y nosotros fundimos las
   tres en un pseudo y recargamos. Ahi esta la siguiente palanca.
   No se deja puesto nada: barrera y pin juntos son cero bytes.

   R47 --- LA CAUSA, CON LA LINEA DE GCC. Reproducida la base de r46 (barrera
   de lectura sobre `forreal` al final + `register int nextnode asm("r12")`):
   308/308 B, 96,207794 %, SIETE filas. Lo unico que separa a las dos versiones
   es que el objetivo tiene UNA sola carga de `_4Path.pfstate` (indice 8) y una
   COPIA `mr r6,r9` (indice 10) que sobrevive hasta `addi r9,r6,0x40`
   (`pfstate->track`), y nosotros tenemos DOS cargas: la del inline (8) y una
   REMATERIALIZACION `lwz r9,_4Path.pfstate@sda21` en el indice 25.
   El `mr` del objetivo es la copia que emite `pre_insert_copies` de gcse.c; la
   rematerializacion nuestra es `update_equiv_regs` de local-alloc.c, cuya
   condicion esta escrita literal en el fuente del arbol
   (orig/prodg/NGC_GNU_SRC/NGC/gcc/local-alloc.c, dentro de update_equiv_regs):
       if (REG_N_REFS (regno) == 2 && REG_BASIC_BLOCK (regno) < 0
           && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
         reg_equiv_replace[regno] = 1;
   es decir: un pseudo con UNA definicion y UN uso que cruza de bloque y cuya
   fuente es la MEM equivalente se sustituye por la MEM. `pfstate` esta en la
   SDA, asi que su carga es una sola instruccion y la sustitucion sale gratis
   para GCC. Para romperla hace falta un tercer uso del pseudo, y en esta
   funcion no hay ninguno que no cueste una instruccion.
   Medido en r47 sobre la base de r46 (todas 308/308 B, 7 filas = sin cambio):
   local `PATHFINDERSTATE *pfs` detras del check de nodeinfo, `Path::pfstate`
   explicito, `pfstate_agg[0]` para `track`, `track` antes de `branches`,
   partir el acceso a `track` en dos sentencias, y `asm("" : "+r"(pfs))` sobre
   la local. PEORES: `pfs` declarada arriba del todo 304 B / 9 filas;
   `asm("" : : "r"(pfstate))` detras del acceso 312 B / 16 filas, delante
   312 B / 15, antes de `branches` 312 B / 13 --el asm carga otra vez, no es de
   cero bytes--; barrera de ranura (`asm volatile("")`) delante de `track`
   13 filas y detras 15; `asm("" : "+r"(track))` 15 filas; leer `pfstate` por
   puntero volatil 312 B / 14 filas.
   LA PALANCA SIGUE FUERA DE TERRITORIO: el `mr` nace dentro del inline
   `PATHI_getnode` de pathi.h, que comparten las 13 unidades de `path`. */
int PATHI_nextnode(int node, int control, int forreal)
{
    PATHFINDNODE *nodeinfo;
    PATHFINDBRANCH *branches;
    PATHTRACK *track;
    unsigned int i;
    int nextnode = -1;

    if (node < 0)
        return -1;

    nodeinfo = PATHI_getnode(node);
    if (!nodeinfo)
        return -1;

    branches = (PATHFINDBRANCH *)(nodeinfo + 1);
    track = pfstate->track[nodeinfo->trackID];

    if (track->repeatnode == node)
    {
        control = track->repeat & 0x7F;

        if (nodeinfo->repeat > 0 && forreal)
        {
            if (--track->repeat == -1)
                track->repeatnode = -1;
        }
    }

    for (i = 0; i < nodeinfo->numbranches; i++)
    {
        if (control < branches[i].controlmin)
            continue;
        if (control > branches[i].controlmax)
            continue;

        nextnode = (short)branches[i].dstnode;
        break;
    }

    if (nextnode < 0)
        nextnode = PATHI_pickclosestbranch(nodeinfo->numbranches, control, branches);

    return nextnode;
}

int PATHI_enternode(int origin, int node, int control, int forreal)
{
    PATHFINDNODE *nodeinfo;
    PATHTRACK *track;
    PATHEVENT *eventp;
    int savecontrol = control;

    if (node < 0)
        return -1;

    node = (short)PATHI_routenode(origin, node);
    if (node < 0)
        return -1;

    nodeinfo = PATHI_getnode(node);
    if (!nodeinfo)
        return -1;

    track = pfstate->track[nodeinfo->trackID];

    while (nodeinfo->index <= 0)
    {
        if (forreal)
        {
            if (nodeinfo->index == 0)
            {
                track->entryinfo = nodeinfo;

                if (nodeinfo->controller == 1)
                    control = PATHI_random() & 0x7F;

                if (songprogress)
                    songprogress(pfstate->idflags, node);
            }
            else if (nodeinfo->index == -1)
            {
                if (nodeinfo->repeat)
                {
                    if (node != track->repeatnode)
                    {
                        track->repeat = nodeinfo->repeat;
                        track->repeatnode = node;
                    }
                }
            }
            else if (nodeinfo->index == -2)
            {
                control = PATHI_random() & 0x7F;
            }
            else if (nodeinfo->index == -3)
            {
                eventp = PATHI_getevent(nodeinfo->extra.sendevent.eventID, PATH_ALL);

                if (eventp)
                {
                    eventp = PATHI_copyevent(eventp);

                    if (eventp)
                        PATHI_addevent(pfstate->idflags | PATH_ALL_VOICES, eventp);
                }
            }
        }

        node = PATHI_nextnode(node, control, forreal);
        if (node >= 0)
            node = (short)PATHI_routenode(origin, node);

        if (node < 0)
        {
            if (songprogress && forreal && origin >= 0)
                songprogress(pfstate->idflags, node);

            return -1;
        }

        nodeinfo = PATHI_getnode(node);
        if (!nodeinfo)
            return -1;

        control = savecontrol;
    }

    return (short)PATHI_routenode(origin, node);
}

int PATHI_routenode(int fromnode, int destnode)
{
    int *router;
    unsigned int routerID;

    if (fromnode < 0)
        return destnode;

    routerID = PATHI_getnode(fromnode)->routerID;
    if (routerID == 0)
        return destnode;

    router = (int *)pfstate->pmap + pfstate->prouters[routerID - 1];

    {
        int i;

        for (i = 0; i < pfstate->prouters[routerID] - pfstate->prouters[routerID - 1]; i++)
        {
            if (destnode == (router[i] >> 16))
                destnode = router[i] & 0xFFFF;
        }
    }

    return destnode;
}

void PATHI_seeknextnode(int trackindex)
{
    PATHTRACK *track;
    PATHTRACK savetrack;
    PATHFINDNODE *nodeinfo;
    int nodebeat;
    int nextnode;
    int savenumevents;
    int result;

    nextnode = -1;
    savenumevents = pfstate->eventindex;
    track = pfstate->track[trackindex];

    if (track->node < 0)
        return;

    savetrack = *track;

    nodeinfo = PATHI_getnode(track->node);

    nodebeat = -1;
    if (track->ramtrack)
        nodebeat = 1;

    if (track->nodebeat > 0 && nodeinfo->extra.beat.playbeats &&
        track->nodebeat <= (int)(nodeinfo->beats * nodeinfo->bars))
    {
        nodebeat = track->nodebeat;
        nextnode = track->node;
    }
    else if (track->node >= 0)
    {
        nextnode = PATHI_nextnode(track->node, track->control, 1);

        if (songprogress && nextnode < 0)
            songprogress(pfstate->idflags, nextnode);
    }

    if (nextnode >= 0)
        nextnode = PATHI_enternode(track->node, nextnode, track->control, 1);

    track->node = nextnode;

    track->nodebeat = (nextnode < 0) ? -1 : (signed char)nodebeat;

    result = PATHI_queuenode(track);
    if (result < 0 && result != PATHERR_PENDING)
    {
        pfstate->eventindex = savenumevents;
        *track = savetrack;
    }
}

int PATHI_queuenode(PATHTRACK *track)
{
    PATHTRACK *mastertrack = 0;
    PATHFINDNODE *nodeinfo;
    PATHBEATINFO masterinfo;
    int playbeat = 0;
    int duration = 0;
    int holdtime;
    int sampleoffset;

    if (pfstate->mastertrack >= 0)
        mastertrack = pfstate->track[pfstate->mastertrack];

    if (track->node < 0)
    {
        track->entryinfo = 0;
        track->newestrequesthandle = -1;
        track->node = -1;
        track->nodebeat = -1;

        return PATH_OK;
    }

    nodeinfo = PATHI_getnode(track->node);

    track->entryinfo = PATHI_getnode(nodeinfo->partID);

    duration = pfstate->psampleoffsets[nodeinfo->index - 1].duration;
    track->bartime = duration;
    track->beattime = duration;

    if (nodeinfo->bars)
        track->bartime = duration / nodeinfo->bars;

    if (nodeinfo->beats)
        track->beattime = track->beattime / (nodeinfo->beats * nodeinfo->bars);

    if (!track->ramtrack)
    {
        track->nodebeat = -1;
    }
    else
    {
        bool havemastertrack = (mastertrack != 0 && mastertrack != track);
        bool forcesynch = false;
        bool playbeats;

        if (track->entryinfo->extra.beat.forcesynch && havemastertrack)
            forcesynch = true;

        playbeats = track->entryinfo->extra.beat.playbeats;

        masterinfo.timetonextbeat = 0;
        masterinfo.beatduration = 0;

        if (havemastertrack)
            PATHI_beatinfo(mastertrack, &masterinfo);

        if (track->nodebeat < 0)
        {
            if (track->entryinfo->synch && havemastertrack)
            {
                track->nodebeat = PATHI_choosesynchtime(track->node, *track->entryinfo, masterinfo,
                                                        track->nextbeattime);
                track->nextbeattime -= pfstate->timerinterval * 5;

                return PATH_OK;
            }
        }
        else if (forcesynch)
        {
            if (masterinfo.timetonextbeat >= pfstate->timerinterval &&
                masterinfo.timetonextbeat < masterinfo.beatduration / 2)
                return PATHERR_PENDING;
        }

        if (track->nodebeat <= 0)
            track->nodebeat = 1;

        track->nextbeattime = 0;
        playbeat = track->nodebeat;

        if (forcesynch && masterinfo.beatduration)
        {
            if (masterinfo.timetonextbeat < pfstate->timerinterval)
                masterinfo.timetonextbeat = masterinfo.timetonextbeat + masterinfo.beatduration;

            track->nextbeattime = masterinfo.timetonextbeat + milliseconds;
            track->nextbeattime -= pfstate->timerinterval * 5;
        }

        if (playbeats)
        {
            int lastbeat = track->nodebeat;

            track->nodebeat = lastbeat + 1;

            if (lastbeat < (int)(nodeinfo->beats * nodeinfo->bars))
                goto beatdone;
        }

        track->nodebeat = 0;

    beatdone:

        if (playbeats)
            duration = duration / (nodeinfo->beats * nodeinfo->bars);
    }

    holdtime = 0;
    sampleoffset = PATHI_sampleoffset(track->node);

    return track->newestrequesthandle =
               track->trackimp->Play(track->node, sampleoffset, playbeat, holdtime, duration);
}

/* 0x80413A68..0x80413A70 -- los 8 B de cola del hueco `auto_05_80413A48_rodata`.
   El troceador metio la cuarta constante de esta unidad (0x80413A60) y estos 8 B
   en un mismo simbolo `lbl_80413A60` de 0x10 B.  Al promocionar, nuestro objeto
   emite su propio pool ($LC0..$LC3, 32 B) y deja de referenciar el hueco: nadie
   lo referencia ya, `-strip-unused-data` se lleva sus 40 B y el `.rodata`
   enlazado sale -8 B (medido con scripts/linkdelta.py).  */
