/* ov022: keep the actor's second effect slot alive, re-placing it toward the
 * camera whenever it is not.
 *
 * A slot that is still held is asked whether its record has run out; one that
 * has is thrown away so the same pass can start a fresh one.
 *
 * A fresh one is placed one unit from the actor toward the camera: the actor's
 * anchor is raised a little, the direction from there to the cached camera
 * position is normalised, and the anchor is walked along it. A request the
 * dispatcher refuses comes back negative and is stored but not used further.
 *
 * A slot that is still running is stepped instead of being re-placed.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define ANCHOR_RAISE 0x200
#define ANCHOR_REACH 0x800
#define EMIT_PARAM 0xb33

/* Ov022Actor */
struct Actor {
    u8 pad0000[0x7b8];
    int nEffectA;                /* 0x07b8 */
    int nEffectB;                /* 0x07bc */
};

/* gCameraCachePos */
extern VecFx32 data_020475ac;

extern void func_ov022_020ad44c(VecFx32 *pOut, struct Actor *pActor);
extern int Ov022_IsIndexedRecordByteZero(int nContext, int nSlot);
extern int Ov022_DispatchSpawnRecord(int nContext, VecFx32 *pAt,
                               int nValue);
extern void Ov022_StoreVToBase101418IfNonNeg(int nContext, int nSlot, int nValue);
extern void func_ov022_020894cc(int nContext, int nSlot, int nValue);
extern void VEC_Subtract(const VecFx32 *pA, const VecFx32 *pB,
                         VecFx32 *pOut);
extern void VEC_MultAdd(int nFactor, const VecFx32 *pStep,
                        const VecFx32 *pFrom, VecFx32 *pOut);
/* VEC_Normalize */
extern int VEC_Normalize(const VecFx32 *pSrc, VecFx32 *pDst);

void Ov022_StepEffectTowardCamera(struct Actor *pActor)
{
    VecFx32 vecAt;
    VecFx32 vecDir;

    if (pActor->nEffectA == 0) {
        return;
    }
    if (pActor->nEffectB >= 0) {
        if (Ov022_IsIndexedRecordByteZero(pActor->nEffectA, pActor->nEffectB) != 0) {
            pActor->nEffectB = -1;
        }
    }
    if (pActor->nEffectB < 0) {
        func_ov022_020ad44c(&vecAt, pActor);
        vecAt.y = vecAt.y + ANCHOR_RAISE;
        VEC_Subtract(&data_020475ac, &vecAt, &vecDir);
        VEC_Normalize(&vecDir, &vecDir);
        VEC_MultAdd(ANCHOR_REACH, &vecDir, &vecAt, &vecAt);
        pActor->nEffectB = Ov022_DispatchSpawnRecord(pActor->nEffectA, &vecAt, 0);
        if (pActor->nEffectB < 0) {
            return;
        }
        Ov022_StoreVToBase101418IfNonNeg(pActor->nEffectA, pActor->nEffectB, EMIT_PARAM);
    } else {
        func_ov022_020894cc(pActor->nEffectA, pActor->nEffectB, 0);
    }
}
