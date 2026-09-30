/* Fire the ranged shot once the wind-up passes 0x770: rebuild the aim direction from the
 * sin/cos table at the stored angle, push it 0x2000 out from the emitter's own position, and
 * hand both the muzzle point and the direction to the projectile spawner at +0x3b0. The
 * one-shot byte at +0x49 keeps it to a single shot per swing. Once the sub-object's busy byte
 * at +0xad clears, the action byte goes to 2 and the next step is scheduled.
 *
 * Matched byte-exact 2026-07-23. `pos` has to be declared BEFORE `dir` even though `dir` is
 * filled in first: mwcc hands out stack slots in declaration order from the top, and the ROM
 * keeps the direction in the LOW pair of slots.
 *
 * One of three byte-identical siblings. */

#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *out);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void Ov208_NodeUpdateApplyMotion(int emitter, VecFx32 pos, VecFx32 dir);
extern void SetIndexedSlot(void *node, int idx, void *cb);
extern short data_0203d210[];

void Ov208_FireRangedShot(int *node) {
    int *owner = (int *)node[0];
    int *state = (int *)node[1];
    VecFx32 pos;
    VecFx32 dir;
    int a;

    state[0xb] = state[0xb] + *(int *)((int)owner + 0x2c);
    if (state[0xb] >= 0x770 && *(unsigned char *)((int)state + 0x49) == 0) {
        a = (int)(((unsigned)(((long long)state[0xc] * 0x28be60db9391LL +
                   0x80000000000LL) >> 0x20) << 4) >> 0x10) >> 4;
        dir.x = (int)data_0203d210[a * 2];
        dir.y = 0;
        dir.z = (int)data_0203d210[a * 2 + 1];
        ScaleVec3Fx12(0x2000, &dir, &pos);
        VEC_Add(&pos, (VecFx32 *)state[2], &pos);
        Ov208_NodeUpdateApplyMotion(*(int *)(state[0] + 0x3b0), pos, dir);
        *(unsigned char *)((int)state + 0x49) = 1;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(char *)(state[0] + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((int)node + 0x20), 0);
}
