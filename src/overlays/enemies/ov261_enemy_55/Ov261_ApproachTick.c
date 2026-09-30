/* Approach tick of the ov261 enemy (and its byte-identical twin): the +0x30 velocity heads on
 * the ground plane towards the +0x2c point of the +0x49 grab slot (capped at 0x100), the +0x3c
 * rate is the frame-time (30/30) and the +0x34 lift eases by a fiftieth towards 0x2000 above the
 * +0x13c height. When the +0x3a8 part's +4 owner matches the actor's, bit 7 of the +0x60 flag
 * high byte clears, animation 0 (looped) plays, the +0x6c index resets and the tick hands off to
 * the grab walk. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov261_GrabWalk(int *node);

void Ov261_ApproachTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 goal;
    VecFx32 dir;
    int len;
    int speed;
    int d;
    int actor;

    goal = *(VecFx32 *)(*(int *)(*state + 0x3a0) + *(u8 *)((char *)state + 0x49) * 0x24 + 0x2c);
    VEC_Subtract(&goal, (void *)state[1], &dir);
    dir.y = 0;
    len = VEC_Normalize(&dir, &dir);
    speed = 0x100;
    if (len < 0x100) {
        speed = len;
    }
    ScaleVec3Fx12(speed, &dir, state + 0xc);
    state[0xf] = *(int *)(*node + 0x2c) * 30 / 30;
    d = 0x2000 - *(int *)(*state + 0x13c);
    state[0xd] += d / 50;
    actor = *state;
    if (*(int *)(*(int *)(actor + 0x3a8) + 4) == *(int *)(actor + 4)) {
        ((struct hw60 *)(actor + 0x60))->hi &= ~0x80;
        Ov107_PostTagUpdate((Actor *)(*state), 0, 1);
        state[0x1b] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov261_GrabWalk);
    }
}
