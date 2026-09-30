/* Slam tick of the ov233 enemy: the +0x4c timer accumulates the frame-time, the
 * +0x10 velocity is the +0x490 item's +0x2c vector turned by the +0x40 yaw and, past 0xff0,
 * reaction 0x164 mode 0xf fires once (+0x61) at the +0xc position. Between 0x2a8 and 0x440 the
 * +0x494 sphere, enlarged 1.375 times and pushed that far along the facing of the yaw, is swept:
 * every entity whose id bit is clear in the +0x62 mask gets the bit set and receives a 0xa8 hit
 * packet whose normal points from the +0x494 point to it (flattened) through its +0x1c4 handler.
 * Once the +4 item's +0xad byte clears, every third slam requests sub-state 9; otherwise a d100
 * picks 0xb (below 20, when one of the eight +0x3c0 children is free), 6 (below 40), 9 (below
 * 60), 0xa (below 80) or 2, and the state ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 pos; int radius; } Sphere;
typedef struct { int m[9]; } Mtx33;
struct hw60 { unsigned short lo : 8, hi : 8; };
struct Kids { char pad[0x3c0]; int kids[8]; };

struct HitPacket40 {
    int nKind;
    VecFx32 vNormal;
    int w[6];
};

extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int id, void *at);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(void *a, void *b, VecFx32 *d);
extern int Ov107_CollectSphereOverlaps(int actor, Sphere *sphere, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov233_SlamSweepTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 mtx;
    VecFx32 facing;
    Sphere sphere;
    int hits[4];
    unsigned int idx;
    long i;
    long n;
    unsigned int mask;
    int roll;
    int free;
    int j;

    state[0x13] += *(int *)(*node + 0x2c);
    idx = ANG2IDX(state[0x10]);
    MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x490) + 0x2c), &mtx, (VecFx32 *)(state + 4));
    if (*(u8 *)((char *)state + 0x61) == 0 && state[0x13] >= 0xff0) {
        Ov107_BuildAndSendUpdate(*state, 0x164, 0xf, (void *)state[3]);
        *(u8 *)((char *)state + 0x61) = 1;
    }
    if (state[0x13] >= 0x2a8 && state[0x13] <= 0x440) {
        struct HitPacket40 packet = {0};
        packet.nKind = 0xa8;
        sphere = *(Sphere *)(*state + 0x494);
        sphere.radius = FX_MUL(sphere.radius, 0x1600);
        idx = ANG2IDX(state[0x10]);
        facing.y = 0;
        facing.x = data_0203d210[idx * 2];
        facing.z = data_0203d210[idx * 2 + 1];
        ScaleVec3Fx12(sphere.radius, &facing, &facing);
        VEC_Add(&facing, &sphere.pos, &sphere.pos);
        n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
        i = 0;
        if (n > 0) {
            do {
                mask = (1 << *(unsigned short *)(hits[i] + 2)) & 0xff;
                if ((*(u8 *)((char *)state + 0x62) & mask) == 0) {
                    *(u8 *)((char *)state + 0x62) |= mask;
                    VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x494), &packet.vNormal);
                    packet.vNormal.y = 0;
                    VEC_Normalize(&packet.vNormal, &packet.vNormal);
                    (*(void (**)(unsigned short, struct HitPacket40 *))(hits[i] + 0x1c4))(*(unsigned short *)(hits[i] + 2), &packet);
                }
            } while (++i < n);
        }
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    roll = RandNextScaled(0x64);
    for (j = 0; j < 8; j++) {
        if ((((struct hw60 *)(((struct Kids *)*state)->kids[j] + 0x60))->lo & 1) == 0) {
            break;
        }
    }
    free = j < 8 ? 1 : 0;
    if (++*(u8 *)((char *)state + 0x64) >= 3) {
        *(u8 *)(*state + 0x1c7) = 9;
        *(u8 *)((char *)state + 0x64) = 0;
    } else if (roll < 0x14 && free) {
        *(u8 *)(*state + 0x1c7) = 0xb;
    } else if (roll < 0x28) {
        *(u8 *)(*state + 0x1c7) = 6;
    } else if (roll < 0x3c) {
        *(u8 *)(*state + 0x1c7) = 9;
    } else if (roll < 0x50) {
        *(u8 *)(*state + 0x1c7) = 0xa;
    } else {
        *(u8 *)(*state + 0x1c7) = 2;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
