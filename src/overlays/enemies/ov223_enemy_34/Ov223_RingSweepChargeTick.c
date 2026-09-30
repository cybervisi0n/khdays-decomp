/* Ring charge tick of the ov223 enemy (variant 0): the +0x38c item's +0x3ac pool entry's +0x20
 * point is announced to the owner (ov107 c5c54) with bit 7 of the +0x60 high byte cleared; the
 * +0x20 target is data_02042258 tilted about x by 0x1922 x (1.0 - timer x 0x1922 / 1.0)
 * (0x1922 minus the timer's fraction) and turned by the item's +0xa0 pose; the strike sweep
 * (ov223 442c, mode 0) runs with a segment from the point to the target of length 16.0 and
 * radius 0.5. The +0x3c timer accumulates the owner's rate; past 1.0 it clears and the tick
 * hands over to Ov223_AiFastCountdownQueue0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int m[9]; } Mtx33;
struct hw60 { unsigned short lo : 8, hi : 8; };
struct Ov223Segment { VecFx32 p0; VecFx32 p1; int nLength; int nRadius; };

extern void MTX_RotX33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern int Ov223_StrikeSweep(int *node, int mode, struct Ov223Segment *seg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern short data_0203d210[];
extern const VecFx32 data_02042258;
extern void Ov223_AiFastCountdownQueue0(int *node);

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov223_RingSweepChargeTick(int *node)
{
    int *state = (int *)node[1];
    struct Ov223Segment seg;
    Mtx33 m;
    VecFx32 at;
    int ang;
    int t;
    unsigned int idx;

    at = *(VecFx32 *)(*(int *)(*(int *)(*(int *)(*state + 0x38c) + 0x3ac)) + 0x20);
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &at);
    ang = 0x1922 - state[0xf] * 0x1922 / 4096;
    idx = ANG2IDX(ang);
    MTX_RotX33_(&m, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33(&data_02042258, &m, (VecFx32 *)(state + 8));
    Vec3TransformViaTempMtx((VecFx32 *)(state + 8), (const void *)(*(int *)(*state + 0x38c) + 0xa0), (VecFx32 *)(state + 8));
    seg.p0 = at;
    seg.p1 = *(VecFx32 *)(state + 8);
    seg.nLength = 0x10000;
    seg.nRadius = 0x800;
    Ov223_StrikeSweep(node, 0, &seg);
    t = state[0xf] + *(int *)(*node + 0x2c);
    state[0xf] = t;
    if (t <= 0x1000) {
        return;
    }
    state[0xf] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov223_AiFastCountdownQueue0);
}
