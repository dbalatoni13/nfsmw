#include "HeliSheet.hpp"
#include "VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct HeliPoly {
    short VertexX[3];
    short VertexY[3];
    short VertexZ[3];

    bVector3 GetVertex(int n) {
        float x = static_cast<float>(this->VertexX[n]) * 0.25f;
        float y = static_cast<float>(this->VertexY[n]) * 0.25f;
        float z = static_cast<float>(this->VertexZ[n]) * 0.0625f;

        return bVector3(x, y, z);
    }

    void GetVertices(bVector3 *vertices);

    // void SetVertex(int n, const bVector3 &v) {}

    void EndianSwap() {
        for (int n = 0; n < NUM_ELEMENTS(this->VertexX); n++) {
            bPlatEndianSwap(&this->VertexX[n]);
            bPlatEndianSwap(&this->VertexY[n]);
            bPlatEndianSwap(&this->VertexZ[n]);
        }
    }
};

struct HeliSection : public bTNode<HeliSection> {
    int32 SectionNumber; // offset 0x8, size 0x4
    int32 NumPolys;      // offset 0xC, size 0x4
    int32 EndianSwapped; // offset 0x10, size 0x4
    HeliPoly *PolyTable; // offset 0x14, size 0x4

    int GetMemoryImageSize() {
        return this->NumPolys * sizeof(HeliPoly);
    }

    int GetNumPolys() {
        return this->NumPolys;
    }

    HeliPoly *GetPoly(int n) {
        return &this->PolyTable[n];
    }

    void EndianSwap();
};

class HeliSheetManager {
  public:
    HeliSheetManager();

    ~HeliSheetManager();

    HeliPoly *FindHeliPoly(const bVector2 &point);

    int Loader(bChunk *chunk);

    int Unloader(bChunk *chunk);

  private:
    bTList<HeliSection> HeliSectionList; // offset 0x0, size 0x8
};

int LoaderHeliSheet(bChunk *chunk);
int UnloaderHeliSheet(bChunk *chunk);

HeliSheetManager gHeliSheetManager;
bChunkLoader bChunkLoaderHeliSheet(BCHUNK_SPEED_HELI_SHEET, LoaderHeliSheet, UnloaderHeliSheet);

void HeliPoly::GetVertices(bVector3 *vertices) {
    for (int n = 0; n < NUM_ELEMENTS(this->VertexX); n++) {
        vertices[n] = GetVertex(n);
    }
}

void HeliSection::EndianSwap() {
    bPlatEndianSwap(&this->SectionNumber);
    bPlatEndianSwap(&this->NumPolys);

    for (int n = 0; n < this->NumPolys; n++) {
        this->PolyTable[n].EndianSwap();
    }

    this->EndianSwapped = 1;
}

float HeliSheetCoordinate::GetElevation(const bVector2 &point, bVector3 *NormalOut, bool *ppoint_valid) {
    if (!this->VertexValid || !bIsPointInPoly(&point, this->Vertex, 3)) {
        HeliPoly *heli_poly = gHeliSheetManager.FindHeliPoly(point);

        if (heli_poly == nullptr) {
            this->VertexValid = false;

            if (ppoint_valid != nullptr) {
                *ppoint_valid = false;
            }

            return this->PreviousElevation;
        }

        this->VertexValid = true;
        heli_poly->GetVertices(this->Vertex);
    }

    bVector3 normal = bNormalize(bCross(this->Vertex[1] - this->Vertex[0], this->Vertex[2] - this->Vertex[0]));
    float plane_distance = bDot(this->Vertex[0], normal);
    float elevation = (plane_distance - (normal.x * point.x + normal.y * point.y)) / normal.z;

    if (ppoint_valid != nullptr) {
        *ppoint_valid = true;
    }

    this->PreviousElevation = elevation;

    if (NormalOut != nullptr) {
        *NormalOut = normal;
    }

    return elevation;
}

HeliSheetManager::HeliSheetManager() {}

HeliSheetManager::~HeliSheetManager() {}

int HeliSheetManager::Loader(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_SPEED_HELI_SHEET) {
        return 0;
    }

    HeliSection *heli_section = reinterpret_cast<HeliSection *>(chunk->GetAlignedData(16));
    heli_section->PolyTable = reinterpret_cast<HeliPoly *>(heli_section + 1);
    if (heli_section->EndianSwapped == 0) {
        heli_section->EndianSwap();
    }

    this->HeliSectionList.AddTail(heli_section);
    return 1;
}

int HeliSheetManager::Unloader(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_SPEED_HELI_SHEET) {
        return 0;
    }

    HeliSection *heli_section = reinterpret_cast<HeliSection *>(chunk->GetAlignedData(16));
    this->HeliSectionList.Remove(heli_section);
    return 1;
}

HeliPoly *HeliSheetManager::FindHeliPoly(const bVector2 &point) {
    int section_number = TheVisibleSectionManager.GetDrivableSectionNumber(&point);

    if (section_number == 0) {
        return nullptr;
    }

    HeliSection *heli_section = nullptr;

    for (HeliSection *h = this->HeliSectionList.GetHead(); h != this->HeliSectionList.EndOfList(); h = h->GetNext()) {
        if (h->SectionNumber == section_number) {
            heli_section = h;
            break;
        }
    }

    if (heli_section == nullptr) {
        return nullptr;
    }

    int x = static_cast<int>(point.x * 4.0f);
    int y = static_cast<int>(point.y * 4.0f);

    for (int n = 0; n < heli_section->GetNumPolys(); n++) {
        HeliPoly *heli_poly = heli_section->GetPoly(n);

        int max_x = bMax(heli_poly->VertexX[0], heli_poly->VertexX[1]);
        max_x = bMax(max_x, heli_poly->VertexX[2]);

        int min_x = bMin(heli_poly->VertexX[1], heli_poly->VertexX[0]);
        min_x = bMin(heli_poly->VertexX[2], min_x);

        if (x <= max_x && x >= min_x) {
            int max_y = bMax(heli_poly->VertexY[0], heli_poly->VertexY[1]);
            max_y = bMax(max_y, heli_poly->VertexY[2]);
            int min_y = bMin(heli_poly->VertexY[1], heli_poly->VertexY[0]);
            min_y = bMin(heli_poly->VertexY[2], min_y);

            if (y <= max_y && y >= min_y) {
                bVector3 vertices[3];

                for (int i = 0; i < 3; i++) {
                    vertices[i] = heli_poly->GetVertex(i);
                }

                if (bIsPointInPoly(&point, vertices, 3)) {
                    return heli_poly;
                }
            }
        }
    }

    return nullptr;
}

int LoaderHeliSheet(bChunk *chunk) {
    return gHeliSheetManager.Loader(chunk);
}

int UnloaderHeliSheet(bChunk *chunk) {
    return gHeliSheetManager.Unloader(chunk);
}
