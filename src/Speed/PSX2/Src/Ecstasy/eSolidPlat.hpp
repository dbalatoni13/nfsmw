
#ifndef ESOLIDPLAT_HPP
#define ESOLIDPLAT_HPP

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

// TODO: these are from ps3 carbon and probably arent accurate

#define CURRENT_ESOLID_PLAT_VERSION 18   //  :12
#define ESTRIP_SKINNED 0x0001            //  :16
#define ESTRIP_UCAP 0x0002               //  :17
#define ESTRIP_GLOSSMAPINALPHA 0x0004    //  :18
#define ESTRIP_OCEAN 0x0008              //  :19
#define ESTRIP_GLOSSYWINDOWS 0x0010      //  :20
#define ESTRIP_SKY 0x0020                //  :21
#define ESTRIP_ROAD_SPECULAR 0x0040      //  :22
#define ESTRIP_LIGHT 0x0080              //  :23
#define ESTRIP_ENVMAP 0x0100             //  :24
#define ESTRIP_CLIP 0x0200               //  :25
#define ESTRIP_DISABLE_RENDERING 0x0400  //  :28
#define ESTRIP_DAMAGE_POLY 0x0800        //  :31
#define ESTRIP_SINGLE_SIDED 0x1000       //  :34
#define ESTRIP_PACKED_XYZ 0x2000         //  :35
#define ESTRIP_HAS_NORMALS 0x4000        //  :36
#define ESTRIP_STRIP_LOCALLY 0x8000      //  :37
#define ESTRIP_NORMALMAP 0x00020000      //  :40
#define ESTRIP_HEIGHTMAP 0x00040000      //  :41
#define ESTRIP_SPECULARMAP 0x00080000    //  :42
#define ESTRIP_GLOSSINESSMAP 0x00100000  //  :43
#define ESTRIP_LIGHTNORMALMAP 0x00800000 //  :44
#define ESTRIP_LIGHTMAP 0x02000000       //  :45
#define MAX_BONES_PER_MESH 16            //  :64
#define MAX_VERTEX_BUFFER_PER_SOLID 16   //  :66
#define MAX_VERTEX_STREAMS 1             //  :95

extern bTList<eSolid> SolidList;

struct eStripEntry {         // 0x10
    uint16 Flags;            /* 0x0 */
    uint8 NumVerts;          /* 0x2 */
    uint8 NumPolys;          /* 0x3 */
    float MaterialDensity;   /* 0x4 */
    uint16 DataQWordOffset;  /* 0x8 */
    int8 Nz;                 /* 0xa */
    int8 Nt;                 /* 0xb */
    uint8 DataQWordSize;     /* 0xc */
    uint8 PolyGroupNumber;   /* 0xd */
    int8 TextureNumber;      /* 0xe */
    int8 LightMaterialIndex; /* 0xf */
};

// total size: 0x24
// Decl: speed/ps3/src/ecstasy/esolidplat.hpp:139
class eSolidPlatInfo {
  public:
    uint16 Version;    // offset 0x0, size 0x2, Decl: speed/ps3/src/ecstasy/esolidplat.hpp:141
    uint16 StripFlags; // offset 0x2, size 0x2
    uint16 NumStrips;  // offset 0x4, size 0x2
    uint16 pad0;
    uint32 SizeofStripData;       // offset 0x8, size 0x4
    eStripEntry *StripEntryTable; // offset 0x1C, size 0x4
    uint8 *StripDataStart;        // offset 0x20, size 0x4
};

#endif
