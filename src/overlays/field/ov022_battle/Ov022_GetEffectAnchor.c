/* ov022: work out the actor's effect anchor in world space.
 *
 * The anchor is a slot position, which is given in the actor's own frame, so it
 * has to be turned the way the actor is turned and then moved to where the
 * actor was. Two rotations do the turning: the replay angle about Z and the
 * facing about Y, the latter negated because the facing is stored biased and
 * measured the other way round. The result is added to the position the actor
 * held on the previous tick.
 *
 * A second slot is looked up first and its position is never read. The call is
 * kept because the lookup is what refreshes the slot, not because the caller
 * wants the point.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define ANGLE_BIAS 0x8000
#define ANGLE_SHIFT 4

struct MtxFx33 {
    int a[9];
};

/* Ov022ActorNode */
struct ActorNode {
    u8 pad00[0x80];
    u16 nAngle;                  /* 0x80 */
    u16 nAngleReplay;            /* 0x82 */
};

/* Ov022Actor */
struct Actor {
    u8 pad0000[0x20];
    struct ActorNode *pNode;     /* 0x0020 */
    u8 pad0024[0x4f8];
    int nSlotA;                  /* 0x051c */
    u8 pad0520[4];
    int nSlotB;                  /* 0x0524 */
    u8 pad0528[0x3dc];
    VecFx32 vecPrevPos;   /* 0x0904 */
};

/* kFxSinCosTable, read as one flat array of halfwords; const so the reads can
 * move past the stores they feed. */
extern const s16 data_0203d210[];

extern void Ov022_GetSlotPos(struct Actor *pActor, int nSlot,
                                VecFx32 *pOut);
extern void MTX_RotZ33_(struct MtxFx33 *pMtx, int nSin, int nCos);
extern void MTX_RotY33_(struct MtxFx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pVec, const struct MtxFx33 *pMtx,
                          VecFx32 *pOut);
extern void VEC_Add(const VecFx32 *pA, const VecFx32 *pB,
                    VecFx32 *pOut);

void Ov022_GetEffectAnchor(VecFx32 *pOut, struct Actor *pActor)
{
    VecFx32 vecAnchor;
    VecFx32 vecSpare;
    struct MtxFx33 mtxFacing;
    struct MtxFx33 mtxReplay;
    VecFx32 vecWorld;
    struct ActorNode *pNode;
    int nIndex;

    pNode = pActor->pNode;
    Ov022_GetSlotPos(pActor, pActor->nSlotA, &vecSpare);
    Ov022_GetSlotPos(pActor, pActor->nSlotB, &vecAnchor);
    nIndex = pNode->nAngleReplay >> ANGLE_SHIFT;
    MTX_RotZ33_(&mtxReplay, data_0203d210[nIndex * 2],
                data_0203d210[nIndex * 2 + 1]);
    nIndex = (u16)(pActor->pNode->nAngle - ANGLE_BIAS) >> ANGLE_SHIFT;
    MTX_RotY33_(&mtxFacing, -data_0203d210[nIndex * 2],
                -data_0203d210[nIndex * 2 + 1]);
    MTX_MultVec33(&vecAnchor, &mtxReplay, &vecAnchor);
    MTX_MultVec33(&vecAnchor, &mtxFacing, &vecAnchor);
    VEC_Add(&vecAnchor, &pActor->vecPrevPos, &vecWorld);
    *pOut = vecWorld;
}
