/* Ov253_RingLayout -- ring lay-out along the chain: the owner's ring entries (stride 0x38,
 * count at +0x8c) are spread over the four segments between the actor's +0x390..+0x39c joints
 * and the +0x3ac one, a quarter of them per segment, each placed at its fraction along the
 * segment (raised by 0.5) with scale 1.0 (the last segment's entries grow with the fraction);
 * the owner's +0x88 model rebinds channels 0, 2, 1 and 4 to its +0xe0 and clears them; the
 * node moves to 020d1af4. */

#include "nitro/fx_types.h"

struct Ov253JointsNext { char *cur[1]; char *next[4]; };
struct Ov253Links { char pad[0x390]; char *link[4]; };

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void BindAnimTrack(int object, int channel, void *target, int flag);
extern void Anim_SetFrameWrapped(int object, int channel, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov253_NotifyIfQueryBit0Set(void);

void Ov253_RingLayout(int *node) {
    int *state = (int *)node[1];
    VecFx32 a;
    VecFx32 b;
    char *joints[5];
    VecFx32 pos;
    int i;
    int actor = *state;
    int quarter = *(int *)(state[1] + 0x8c) / 4;
    int seg;
    int t;
    char *entry;

    for (i = 0; i < 4; i++) {
        joints[i] = ((struct Ov253Links *)actor)->link[i];
    }
    joints[i] = *(char **)(actor + 0x3ac);
    for (i = 0; i < *(int *)(state[1] + 0x8c); i++) {
        entry = *(char **)(state[1] + 0x90) + i * 0x38;
        seg = i / quarter;
        t = ((i % quarter) << 12) / quarter;
        a = *(VecFx32 *)(joints[seg] + 0x14);
        b = *(VecFx32 *)(joints[seg + 1] + 0x14);
        VEC_Subtract(&b, &a, &pos);
        ScaleVec3Fx12(t, &pos, &pos);
        VEC_Add(&a, &pos, &pos);
        pos.y += 0x800;
        *(VecFx32 *)(entry + 0x2c) = pos;
        if (seg + 1 == 4) {
            *(int *)entry = (t << 1) + 0x1000;
        } else {
            *(int *)entry = 0x1000;
        }
    }
    BindAnimTrack(*(int *)(state[1] + 0x88), 0, (void *)(*(int *)(state[1] + 0x88) + 0xe0), 0);
    Anim_SetFrameWrapped(*(int *)(state[1] + 0x88), 0, 0);
    BindAnimTrack(*(int *)(state[1] + 0x88), 2, (void *)(*(int *)(state[1] + 0x88) + 0xe0), 0);
    Anim_SetFrameWrapped(*(int *)(state[1] + 0x88), 2, 0);
    BindAnimTrack(*(int *)(state[1] + 0x88), 1, (void *)(*(int *)(state[1] + 0x88) + 0xe0), 0);
    Anim_SetFrameWrapped(*(int *)(state[1] + 0x88), 1, 0);
    BindAnimTrack(*(int *)(state[1] + 0x88), 4, (void *)(*(int *)(state[1] + 0x88) + 0xe0), 0);
    Anim_SetFrameWrapped(*(int *)(state[1] + 0x88), 4, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov253_NotifyIfQueryBit0Set);
}
