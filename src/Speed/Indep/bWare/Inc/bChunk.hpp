#ifndef BCHUNK_HPP
#define BCHUNK_HPP

#include <types.h>

// TODO use the real nested macro instead of this
#define BCHUNK_NESTED_FLAG 0x80000000

#define NESTED_BCHUNK 0x80000000
#define NESTED_BCHUNK_MASK 0x7fffffff
#define DATA_BCHUNK 0x00000000
#define BCHUNK_ALIGNMENT_PADDING 0x11111111
#define BCHUNK_LOADER_TABLE_SIZE 64
#define BCHUNK_NULL ((unsigned int)0)
#define BCHUNK_NAME 0x00001 + DATA_BCHUNK
#define BCHUNK_FILE 0x00002 + DATA_BCHUNK
#define BCHUNK_LONG_NUMBER 0x00003 + DATA_BCHUNK
#define BCHUNK_FLOAT_NUMBER 0x00004 + DATA_BCHUNK
#define BCHUNK_BPOLY 0x10001 + DATA_BCHUNK
#define BCHUNK_BMESH 0x10100 + NESTED_BCHUNK
#define BCHUNK_BMESH_DATA 0x10101 + DATA_BCHUNK
#define BCHUNK_TEXTURE_NAME 0x10102 + DATA_BCHUNK
#define BCHUNK_MATERIAL_NAME 0x10103 + DATA_BCHUNK
#define BCHUNK_ATTRIBUTES 0x10104 + DATA_BCHUNK
#define BCHUNK_EXPORTER_CONFIG 0x10F00 + DATA_BCHUNK
#define BCHUNK_BBG_FILE_CHECKSUM 0x10f10 + DATA_BCHUNK
#define BCHUNK_BSCENE 0x11000 + NESTED_BCHUNK
#define BCHUNK_BSCENE_VERSION 0x11001 + DATA_BCHUNK
#define BCHUNK_BSCENE_DATE 0x11002 + DATA_BCHUNK
#define BCHUNK_BSCENE_DATA 0x11003 + DATA_BCHUNK
#define BCHUNK_BSCENE_VERSION_EXPORTER 0x11004 + DATA_BCHUNK
#define BCHUNK_BANIM_TREE 0x11100 + NESTED_BCHUNK
#define BCHUNK_BANIM_TREE_DATA 0x11100 + DATA_BCHUNK
#define BCHUNK_BANIM_TREE_NODE 0x11101 + DATA_BCHUNK
#define BCHUNK_BANIM_LOCAL_MAP 0x11102 + DATA_BCHUNK
#define BCHUNK_BGEOM 0x11200 + NESTED_BCHUNK
#define BCHUNK_BGEOM_VLIST 0x11201 + DATA_BCHUNK
#define BCHUNK_BGEOM_FLIST 0x11202 + DATA_BCHUNK
#define BCHUNK_BGEOM_CLIST 0x11203 + DATA_BCHUNK
#define BCHUNK_BGEOM_NLIST 0x11204 + DATA_BCHUNK
#define BCHUNK_BGEOM_ULIST 0x11205 + DATA_BCHUNK
#define BCHUNK_BGEOM_MATRIX 0x11206 + DATA_BCHUNK
#define BCHUNK_BGEOM_DATA 0x11207 + DATA_BCHUNK
#define BCHUNK_BGEOM_PROPERTIES 0x11208 + DATA_BCHUNK
#define BCHUNK_BGEOM_INSTANCE 0x11209 + DATA_BCHUNK
#define BCHUNK_BGEOM_STRIPPED_INSTANCE 0x1120a + DATA_BCHUNK
#define BCHUNK_BGEOM_EXTENTS 0x11210 + DATA_BCHUNK
#define BCHUNK_BGEOM_LLIST 0x11211 + DATA_BCHUNK
#define BCHUNK_BGEOM_ELIST 0x11212 + DATA_BCHUNK
#define BCHUNK_BGEOM_PLIST 0x11213 + DATA_BCHUNK
#define BCHUNK_BGEOM_BNLIST 0x11214 + DATA_BCHUNK
#define BCHUNK_BGEOM_STRIP 0x11215 + DATA_BCHUNK
#define BCHUNK_BGEOM_STRIPLIST 0x11216 + NESTED_BCHUNK
#define BCHUNK_BGEOM_SMOOTHLIST 0x11217 + DATA_BCHUNK
#define BCHUNK_BGEOM_AFULIST 0x11218 + NESTED_BCHUNK
#define BCHUNK_BGEOM_AULIST 0x11219 + DATA_BCHUNK
#define BCHUNK_BGEOM_PIVOT_MATRIX 0x1121A + DATA_BCHUNK
#define BCHUNK_BGEOM_AFFLIST 0x1121B + NESTED_BCHUNK
#define BCHUNK_BGEOM_AFLIST 0x1121C + DATA_BCHUNK
#define BCHUNK_BGEOM_ALIST 0x1121D + NESTED_BCHUNK
#define BCHUNK_BGEOM_ARLIST 0x1121E + DATA_BCHUNK
#define BCHUNK_BGEOM_APLIST 0x1121F + DATA_BCHUNK
#define BCHUNK_BGEOM_ASLIST 0x11220 + DATA_BCHUNK
#define BCHUNK_BGEOM_PARENT 0x11221 + DATA_BCHUNK
#define BCHUNK_BGEOM_ICLIST 0x11222 + DATA_BCHUNK
#define BCHUNK_BGEOM_ICELIST 0x11223 + DATA_BCHUNK
#define BCHUNK_BGEOM_CLRCHANNEL 0x11224 + DATA_BCHUNK
#define BCHUNK_BGEOM_RVLIST 0x11401 + DATA_BCHUNK
#define BCHUNK_BGEOM_RFLIST 0x11402 + DATA_BCHUNK
#define BCHUNK_BGEOM_RCLIST 0x11403 + DATA_BCHUNK
#define BCHUNK_BGEOM_RNLIST 0x11404 + DATA_BCHUNK
#define BCHUNK_BGEOM_RULIST 0x11405 + DATA_BCHUNK
#define BCHUNK_BGEOM_RLLIST 0x11411 + DATA_BCHUNK
#define BCHUNK_BGEOM_RELIST 0x11412 + DATA_BCHUNK
#define BCHUNK_BGEOM_RPLIST 0x11413 + DATA_BCHUNK
#define BCHUNK_BGEOM_RBNLIST 0x11414 + DATA_BCHUNK
#define BCHUNK_BGEOM_RSTRIPL 0x11216 + DATA_BCHUNK
#define BCHUNK_BGEOM_RSMOOTH 0x11417 + DATA_BCHUNK
#define BCHUNK_BGEOM_RAFFLIST 0x11418 + DATA_BCHUNK
#define BCHUNK_BGEOM_RICLIST 0x11419 + DATA_BCHUNK
#define BCHUNK_BGEOM_RICELIST 0x11420 + DATA_BCHUNK
#define BCHUNK_BANIM 0x11300 + NESTED_BCHUNK
#define BCHUNK_BANIM_DATA 0x11300 + DATA_BCHUNK
#define BCHUNK_BANIM_NODE 0x11301 + NESTED_BCHUNK
#define BCHUNK_BANIM_NODE_DATA 0x11302 + DATA_BCHUNK
#define BCHUNK_BANIM_ROT_LIST 0x11303 + DATA_BCHUNK
#define BCHUNK_BANIM_NODE_MAX 0x11304 + DATA_BCHUNK
#define BCHUNK_BANIM_POS_LIST 0x11305 + DATA_BCHUNK
#define BCHUNK_BANIM_SCL_LIST 0x11306 + DATA_BCHUNK
#define BCHUNK_MATLIST 0x12000 + NESTED_BCHUNK
#define BCHUNK_MATERIAL 0x12001 + NESTED_BCHUNK
#define BCHUNK_MATERIAL_ID 0x12003 + DATA_BCHUNK
#define BCHUNK_MATERIAL_COMPIDS 0x12004 + DATA_BCHUNK
#define BCHUNK_MATERIAL_CUSTOMATTRIBUTE 0x12100 + DATA_BCHUNK
#define BCHUNK_MATERIAL_MEDIAMUNCHERITEM 0x12101 + DATA_BCHUNK
#define BCHUNK_MATDATA 0x12002 + DATA_BCHUNK
#define BCHUNK_TEXTURE 0x12004 + NESTED_BCHUNK
#define BCHUNK_TEXTURE_ID 0x12005 + DATA_BCHUNK
#define BCHUNK_TEXTURE_TYPE 0x12006 + DATA_BCHUNK
#define BCHUNK_TEXTURE_AMOUNT 0x12007 + DATA_BCHUNK
#define BCHUNK_TEXTURE_COLOUR 0x12008 + DATA_BCHUNK
#define BCHUNK_TEXTURE_VALUE 0x12009 + DATA_BCHUNK
#define BCHUNK_SELECTION_SET 0x13000 + NESTED_BCHUNK
#define BCHUNK_NAMED_SELECTION 0x13000 + DATA_BCHUNK
#define BCHUNK_SELECTION_NAME 0x13001 + DATA_BCHUNK
#define BCHUNK_VERT_SELECTION_SET 0x14100 + DATA_BCHUNK
#define BCHUNK_EDGE_SELECTION_SET 0x14200 + DATA_BCHUNK
#define BCHUNK_FACE_SELECTION_SET 0x14300 + DATA_BCHUNK
#define BCHUNK_SUBOBJ_SELECTION_INDICES 0x14400 + DATA_BCHUNK
#define BCHUNK_BCAMERA 0x15000 + NESTED_BCHUNK
#define BCHUNK_BCAMERA_DATA 0x15000 + DATA_BCHUNK
#define BCHUNK_BCAMERA_POS_LIST 0x15001 + DATA_BCHUNK
#define BCHUNK_BCAMERA_ROT_LIST 0x15002 + DATA_BCHUNK
#define BCHUNK_BCAMERA_FOV_LIST 0x15003 + DATA_BCHUNK
#define BCHUNK_BCAMERA_FOCAL_DISTANCE_LIST 0x15004 + DATA_BCHUNK
#define BCHUNK_LIGHT 0x18000 + NESTED_BCHUNK
#define BCHUNK_LIGHT_TYPE 0x18001 + DATA_BCHUNK
#define BCHUNK_LIGHT_ATTENUATION 0x18002 + DATA_BCHUNK
#define BCHUNK_LIGHT_OVERSHOOT 0x18003 + DATA_BCHUNK
#define BCHUNK_LIGHT_SHAPE 0x18004 + DATA_BCHUNK
#define BCHUNK_LIGHT_KEY_COUNT 0x18005 + DATA_BCHUNK
#define BCHUNK_LIGHT_KEY 0x18006 + DATA_BCHUNK
#define BCHUNK_LIGHT_NAMESET 0x18007 + DATA_BCHUNK
#define BCHUNK_LIGHT_RANGE_END 0x18100 + DATA_BCHUNK
#define BCHUNK_SPLINE 0x19000 + NESTED_BCHUNK
#define BCHUNK_SPLINE_ID 0x19001 + DATA_BCHUNK
#define BCHUNK_SPLINE_LENGTH 0x19002 + DATA_BCHUNK
#define BCHUNK_SPLINE_CLOSED 0x19003 + DATA_BCHUNK
#define BCHUNK_SPLINE_NUM_CVS 0x19004 + DATA_BCHUNK
#define BCHUNK_SPLINE_CV_TABLE 0x19005 + DATA_BCHUNK
#define BCHUNK_SPLINE_PROPERTIES 0x19006 + DATA_BCHUNK
#define BCHUNK_SPLINE_MATRIX 0x19007 + DATA_BCHUNK
#define BCHUNK_SPLINE_RANGE_END 0x19100 + DATA_BCHUNK
#define BCHUNK_CAFFEINE_LAYER 0x17000 + NESTED_BCHUNK
#define BCHUNK_CAFFEINE_LAYER_HEADER 0x17001 + DATA_BCHUNK
#define BCHUNK_CAFFEINE_OBJECT 0x17002 + DATA_BCHUNK
#define BCHUNK_CAFFEINE_ATTRIBUTE 0x17003 + DATA_BCHUNK
#define BCHUNK_CAFFEINE_MESH 0x17004 + DATA_BCHUNK

