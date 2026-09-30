/* Pick the next strafe direction: turn the stored angle into a table index, read the sin/cos
 * pair, and slew the heading at rate 0x300. Then, unless the busy byte at state[0x11] says
 * otherwise, roll a d100 and set the action byte to 6 on a low roll or 5 otherwise.
 *
 * Three details are load-bearing: the y component is written BETWEEN the two table reads (the
 * ROM materialises its zero before the multiply), the d100 goes through the documented
 * `RandNextScaled(N) + (v - v)` copy artifact with an uninitialised scratch and a K&R extern,
 * and the 6/5 choice is an if/else -- as a ternary mwcc colours the pair r2/r1 instead of r1/r0.
 *
 * One of five byte-identical siblings. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int a, void *b, void *c);
extern void SetIndexedSlot(void *node, int idx, void *cb);
extern short data_0203d210[];

void Ov142_PickStrafeDirection(int *node) {
    int *state = (int *)node[1];
    VecFx32 v;
    int a;
    int roll;
    int scratch;

    a = (int)(unsigned short)((unsigned int)(((long long)state[2] * 0x28be60db9391LL + 0x80000000000LL) >> 32) >> 12) >> 4;
    v.x = data_0203d210[a * 2];
    v.y = 0;
    v.z = data_0203d210[a * 2 + 1];
    ScaleVec3Fx12(0x300, &v, state + 6);
    if (*(unsigned char *)state[0x11] != 0) {
        return;
    }
    roll = RandNextScaled(0x65) + (scratch - scratch);
    if (roll < 0x28) {
        *(char *)(state[0] + 0x1c7) = 6;
    } else {
        *(char *)(state[0] + 0x1c7) = 5;
    }
    SetIndexedSlot(node, *(signed char *)((int)node + 0x20), 0);
}
