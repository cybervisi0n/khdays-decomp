/* Stomp tick: until the +0x74 flag is set the +0x18 timer accumulates the frame rate; past 0.23
 * the flag is set and, while the actor's +0x3d0 hit points are positive, the actor's +0x24 hook
 * receives note 6 of data_ov278_020d63e4, reaction 0x166 mode 0xb fires at the +0x3c0 foot's +4
 * point and a sphere there, of twice the foot's +0x90 radius, sweeps the actor list: every entity
 * in it is pushed 0.375 away horizontally (kind 1, on behalf of the +0x384 rider); on acceptance the
 * 14-byte message data_ov278_020d6408 carries its +0x74 point raised by 0.5 and its +0x1b4 byte to
 * the rider's +0x24 hook, and reaction 0x166 mode 0xd fires at that raised point. Once the +0x20
 * idle byte clears, pose 9 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int value; } Fx32;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;
typedef struct { VecFx32 center; int nRadius; } Sphere;
typedef struct { u16 lo; u16 hi; } Cmd4;

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, void *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd4 data_ov278_020d63e4[];
extern const Cmd14 data_ov278_020d6408;

void Ov278_StompTick(int *node)
{
    Cmd4 note;
    int *state = (int *)node[1];
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    Sphere sphere;
    int hits[4];
    VecFx32 push;
    VecFx32 raw;
    int n;
    int i;

    if (*((u8 *)state + 0x74) == 0) {
        state[6] += *(int *)(node[0] + 0x2c);
        if (state[6] > 0x3bb) {
            *((u8 *)state + 0x74) = 1;
            if (*(short *)(*state + 0x3d0) > 0) {
                {
                    Cmd4 *p = &note;

                    p->hi = data_ov278_020d63e4[6].hi;
                    p->lo = data_ov278_020d63e4[6].lo;
                    if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
                        (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, p, 4);
                    }
                }
                Ov107_BuildAndSendUpdate(*state, 0x166, 0xb, (void *)(**(int **)(*state + 0x3c0) + 4));
                sphere.center = *(VecFx32 *)(**(int **)(*state + 0x3c0) + 4);
                n = *(int *)(**(int **)(*state + 0x3c0) + 0x90);
                sphere.nRadius = n << 1;
                n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
                for (i = 0; i < n; i++) {
                    Cmd14 msg;

                    VEC_Subtract((void *)(hits[i] + 0x74), &sphere.center, &push);
                    push.y = 0;
                    VEC_Normalize(&push, &push);
                    ScaleVec3Fx12(0x600, &push, &push);
                    if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x384), 1, &push, 0) == 0) {
                        continue;
                    }
                    msg = data_ov278_020d6408;
                    raw = *(VecFx32 *)(hits[i] + 0x74);
                    raw.y += 0x800;
                    PACK(msg, scratchX, *(Fx32 *)&raw.x, 5);
                    PACK(msg, scratchY, *(Fx32 *)&raw.y, 8);
                    PACK(msg, scratchZ, *(Fx32 *)&raw.z, 11);
                    msg.flag = *(u8 *)(hits[i] + 0x1b4);
                    if (*(void (**)(int, Cmd14 *, int))(*(int *)(*state + 0x384) + 0x24) != 0) {
                        (*(void (**)(int, Cmd14 *, int))(*(int *)(*state + 0x384) + 0x24))(*(int *)(*state + 0x384), &msg, 0xe);
                    }
                    Ov107_BuildAndSendUpdate(*state, 0x166, 0xd, &raw);
                }
            }
        }
    }
    if (*(u8 *)state[8] != 0) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 9;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
