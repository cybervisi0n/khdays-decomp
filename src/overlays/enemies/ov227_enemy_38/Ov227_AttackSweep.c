/* Attack sweep of the ov227 enemy: the entities hit by the given box (020c8fd0) or, without one, by a
 * sphere around the owner's +0x3b0 hand (radius 2.47, or 4.56 for attack 7, pushed that far along the
 * +0x50 heading) are handled once per attack (the +0x76 mask, the owner's +0x438 one for attack 6).
 * Each is pushed by 1.0 along the flattened direction away from the owner; attack 6 first offers a
 * 40-byte hit packet to its +0x1c4 hook (the owner's +0x290 power and +0x28c / +0x1a0 knock bits for
 * the attack, knock 100, a random reaction 70 % of the time), otherwise the ov107 checker decides.
 * An accepted hit sends message 0 at the entity (or the matching point on the sphere) and marks it;
 * any hit fires reaction 0x4f (attack 6) or 0x51 at the +8 target. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 pos; int nRadius; } Sphere;
struct Bit6 { u8 b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1; };

struct HitPacket40 {
    int nKind;
    VecFx32 vNormal;
    int nPower;
    int nReaction;
    u8 bKnock;
    int pSource;
    int w[2];
};

typedef int (*HitHook)(u16 id, struct HitPacket40 *packet);

extern int Ov107_CollectEntitiesTouchingDisc(int owner, void *query, int *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *d);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(int hit, int a, int b, u8 kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int a, int id, int at);
extern const short data_0203d210[];
extern const VecFx32 data_02042258;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov227_AttackSweep(int *state, int kind, void *box)
{
    Sphere sphere;
    int hits[4];
    VecFx32 off;
    VecFx32 push;
    VecFx32 pos;
    u8 *mask;
    long n;
    int hit;
    int bit;
    int k6;
    long i;
    int found;

    hit = 0;
    if (kind == 6) {
        mask = (u8 *)(*state + 0x438);
    } else {
        mask = (u8 *)state + 0x76;
    }
    if (box != 0) {
        n = Ov107_CollectEntitiesTouchingDisc(*state, box, hits);
    } else {
        sphere.pos = *(VecFx32 *)(**(int **)(*state + 0x3b0) + 0x20);
        sphere.nRadius = 0x2785;
        if (kind == 7) {
            sphere.nRadius = 0x48f6;
        } else {
            unsigned int idx = ANG2IDX(state[0x14]);

            off.x = data_0203d210[idx * 2];
            off.y = 0;
            off.z = data_0203d210[idx * 2 + 1];
            ScaleVec3Fx12(0x2785, &off, &off);
            VEC_Add(&off, &sphere.pos, &sphere.pos);
        }
        n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
    }
    i = 0;
    if (n > 0) {
        k6 = kind * 6;
        do {
            found = 0;
            bit = (1 << *(u16 *)(hits[i] + 2)) & 0xff;
            if ((*mask & bit) != 0) {
                continue;
            }
            VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
            VEC_Normalize(&push, &pos);
            push.y = 0;
            if (VEC_Normalize(&push, &push) == 0) {
                push = data_02042258;
            }
            ScaleVec3Fx12(0x1000, &push, &push);
            if (kind == 6 && *(HitHook *)(hits[i] + 0x1c4) != 0) {
                struct HitPacket40 packet = {0};
                int owner = *state;
                u8 *attack = (u8 *)(owner + 0x28c + k6);

                packet.vNormal = push;
                packet.nPower = *(u16 *)(k6 + owner + 0x290);
                packet.nKind |= 1;
                packet.pSource = *state;
                packet.bKnock = 100;
                if (*(int *)(owner + 0x1a0) != 0 &&
                    ((struct Bit6 *)(*(int *)(owner + 0x1a0) + kind * 4 + 6))->b6 == 0) {
                    packet.nKind |= 8;
                }
                if ((attack[0] & 1) != 0) {
                    packet.nKind |= 0x10;
                }
                if ((attack[1] & 1) != 0) {
                    packet.nKind |= 0x20;
                }
                if ((attack[1] & 2) != 0) {
                    packet.nKind |= 0x40;
                }
                if ((unsigned int)RandNextScaled(100) < 0x46) {
                    switch (RandNextScaled(5)) {
                    case 0:
                        packet.nReaction = 0xa;
                        break;
                    case 1:
                        packet.nReaction = 9;
                        break;
                    case 2:
                        packet.nReaction = 5;
                        break;
                    case 3:
                        packet.nReaction = 6;
                        break;
                    default:
                        packet.nReaction = 1;
                        break;
                    }
                }
                found = (*(HitHook *)(hits[i] + 0x1c4))(*(u16 *)(hits[i] + 2), &packet);
            }
            if (found == 0) {
                found = Ov107_InvokeHitCallback(hits[i], *state, *state, kind, &push, 0);
            }
            if (found != 0) {
                if (box != 0) {
                    pos = *(VecFx32 *)(hits[i] + 0x74);
                } else {
                    ScaleVec3Fx12(sphere.nRadius, &pos, &pos);
                    VEC_Add(&pos, &sphere.pos, &pos);
                }
                func_ov107_020c0b90(*state, 0, pos, 0);
                hit = 1;
                *mask |= bit;
            }
        } while (++i < n);
    }
    if (hit == 0) {
        return;
    }
    switch (kind) {
    case 6:
        Ov107_BuildAndSendUpdate(*state, 0, 0x4f, state[2]);
        return;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    default:
        Ov107_BuildAndSendUpdate(*state, 0, 0x51, state[2]);
        return;
    }
}
