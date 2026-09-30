/* Slam tick of an ov252 part: it faces the target; the first frame plays pose 0x21 and marks +0x88 bit
 * 7. Then +0x6c accumulates the frame rate and the four +0x89 cues play sounds 0x148/0xc (at the +0x568
 * fist) and 0x148/0x11 (at the +0x564 fist) alternately at 2.79, 4.58, 6.37 and 8.17. +0x64 times the
 * hit windows: 0.73-1.33 and 4.38-5.11 test the +0x568 fist and 2.42-3.09 and 5.74-6.47 the +0x564 fist
 * (sphere of the actor's +0x80 radius against kind 5, 020ce0a8; a hit plays sound 0/0x4e at the actor);
 * outside them +0x86 clears. Once the partner holds no queued move pose 0x22 plays, +0x64 clears and
 * the node moves on to 020d2c0c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;

extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern unsigned char Ov252_ReboundHitTest(int *state, int kind, Sphere *sphere, void *cyl, void *box);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_AiHoldThenQueue13(void);

void Ov252_PartSlamTick(int *node)
{
    int *state = (int *)node[1];
    Sphere fist;

    Ov252_CheckTarget(node, 0, 1);
    if (!(*((unsigned char *)state + 0x88) & 0x80)) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x21, 0);
        *((unsigned char *)state + 0x88) |= 0x80;
        return;
    }
    state[0x1b] += *(int *)(node[0] + 0x2c);
    if (state[0x1b] >= 0x2ca0 && *((unsigned char *)state + 0x89) == 4) {
        *((unsigned char *)state + 0x89) -= 1;
        Ov107_BuildAndSendUpdate(*state, 0x148, 0xc, (void *)(*(int *)(*state + 0x568) + 0x14));
    }
    if (state[0x1b] >= 0x4950 && *((unsigned char *)state + 0x89) == 3) {
        *((unsigned char *)state + 0x89) -= 1;
        Ov107_BuildAndSendUpdate(*state, 0x148, 0x11, (void *)(*(int *)(*state + 0x564) + 0x14));
    }
    if (state[0x1b] >= 0x6600 && *((unsigned char *)state + 0x89) == 2) {
        *((unsigned char *)state + 0x89) -= 1;
        Ov107_BuildAndSendUpdate(*state, 0x148, 0xc, (void *)(*(int *)(*state + 0x568) + 0x14));
    }
    if (state[0x1b] >= 0x82b0 && *((unsigned char *)state + 0x89) == 1) {
        *((unsigned char *)state + 0x89) -= 1;
        Ov107_BuildAndSendUpdate(*state, 0x148, 0x11, (void *)(*(int *)(*state + 0x564) + 0x14));
    }
    fist.nRadius = *(int *)(*state + 0x80);
    state[0x19] += *(int *)(node[0] + 0x2c);
    if ((state[0x19] >= 0xbb0 && state[0x19] <= 0x1540) || (state[0x19] >= 0x4620 && state[0x19] <= 0x51d0)) {
        fist.center = *(VecFx32 *)(*(int *)(*state + 0x568) + 0x14);
        if (Ov252_ReboundHitTest(state, 5, &fist, 0, 0) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x4e, (void *)(*state + 0x74));
        }
    } else if ((state[0x19] >= 0x26c8 && state[0x19] <= 0x3168) || (state[0x19] >= 0x5be8 && state[0x19] <= 0x6798)) {
        fist.center = *(VecFx32 *)(*(int *)(*state + 0x564) + 0x14);
        if (Ov252_ReboundHitTest(state, 5, &fist, 0, 0) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x4e, (void *)(*state + 0x74));
        }
    } else {
        *((unsigned char *)state + 0x86) = 0;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x22, 0);
    state[0x19] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_AiHoldThenQueue13);
}
