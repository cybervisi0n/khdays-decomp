/* Turn tick of the ov221 enemy. The target is re-acquired into the owner's +0x3e8; with one,
 * the tracking step (Ov225_MeasureTargetGap) ends the state when negative, otherwise the +0x3fc
 * part's +0x2c offset is rotated by the +0x50 heading (sine/cosine, MTX_RotY33_) into the
 * +0x14 point. The +0x5c timer accumulates the owner's rate and once (+0x75) past 0x440 fires
 * reaction 0x14b mode 5 at the +8 point. Once the +4 item goes idle (+0xad) the +0x78 target
 * flag is set, sub-state 6 is requested and the state ends. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

static inline unsigned short FX_RadToIdx(int rad) {
    return (unsigned short)((0x28BE60DB9391LL * rad + 0x80000000000LL) >> 44);
}

extern int Ov107_FindNearestObject(int owner, int flag);
extern int Ov225_MeasureTargetGap(int *node, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void MTX_RotY33_(Mtx33 *m, int s, int c);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *m, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern const short data_0203d210[];

void Ov225_TurnTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    unsigned short idx;

    *(int *)(*state + 0x3e8) = Ov107_FindNearestObject(*state, 0);
    if (*(int *)(*state + 0x3e8) != 0) {
        if (Ov225_MeasureTargetGap(node, 0) < 0) {
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
        idx = FX_RadToIdx(state[0x14]);
        MTX_RotY33_(&m, data_0203d210[(idx >> 4) * 2], data_0203d210[(idx >> 4) * 2 + 1]);
        MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x3fc) + 0x2c), &m, (VecFx32 *)(state + 5));
    }
    state[0x17] += *(int *)(node[0] + 0x2c);
    if (*(unsigned char *)((char *)state + 0x75) == 0 && state[0x17] >= 0x440) {
        Ov107_BuildAndSendUpdate(*state, 0x14b, 5, (void *)state[2]);
        *(unsigned char *)((char *)state + 0x75) = 1;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    state[0x1e] = 1;
    *(unsigned char *)(*state + 0x1c7) = 6;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
