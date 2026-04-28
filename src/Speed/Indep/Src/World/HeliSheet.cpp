#include "HeliSheet.hpp"
#include "VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern double lbl_8040CE80;
extern float lbl_8040CE88;
extern float lbl_8040CE8C;
extern float lbl_8040CE90;
extern double lbl_8040CE98;
extern float lbl_8040CEA0;
extern float lbl_8040CEA4;

bool bIsPointInPoly(const bVector2 *point, const bVector3 *points, int num_points);

VisibleSectionBoundary *VisibleSectionManager_FindBoundary(VisibleSectionManager *manager, const bVector2 *point)
    asm("FindBoundary__21VisibleSectionManagerPC8bVector2");

struct HeliPoly {
    short VertexX[3];
    short VertexY[3];
    short VertexZ[3];

    void GetVertices(bVector3 *vertices);

    bVector3 GetVertex(int n) {
        return bVector3(static_cast<float>(this->VertexX[n]) * lbl_8040CEA0, static_cast<float>(this->VertexY[n]) * lbl_8040CEA0,
                        static_cast<float>(this->VertexZ[n]) * lbl_8040CEA4);
    }
};

struct HeliSection : public bTNode<HeliSection> {
    int SectionNumber;
    int NumPolys;
    int EndianSwapped;
    HeliPoly *PolyTable;

    int GetNumPolys() {
        return this->NumPolys;
    }

    HeliPoly *GetPoly(int n) {
        return reinterpret_cast<HeliPoly *>(reinterpret_cast<unsigned char *>(this->PolyTable) + n * sizeof(HeliPoly));
    }

    void EndianSwap();
};

struct HeliSheetManager {
    bTList<HeliSection> HeliSectionList;

    HeliSheetManager();

    int Loader(bChunk *chunk);

    int Unloader(bChunk *chunk);

    HeliPoly *FindHeliPoly(const bVector2 &point);
};

int LoaderHeliSheet(bChunk *chunk);
int UnloaderHeliSheet(bChunk *chunk);

HeliSheetManager gHeliSheetManager;
bChunkLoader bChunkLoaderHeliSheet(0x34159, LoaderHeliSheet, UnloaderHeliSheet);

void HeliPoly::GetVertices(bVector3 *vertices) {
    int n = 0;

    do {
        vertices[n] = GetVertex(n);
        n++;
    } while (n < 3);
}

void HeliSection::EndianSwap() {
    bEndianSwap32(&this->SectionNumber);
    bEndianSwap32(&this->NumPolys);

    for (int n = 0; n < this->NumPolys; n++) {
        HeliPoly *heli_poly = reinterpret_cast<HeliPoly *>(reinterpret_cast<unsigned char *>(this->PolyTable) + n * sizeof(HeliPoly));

        for (int i = 0; i < 3; i++) {
            bEndianSwap16(&heli_poly->VertexX[i]);
            bEndianSwap16(&heli_poly->VertexY[i]);
            bEndianSwap16(&heli_poly->VertexZ[i]);
        }
    }

    this->EndianSwapped = 1;
}

float HeliSheetCoordinate::GetElevation(const bVector2 &point, bVector3 *NormalOut, bool *ppoint_valid) {
    if (*reinterpret_cast<unsigned int *>(&this->VertexValid) == 0 || !bIsPointInPoly(&point, this->Vertex, 3)) {
        HeliPoly *heli_poly = gHeliSheetManager.FindHeliPoly(point);

        if (heli_poly == 0) {
            *reinterpret_cast<unsigned int *>(&this->VertexValid) = 0;
            if (ppoint_valid != 0) {
                *reinterpret_cast<unsigned int *>(ppoint_valid) = 0;
            }
            return this->PreviousElevation;
        }

        *reinterpret_cast<unsigned int *>(&this->VertexValid) = 1;
        heli_poly->GetVertices(this->Vertex);
    }

    bVector3 edge0 = this->Vertex[1] - this->Vertex[0];
    bVector3 edge1 = this->Vertex[2] - this->Vertex[0];
    bVector3 normal = bNormalize(bCross(edge0, edge1));

    float elevation = ((this->Vertex[0].z * normal.z + this->Vertex[0].x * normal.x + this->Vertex[0].y * normal.y) -
                       (normal.x * point.x + normal.y * point.y)) /
                      normal.z;

    if (ppoint_valid != 0) {
        *reinterpret_cast<unsigned int *>(ppoint_valid) = 1;
    }

    this->PreviousElevation = elevation;

    if (NormalOut != 0) {
        NormalOut->x = normal.x;
        NormalOut->y = normal.y;
        NormalOut->z = normal.z;
    }

    return elevation;
}

