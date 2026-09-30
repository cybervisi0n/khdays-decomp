/* Jump entry: counts the +0x28 timer up by the scene step and, once past 0x2aaa (latched at
 * +0x51), fires effect 0x166 of kind 8 at the +0x38 anchor. Once the +4 child's +0xad byte clears
 * pose 0xb plays (looping), flag 0x40 is raised in the actor's +0x60 high byte, the +0x3c jump
 * velocity is the direction from the actor's +0x74 position to its +0x190 target scaled by a
 * fiftieth of the distance, its +0x40 rise is 0.75 minus that fiftieth (at least 1/16), and the
 * node moves to 020cff4c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int  VEC_Normalize(void *a, void *d);
extern int  VEC_Mag(void *v);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov278_JumpTick(void);

void Ov278_EnterJump(int *node) {
    int *state = (int *)node[1];
    VecFx32 dir;

    if (*((unsigned char *)state + 0x51) == 0) {
        state[0xa] += *(int *)(*node + 0x2c);
        if (state[0xa] >= 0x2aaa) {
            *((unsigned char *)state + 0x51) = 1;
            Ov107_BuildAndSendUpdate(*state, 0x166, 8, (void *)state[0xe]);
        }
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0xb, 1);
    {
        unsigned short *p = (unsigned short *)(*state + 0x60);
        unsigned int hi = ((unsigned int)*p << 0x10) >> 0x18;
        hi |= 0x40;
        *p = (unsigned short)((*p & ~0xff00) | ((hi << 0x18) >> 16));
    }
    VEC_Subtract((void *)(*state + 0x190), (void *)(*state + 0x74), &dir);
    dir.y = 0;
    VEC_Normalize(&dir, state + 0xf);
    ScaleVec3Fx12(VEC_Mag(&dir) / 50, state + 0xf, state + 0xf);
    state[0x10] = 0xc00 - VEC_Mag(&dir) / 50;
    if (state[0x10] < 0x100) state[0x10] = 0x100;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov278_JumpTick);
}
