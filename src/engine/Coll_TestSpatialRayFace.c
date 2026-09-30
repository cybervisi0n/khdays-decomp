/* Cached Q27 ray intersection with full XYZ spatial edge planes. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct VecFx16 { s16 x, y, z; } VecFx16;
typedef struct CollisionPlane {
    VecFx16 normal;
    u16 padding06;
    fx32 distance;
} CollisionPlane;
typedef struct CollisionFace84 {
    u8 pad00[16];
    u16 flags10, vertexCount12;
    CollisionPlane plane;
    CollisionPlane edges[4];
    u8 pad50[52];
} CollisionFace84;
typedef struct CollCastState {
    u8 pad00[16];
    VecFx32 vRayStart;
    VecFx32 vDir;
    u8 pad28[44];
    VecFx32 vRayEnd;
    VecFx32 vCandidateHit;
    s32 nCandidateT;
    const CollisionPlane *pCachedPlane;
    s32 nRadius;
    s32 nNearestHit;
} CollCastState;
extern s32 Mem_CompareBytes(const void *left, const void *right, s32 count);
extern void FX_DivAsync(s32 numerator, s32 denominator);
extern s64 FX_GetDivResultFx64c(void);

#pragma inline_max_size(1300)
static inline fx32 PlaneDot(const VecFx16 *normal, const VecFx32 *point)
{
    return (fx32)(((s64)point->x * normal->x +
                   (s64)point->y * normal->y +
                   (s64)point->z * normal->z + 0x800) >> 12);
}
static inline void WaitDivider(void)
{
    while (*(volatile u16 *)0x04000280 & 0x8000) {
    }
}

extern fx32 VEC_DotProductFx16(const VecFx32 *point, const VecFx16 *normal);

s32 Coll_TestSpatialRayFace(const CollisionFace84 *face, CollCastState *query)
{
    /* startDot is reused for the denominator after the endpoint guards. */
    s32 distance, startDot, endDot, planeDistance;
    if (!query->pCachedPlane ||
        Mem_CompareBytes(query->pCachedPlane, &face->plane, sizeof(CollisionPlane))) {
        query->pCachedPlane = &face->plane;
        startDot = PlaneDot(&face->plane.normal, &query->vRayStart);
        endDot = PlaneDot(&face->plane.normal, &query->vRayEnd);
        planeDistance = face->plane.distance;
        if (startDot <= endDot) goto reject;
        distance = startDot - planeDistance;
        if (distance < -128) goto reject;
        if (endDot - planeDistance > 128) goto reject;
        startDot -= endDot;
        FX_DivAsync(distance, startDot);
        if (distance >= (s32)(((s64)query->nNearestHit * startDot) >> 27)) {
            WaitDivider();
            goto reject;
        }
        if (distance < 0) {
            query->nCandidateT = 0;
            query->vCandidateHit = query->vRayStart;
            WaitDivider();
        } else if (distance >= startDot) {
            query->nCandidateT = 1 << 27;
            query->vCandidateHit = query->vRayEnd;
            WaitDivider();
        } else {
            query->nCandidateT = (s32)(FX_GetDivResultFx64c() >> 5);
            Vec3ScaleAddQ27(query->nCandidateT, &query->vDir,
                         &query->vRayStart, &query->vCandidateHit);
        }
    } else {
        if (query->nCandidateT < 0) return -4096;
        if (query->nCandidateT >= query->nNearestHit) goto reject;
    }
    {
        /* Keep signed64 coordinates live across all three inlined edge tests. */
        s64 y = query->vCandidateHit.y;
        s64 x = query->vCandidateHit.x;
        s64 z = query->vCandidateHit.z;
#define EDGE_TEST(n) \
        if ((s32)((x * face->edges[n].normal.x + y * face->edges[n].normal.y + z * face->edges[n].normal.z + 0x800) >> 12) < face->edges[n].distance - 128) return -4096
        EDGE_TEST(0);
        EDGE_TEST(1);
        EDGE_TEST(2);
#undef EDGE_TEST
    }
    if (face->vertexCount12 == 4 &&
        VEC_DotProductFx16(&query->vCandidateHit, &face->edges[3].normal) <
            face->edges[3].distance - 128) return -4096;
    return query->nNearestHit = query->nCandidateT;
reject:
    query->nCandidateT = -4096;
    return -4096;
}
