/* Ov022_SendAimAtTarget -- aim one step short of a target and send it.
 *
 * The point is the actor's own reference for the target raised by the caller's
 * height, pushed one unit back along the direction from the target to the
 * source, so the aim lands just in front of it. The record carries the actor's
 * kind, the facing angle and a fixed pair of values.
 *
 * When the caller asks for the check, an actor whose hit state is held sends
 * nothing, and a busy one only sends while neither its own hit result bits nor
 * its hit owner's flag are set.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct AimRecord {
    u8 nKind;                    /* 0x00 */
    u8 pad001[3];
    VecFx32 vecPos;       /* 0x04 */
    short nField10;              /* 0x10 */
    short nField12;              /* 0x12 */
    u8 nSub;                     /* 0x14 */
    u8 nFlags;                   /* 0x15 */
    u16 nAngle;                  /* 0x16 */
    u16 nRange;                  /* 0x18 */
};

struct ActorNode {
    u8 pad00[0x80];
    u16 nAngle;                  /* 0x80 */
};

struct HitOwner {
    u8 pad000[0x1ac];
    u16 nFlags;                  /* 0x1ac */
};

struct Actor {
    u8 pad0000[9];
    u8 nKind;                    /* 0x0009 */
    u8 pad000a[0x16];
    struct ActorNode *pNode;     /* 0x0020 */
    u8 pad0024[0x2698];
    unsigned int nHitResult;     /* 0x26bc */
    u8 pad26c0[4];
    u8 nHitState;                /* 0x26c4 */
    u8 pad26c5[0xf];
    struct HitOwner *pHitOwner;  /* 0x26d4 */
};

extern void func_ov022_020ad44c(VecFx32 *pOut, struct Actor *pActor,
                                VecFx32 *pTo, int nHeight);
extern void VEC_Subtract(VecFx32 *pA, VecFx32 *pB,
                         VecFx32 *pOut);
extern int VEC_Normalize(VecFx32 *pOut, VecFx32 *pIn);
extern void VEC_Add(VecFx32 *pA, VecFx32 *pB,
                    VecFx32 *pOut);
extern void Ov022_SendAimMessage(void *pOwner, int nSlot,
                                struct AimRecord *pRec);

#define AIM_FIELD10 0xa3d
#define AIM_RANGE 0x61
#define AIM_SLOT 1
#define OWNER_OFFSET 0x2648

#define HIT_STATE_BUSY 1
#define HIT_STATE_HELD 2

void Ov022_SendAimAtTarget(struct Actor *pActor, VecFx32 *pFrom,
                         VecFx32 *pTo, int nHeight, int bCheck)
{
    VecFx32 vecPos;
    VecFx32 vecDir;
    struct AimRecord rec;
    int bSend;

    bSend = 1;
    func_ov022_020ad44c(&vecPos, pActor, pTo, nHeight);
    VEC_Subtract(pFrom, pTo, &vecDir);
    VEC_Normalize(&vecDir, &vecDir);
    VEC_Add(&vecPos, &vecDir, &vecPos);
    vecPos.y = vecPos.y + nHeight;
    rec.nKind = pActor->nKind;
    rec.vecPos = vecPos;
    rec.nField10 = AIM_FIELD10;
    rec.nField12 = pActor->pNode->nAngle - 0x8000;
    rec.nSub = 0;
    if (bCheck != 0) {
        if (pActor->nHitState == HIT_STATE_BUSY) {
            if ((pActor->nHitResult & 0x11) != 0) {
                bSend = 0;
            }
            if ((pActor->pHitOwner->nFlags & 4) != 0) {
                bSend = 0;
            }
        } else if (pActor->nHitState == HIT_STATE_HELD) {
            bSend = 0;
        }
    }
    rec.nFlags = 2;
    if (bSend != 0) {
        rec.nFlags |= 1;
        rec.nAngle = 0;
        rec.nRange = AIM_RANGE;
        Ov022_SendAimMessage((u8 *)pActor + OWNER_OFFSET, AIM_SLOT, &rec);
    }
}