HeliSheetManager::HeliSheetManager() {}

int HeliSheetManager::Loader(bChunk *chunk) {
    if (chunk->ID != 0x34159) {
        return 0;
    }

    int *heli_section = reinterpret_cast<int *>((reinterpret_cast<int>(chunk) + 0x17U) & 0xfffffff0);
    heli_section[5] = reinterpret_cast<int>(heli_section + 6);
    if (heli_section[4] == 0) {
        reinterpret_cast<HeliSection *>(heli_section)->EndianSwap();
    }

    unsigned int *tail = reinterpret_cast<unsigned int *>(this->HeliSectionList.HeadNode.Prev);
    *tail = reinterpret_cast<unsigned int>(heli_section);
    this->HeliSectionList.HeadNode.Prev = reinterpret_cast<bNode *>(heli_section);
    heli_section[1] = reinterpret_cast<int>(tail);
    heli_section[0] = reinterpret_cast<int>(this);
    return 1;
}

int HeliSheetManager::Unloader(bChunk *chunk) {
    if (chunk->ID != 0x34159) {
        return 0;
    }

    int *heli_section = reinterpret_cast<int *>((reinterpret_cast<int>(chunk) + 0x17U) & 0xfffffff0);
    int prev = heli_section[0];
    int *next = reinterpret_cast<int *>(heli_section[1]);

    *next = prev;
    *reinterpret_cast<int **>(prev + 4) = next;
    return 1;
}

int LoaderHeliSheet(bChunk *chunk) {
    return gHeliSheetManager.Loader(chunk);
}

int UnloaderHeliSheet(bChunk *chunk) {
    return gHeliSheetManager.Unloader(chunk);
}

HeliPoly *HeliSheetManager::FindHeliPoly(const bVector2 &point) {
    int section_number;
    HeliSection *heli_section;
    int x;
    int y;

    VisibleSectionBoundary *boundary = VisibleSectionManager_FindBoundary(&TheVisibleSectionManager, &point);
    if (boundary == 0) {
        section_number = 0;
    } else {
        section_number = boundary->SectionNumber;
    }

    if (section_number != 0) {
        heli_section = 0;

        for (HeliSection *h = this->HeliSectionList.GetHead(); h != this->HeliSectionList.EndOfList(); h = h->GetNext()) {
            if (h->SectionNumber == section_number) {
                heli_section = h;
                break;
            }
        }

        if (heli_section != 0) {
            x = static_cast<int>(point.x * lbl_8040CE90);
            y = static_cast<int>(point.y * lbl_8040CE90);

            for (int n = 0; n < heli_section->GetNumPolys(); n++) {
                HeliPoly *heli_poly = heli_section->GetPoly(n);
                int max_x = bMax(heli_poly->VertexX[0], heli_poly->VertexX[1]);
                max_x = bMax(max_x, heli_poly->VertexX[2]);
                int min_x = bMin(heli_poly->VertexX[0], heli_poly->VertexX[1]);
                min_x = bMin(min_x, heli_poly->VertexX[2]);

                if (x <= max_x && min_x <= x) {
                    int max_y = bMax(heli_poly->VertexY[0], heli_poly->VertexY[1]);
                    max_y = bMax(max_y, heli_poly->VertexY[2]);
                    int min_y = bMin(heli_poly->VertexY[0], heli_poly->VertexY[1]);
                    min_y = bMin(min_y, heli_poly->VertexY[2]);

                    if (y <= max_y && min_y <= y) {
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
        }
    }

    return 0;
}
