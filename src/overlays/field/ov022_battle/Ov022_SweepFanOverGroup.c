/* Ov022_SweepFanOverGroup -- test a horizontal fan against every member of a
 * group.
 *
 * Nothing is tested while the actor carries the no-contact flag. The fan's
 * axis is taken once, normalised and flattened, and the group named by the
 * query is looked up. Every member that is present and open to contact is
 * kept when it lies within the fan's reach plus its own radius and the
 * flattened direction to it makes at least the fan's spread with the axis.
 *
 * Any hit answers yes. Only the local player reports one: the contact is
 * turned into a hit record and handed on, and a member further than the reach
 * plus a unit and a half leaves the actor's impact point one reach along the
 * way to it.
 */

/* Ov022FanQuery */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct FanQuery {
    VecFx32 vecOrigin;           /* 0x00 */
    int nRadius;                 /* 0x0c */
    int nGroup;                  /* 0x10 */
    VecFx32 vecDir;              /* 0x14 */
    int nCosSpread;              /* 0x20 */
};

/* Ov002Element (only the list link and the flags are used here) */
struct GroupMember {
    u8 pad0000[4];
    struct GroupMember *pNext;   /* 0x04 */
    u8 pad0008[0xa];
    u16 nFlags;                  /* 0x12 */
};

/* Ov022ContactPoint */
struct ContactPoint {
    VecFx32 vecPos;              /* 0x00 */
    int nRadius;                 /* 0x0c */
};

struct HitRecord {
    VecFx32 vecPos;              /* 0x00 */
};

/* Ov022Actor */
struct Actor {
    unsigned int nFlags;         /* 0x0000 */
    u8 pad0004[0x26c4];
    VecFx32 vecImpact;           /* 0x26c8 */
};

extern int VEC_Mag(VecFx32 *pVec);
extern void VEC_Normalize(VecFx32 *pSrc, VecFx32 *pDst);   /* VEC_Normalize */
extern void VEC_Subtract(VecFx32 *pA, VecFx32 *pB, VecFx32 *pOut);
extern int VEC_Distance(VecFx32 *pA, VecFx32 *pB);         /* VEC_Distance */
extern int VEC_DotProduct(VecFx32 *pA, VecFx32 *pB);
extern void VEC_MultAdd(int nScale, VecFx32 *pVec, VecFx32 *pBase, VecFx32 *pOut);
extern int Ov002_GetSlotTableByte(int nGroup);
extern struct GroupMember *Ov002_List_GetWord(int nId);
extern struct ContactPoint *Ov002_TriggerEntryActive(struct GroupMember *pMember);
extern int Session_GetLocalPlayerIndex(void);
extern void Ov022_BuildHitPush(struct HitRecord *pHit, void *pCtx,
                                VecFx32 *pContact, struct ContactPoint *pPoint,
                                VecFx32 *pDir);
extern void Ov022_ReportHit(struct Actor *pActor, void *pCtx,
                                struct GroupMember *pMember,
                                struct HitRecord *pHit, int nArg);

#define ACTOR_NO_CONTACT 0x10000
#define MEMBER_OPEN 0x40
#define IMPACT_MARGIN 0x1800

int Ov022_SweepFanOverGroup(struct Actor *pActor, struct FanQuery *pFan, void *pCtx)
{
    VecFx32 vecToPoint;
    VecFx32 vecAxis;
    struct HitRecord hit;
    struct GroupMember *pMember;
    struct ContactPoint *pPoint;
    int nCosMin;
    int bHit;

    bHit = 0;
    if ((pActor->nFlags & ACTOR_NO_CONTACT) != 0) {
        return 0;
    }
    vecAxis = pFan->vecDir;
    if (VEC_Mag(&vecAxis) != 0) {
        VEC_Normalize(&vecAxis, &vecAxis);
    }
    vecAxis.y = 0;
    nCosMin = -pFan->nCosSpread;
    pMember = Ov002_List_GetWord((u16)Ov002_GetSlotTableByte(pFan->nGroup));
    if (pMember != 0) {
        do {
            pPoint = Ov002_TriggerEntryActive(pMember);
            if (pPoint != 0 && (pMember->nFlags & MEMBER_OPEN) != 0
                && VEC_Distance(&pPoint->vecPos, &pFan->vecOrigin)
                       <= pFan->nRadius + pPoint->nRadius) {
                VEC_Subtract(&pPoint->vecPos, &pFan->vecOrigin, &vecToPoint);
                if (VEC_Mag(&vecToPoint) != 0) {
                    VEC_Normalize(&vecToPoint, &vecToPoint);
                }
                vecToPoint.y = 0;
                if (VEC_DotProduct(&vecToPoint, &vecAxis) >= nCosMin) {
                    bHit = 1;
                    if (Session_GetLocalPlayerIndex() == 0 && pCtx != 0) {
                        Ov022_BuildHitPush(&hit, pCtx, &pFan->vecOrigin, pPoint,
                                            &pFan->vecDir);
                        Ov022_ReportHit(pActor, pCtx, pMember, &hit, 0);
                        if (VEC_Distance(&pFan->vecOrigin,
                                         &Ov002_TriggerEntryActive(pMember)->vecPos)
                            >= pFan->nRadius + IMPACT_MARGIN) {
                            VEC_Subtract(&Ov002_TriggerEntryActive(pMember)->vecPos,
                                         &pFan->vecOrigin, &vecToPoint);
                            if (VEC_Mag(&vecToPoint) != 0) {
                                VEC_Normalize(&vecToPoint, &vecToPoint);
                            }
                            VEC_MultAdd(pFan->nRadius, &vecToPoint, &pFan->vecOrigin,
                                        &pActor->vecImpact);
                        }
                    }
                }
            }
            pMember = pMember->pNext;
        } while (pMember != 0);
    }
    return bHit;
}
