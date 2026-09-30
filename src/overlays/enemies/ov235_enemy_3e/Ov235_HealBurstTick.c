/* Healing burst tick of an ov235 state: at 1.37 on the +0x54 timer effect 0xc is spawned at the
 * +4 point (+0x65). From 1.2 on the +0x44 timer the burst grows: the +0x3b8 sphere shape is
 * shown and placed on the owner's +0x3ac body point with a radius of up to 3.0 (+0x48 over
 * 0.28), every actor inside it is pushed 0.5 away (kind 5), its +0x74 point is sent to the
 * owner's +0x24 hook in the 14-byte message of data_ov235_020d2526 and reaction +0x3c8 mode 0x21
 * fires there; the owner's HP (+0x21a) is raised to the +0x84 base plus a fifth of its max
 * (+0x218), scaled by the +0x44 progress over 4.5 and kept within [base, max], and bit 0 of +0x1ae
 * is raised. Before that, a +0x8c/+0x8d request ends the state at once: effect 0xd, note 6 of
 * data_ov235_020d24d0 to the hook, the shape hidden, animation 0x24, motion 0x1a on the +0x3a8
 * part, +0x88 cleared and Ov235_GlideTick25 takes over. Past 4.5 the burst ends the same way,
 * clearing bit 0 of +0x1ae instead. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int value; } Fx32;
typedef struct { VecFx32 c; int r; } Sphere;
typedef struct { u16 lo; u16 hi; } Cmd4;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;
struct Nib { u8 lo : 4, hi : 4; };

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

extern void func_ov107_020c0b90(int owner, int id, VecFx32 at, int flag);
extern int FX_Div(int num, int den);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov235_020d2526;
extern const Cmd4 data_ov235_020d24d0[];
extern const VecFx32 data_02041dc8;
extern void Ov235_GlideTick25(int *node);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

static inline int ClampHp(char *actor, int hp)
{
    if (hp < 0) {
        return 0;
    }
    if (hp > *(short *)(actor + 0x218)) {
        return *(short *)(actor + 0x218);
    }
    return hp;
}

void Ov235_HealBurstTick(int *node)
{
    VecFx32 *pos;
    int *state = (int *)node[1];
    Sphere sph;
    VecFx32 push;
    int hits[4];
    Cmd4 note;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int n;
    int i;
    int t;

    state[0x15] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x65) == 0 && state[0x15] >= 0x15dd) {
        func_ov107_020c0b90(*state, 0xc, *(VecFx32 *)state[1], 0);
        *((u8 *)state + 0x65) = 1;
    }
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (state[0x11] >= 0x1344) {
        int hp;
        int max;
        int v;

        state[0x12] += *(int *)(node[0] + 0x2c);
        n = FX_Div(state[0x12], 0x480);
        if (n > 0x1000) {
            n = 0x1000;
        }
        sph.c = *(VecFx32 *)(*(int *)(*state + 0x3ac) + 0x14);
        sph.r = FX_Mul(n, 0x3000);
        ((struct Nib *)*(int *)(*state + 0x3b8))->hi |= 1;
        *(Sphere *)(*(int *)(*state + 0x3b8) + 0x58) = sph;
        n = Ov107_CollectSphereOverlaps(*state, &sph, hits);
        for (i = 0; i < n; i++) {
            Cmd14 msg;

            pos = (VecFx32 *)(hits[i] + 0x74);
            VEC_Subtract(pos, &sph.c, &push);
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, 5, &push, 0) == 0) {
                continue;
            }
            msg = data_ov235_020d2526;
            PACK(msg, scratchX, *(Fx32 *)&pos->x, 5);
            PACK(msg, scratchY, *(Fx32 *)&pos->y, 8);
            PACK(msg, scratchZ, *(Fx32 *)&pos->z, 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x3c8), 0x21, pos);
        }
        t = FX_Div(state[0x11], 0x4800);
        if (t > 0x1000) {
            t = 0x1000;
        }
        max = *(short *)(*state + 0x218) << 12;
        v = FX_Mul(state[0x21] + max / 5, t);
        v = v > max ? max : (v < state[0x21] ? state[0x21] : v);
        *(short *)(*state + 0x21a) = ClampHp((char *)*state, v >> 12);
        *(u16 *)(*state + 0x1ae) |= 1;
    } else if (*((signed char *)state + 0x8c) >= 1 || *((signed char *)state + 0x8d) >= 1) {
        note.lo = data_ov235_020d24d0[6].lo;
        note.hi = data_ov235_020d24d0[6].hi;
        note.hi = (u16)note.hi;
        func_ov107_020c0b90(*state, 0xd, data_02041dc8, 0);
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, &note, 4);
        }
        ((struct Nib *)*(int *)(*state + 0x3b8))->hi &= ~1;
        Ov107_PostTagUpdate((Actor *)(*state), 0x24, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3a8), 0x1a, 0);
        state[0x22] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_GlideTick25);
        return;
    }
    if (state[0x11] <= 0x4800) {
        return;
    }
    ((struct Nib *)*(int *)(*state + 0x3b8))->hi &= ~1;
    Ov107_PostTagUpdate((Actor *)(*state), 0x24, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a8), 0x1a, 0);
    *(u16 *)(*state + 0x1ae) &= ~1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_GlideTick25);
}