// total size: 0x8
struct bChunk {
    bChunk() {}

    bChunk(unsigned int id, int size) {
        this->ID = id;
        this->Size = size;
    }

    unsigned int GetID() {
        return this->ID;
    }

    int GetSize() {
        return this->Size;
    }

    int IsNestedChunk() {
        return this->GetID() & BCHUNK_NESTED_FLAG;
    }

    int IsDataChunk() {
        return !IsNestedChunk();
    }

    int CountChildren() {
        /* anonymous block */ {
            int ret;
            /* anonymous block */ { struct bChunk *c; }
        }
    }

    char *GetData() {
        return reinterpret_cast<char *>(&this[1]);
    }

    char *GetAlignedData(int alignment_size) {
        return reinterpret_cast<char *>(reinterpret_cast<uintptr_t>(this->GetData() + alignment_size - 1) & ~(alignment_size - 1));
    }

    int GetAlignedSize(int alignment_size) {
        return this->Size - (this->GetAlignedData(alignment_size) - this->GetData());
    }

    void VerifyAlignment(int alignment_size) {
        unsigned int *pdata;
    }

    bChunk *GetFirstChunk() {
        return &this[1];
    }

    bChunk *GetLastChunk() {
        return (bChunk *)((char *)this + this->Size) + 1;
    }

