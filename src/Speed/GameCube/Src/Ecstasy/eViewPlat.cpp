#include "Speed/GameCube/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

void CreateViewMatricies(eView *view, float force_near_z, float force_far_z, float force_screen_far_z);

eViewPlatInfo ViewPlatInfoTable[NUM_EVIEWS];

// TODO
int GetPlaneState(const bVector4 *plane, const bVector3 *point) {
    float dot;

    double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10, FP11;

    asm("psq_l 0, 0(3), 0, 0\n"
        "addi 3, 3, 8\n"
        "psq_l 12, 0(3), 0, 0\n"
        "psq_l 11, 0(4), 0, 0\n"
        "addi 4, 4, 8\n"
        "psq_l 13, 0(4), 1, 1\n"
        "ps_mul 12, 12, 13\n"
        "ps_mul 0, 0, 11\n"
        "ps_add 0, 0, 12\n"
        "ps_merge11 13, 0, 13\n"
        "ps_add 0, 0, 13\n"
        "psq_st 0, %0, 0, 0"
        : "=m"(dot));

    return dot > 0.0f;
}

void TransformBound(bMatrix4 *mat, bVector3 *min, bVector3 *max) {
    double float_newmin[2];
    double float_newmax[2];
    float *jmin;
    float *jmax;
    float *jmat;

    {
        double *src_xy = reinterpret_cast<double *>(&mat->v3);

        asm("psq_l 0, 0(%0), 0, 0" : : "b"(src_xy));
        asm("psq_st 0, 8(1), 0, 0");

        float *src_z = &mat->v3.z;

        asm("psq_l 0, 0(%0), 1, 0" : : "b"(src_z));
        asm("psq_st 0, 0x10(1), 0, 0");
        asm("psq_l 13, 0(%0), 0, 0" : : "b"(src_xy));
        asm("psq_st 13, 0x18(1), 0, 0");
        asm("psq_l 0, 0(%0), 1, 0" : : "b"(src_z));

        int j = 0;

        asm("psq_st 0, 0x20(1), 0, 0");

        jmin = &min->x;
        jmax = &max->x;
        jmat = &mat->v0.x;

        for (; j < 3; j++) {
            float *mat = jmat;
            float *newmin = reinterpret_cast<float *>(float_newmin);
            float *newmax = reinterpret_cast<float *>(float_newmax);

            for (int i = 0; i < 3; i++) {
                float a = *mat * *jmin;
                float b = *mat * *jmax;

                if (a < b) {
                    *newmin += a;
                    *newmax += b;
                } else {
                    *newmin += b;
                    *newmax += a;
                }
                mat++;
                newmin++;
                newmax++;
            }
            jmin++;
            jmax++;
            jmat += 4;
        }
    }

    asm("psq_l 0, 8(1), 0, 0");
    asm("psq_st 0, 0(%0), 0, 0" : : "b"(min));
    asm("psq_l 0, 0x10(1), 0, 0");
    asm("psq_st 0, 8(%0), 1, 0" : : "b"(min));
    asm("psq_l 0, 0x18(1), 0, 0");
    asm("psq_st 0, 0(%0), 0, 0" : : "b"(max));
    asm("psq_l 0, 0x20(1), 0, 0");
    asm("psq_st 0, 8(%0), 1, 0" : : "b"(max));
}

// UNSOLVED, register allocation shifted by one (r5->r6 etc.) in the plane loop
eVisibleState eViewPlatInterface::GetVisibleStateSB(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world) {
    if (!aabb_min || !aabb_max) {
        return EVISIBLESTATE_NOT;
    }

    bVector3 min = *aabb_min;
    bVector3 max = *aabb_max;

    if (local_world) {
        TransformBound(local_world, &min, &max);
    }

    eViewPlatInfo *plat_info = GetPlatInfo();
    bVector3 centre((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
    bVector3 diag = max - centre;
    unsigned int partialFlag = 0;

    {
        for (int clip_plane = 1; clip_plane <= 6; clip_plane++) {
            bVector4 *plane = &plat_info->ClippingPlanes[clip_plane - 1];
            float np;
            double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8;
            float mp;

            asm volatile("psq_l 11, 0(%1), 0, 0\n"
                         "psq_l 13, 0(%2), 1, 0\n"
                         "psq_l 12, 0(%3), 0, 0\n"
                         "addi 11, %3, 8\n"
                         "psq_l 0, 0(11), 1, 0\n"
                         "ps_abs 12, 12\n"
                         "ps_abs 0, 0\n"
                         "ps_mul 11, 11, 12\n"
                         "ps_mul 13, 13, 0\n"
                         "ps_add 13, 11, 13\n"
                         "ps_merge11 11, 13, 11\n"
                         "ps_add 13, 13, 11\n"
                         "psq_st 13, %0, 0, 0"
                         : "=m"(np)
                         : "b"(&diag.x), "b"(&diag.z), "b"(plane));
            asm volatile("psq_l 0, 0(%1), 0, 0\n"
                         "psq_l 12, 0(%2), 1, 1\n"
                         "psq_l 11, 0(%3), 0, 0\n"
                         "psq_l 13, 0(11), 0, 0\n"
                         "ps_mul 13, 12, 13\n"
                         "ps_mul 0, 0, 11\n"
                         "ps_add 0, 0, 13\n"
                         "ps_merge11 12, 0, 12\n"
                         "ps_add 0, 0, 12\n"
                         "psq_st 0, %0, 0, 0"
                         : "=m"(mp)
                         : "b"(&centre.x), "b"(&centre.z), "b"(plane));

            if (mp + np < 0.0f) {
                return EVISIBLESTATE_NOT;
            }
            if (mp - np < 0.0f) {
                partialFlag = 1;
            }
        }
    }

    if (partialFlag) {
        return EVISIBLESTATE_PARTIAL;
    }
    return EVISIBLESTATE_FULL;
}

eVisibleState eViewPlatInterface::GetVisibleStateSB(const bVector3 *position, bMatrix4 *local_world) {
    eViewPlatInfo *plat_info = GetPlatInfo();

    for (int i = 0; i < 6; i++) {
        if (!GetPlaneState(&plat_info->ClippingPlanes[i], position)) {
            return EVISIBLESTATE_NOT;
        }
    }
    return EVISIBLESTATE_FULL;
}

void eViewPlatInterface::GetScreenPosition(bVector3 *screen_position, const bVector3 *world_position) {
    eView *view;
    eViewPlatInfo *plat_info = GetPlatInfo();
    bMatrix4 *world_view = plat_info->GetWorldViewMatrix();
    bMatrix4 *view_screen = plat_info->GetViewScreenMatrix();

    eRotTransPers(screen_position, world_position, world_view, view_screen, 0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
}

void eViewPlatInfo::CalculateViewMatricies(eView *view, float force_near_z, float force_far_z, float force_screen_far_z) {
    CreateViewMatricies(view, force_near_z, force_far_z, force_screen_far_z);

    eRenderTarget *render_target = view->GetRenderTarget();

    if (render_target) {
        render_target->WorldClip = &WorldClipMatrix;
        render_target->WorldView = &WorldViewMatrix;
        render_target->ViewScreen = &ViewScreenMatrix;
    }
}

int eViewPlatInterface::GetPixelWidth() {
    eView *view = static_cast<eView *>(this);
    eRenderTarget *render_target = view->GetRenderTarget();

    return render_target->ScissorW;
}

eViewPlatInfo *eViewPlatInterface::GimmeMyViewPlatInfo(int view_id) {
    return &ViewPlatInfoTable[view_id];
}
