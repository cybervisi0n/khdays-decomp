/* Waits for the state-0x21 actor action to finish. It advances the motion callback, then either
 * transitions through state 0x22 when the scene phase is above 1 or clears both motion vectors and
 * returns the actor to mode 0 or 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef int (*ActorHook)(int actor);
typedef void (*ActorFinishHook)(int actor, int mode);

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov022_IsState9Or6WithFlag200(void *state);
extern int Ov022_ActorSetState(int actor, int mode);
extern int data_ov038_020b4ca0;

int Ov038_WaitForActionCompletion(int actor)
{
    VecFx32 zero;
    VecFx32 delta;

    int scene = data_ov038_020b4ca0 + 0x2c + 0x2c00;
    int result = 0;
    int canFinish = 0;
    int clear;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(u64 *)(actor + 0x464) |= 0x10000ULL;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(u64 *)(actor + 0x46c) |= 0x10000ULL;
    }

    zero.x = zero.y = zero.z = 0;
    if ((*(u32 *)(actor + 0x24) & 4) == 0) {
        *(u64 *)actor |= 0x400000000000ULL;
        *(int *)(actor + 0x58) = 0;
    }

    delta = zero;
    delta.y = 0;
    VEC_Add((VecFx32 *)(actor + 0x498), &delta,
            (VecFx32 *)(actor + 0x498));
    (*(ActorHook *)(actor + 0x668))(actor);

    if ((*(u16 *)(actor + 0x1a) & 1) == 0) {
        canFinish = 1;
    }
    if (Ov022_IsState9Or6WithFlag200((void *)(actor + 0x22f8)) == 0 || canFinish) {
        if (*(int *)(scene + 0x228) > 1) {
            result = Ov022_ActorSetState(actor, 0x22);
        } else {
            clear = 0;
            *(int *)(actor + 0x698) = *(int *)(actor + 0x69c) =
                *(int *)(actor + 0x6a0) = *(int *)(actor + 0x498) =
                *(int *)(actor + 0x49c) = *(int *)(actor + 0x4a0) = clear;
            *(u64 *)actor |= 4ULL;
            if ((*(u32 *)(actor + 0x24) & 4) != 0) {
                (*(ActorFinishHook *)(actor + 0x664))(actor, clear);
                result = Ov022_ActorSetState(actor, 0);
            } else {
                result = Ov022_ActorSetState(actor, 2);
            }
        }
    }
    return result;
}