    bChunk *GetNext() {
        return this->GetLastChunk();
    }

    unsigned int ID; // offset 0x0, size 0x4
    int Size;        // offset 0x4, size 0x4
};

inline bChunk *GetLastChunk(bChunk *first_chunk, int sizeof_binary) {
    return reinterpret_cast<bChunk *>(reinterpret_cast<char *>(first_chunk) + sizeof_binary);
}

typedef int (*bChunkLoaderFunction)(bChunk *);

// total size: 0x10
class bChunkLoader {
    bChunkLoader *Next;                    // offset 0x0, size 0x4
    unsigned int ID;                       // offset 0x4, size 0x4
    bChunkLoaderFunction LoaderFunction;   // offset 0x8, size 0x4
    bChunkLoaderFunction UnloaderFunction; // offset 0xC, size 0x4
    static bChunkLoader *sLoaderTable[64];
    static unsigned char sNumLoaders[64];

  public:
    static bChunkLoader *FindLoader(unsigned int id);

    static unsigned int CallLoaders(bChunk *chunks, int sizeof_chunks, bool abort_on_error);

    static unsigned int CallUnloaders(bChunk *chunks, int sizeof_chunks, bool abort_on_error);

    static unsigned int GetHash(unsigned int id) {
        return id + (id >> 6) + (id >> 0xc) & 0x3f;
    }

