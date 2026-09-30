/* Aimed attack wait tick of the ov158 enemy: the +0x18 direction takes the owner's +0xa0 basis
 * turned by the +0x39c aim (9f48) and scaled by its reach. The +0x3c timer accumulates the
 * owner's rate: in phase 0 (+0x54) past 0x22aa the data_ov158_020cf540[8..9] message pair goes
 * to the +0x24 hook (arg 4), phase 1 begins and reaction 0x150 mode 6 fires at the +0x398
 * bone's +0x14; in phase 1 past 0x23bb the +0x3a0 emitter is launched (Ov158_RelayoutAndStoreVec)
 * from that bone along the sine/cosine of the +0xc heading and phase 2 begins. Once the +4
 * item is idle (+0xad) the +0x44 delay is re-armed to a random value in [+0x224, +0x228],
 * sub-state 2 is queued and the state ends. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

static inline unsigned short FX_RadToIdx(int rad) {
    return (unsigned short)((0x28BE60DB9391LL * rad + 0x80000000000LL) >> 44);
}

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void Ov158_RelayoutAndStoreVec(int emitter, void *at, VecFx32 *dir);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern unsigned short data_ov158_020cf540[];
extern short data_0203d210[];

void Ov158_AimedAttackWaitTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 aim;
    VecFx32 dir;
    unsigned short pair[2];
    unsigned short *pp;
    void (*cb)();
    unsigned short idx;
    int reach;
    int lo;
    int diff;

    reach = Ov107_ActionResource_GetOffsetAndScale(*(int *)(*state + 0x39c), &aim);
    Vec3TransformViaTempMtx((VecFx32 *)(state + 6), (char *)*state + 0xa0, &aim);
    ScaleVec3Fx12(reach, (VecFx32 *)(state + 6), (VecFx32 *)(state + 6));
    state[0xf] += *(int *)(node[0] + 0x2c);
    if (*(unsigned char *)((char *)state + 0x54) == 0 && state[0xf] > 0x22aa) {
        pp = pair;
        pp[1] = data_ov158_020cf540[9];
        pp[0] = data_ov158_020cf540[8];
        cb = *(void (**)())(*state + 0x24);
        if (cb != 0) cb(*state, pp, 4);
        *(unsigned char *)((char *)state + 0x54) = 1;
        Ov107_BuildAndSendUpdate(*state, 0x150, 6, (void *)(*(int *)(*state + 0x398) + 0x14));
    } else if (*(unsigned char *)((char *)state + 0x54) == 1 && state[0xf] > 0x23bb) {
        idx = FX_RadToIdx(state[3]);
        dir.x = data_0203d210[(idx >> 4) * 2];
        dir.y = 0;
        dir.z = data_0203d210[(idx >> 4) * 2 + 1];
        Ov158_RelayoutAndStoreVec(*(int *)(*state + 0x3a0), (void *)(*(int *)(*state + 0x398) + 0x14), &dir);
        *(unsigned char *)((char *)state + 0x54) = 2;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    lo = *(int *)(*state + 0x224);
    diff = *(int *)(*state + 0x228) - lo;
    if (diff < 0) {
        diff = -diff;
    }
    state[0x11] = lo + RandNextScaled(diff + 1);
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
