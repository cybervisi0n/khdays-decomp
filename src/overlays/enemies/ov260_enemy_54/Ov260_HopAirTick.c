/* Hop air tick of the ov260 actor: the +0x70 timer accumulates the frame rate, +0x20 follows the
 * +0x2c velocity with the +0x428 part's +0x30 height and the velocity is damped in 0x88 steps of the
 * frame time. Once the partner holds no queued move the first time pose 0x1d plays and the part takes
 * motion 0x11 (+0x7b bit 0); after that, farther than 1.0 (flat) from the +0x420 target it lands at
 * the target's point raised by its +0x13c height plus 3.82 (+0x14), arms the recoil entry (+0xc =
 * 020cf89c) and moves on to 020cf484, otherwise it moves on to the recoil entry directly. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_RecoilEntry(void);
extern void Ov260_BurstEntry(void);

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov260_HopAirTick(int *node)
{
    VecFx32 *vel;
    int *state = (int *)node[1];
    VecFx32 d;
    int left;

    vel = (VecFx32 *)(state + 0xb);
    state[0x1c] += *(int *)(node[0] + 0x2c);
    *(VecFx32 *)(state + 8) = *vel;
    state[9] = *(int *)(*(int *)(*state + 0x428) + 0x30);
    for (left = *(int *)(node[0] + 0x2c); left > 0; left -= 0x88) {
        ScaleVec3Fx12(0x1000 - FX_MUL(FX_Div(left <= 0x88 ? left : 0x88, 0x88), 0x11f), vel, vel);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if ((*((u8 *)state + 0x7b) & 1) == 0) {
        *((u8 *)state + 0x7b) |= 1;
        Ov107_PostTagUpdate((Actor *)(*state), 0x1d, 0);
        Ov107_StartAnim(*(int *)(*state + 0x428), 0x11, 0);
        return;
    }
    VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x420) + 0x190), (VecFx32 *)state[4], &d);
    d.y = 0;
    if (VEC_Normalize(&d, &d) > 0x1000) {
        *(VecFx32 *)(state + 5) = *(VecFx32 *)(*(int *)(*state + 0x420) + 0x190);
        state[6] += *(int *)(*state + 0x13c) + 0x3d2b;
        state[3] = (int)Ov260_RecoilEntry;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_BurstEntry);
        return;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_RecoilEntry);
}
