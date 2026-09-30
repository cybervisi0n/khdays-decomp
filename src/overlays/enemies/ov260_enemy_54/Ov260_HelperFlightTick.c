/* Flight tick of an ov260 helper: the +0x40 timer accumulates the frame rate and the +0x28 velocity is
 * the +8 facing at half speed; past the +0x44 flight time it sinks by 128/136 of the frame rate. A hit
 * (020d0e14 at the owner's position) ends it (move 0). Hitting a wall (020fff920) bounces it: the
 * impact point (+0x34) is the wall-reflected velocity from the +0x18 anchor, the owner is knocked back
 * there (mode 0), its +0x390 part plays effect 0x14 there, bit 7 of the +0x60 high byte is set and
 * bit 0 dropped, the +0x388 shape hides, the velocity rests, the timer restarts and the node moves on
 * to 020d16e4. A blocked path from the last point (+0x1c) or 5.0 of flight also end it (knock-back
 * mode 1, the blocked path with effect 0x14); otherwise +0x1c follows the anchor. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void Vec3TransformViaTempMtx(VecFx32 *out, void *q, const VecFx32 *in);
extern int Ov260_AttackHitTest(int *state, void *sphere, void *cyl);
extern void *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *direction);
extern void ScaleVec3Fixed27(int plane, VecFx32 *in, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_ShockwaveTick(void);
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov260_HelperFlightTick(int *node)
{
    int owner;
    int *state = (int *)node[1];
    int scene;
    VecFx32 dir;
    VecFx32 w;
    void *hit;

    owner = *state;
    scene = *(int *)(owner + 4);
    state[0x10] += *(int *)(node[0] + 0x2c);
    Vec3TransformViaTempMtx(&dir, (void *)(state + 2), &data_02042258);
    state[10] = FX_MUL(dir.x, 0x800);
    state[12] = FX_MUL(dir.z, 0x800);
    if (state[0x10] >= state[0x11]) {
        state[0xb] += -(*(int *)(node[0] + 0x2c) << 7) / 0x88;
    }
    if (Ov260_AttackHitTest(state, (void *)(owner + 0x74), 0)) {
        *(signed char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    hit = Collision_CastRay(*(void **)(scene + 0x7c), (VecFx32 *)state[6], (VecFx32 *)(state + 10));
    if (hit != 0) {
        w = *(VecFx32 *)(state + 10);
        ScaleVec3Fixed27(*(int *)((char *)hit + 0xc), &w, &w);
        VEC_Add(&w, (VecFx32 *)state[6], (VecFx32 *)(state + 0xd));
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)(state + 0xd), 0);
        Ov260_PlaySound(*(int *)(*state + 0x390), 0x14, (int)(state + 0xd));
        {
            u16 hw = *(u16 *)(*state + 0x60);
            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                ((((((unsigned int)hw << 0x10) >> 0x18) | 0x80) << 0x18) >> 0x10);
        }
        {
            u16 hw = *(u16 *)(*state + 0x60);
            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~1) << 0x18) >> 0x10);
        }
        ((B8 *)(*(int *)(*state + 0x388) + 8))->f &= ~1;
        *(VecFx32 *)(state + 10) = data_02041dc8;
        state[0x10] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_ShockwaveTick);
        return;
    }
    VEC_Subtract((VecFx32 *)state[6], (VecFx32 *)(state + 7), &dir);
    if (Collision_CastSphereEx(*(void **)(scene + 0x7c), (VecFx32 *)(state + 7), &dir, *(int *)(owner + 0x80) / 2, 0)) {
        func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[6], 0);
        Ov260_PlaySound(*(int *)(*state + 0x390), 0x14, state[6]);
        *(signed char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (state[0x10] >= 0x5000) {
        func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[6], 0);
        *(signed char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(VecFx32 *)(state + 7) = *(VecFx32 *)state[6];
}
