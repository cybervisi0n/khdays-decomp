/* Lunge tick of the ov256 actor: after the aim update (020ccd54) the first tick knocks it back in
 * place (mode 3, +0x54 counter). The +0x10 velocity is the +0x34 direction at 1.5 + 0.75 per
 * level (+0x45c) and the +0x4c travel grows by 1.5 + 0.5 per level; a 5.19 sphere on the +0x3f4
 * part strikes along (0, 1.0, 1.0) through 020cd0e8. Once the +4 item's +0xad byte clears: past
 * the +0x58 mark plus 5.0 pose 0x16 / partner motion 8 play and the node moves on to 020cf68c;
 * before it pose 0x15 / motion 7 replay with another knock-back (mode 0xf). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 pos; int radius; } Sphere;

extern int Ov256_PickTarget(int *node);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov256_AttackHitTest(int *node, Sphere *s, int a, int b, VecFx32 *dir, int c, int d, int e);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_TickReactionAim(void);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_ov256_020d25ac;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov256_LungeTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    Sphere s;
    unsigned int idx;

    Ov256_PickTarget(node);
    {
    VecFx32 dir = data_ov256_020d25ac;

    if (state[0x15] == 0) {
        func_ov107_020c0b90(*state, 3, data_02041dc8, 0);
        state[0x15]++;
    }
    idx = ANG2IDX(state[0x10]);
    MTX_RotY33_(&m, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    ScaleVec3Fx12((*(int *)(*state + 0x45c) * 3 << 8) + 0x1800, (VecFx32 *)(state + 0xd), (VecFx32 *)(state + 4));
    state[0x13] += (*(int *)(*state + 0x45c) << 11) + 0x1800;
    s.radius = 0x5300;
    s.pos = *(VecFx32 *)(*(int *)(*state + 0x3f4) + 0x14);
    Ov256_AttackHitTest(node, &s, 0, 0, &dir, 1, 0, 1);
    }
    if (*(u8 *)(state[1] + 0xad) == 0 && state[0x13] >= state[0x16] + 0x5000) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x16, 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 8, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_TickReactionAim);
        return;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x15, 0);
    Ov107_StartAnim(*(int *)(*state + 0x450), 7, 0);
    func_ov107_020c0b90(*state, 0xf, data_02041dc8, 0);
}
