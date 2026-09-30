/* Shot windup of the ov123 enemy (and its byte-identical twin): re-acquires the target into
 * +0x24 (none requests sub-state 2 and ends the state); the +0x28 timer accumulates the
 * frame-time and, once past 0xccc with the +0x2c flag clear, the direction from the +0x390
 * item's +0x14 point to the target's +0x74 is normalised, its height kept, the forward axis
 * rotated by the actor's +0xa0 orientation takes the flat part, the flag latches and the +0x394
 * shot is launched (cd484) along it. Once the +0x30 busy byte clears sub-state 2 is requested
 * and the state ends. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov123_FindTarget(int actor, int mode);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void Ov123_StoreVec3ThenSetupAndSetHw60(int shot, void *from, VecFx32 *dir);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;

void Ov123_ShotWindup(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int y;

    state[9] = Ov123_FindTarget(*state, 0);
    if (state[9] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[10] += *(int *)(*node + 0x2c);
    if (state[0xb] == 0 && state[10] >= 0xccc) {
        VEC_Subtract((void *)(state[9] + 0x74), (void *)(*(int *)(*state + 0x390) + 0x14), &dir);
        VEC_Normalize(&dir, &dir);
        y = dir.y;
        state[0xb] = 1;
        Vec3TransformViaTempMtx(&dir, (void *)(*state + 0xa0), &data_02042258);
        dir.y = y;
        VEC_Normalize(&dir, &dir);
        Ov123_StoreVec3ThenSetupAndSetHw60(*(int *)(*state + 0x394), (void *)(*(int *)(*state + 0x390) + 0x14), &dir);
    }
    if (*(unsigned char *)state[0xc] != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
