/* ov022: find what the actor's reach touches, trying three sweeps in turn.
 *
 * The straight sweep goes first. If it finds something the caller cannot use,
 * or finds nothing, the same sweep is retried from a point one radius lower and
 * then from one radius higher, each with its own kind so the caller knows which
 * one answered. The lowered sweep only counts if the surface it hit is steep
 * enough, and both offset sweeps are thrown away if the contact point ends up on
 * the wrong side of the reach limit.
 *
 * A hit that carries an owner answers with the owner's handle instead of the
 * contact itself, and only if that handle exists.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define STEEP_LIMIT 0x100
#define PARTS_PER_HIT 4
#define KIND_OWNED 4

struct Part {
    u8 pad00[0x14];
    u8 verts[0x6c];              /* 0x14 */
    u8 aId[4];                   /* 0x80 */
};

struct Owner {
    u8 pad00[0x28];
    int *pSlot;                  /* 0x28 */
};

struct Hit {
    int nFace;                   /* 0x00 */
    struct Part *pPart;          /* 0x04 */
    struct Owner *pOwner;        /* 0x08 */
    void *pShape;                /* 0x0c */
    u8 pad10[0x88];
};

struct Thing {
    u8 pad00[0xc];
    u8 nRole;                    /* 0x0c */
};

struct Ray {
    u8 pad00[4];
    int nLimit;                  /* 0x04 */
    u8 pad08[4];
    VecFx32 vec;             /* 0x0c */
    int nRadius;                 /* 0x18 */
    int nMask;                   /* 0x1c */
};

struct Contact {
    u8 pad00[4];
    int nKind;                   /* 0x04 */
    u8 nState;                   /* 0x08 */
    u8 pad09[3];
    VecFx32 vec;             /* 0x0c */
    int nHandle;                 /* 0x18 */
    struct Hit hit;              /* 0x1c */
};

struct Scene {
    u8 pad00[0x20];
    void *pWorld;                /* 0x20 */
};

extern struct Hit *EntityMgr_RunSphereCast(int nMask, struct Ray *pRay, VecFx32 *pFrom, int nRadius,
                                           void *pWorld);
extern struct Hit *EntityMgr_RunRayCast(int nMask, struct Ray *pRay, VecFx32 *pFrom, void *pWorld);
extern struct Hit *EntityMgr_RunCastSimple(int nMask, struct Ray *pRay, VecFx32 *pFrom, void *pWorld);
extern struct Thing *Actor_GetRecord(struct Hit *pHit, int nId);
extern void Vec3ScaleAddQ27(void *pShape, VecFx32 *pFrom, struct Ray *pRay,
                          VecFx32 *pOut);
extern void VecFx32FromVecS16(int nFace, u8 *pVerts, VecFx32 *pOut);
extern void VEC_Normalize(VecFx32 *pOut, VecFx32 *pIn);

int Ov022_ResolveReachSweep(struct Scene *pScene, struct Ray *pRay,
                        struct Contact *pOut)
{
    VecFx32 vFrom;
    VecFx32 vPoint;
    VecFx32 vNormal;
    int bTaken;
    struct Hit *pHit;
    int nKind;
    int nOffset;
    int i;

    bTaken = 0;
    nKind = 0;
    if (pOut != 0) {
        pOut->nKind = 0;
        pOut->nState = 0;
    }
    pHit = EntityMgr_RunSphereCast((u16)pRay->nMask, pRay, &pRay->vec, pRay->nRadius, pScene->pWorld);
    if (pHit != 0) {
        if (pHit->pOwner == 0) {
            for (i = 0; i < PARTS_PER_HIT; i++) {
                struct Thing *pThing = Actor_GetRecord(pHit,
                                                     pHit->pPart->aId[i]);

                if (pThing != 0 && pThing->nRole == 2) {
                    pHit = 0;
                    break;
                }
            }
        }
        if (pHit != 0) {
            Vec3ScaleAddQ27(pHit->pShape, &pRay->vec, pRay, &vPoint);
            nKind = 1;
        }
    }
    if (pHit == 0) {
        nOffset = -pRay->nRadius;
        vFrom.x = 0;
        vFrom.z = 0;
        vFrom.y = nOffset;
        vFrom = pRay->vec;
        vFrom.y = vFrom.y - pRay->nRadius;
        pHit = EntityMgr_RunRayCast((u16)pRay->nMask, pRay, &vFrom, pScene->pWorld);
        if (pHit != 0) {
            VecFx32FromVecS16(pHit->nFace, pHit->pPart->verts, &vNormal);
            VEC_Normalize(&vNormal, &vNormal);
            if (vNormal.y >= STEEP_LIMIT || vNormal.y <= -STEEP_LIMIT) {
                nKind = 2;
                Vec3ScaleAddQ27(pHit->pShape, &vFrom, pRay, &vPoint);
                if (vPoint.y > pRay->nLimit) {
                    nKind = 0;
                    pHit = 0;
                }
            } else {
                pHit = 0;
            }
        }
    }
    if (pHit == 0) {
        nOffset = pRay->nRadius;
        vFrom.x = 0;
        vFrom.z = 0;
        vFrom.y = nOffset;
        vFrom = pRay->vec;
        vFrom.y = vFrom.y + pRay->nRadius;
        pHit = EntityMgr_RunCastSimple((u16)pRay->nMask, pRay, &vFrom, pScene->pWorld);
        if (pHit != 0) {
            nKind = 3;
            Vec3ScaleAddQ27(pHit->pShape, &vFrom, pRay, &vPoint);
            if (vPoint.y < pRay->nLimit) {
                nKind = 0;
                pHit = 0;
            }
        }
    }
    if (pHit != 0) {
        bTaken = 1;
        if (pHit->pOwner != 0) {
            nKind = KIND_OWNED;
            if (pHit->pOwner->pSlot[0x158 / 4] == 0) {
                bTaken = 0;
            }
        }
        if (pOut != 0 && bTaken != 0) {
            pOut->nKind = nKind;
            pOut->nState = 4;
            if (nKind != KIND_OWNED) {
                pOut->hit = *pHit;
            } else {
                pOut->nHandle = pHit->pOwner->pSlot[0x158 / 4];
            }
            pOut->vec = vPoint;
        }
    }
    return bTaken;
}
