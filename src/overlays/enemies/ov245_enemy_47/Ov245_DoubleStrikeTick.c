/* Ov245_DoubleStrikeTick -- double-strike tick: the +0x3a0 item's forward vector (020c9f48,
 * speed returned) rotated by the actor's +0xa0 placement and scaled becomes the +0x1c
 * direction; the +0x394 / +0x398 items' +0x14 anchors offset by it give two strike centres.
 * The +0x40 timer runs up by the frame step: past 0.166 (latched at +0x48) effect 3 plays at
 * the +0x390 item's anchor and reaction 0x11a/4 fires at the +0xc anchor; past 0.996 each
 * centre is swept with a 0.1875 sphere (020c8eb8) and every hit whose +2 slot bit is not in the
 * +0x49 mask is pushed along the direction through the +0x3cc item (020ca918, mode 6): a
 * landing hit gets effect 1 at the centre, its bit set, the direction zeroed and reaction
 * 0x11a/5 at the centre. Once the +4 item's animation is free (+0xad) the direction is copied
 * to +0x28 and the node moves to 020d6c78. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct Sphere { VecFx32 centre; int radius; };

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern int Ov107_CollectSphereOverlaps(int actor, struct Sphere *sphere, int *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov245_DecayCopyPosFireOnHitFlag(void);

void Ov245_DoubleStrikeTick(int *node) {
    int *state = (int *)node[1];
    int hits[4];
    VecFx32 centres[2];
    VecFx32 fwd;
    struct Sphere sphere;
    VecFx32 zero;
    int j;
    int speed;
    int nHits;
    int i;
    VecFx32 *centre;
    VecFx32 *anchor;

    speed = Ov107_ActionResource_GetOffsetAndScale(*(int *)(*state + 0x3a0), &fwd);
    Vec3TransformViaTempMtx((VecFx32 *)(state + 7), (void *)(*state + 0xa0), &fwd);
    ScaleVec3Fx12(speed, (VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    VEC_Add((VecFx32 *)(*(int *)(*state + 0x394) + 0x14), (VecFx32 *)(state + 7), &centres[0]);
    VEC_Add((VecFx32 *)(*(int *)(*state + 0x398) + 0x14), (VecFx32 *)(state + 7), &centres[1]);
    state[0x10] += *(int *)(node[0] + 0x2c);
    if (*((unsigned char *)state + 0x48) == 0 && state[0x10] >= 0x2a8) {
        *((unsigned char *)state + 0x48) = 1;
        func_ov107_020c0b90(*state, 3, *(VecFx32 *)(*(int *)(*state + 0x390) + 0x14), 0);
        Ov107_BuildAndSendUpdate(*state, 0x11a, 4, (void *)state[3]);
    }
    if (state[0x10] >= 0xff0) {
        zero = data_02041dc8;
        centre = centres;
        anchor = centres;
        for (j = 0; j < 2; j++) {
            sphere.centre = *centre;
            sphere.radius = 0x300;
            nHits = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
            for (i = 0; i < nHits; i++) {
                if ((*((unsigned char *)state + 0x49) & (1 << *(unsigned short *)(hits[i] + 2))) == 0) {
                    if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x3cc), 6, (VecFx32 *)(state + 7), 0) != 0) {
                        func_ov107_020c0b90(*state, 1, *centre, 0);
                        *((unsigned char *)state + 0x49) |= 1 << *(unsigned short *)(hits[i] + 2);
                        *(VecFx32 *)(state + 7) = zero;
                        Ov107_BuildAndSendUpdate(*state, 0x11a, 5, anchor);
                    }
                }
            }
            centre++;
            anchor++;
        }
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(VecFx32 *)(state + 10) = *(VecFx32 *)(state + 7);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_DecayCopyPosFireOnHitFlag);
}
