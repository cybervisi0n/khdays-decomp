/* Ov022_SettlePointOnGround -- find where a move actually ends up.
 *
 * The move is cast first: anything solid in the way stops it short of the
 * contact by the caster's own radius, and a surface tagged as pass-through is
 * ignored so the move runs its full length through it.
 *
 * Whatever point that leaves is then dropped onto the ground: it is lifted a
 * little, a long downward cast looks for the floor, and the point settles on
 * whatever it finds. With no floor under it the lift is simply taken back.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct CollCastParams {
    VecFx32 *pOrigin;         /* 0x00 */
    VecFx32 *pDir;            /* 0x04 */
    int nRadius;                     /* 0x08 */
    u16 wDirIsUnit;                  /* 0x0c */
    u16 wFlagE;                      /* 0x0e */
    void *pExtra;                    /* 0x10 */
};

struct HitInfo {
    u8 pad00[0x80];
    u8 aSurfaceSlots[4];             /* 0x80 */
};

struct Hit {
    void *pModel;                    /* 0x00 */
    struct HitInfo *pInfo;           /* 0x04 */
    void *pState;                    /* 0x08 */
    int nNearestHit;                 /* 0x0c */
};

struct CollSurfaceAttr {
    u8 pad00[0xc];
    u8 aTags[4];                     /* 0x0c */
};

struct ActorNode;

/* Ov022Actor */
struct Actor {
    u8 pad0000[0x20];
    struct ActorNode *pNode;         /* 0x0020 */
};

/* Ov022ReactionCtx */
struct ReactionCtx {
    u8 pad00[0x58];
    struct Actor *pActor;            /* 0x58 */
};

extern struct Hit *func_0202c248(int nGroup, struct CollCastParams *pCast);
extern struct Hit *func_0202c208(int nGroup, struct CollCastParams *pCast);
extern struct CollSurfaceAttr *Actor_GetRecord(struct Hit *pHit, unsigned int nSlot);
extern void Vec3ScaleAddQ27(int nScale, VecFx32 *pDir, VecFx32 *pBase,
                          VecFx32 *pOut);
extern void VEC_Add(VecFx32 *pA, VecFx32 *pB, VecFx32 *pOut);

#define CAST_FLAGS 0xf
#define SURFACE_PASS_THROUGH 2
#define SURFACE_SLOTS 4
#define GROUND_LIFT 0x14000
#define GROUND_REACH 0x28000

void Ov022_SettlePointOnGround(VecFx32 *pOut, struct ReactionCtx *pCtx,
                         VecFx32 *pOrigin, VecFx32 *pDir,
                         int nRadius, unsigned int nGroup)
{
    VecFx32 vecAt;
    VecFx32 vecDrop;
    struct CollCastParams cast;
    struct Actor *pActor;
    int nSlot;
    int bStopped;
    struct Hit *pHit;
    struct CollSurfaceAttr *pAttr;

    pActor = pCtx->pActor;
    bStopped = 0;
    cast.wFlagE = CAST_FLAGS;
    cast.pOrigin = pOrigin;
    cast.pDir = pDir;
    cast.wDirIsUnit = 0;
    cast.pExtra = pActor->pNode;
    cast.nRadius = nRadius;
    pHit = func_0202c248((u16)nGroup, &cast);
    if (pHit != 0) {
        nSlot = 0;
        do {
            pAttr = Actor_GetRecord(pHit, pHit->pInfo->aSurfaceSlots[nSlot]);
            if (pAttr != 0 && pAttr->aTags[0] == SURFACE_PASS_THROUGH) {
                pHit = 0;
                break;
            }
            nSlot++;
        } while (nSlot < SURFACE_SLOTS);
        if (pHit != 0) {
            bStopped = 1;
            Vec3ScaleAddQ27(pHit->nNearestHit - nRadius, pDir, pOrigin, &vecAt);
        }
    }
    if (bStopped == 0) {
        VEC_Add(pOrigin, pDir, &vecAt);
    }
    vecAt.y = vecAt.y + GROUND_LIFT;
    vecDrop.x = 0;
    vecDrop.y = -GROUND_REACH;
    vecDrop.z = 0;
    cast.pOrigin = &vecAt;
    cast.wDirIsUnit = 0;
    cast.pDir = &vecDrop;
    cast.wFlagE = CAST_FLAGS;
    cast.pExtra = pActor->pNode;
    /* Written as a mask where another call truncates with a cast: mwcc would otherwise compute the
     * truncation once and keep it, while the ROM truncates again at each call. */
    pHit = func_0202c208(nGroup & 0xffff, &cast);
    if (pHit != 0) {
        Vec3ScaleAddQ27(pHit->nNearestHit, &vecDrop, &vecAt, &vecAt);
    } else {
        vecAt.y = vecAt.y - GROUND_LIFT;
    }
    *pOut = vecAt;
}
