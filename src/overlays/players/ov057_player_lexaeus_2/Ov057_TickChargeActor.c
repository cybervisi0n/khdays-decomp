
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef int (*ActorHook)(int pActor);
typedef void (*ActorFinishHook)(int pActor, int mode);

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov022_IsState9Or6WithFlag200(void *state);
extern int Ov022_ActorSetState(int pActor, int mode);
extern int data_ov057_020b74a0;

/* Ticks the charge phase, then either advances to mode 0x22 or clears both
 * motion vectors and finishes the actor once its charge sequence ends. */
int Ov057_TickChargeActor(int pActor)
{
    VecFx32 zero;
    VecFx32 delta;

    int pSceneBlock = data_ov057_020b74a0 + 0x2c + 0x2c00;
    int result = 0;
    int actionEnded = 0;
    int zeroValue;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(u64 *)(pActor + 0x464) |= 0x10000ULL;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(u64 *)(pActor + 0x46c) |= 0x10000ULL;
    }

    zero.x = zero.y = zero.z = 0;
    if ((*(u32 *)(pActor + 0x24) & 4) == 0) {
        *(u64 *)pActor |= 0x400000000000ULL;
        *(int *)(pActor + 0x58) = 0;
    }

    delta = zero;
    delta.y = 0;
    VEC_Add((VecFx32 *)(pActor + 0x498), &delta,
            (VecFx32 *)(pActor + 0x498));
    (*(ActorHook *)(pActor + 0x668))(pActor);

    if ((*(u16 *)(pActor + 0x1a) & 1) == 0) {
        actionEnded = 1;
    }
    if (Ov022_IsState9Or6WithFlag200((void *)(pActor + 0x22f8)) == 0 || actionEnded) {
        if (*(int *)(pSceneBlock + 0x228) > 1) {
            result = Ov022_ActorSetState(pActor, 0x22);
        } else {
            zeroValue = 0;
            *(int *)(pActor + 0x698) = *(int *)(pActor + 0x69c) =
                *(int *)(pActor + 0x6a0) = *(int *)(pActor + 0x498) =
                *(int *)(pActor + 0x49c) = *(int *)(pActor + 0x4a0) = zeroValue;
            *(u64 *)pActor |= 4ULL;
            if ((*(u32 *)(pActor + 0x24) & 4) != 0) {
                (*(ActorFinishHook *)(pActor + 0x664))(pActor, zeroValue);
                result = Ov022_ActorSetState(pActor, 0);
            } else {
                result = Ov022_ActorSetState(pActor, 2);
            }
        }
    }
    return result;
}