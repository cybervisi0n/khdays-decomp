/* Hover tick of an ov254 helper: the +0x30 phase (a 0x3000 cycle) bobs a point 10.7 ahead of the
 * +0x394 owner at 3 * sin(phase) around 3.0 below its +0xb0 position, turned by the phase. A
 * helper just released (+0x60 high byte bit 7) is placed there (020c5c54) at rest; otherwise the
 * +0x1c velocity heads there from the +0x18 track. Each completed cycle arms it (+0x34, +0x38
 * cleared); armed, with the owner's +0x4d8 cooldown out and a 1-in-4 roll, a target (020cab14)
 * roughly ahead of the flattened velocity (dot >= 0.875) makes it dive: the velocity keeps
 * 1.5 x its speed towards the target's +0x190 point, the owner cooldown restarts (0x580) and the
 * next move is 3. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int m[9]; } Mtx33;
struct Hw60 { u16 lo : 8; u16 hi : 8; };

extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov254_HoverTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    VecFx32 v;
    VecFx32 base;
    VecFx32 d;
    VecFx32 b;
    VecFx32 a;
    int k;
    int target;
    int speed;
    int pending;
    int cycled;

    k = ANG2IDX(state[0xc] * 0x6488 / 0x3000) * 2;
    v.x = 0;
    v.y = data_0203d210[k] * 3;
    v.z = 0xab31;
    MTX_RotY33_(&m, data_0203d210[k], data_0203d210[k + 1]);
    MTX_MultVec33(&v, &m, &v);
    base = *(VecFx32 *)(*(int *)(*state + 0x394) + 0xb0);
    base.y -= 0x3000;
    VEC_Add(&v, &base, &v);
    if ((((struct Hw60 *)(*state + 0x60))->lo & 0x80) != 0) {
        ((struct Hw60 *)(*state + 0x60))->hi &= ~0x80;
        Ov107_MoveNodeAndRelayout((Actor *)(*state), &v);
        *(VecFx32 *)(state + 7) = data_02041dc8;
    } else {
        VEC_Subtract(&v, (VecFx32 *)state[6], (VecFx32 *)(state + 7));
    }
    state[0xc] += *(int *)(node[0] + 0x2c);
    if (state[0xc] > 0x3000) {
        state[0xc] -= 0x3000;
        state[0xd] = 1;
        state[0xe] = 0;
    }
    pending = state[0xe];
    cycled = state[0xd];
    if (pending == 0 && cycled == 0) {
        return;
    }
    if (*(int *)(*(int *)(*state + 0x394) + 0x4d8) > 0) {
        return;
    }
    if (RandNextScaled(100) >= 0x19) {
        return;
    }
    target = Ov107_FindNearestObject(*state, 0);
    if (target == 0) {
        return;
    }
    VEC_Subtract((VecFx32 *)(target + 0x190), (VecFx32 *)state[6], &d);
    VecSet(&a, d.x, 0, d.z);
    VEC_Normalize(&a, &a);
    VecSet(&b, state[7], 0, state[9]);
    VEC_Normalize(&b, &b);
    if (VEC_DotProduct(&a, &b) < 0xe00) {
        return;
    }
    speed = VEC_Normalize((VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    VEC_Normalize(&d, &d);
    ScaleVec3Fx12((int)(((long long)speed * 0x1800 + 0x800) >> 12), &d, (VecFx32 *)(state + 7));
    *(int *)(*(int *)(*state + 0x394) + 0x4d8) = 0x580;
    *(signed char *)(*state + 0x1c7) = 3;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
