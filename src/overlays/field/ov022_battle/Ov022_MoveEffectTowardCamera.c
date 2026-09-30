/* ov022: move the actor's held effect slot to keep facing the camera.
 *
 * The companion to the step that creates the slot. This one only runs while a
 * slot is actually held, and it moves rather than places: the actor's anchor is
 * raised the same amount, the direction to the cached camera position is
 * normalised and then flattened, and the anchor is walked one unit along the
 * result. Dropping the vertical keeps the effect level with the actor however
 * far above or below the camera sits.
 *
 * The slot is asked afterwards whether its record has run out, and released
 * when it has.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define ANCHOR_RAISE 0x200
#define ANCHOR_REACH 0x800

/* Ov022Actor */
struct Actor {
    u8 pad0000[0x7b8];
    int nEffectA;                /* 0x07b8 */
    int nEffectB;                /* 0x07bc */
};

/* gCameraCachePos */
extern VecFx32 data_020475ac;

extern void func_ov022_020ad44c(VecFx32 *pOut, struct Actor *pActor);
extern void func_ov022_02089478(int nContext, int nSlot, VecFx32 *pAt);
extern int Ov022_IsIndexedRecordByteZero(int nContext, int nSlot);
extern void VEC_Subtract(const VecFx32 *pA, const VecFx32 *pB,
                         VecFx32 *pOut);
extern void VEC_MultAdd(int nFactor, const VecFx32 *pStep,
                        const VecFx32 *pFrom, VecFx32 *pOut);
/* VEC_Normalize */
extern int VEC_Normalize(const VecFx32 *pSrc, VecFx32 *pDst);

void Ov022_MoveEffectTowardCamera(struct Actor *pActor)
{
    VecFx32 vecAt;
    VecFx32 vecDir;

    if (pActor->nEffectA == 0) {
        return;
    }
    if (pActor->nEffectB < 0) {
        return;
    }
    func_ov022_020ad44c(&vecAt, pActor);
    vecAt.y = vecAt.y + ANCHOR_RAISE;
    VEC_Subtract(&data_020475ac, &vecAt, &vecDir);
    VEC_Normalize(&vecDir, &vecDir);
    vecDir.y = 0;
    VEC_MultAdd(ANCHOR_REACH, &vecDir, &vecAt, &vecAt);
    func_ov022_02089478(pActor->nEffectA, pActor->nEffectB, &vecAt);
    if (Ov022_IsIndexedRecordByteZero(pActor->nEffectA, pActor->nEffectB) != 0) {
        pActor->nEffectB = -1;
    }
}
