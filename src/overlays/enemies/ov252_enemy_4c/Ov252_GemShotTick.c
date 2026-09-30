/* Tick of an ov252 gem shot: its model's +0x5c bit 1 clears and +0x30 accumulates the frame rate; the
 * nearest target (020cab14) goes to +8, and without one the layers fade (mode 2) and the node moves on
 * to 020d38e8. The shot homes on the target until 0.81 (or while unlocked, +0x35), then keeps the
 * locked +0x24 heading; it moves at 1.0 (+0x35 = 1), 0.375 (+0x38 big) or 0.1875. In phase 1 its
 * 2.0 sphere pushes targets up and away (020ca918 kind 1) with sound 0x148/9 and effect 1, flipping
 * the shot's bit in the spawner's +0x57c mask; hitting ground or walls flips it too. The model follows
 * the shot, a pending +0x36 start plays the layers (mode 1, 1), and past its range (2.5 / 5.75 / 3.5)
 * the bit flips, +0x3c is set and effect 1 plays. With the bit clear the layers fade and the node moves
 * on to 020d38e8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;

extern int Ov107_FindNearestObject(int actor, int *distOut);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int Collision_CastRay(int grid, void *pos, void *vel);
extern int Collision_CastSphere(int grid, void *pos, void *vel, int radius);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern void Ov252_GemFinishTick(void);

void Ov252_GemShotTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 aim;
    int hits[4];
    Sphere sphere;
    VecFx32 push;
    signed char i;
    int n;
    int owner;
    int speed;
    int range;

    *(int *)(*state + 0x5c) &= ~2;
    state[0xc] += *(int *)(node[0] + 0x2c);
    state[2] = Ov107_FindNearestObject(state[1], 0);
    if (state[2] == 0) {
        SetSubitemState(*state, 0, 2, 0);
        SetSubitemState(*state, 2, 2, 0);
        SetSubitemState(*state, 4, 2, 0);
        SetSubitemState(*state, 1, 2, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_GemFinishTick);
        return;
    }
    VEC_Subtract((VecFx32 *)(state[2] + 0x190), (VecFx32 *)(state + 3), &aim);
    VEC_Normalize(&aim, &aim);
    if (state[0xc] < 0xd00 || *((u8 *)state + 0x35) == 0) {
        *(VecFx32 *)(state + 6) = aim;
        *(VecFx32 *)(state + 9) = aim;
    } else {
        *(VecFx32 *)(state + 6) = *(VecFx32 *)(state + 9);
    }
    speed = *((u8 *)state + 0x35) == 1 ? 0x1000 : (state[0xe] != 0 ? 0x600 : 0x300);
    ScaleVec3Fx12(speed, (VecFx32 *)(state + 6), (VecFx32 *)(state + 6));
    VEC_Add((VecFx32 *)(state + 3), (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
    if (*(int *)(state[1] + 0x50) == 1) {
        sphere.center = *(VecFx32 *)(state + 3);
        sphere.nRadius = 0x2000;
        n = Ov107_CollectSphereOverlaps(state[1], &sphere, hits);
        for (i = 0; i < n; i++) {
            VEC_Subtract((VecFx32 *)(hits[i] + 0x190), (VecFx32 *)(state + 3), &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            push.y += 0x1000;
            if (Ov107_InvokeHitCallback(hits[i], state[1], state[1], 1, &push, 0) != 0) {
                Ov107_BuildAndSendUpdate(state[1], 0x148, 9, state + 3);
                func_ov107_020c0b90(state[1], 1, *(VecFx32 *)(state + 3), 0);
                *(u16 *)(state[1] + 0x57c) ^= 1 << *((signed char *)state + 0x34);
            }
        }
    }
    owner = *(int *)(state[1] + 4);
    if (Collision_CastRay(*(int *)(owner + 0x7c), state + 3, state + 6) != 0) {
        *(u16 *)(state[1] + 0x57c) ^= 1 << *((signed char *)state + 0x34);
    }
    if (Collision_CastSphere(*(int *)(owner + 0x7c), state + 3, state + 6, 0x2000) != 0) {
        *(u16 *)(state[1] + 0x57c) ^= 1 << *((signed char *)state + 0x34);
    }
    Srt_SetTranslation((void *)(*state + 4), (VecFx32 *)(state + 3));
    if (*(u8 *)(*state + 0xad) == 0 && *((u8 *)state + 0x36) != 0) {
        *((u8 *)state + 0x36) = 0;
        SetSubitemState(*state, 0, 1, 1);
        SetSubitemState(*state, 2, 1, 1);
        SetSubitemState(*state, 4, 1, 1);
        SetSubitemState(*state, 1, 1, 1);
    }
    range = *((u8 *)state + 0x35) == 1 ? 0x2800 : (state[0xe] != 0 ? 0x5c00 : 0x3800);
    if (state[0xc] >= range) {
        *(u16 *)(state[1] + 0x57c) ^= 1 << *((signed char *)state + 0x34);
        state[0xf] = 1;
        func_ov107_020c0b90(state[1], 1, *(VecFx32 *)(state + 3), 0);
    }
    if (*(u16 *)(state[1] + 0x57c) & (1 << *((signed char *)state + 0x34))) {
        return;
    }
    SetSubitemState(*state, 0, 2, 0);
    SetSubitemState(*state, 2, 2, 0);
    SetSubitemState(*state, 4, 2, 0);
    SetSubitemState(*state, 1, 2, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_GemFinishTick);
}
