/* Barrage tick of the ov254 actor: the +0x58 timer accumulates the frame rate. The first 0xaa0
 * only arm it (+0x70 bit 0). Armed, every 0x200 one of up to 16 shots is fired: on the first,
 * everything within 6.0 of a point 1.0 ahead of the +0x414 part is struck by a 0xa8 packet pushed
 * 1.25 away (flattened) through its +0x1c4 handler, and +0x84 marks an aimed barrage (third run on
 * and the route distance 020cd128 below 15.7). Each shot takes the first idle one of the sixteen
 * +0x46c shells (+0x388 clear) to a point ahead of the track: aimed, fanning out +-(n % 8) / 7 of
 * 60 degrees at 9.7 + 4.0 per row of 8; otherwise along the heading 7.7 + 3.3 x n / 15 further
 * with +-0.5 jitter. When that point is on the ground (020ccfd4 == 3) the shell is launched there
 * (020d189c). Once the +4 item's +0xad byte clears the third run plays pose 0x1c / partner motion
 * 0xb and moves on to 020cfb24; earlier runs replay pose 0x1b. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { VecFx32 pos; int radius; } Sphere;
struct Items46c { char pad[0x46c]; int item[16]; };

struct HitPacket40 {
    int nKind;
    VecFx32 vNormal;
    int w[6];
};

extern int Ov254_TrackTargetFlatDistance(int *node);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *d);
extern int Ov107_CollectSphereOverlaps(int actor, Sphere *sphere, int *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern int Ov254_ProbeGround(int *self, VecFx32 pos, int *outY);
extern void Ov254_InvokeHookAndRearm2(int shell, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov254_OrbitTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov254_BarrageTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;
    Sphere sphere;
    int hits[4];
    int dist;
    unsigned int idx;
    long i;
    long n;
    int off;
    int j;

    dist = Ov254_TrackTargetFlatDistance(node);
    state[0x16] += *(int *)(node[0] + 0x2c);
    if ((*((u8 *)state + 0x70) & 1) == 0) {
        if (state[0x16] >= 0xaa0) {
            *((u8 *)state + 0x70) |= 1;
            state[0x16] = 0;
        }
    } else if (state[0x10] < 0x10 && state[0x16] >= 0x200) {
        if (state[0x10] == 0) {
            struct HitPacket40 packet = {0};
            packet.nKind = 0xa8;
            sphere.radius = 0x6000;
            idx = ANG2IDX(state[0xc]);
            sphere.pos.y = 0;
            sphere.pos.x = data_0203d210[idx * 2];
            sphere.pos.z = data_0203d210[idx * 2 + 1];
            ScaleVec3Fx12(sphere.radius, &sphere.pos, &sphere.pos);
            VEC_Add(&sphere.pos, (VecFx32 *)(*(int *)(*state + 0x414) + 0x14), &sphere.pos);
            n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
            i = 0;
            if (n > 0) {
                do {
                    VEC_Subtract((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)(*state + 0x74), &packet.vNormal);
                    packet.vNormal.y = 0;
                    VEC_Normalize(&packet.vNormal, &packet.vNormal);
                    ScaleVec3Fx12(0x1400, &packet.vNormal, &packet.vNormal);
                    (*(void (**)(unsigned short, struct HitPacket40 *))(hits[i] + 0x1c4))(*(unsigned short *)(hits[i] + 2), &packet);
                } while (++i < n);
            }
            state[0x21] = (state[0x11] + 1 >= 3 && dist < 0xfb31) ? 1 : 0;
        }
        for (i = 0; i < 0x10; i++) {
            if (*(int *)(((struct Items46c *)*state)->item[i] + 0x388) == 0) {
                if (state[0x21] != 0) {
                    off = state[0x10] % 8 * 0x10c1 / 7;
                    if (state[0x10] % 2 == 0) {
                        off = -off;
                    }
                    idx = ANG2IDX(state[0xc] + off);
                    VecSet(&v, data_0203d210[idx * 2], 0, data_0203d210[idx * 2 + 1]);
                    ScaleVec3Fx12((state[0x10] / 8 << 14) + 0x9b31, &v, &v);
                } else {
                    idx = ANG2IDX(state[0xc]);
                    VecSet(&v, data_0203d210[idx * 2], 0, data_0203d210[idx * 2 + 1]);
                    ScaleVec3Fx12(state[0x10] * 0x32000 / 15 + 0x7b31, &v, &v);
                    j = RandNextScaled(0x2001) - 0x1000;
                    v.x += j;
                    j = RandNextScaled(0x2001) - 0x1000;
                    v.z += j;
                }
                VEC_Add(&v, (VecFx32 *)state[2], &v);
                if (Ov254_ProbeGround(state, v, &v.y) == 3) {
                    Ov254_InvokeHookAndRearm2(((struct Items46c *)*state)->item[i], &v);
                }
                state[0x10]++;
                state[0x16] = 0;
                break;
            }
        }
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (++state[0x11] < 3) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x1b, 0);
        state[0x10] = 0;
        state[0x16] = 0;
        *((u8 *)state + 0x70) = 0;
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1c, 0);
    Ov107_StartAnim(*(int *)(*state + 0x430), 0xb, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_OrbitTick);
}
