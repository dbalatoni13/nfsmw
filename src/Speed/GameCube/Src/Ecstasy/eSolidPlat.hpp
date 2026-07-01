
#ifndef ESOLIDPLAT_HPP
#define ESOLIDPLAT_HPP

#include "types.h"

class eSolid;

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

// total size: 0x10
struct eStripEntry {
    uint32 DataOffset;          // offset 0x0, size 0x4
    uint16 DataSize;            // offset 0x4, size 0x2
    uint16 Flags;               // offset 0x6, size 0x2
    uint8 NumVerts;             // offset 0x8, size 0x1
    uint8 PolyGroupNumber;      // offset 0x9, size 0x1
    int8 TextureNumber;         // offset 0xA, size 0x1
    int8 LightMaterialIndex;    // offset 0xB, size 0x1
    uint8 VertexDescription;    // offset 0xC, size 0x1
    uint8 VertexFormat;         // offset 0xD, size 0x1
    uint16 DataDisplayListSize; // offset 0xE, size 0x2
};

// total size: 0x24
// Decl: speed/ps3/src/ecstasy/esolidplat.hpp:139
class eSolidPlatInfo {
  public:
    uint16 Version;               // offset 0x0, size 0x2, Decl: speed/ps3/src/ecstasy/esolidplat.hpp:141
    uint16 StripFlags;            // offset 0x2, size 0x2
    uint16 NumStrips;             // offset 0x4, size 0x2
    uint16 NumIdxClrTable;        // offset 0x6, size 0x2
    uint32 SizeofStripData;       // offset 0x8, size 0x4
    uint32 DataOffset0;           // offset 0xC, size 0x4
    uint32 DataOffset1;           // offset 0x10, size 0x4
    uint32 DataOffset2;           // offset 0x14, size 0x4
    uint32 DataOffset3;           // offset 0x18, size 0x4
    eStripEntry *StripEntryTable; // offset 0x1C, size 0x4
    uint8 *StripDataStart;        // offset 0x20, size 0x4

    void FixStripEntryTable(eSolid *solid, uint8 *strip_entry_data, uint8 *previous_strip_entry_data);
};

#endif
