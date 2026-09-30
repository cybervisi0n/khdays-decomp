/* d02c8 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int value; } Fx32;
typedef struct { u16 lo; u16 hi; } Cmd4;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;
typedef struct { VecFx32 center; int nRadius; } Sphere;
struct Nibbles { u8 lo : 4; u8 hi : 4; };

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int FX_Div(int a, int b);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov257_020d32be;
extern const Cmd4 data_ov257_020d325c[];
extern const VecFx32 data_02041dc8;
extern void Ov257_GlideTick(int *node);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

static inline int ClampHp(int v, int max)
{
    if (v < 0) {
        return 0;
    }
    if (v > max) {
        return max;
    }
    return v;
}

void Ov257_HealBurstTick(int *node)
{
    Cmd4 note;
    Sphere sphere;
    VecFx32 *pos;
    int *state = (int *)node[1];
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int ratio;
    int n;
    int i;
    VecFx32 push;
    int hits[4];

    state[0x15] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x76) == 0 && state[0x15] >= 0x15dd
        && *((signed char *)state + 0x88) == 0 && *((signed char *)state + 0x89) == 0) {
        func_ov107_020c0b90(*state, 0xc, *(VecFx32 *)state[1], 0);
        *((u8 *)state + 0x76) = 1;
    }
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (state[0x11] >= 0x1344) {
        int owner;
        int hpMax;
        int hp;

        state[0x12] += *(int *)(node[0] + 0x2c);
        n = FX_Div(state[0x12], 0x480);
        if (n > 0x1000) {
            n = 0x1000;
        }
        sphere.center = *(VecFx32 *)(*(int *)(*state + 0x3d4) + 0x14);
        sphere.nRadius = FX_Mul(n, 0x3000);
        ((struct Nibbles *)*(int *)(*state + 0x3fc))->hi |= 1;
        *(Sphere *)(*(int *)(*state + 0x3fc) + 0x58) = sphere;
        n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
        for (i = 0; i < n; i++) {
            Cmd14 msg;

            pos = (VecFx32 *)(hits[i] + 0x74);
            VEC_Subtract(pos, &sphere.center, &push);
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, 5, &push, 0) == 0) {
                continue;
            }
            msg = data_ov257_020d32be;
            PACK(msg, scratchX, *(Fx32 *)&pos->x, 5);
            PACK(msg, scratchY, *(Fx32 *)&pos->y, 8);
            PACK(msg, scratchZ, *(Fx32 *)&pos->z, 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x408), 0x21, pos);
        }
        i = FX_Div(state[0x11], 0x4800);
        if (i > 0x1000) {
            i = 0x1000;
        }
        owner = *state;
        hpMax = *(short *)(owner + 0x218);
        if (i * hpMax > *(short *)(owner + 0x21a) << 12) {
            *(short *)(owner + 0x21a) = ClampHp((i * hpMax) >> 12, hpMax);
        }
        *(u16 *)(*state + 0x1ae) |= 1;
    } else if (*((signed char *)state + 0x88) >= 1 || *((signed char *)state + 0x89) >= 1) {
        note.lo = data_ov257_020d325c[3].lo;
        note.hi = data_ov257_020d325c[3].hi;
        note.hi = (u16)note.hi;
        func_ov107_020c0b90(*state, 0xd, data_02041dc8, 0);
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, &note, 4);
        }
        ((struct Nibbles *)*(int *)(*state + 0x3fc))->hi &= ~1;
        Ov107_PostTagUpdate((Actor *)(*state), 0x1a, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3d0), 0x17, 0);
        if (*((u8 *)state + 0x7a) == 2) {
            state[0x1f] = 0;
        }
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_GlideTick);
        return;
    }
    if (state[0x11] <= 0x4800) {
        return;
    }
    ((struct Nibbles *)*(int *)(*state + 0x3fc))->hi &= ~1;
    Ov107_PostTagUpdate((Actor *)(*state), 0x1a, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 0x17, 0);
    *(u16 *)(*state + 0x1ae) &= ~1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_GlideTick);
}
