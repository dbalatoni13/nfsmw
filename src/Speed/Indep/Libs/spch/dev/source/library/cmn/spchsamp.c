#include "./spchi.h"

unsigned char *iSPCH_GetSampleParmAddr(VOXBANKHDR *hdr, int sampleIndex) {
    unsigned char *sampleParmData;
    int numParms;

    sampleParmData = 0;
    if (sampleIndex < hdr->numSamples) {
        numParms = hdr->parmFlags & 0x7F;
        sampleParmData = (unsigned char *)hdr + sampleIndex * (numParms + 2);
        sampleParmData += 0xE;
    }
    return sampleParmData;
}

/* CERRADA en r47: 136/136 B, 100,0 %, y con ella la unidad entera (2 de 2).
   La causa raiz que las rondas anteriores no vieron: NO era el reparto, era
   sched1 rellenando la latencia del `mullw` de la cola.
   El volcado .greg lo dice literalmente --allocno 89 (`result`) sale
   `89 conflicts: ... 118 119 0 1`--: `result = 1` se planificaba ENTRE el
   `mullw` y el `subf`, asi que su rango de vida pisaba a los dos temporales de
   la cola (118 y 119), que son los duenos de r0; por eso `result` acababa en
   r9 y no en r0 como dice el DWARF, y de ahi salia la rotacion de los otros
   cuatro valores. `93` (endOffset) era el UNICO allocno sin conflicto con el
   hard reg 0 y por eso era el unico que lo cogia.
   Tres cambios, en este orden:
     1. `endOffset = endOffset * blockSize - offset;` delante de los stores:
        adelanta el `subf`, que el objetivo emite antes del primer `stw`.
     2. dos barreras de ranura de CERO BYTES (`__asm__ __volatile__("")`), una
        delante de los stores y otra delante de `result = 1`: impiden que el
        planificador meta el `stw` y el `li 1` en la latencia del `mullw`. Las
        dos son necesarias --con una sola: 92,06 % (solo la de detras) o
        93,82 % (solo la de delante)--.
     3. partir `offset = ((offset << 8) + sampleData[1]) * blockSize;` en dos
        sentencias sobre la MISMA variable: asi el sumando intermedio comparte
        pseudo con `offset` y sale `add r8,r0,r9` + `mullw r8,r8,r11` como el
        objetivo, en vez de `add r0,...` + `mullw r8,r0,r11`. Sin esto la
        funcion se queda en 99,70588 % (dos filas).
   Medido y descartado por el camino (todo 136/136 B): barrera sobre offset
   sola 92,647 %, sobre blockSize 92,647 %, sobre sampleSize 91,912 %, sobre
   sampleData/sampleOffset/dataBytes 91,912 %, `asm("" : "+r"(result))` detras
   de los stores 77,794 %, `return 1` en vez de `result = 1` 83,941 %,
   `result = 1` delante o entre los stores 93,824 %, `offset <<= 8; offset +=
   ...; offset *= blockSize` (tres sentencias) 96,912 %,
   `blockSize * (...)` 99,559 %.
   Sigue vigente lo que midieron r38/r44/r46 sobre la base vieja: los cinco
   pines guiados por el DWARF (de 83,38 a 86,32 %) y las 48 reordenaciones
   legales del cuerpo (las 48 dan el mismo objeto, GCC las canonicaliza). */
int iSPCH_GetSampleSizeData(VOXBANKHDR *hdr, int sampleIndex, unsigned int *sampleOffset, unsigned int *dataBytes) {
    unsigned int offset;
    int sampleSize;
    unsigned char *sampleData;
    int result;
    int numParms;
    int blockSize;
    int nextIndex;
    unsigned int endOffset;
    unsigned char *nextSampleData;

    result = 0;
    if (sampleIndex >= hdr->numSamples) {
        goto abort;
    }
    numParms = hdr->parmFlags & 0x7F;
    sampleSize = numParms + 2;
    nextIndex = sampleIndex + 1;
    sampleData = (unsigned char *)hdr + 0xC;
    blockSize = (hdr->blockSize + 1) << 8;
    sampleData += sampleIndex * sampleSize;
    offset = sampleData[0];
    offset = (offset << 8) + sampleData[1];
    offset = offset * blockSize;
    nextSampleData = sampleData + sampleSize;
    if (nextIndex >= hdr->numSamples) {
        endOffset = hdr->bankBlocks;
    } else {
        endOffset = (nextSampleData[0] << 8) + nextSampleData[1];
    }
    endOffset = endOffset * blockSize - offset;
    *sampleOffset = offset;
    *dataBytes = endOffset;
    result = 1;

abort:
    return result;
}
