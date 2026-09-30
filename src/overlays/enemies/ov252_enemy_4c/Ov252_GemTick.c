/* Tick of an ov252 gem: its model's +0x5c bit 1 clears and +0x20 accumulates the frame rate; once its
 * part stops animating the first time, layers 0, 2, 4 and 1 play (mode 3, 1). While the spawner is in
 * phase 1 and after 0.23, a box around the gem (1.5 x 7.5 x scale x 1.5, raised 6.84 x scale) hits
 * targets: each is pushed away at 0.5 (020ca918 kind 4), the spawner plays effect 0x11 on it and sound
 * 0/0x53 at the gem, and the gem's bit in the spawner's +0x57e mask flips. The mask bit also flips for
 * every live bomb (+0x714 pairs) closer than 1.5, after 16.0, or when the spawner is within 10.0.
 * With the bit clear the layers fade (mode 4), the spawner plays effect 6 at the gem and the node moves
 * on to 020d4204. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; VecFx32 axisX; VecFx32 axisY; VecFx32 axisZ; int nExtentX; int nExtentY; int nExtentZ; } Box;
struct BombPair { int obj; int active; };
struct Ov252Spawner { char pad[0x714]; struct BombPair pair[12]; };

extern void SetSubitemState(int rig, int channel, int a, int b);
extern int Ov107_CollectCapsuleOverlaps(int owner, Box *box, int *hits);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_MarkerFinishTick(void);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

void Ov252_GemTick(int *node)
{
    int *state = (int *)node[1];
    int hits[4];
    Box box;
    VecFx32 push;
    VecFx32 diff;
    VecFx32 other;
    VecFx32 d;
    VecFx32 me;
    signed char i;
    int n;
    signed char j;
    int dist;

    *(int *)(*state + 0x5c) &= ~2;
    state[8] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x26) == 0 && *(u8 *)(*state + 0xad) == 0) {
        *((u8 *)state + 0x26) += 1;
        SetSubitemState(*state, 0, 3, 1);
        SetSubitemState(*state, 2, 3, 1);
        SetSubitemState(*state, 4, 3, 1);
        SetSubitemState(*state, 1, 3, 1);
    }
    if (*(int *)(state[1] + 0x50) == 1 && state[8] >= 0x3b8) {
        box.center = *(VecFx32 *)(state + 2);
        box.center.y += (state[6] / 0x1000) * 0x6d80;
        box.axisX = data_02042270;
        box.axisY = data_02042264;
        box.axisZ = data_02042258;
        box.nExtentX = 0x1800;
        box.nExtentY = (state[6] / 0x1000) * 0x7800;
        box.nExtentZ = 0x1800;
        n = Ov107_CollectCapsuleOverlaps(state[1], &box, hits);
        for (i = 0; i < n; i++) {
            VEC_Subtract((VecFx32 *)(hits[i] + 0x190), (VecFx32 *)(state + 2), &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], state[1], state[1], 4, &push, 0) != 0) {
                func_ov107_020c0b90(state[1], 0x11, *(VecFx32 *)(hits[i] + 0x190), 0);
                Ov107_BuildAndSendUpdate(state[1], 0, 0x53, state + 2);
                *(u16 *)(state[1] + 0x57e) ^= 1 << *((signed char *)state + 0x24);
            }
        }
    }
    for (j = 0; j < 12; j++) {
        if (((struct Ov252Spawner *)state[1])->pair[j].active != 0) {
            other = *(VecFx32 *)(((struct Ov252Spawner *)state[1])->pair[j].obj + 0x14);
            other.y = state[3];
            VEC_Subtract(&other, (VecFx32 *)(state + 2), &diff);
            if (VEC_Normalize(&diff, &diff) < 0x1800) {
                *(u16 *)(state[1] + 0x57e) ^= 1 << *((signed char *)state + 0x24);
            }
        }
    }
    me = *(VecFx32 *)(state + 2);
    me.y = *(int *)(state[1] + 0xb4);
    VEC_Subtract((VecFx32 *)(state[1] + 0xb0), &me, &d);
    dist = VEC_Normalize(&d, &d);
    if (state[8] >= 0x10000 || dist < 0xa000) {
        *(u16 *)(state[1] + 0x57e) ^= 1 << *((signed char *)state + 0x24);
    }
    if (*(u16 *)(state[1] + 0x57e) & (1 << *((signed char *)state + 0x24))) {
        return;
    }
    SetSubitemState(*state, 0, 4, 0);
    SetSubitemState(*state, 2, 4, 0);
    SetSubitemState(*state, 4, 4, 0);
    SetSubitemState(*state, 1, 4, 0);
    func_ov107_020c0b90(state[1], 6, *(VecFx32 *)(state + 2), 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_MarkerFinishTick);
}
