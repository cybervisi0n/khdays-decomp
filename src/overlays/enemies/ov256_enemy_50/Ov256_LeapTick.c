/* Leap tick of the ov256 actor: once the +0x4c timer passes the remaining charges x 0x88 (+0x69), the
 * charges clear, the +0x428 shape shows and bits 1-2 and 7 of the +0x60 high byte drop on the actor
 * and both claws. The timer runs, the +0x10 velocity is the +0x450 owner's +0x2c vector turned by its
 * heading (020cd054) at triple length and, unless landing on a claw (+0x78), a 2.69 sphere at its +0x74
 * position hits along the +0x34 direction (020cd0e8, mask 1). At 0x6e8 it lands: on a claw the +0xc
 * point takes the +0x1c spot, otherwise it is knocked back there (mode 7); the timer and charge flags
 * clear and the node moves on to 020cee70. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;
typedef struct { VecFx32 pos; int nRadius; } Sphere;

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov256_AttackHitTest(int *node, void *sphere, void *box, void *seg, VecFx32 *push, int bMask, int unused, int kind);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_SweepTick(void);

static inline void HighByteClear(char *obj, int bits)
{
    u16 hw = *(u16 *)(obj + 0x60);
    *(u16 *)(obj + 0x60) = (hw & ~0xff00) |
        (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~bits) << 0x18) >> 0x10);
}

void Ov256_LeapTick(int *node)
{
    int *state = (int *)node[1];
    Sphere sphere;
    VecFx32 dir;
    VecFx32 v;

    dir = *(VecFx32 *)(state + 0xd);
    if (state[0x13] >= *((u8 *)state + 0x69) * 0x88) {
        *((u8 *)state + 0x69) = 0;
        ((B8 *)(*(int *)(*state + 0x428) + 8))->f |= 1;
        HighByteClear((char *)*state, 0x86);
        HighByteClear(*(char **)(*state + 0x434), 0x86);
        HighByteClear(*(char **)(*state + 0x438), 0x86);
    }
    state[0x13] += *(int *)(node[0] + 0x2c);
    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    ScaleVec3Fx12(0x3000, &v, (VecFx32 *)(state + 4));
    sphere.pos = *(VecFx32 *)(*state + 0x74);
    sphere.nRadius = 0x2b00;
    if (state[0x1e] == 0) {
        Ov256_AttackHitTest(node, &sphere, 0, 0, &dir, 1, 0, 0);
    }
    if (state[0x13] < 0x6e8) {
        return;
    }
    if (state[0x1e] != 0) {
        *(VecFx32 *)state[3] = *(VecFx32 *)(state + 7);
    } else {
        func_ov107_020c0b90(*state, 7, *(VecFx32 *)state[3], 0);
    }
    state[0x13] = 0;
    *((u8 *)state + 0x6a) = 0;
    *((u8 *)state + 0x69) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_SweepTick);
}
