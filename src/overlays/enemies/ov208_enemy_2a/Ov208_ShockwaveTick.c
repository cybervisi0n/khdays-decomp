/* Shockwave tick of the ov208 enemy (x3 with ov209/ov268). While the +0x2c timer (fed by the
 * owner's rate) is at most 0x3b8, an axis-aligned box centred on the +0x20 point with half-extent
 * timer x 5 / 0x3b8 (fixed point) pushes every entity whose +0x1b4 kind bit is clear in the +0x4e
 * mask away by 0x800 (kind 1); on acceptance effect 1 spawns at the entity's +0x74 position,
 * reaction 0x154 mode 9 fires there and the kind bit is set. Once the +0x50 idle byte clears
 * sub-state 2 is requested and the state ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct BoxQuery {
    VecFx32 vCenter;
    VecFx32 vAxisX;
    VecFx32 vAxisZ;
    VecFx32 vAxisY;
    int nExtent;
    int bFlag;
};

extern int Ov107_CollectEntitiesTouchingDisc(int owner, struct BoxQuery *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;

void Ov208_ShockwaveTick(int *node)
{
    int *state = (int *)node[1];
    int hits[4];
    struct BoxQuery query;
    VecFx32 push;
    VecFx32 at;
    int n;
    int i;

    state[0xb] += *(int *)(*node + 0x2c);
    if (state[0xb] <= 0x3b8) {
        query.vCenter = *(VecFx32 *)(state + 8);
        query.vAxisX = data_02042270;
        query.vAxisZ = data_02042258;
        query.vAxisY = data_02042264;
        query.nExtent = (state[0xb] * 5 << 12) / 0x3b8;
        query.bFlag = 1;
        n = Ov107_CollectEntitiesTouchingDisc(*state, &query, hits);
        i = 0;
        if (n > 0) {
            do {
                if (((*((u8 *)state + 0x4e) >> *(u8 *)(hits[i] + 0x1b4)) & 1) == 0) {
                    VEC_Subtract((void *)(hits[i] + 0x74), &query.vCenter, &push);
                    VEC_Normalize(&push, &push);
                    ScaleVec3Fx12(0x800, &push, &push);
                    if (Ov107_InvokeHitCallback(hits[i], *state, *state, 1, &push, 0) != 0) {
                        at = *(VecFx32 *)(hits[i] + 0x74);
                        func_ov107_020c0b90(*state, 1, at, 0);
                        Ov107_BuildAndSendUpdate(*state, 0x154, 9, &at);
                        *((u8 *)state + 0x4e) |= 1 << *(u8 *)(hits[i] + 0x1b4);
                    }
                }
            } while (++i < n);
        }
    }
    if (*(u8 *)state[0x14] != 0) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