    bChunkLoader(unsigned int id, bChunkLoaderFunction loader, bChunkLoaderFunction unloader);

    bChunkLoaderFunction GetLoaderFunction() {
        return this->LoaderFunction;
    }

    bChunkLoaderFunction GetUnloaderFunction() {
        return this->UnloaderFunction;
    }
};

typedef enum { kResolved = 1 } kCarpHeaderFlags;

// total size: 0x10
class bChunkCarpHeader {
    enum kCarpHeaderFlags {
        kResolved = 1,
    };

    int mCrpSize;                   // offset 0x0, size 0x4
    int mSectionNumber;             // offset 0x4, size 0x4
    int mFlags;                     // offset 0x8, size 0x4
    bChunkCarpHeader *mLastAddress; // offset 0xC, size 0x4

  public:
    void PlatformEndianSwap();

    bChunkCarpHeader() {}

    ~bChunkCarpHeader() {}

    int GetCarpSize() const {
        return mCrpSize;
    }

    int GetSectionNumber() const {
        return mSectionNumber;
    }

    int GetFlags() const {
        return mFlags;
    }

    bool IsResolved() const {
        return (mFlags & kResolved) != 0;
    }

    void SetResolved() {
        mFlags |= kResolved;
    }

    void SetNotResolved() {
        mFlags &= ~kResolved;
    }

    bChunkCarpHeader *GetLastAddress() const {
        return this->mLastAddress;
    }

    void SetLastAddress(bChunkCarpHeader *address) {
        this->mLastAddress = address;
    }
};

#endif
