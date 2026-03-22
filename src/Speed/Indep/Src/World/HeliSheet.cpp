#include "VisibleSection.hpp"

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
};

struct HeliSheetManager {
    bTList<HeliSection> HeliSectionList;

    HeliPoly *FindHeliPoly(const bVector2 &point);
};

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
