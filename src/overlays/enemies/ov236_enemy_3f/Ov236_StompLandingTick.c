/* Stomp-landing tick: the +0x14 timer accumulates the frame rate. Inside its (0x26ee, 0x56ee)
 * window an oriented box (the +0x1c point with y set to the +0x3a8 prop's +0x18 height less the
 * +0x398 joint's y, world axes, half-extents 3.0 / 0.625 / 3.0) sweeps the actor list: every
 * entity whose +0x1b4 kind is not yet in the +0x74 mask that accepts a kind-0 hit pushed 0.25
 * away from the box centre gets the overlay's 14-byte message (its kind, its +0x74 position with
 * y raised by 0x800) through the +0x394 target's +0x24 hook, reaction 0x127/0xd at that point and
 * its mask bit. Once the +0x24 flag byte clears the next move is 9. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; VecFx32 axis[3]; int ext[3]; } Box;
typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;
struct Msg14 { u16 h[7]; };

struct Ov236Target {
    char pad000[0x24];
    void (*pfnMessage)(struct Ov236Target *self, void *msg, int size);
};

extern int Ov107_CollectCapsuleOverlaps(int owner, Box *box, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int a, int id, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_0204224c;
extern const VecFx32 data_02042240;
extern const VecFx32 data_0204227c;
extern const struct Msg14 data_ov236_020d6404;

#define PACK3(msg, base, v) \
    ((u8 *)(msg))[(base)] = (u8)(((u32)(v) >> 0x10 & 0x7f) | ((u32)(v) >> 0x18 & 0x80)); \
    ((u8 *)(msg))[(base) + 1] = (u8)((u32)(v) >> 8); \
    ((u8 *)(msg))[(base) + 2] = (u8)(v)

void Ov236_StompLandingTick(int *node)
{
    int *state = (int *)node[1];
    int hits[4];
    Box box;
    VecFx32 d;
    VecFx32 raw;
    struct Msg14 msg;
    struct Msg14 tmpl;
    FxVec vContact;
    int nHits;
    int i;

    state[5] += *(int *)(node[0] + 0x2c);
    if (state[5] > 0x26ee && state[5] < 0x56ee) {
        box.center = *(VecFx32 *)state[7];
        box.center.y = *(int *)(*(int *)(*state + 0x3a8) + 0x18);
        box.center.y -= *(int *)(*(int *)(*state + 0x398) + 0x74);
        box.axis[0] = data_0204224c;
        box.axis[1] = data_02042240;
        box.axis[2] = data_0204227c;
        box.ext[0] = 0x3000;
        box.ext[2] = 0x3000;
        box.ext[1] = 0xa00;
        nHits = Ov107_CollectCapsuleOverlaps(*state, &box, hits);
        i = 0;
        if (nHits > 0) {
            tmpl = data_ov236_020d6404;
            do {
                if (((*(u8 *)((char *)state + 0x74) >> *(u8 *)(hits[i] + 0x1b4)) & 1) == 0) {
                    VEC_Subtract((void *)(hits[i] + 0x74), &box.center, &d);
                    VEC_Normalize(&d, &d);
                    ScaleVec3Fx12(0x400, &d, &d);
                    if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x394), 0, &d, 0) != 0) {
                        msg = tmpl;
                        raw = *(VecFx32 *)(hits[i] + 0x74);
                        raw.y += 0x800;
                        vContact.x = *(Fx32 *)&raw.x;
                        PACK3(&msg, 5, vContact.x.value);
                        vContact.y = *(Fx32 *)&raw.y;
                        PACK3(&msg, 8, vContact.y.value);
                        vContact.z = *(Fx32 *)&raw.z;
                        PACK3(&msg, 11, vContact.z.value);
                        ((u8 *)&msg)[4] = *(u8 *)(hits[i] + 0x1b4);
                        if (((struct Ov236Target *)*(int *)(*state + 0x394))->pfnMessage != 0) {
                            ((struct Ov236Target *)*(int *)(*state + 0x394))->pfnMessage((struct Ov236Target *)*(int *)(*state + 0x394), &msg, 0xe);
                        }
                        Ov107_BuildAndSendUpdate(*state, 0x127, 0xd, &raw);
                        *(u8 *)((char *)state + 0x74) |= 1 << *(u8 *)(hits[i] + 0x1b4);
                    }
                }
            } while (++i < nHits);
        }
    }
    if (*(u8 *)state[9] == 0) {
        *(signed char *)(*state + 0x1c7) = 9;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    }
}
