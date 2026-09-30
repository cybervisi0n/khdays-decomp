/* Recovery tick of the ov255 enemy. The +0x50 clock runs; at 1.37 (once, +0x65) message 0xc goes to the
 * +4 point. The +0x44 clock runs too: from 1.2 the +0x48 burst clock grows a sphere on the owner's
 * +0x3a8 bone (radius up to 3.0 over 0.28) mirrored into the +0x3e8 part (flag bit 4 raised); every
 * entity it holds is pushed by 0.5 away from the centre (kind 5) and, on acceptance, the overlay's
 * 14-byte template carries its position to the owner's +0x24 hook with the owner's +0x3f8 reaction
 * mode 0x21. Meanwhile the stamina (+0x21a) refills from the +0x68 base towards the +0x218 maximum
 * over 4.5 and the owner stays marked (+0x1ae bit 0). Before 1.2, a pending +0x78 / +0x79 request
 * aborts: message 0xd, the overlay's short note to the hook, the part flag dropped, pose 0x1d, the
 * +0x3a4 rig motion 0x18, +0x6c cleared and 020d054c. After 4.5 the tick ends the same way (without
 * the note) and unmarks the owner. */

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
extern const Cmd14 data_ov255_020d2b98;
extern const Cmd4 data_ov255_020d2b20[];
extern const VecFx32 data_02041dc8;
extern void Ov255_GlideTick2(int *node);

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

void Ov255_HealBurstTick(int *node)
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

    state[0x14] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x65) == 0 && state[0x14] >= 0x15dd) {
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
        sph.c = *(VecFx32 *)(*(int *)(*state + 0x3a8) + 0x14);
        sph.r = FX_Mul(n, 0x3000);
        ((struct Nib *)*(int *)(*state + 0x3e8))->hi |= 1;
        *(Sphere *)(*(int *)(*state + 0x3e8) + 0x58) = sph;
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
            msg = data_ov255_020d2b98;
            PACK(msg, scratchX, *(Fx32 *)&pos->x, 5);
            PACK(msg, scratchY, *(Fx32 *)&pos->y, 8);
            PACK(msg, scratchZ, *(Fx32 *)&pos->z, 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x3f8), 0x21, pos);
        }
        t = FX_Div(state[0x11], 0x4800);
        if (t > 0x1000) {
            t = 0x1000;
        }
        max = *(short *)(*state + 0x218);
        v = FX_Mul(state[0x1a] + (max << 14) / 10, t);
        v = v > max << 12 ? max << 12 : (v < state[0x1a] ? state[0x1a] : v);
        *(short *)(*state + 0x21a) = ClampHp((char *)*state, v >> 12);
        *(u16 *)(*state + 0x1ae) |= 1;
    } else if (*((signed char *)state + 0x78) >= 1 || *((signed char *)state + 0x79) >= 1) {
        note.lo = data_ov255_020d2b20[4].lo;
        note.hi = data_ov255_020d2b20[4].hi;
        note.hi = (u16)note.hi;
        func_ov107_020c0b90(*state, 0xd, data_02041dc8, 0);
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, &note, 4);
        }
        ((struct Nib *)*(int *)(*state + 0x3e8))->hi &= ~1;
        Ov107_PostTagUpdate((Actor *)(*state), 0x1d, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3a4), 0x18, 0);
        state[0x1b] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_GlideTick2);
        return;
    }
    if (state[0x11] <= 0x4800) {
        return;
    }
    ((struct Nib *)*(int *)(*state + 0x3e8))->hi &= ~1;
    Ov107_PostTagUpdate((Actor *)(*state), 0x1d, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a4), 0x18, 0);
    *(u16 *)(*state + 0x1ae) &= ~1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_GlideTick2);
}
