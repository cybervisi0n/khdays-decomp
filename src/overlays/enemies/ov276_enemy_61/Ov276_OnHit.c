/* Hit handler of the ov276 enemy: ignored while the +0x21a stamina is spent. With a source
 * actor, the facing of the +0x40 yaw against the direction from the +0x474 point to it decides
 * whether the hit comes from the front. Sub-state 6 forces hit mode 0, the damage is resolved
 * (doubled in sub-state 7); a 0x4000-flagged hit sets result 1 unless the actor is guarded
 * (+0x1c4 bit 1). The source and the +0x68 flag-4 bit are recorded; a frontal, unguarded hit in
 * sub-states 2/4 (with flag 0x20) or 5 fires reaction 0x164 mode 5, sets result 1 and, for 2/4,
 * requests sub-state 5. Otherwise the hit point is recorded (flattened), the stamina drops by the
 * damage (clamped to the +0x218 maximum) and a 0x8000 hit that is not the 8|0x80/0x80 special
 * fires reaction 0x164 with the mode taken from the overlay's pairs (0x22 hits use the first
 * pair) alternated by the +0x63 toggle; spent stamina requests sub-state 3. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct ActorHitEvent {
    unsigned int uFlagsLo : 16;
    unsigned int uFlagsHi : 16;
    VecFx32 vPoint;
    int nDamage10;
    char pad014[0xc];
    unsigned int uMode20;
    unsigned int uResultLo : 16;
    unsigned int uResultHi : 16;
    int nDamage;
};

struct ModePair { u8 a[2]; };
struct ModeTable { struct ModePair pair22; struct ModePair pair; };

extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern int VEC_DotProduct(VecFx32 *a, VecFx32 *b);
extern int Ov107_CalcHitDamage(char *actor, struct ActorHitEvent *hit);
extern void Ov107_BuildAndSendUpdate(char *actor, int a, int id, void *at);
extern short data_0203d210[];
extern const struct ModeTable data_ov276_020d2bf8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

int Ov276_OnHit(char *actor, int other, struct ActorHitEvent *hit)
{
    int *state = *(int **)(actor + 0x214);
    VecFx32 facing;
    VecFx32 d;
    struct ModePair pair22;
    struct ModePair pair;
    int frontal;
    int guarded;
    int delta;
    int rem;
    unsigned int idx;

    frontal = 0;
    guarded = (*(u8 *)(actor + 0x1c4) & 2) != 0;
    if (*(short *)(actor + 0x21a) <= 0) {
        return 0;
    }
    if (other != 0) {
        idx = ANG2IDX(state[0x10]);
        facing.x = data_0203d210[idx * 2];
        facing.y = 0;
        facing.z = data_0203d210[idx * 2 + 1];
        VEC_Subtract((void *)(other + 0x74), (void *)(*state + 0x474), &d);
        VEC_Normalize(&d, &d);
        if (VEC_DotProduct(&facing, &d) >= 0) {
            frontal = 1;
        }
    }
    if (*(signed char *)(*state + 0x1c6) == 6) {
        hit->uMode20 = 0;
    }
    hit->nDamage = Ov107_CalcHitDamage(actor, hit);
    if (*(signed char *)(*state + 0x1c6) == 7) {
        hit->nDamage <<= 1;
    }
    if ((hit->uFlagsLo & 0x4000) != 0 && !guarded) {
        hit->uResultLo = 1;
        return 1;
    }
    state[2] = other;
    state[0x1a] = hit->uFlagsLo & 4;
    switch (*(signed char *)(*state + 0x1c6)) {
    case 2:
    case 4:
        if ((hit->uFlagsLo & 0x20) != 0 && !guarded && frontal) {
            Ov107_BuildAndSendUpdate(actor, 0x164, 5, (void *)state[3]);
            hit->uResultLo |= 1;
            *(u8 *)(*state + 0x1c7) = 5;
            return 1;
        }
        break;
    case 5:
        if (frontal && !guarded) {
            Ov107_BuildAndSendUpdate(actor, 0x164, 5, (void *)state[3]);
            hit->uResultLo |= 1;
            return 1;
        }
        break;
    }
    *(VecFx32 *)(state + 10) = hit->vPoint;
    state[0xb] = 0;
    delta = *(short *)(actor + 0x21a) - hit->nDamage;
    if (delta < 0) {
        rem = 0;
    } else {
        rem = *(short *)(actor + 0x218);
        if (delta <= rem) {
            rem = delta;
        }
    }
    *(short *)(actor + 0x21a) = (short)rem;
    if ((hit->uFlagsLo & 0x8000) != 0) {
        if ((hit->uFlagsLo & 8) == 0 || (hit->uFlagsLo & 0x80) == 0 || hit->uFlagsHi != 0x80) {
            if ((hit->uFlagsLo & 0x22) != 0) {
                pair22 = data_ov276_020d2bf8.pair22;
                Ov107_BuildAndSendUpdate(actor, 0x164, pair22.a[*(u8 *)((char *)state + 0x63)], (void *)state[3]);
            } else {
                pair = data_ov276_020d2bf8.pair;
                Ov107_BuildAndSendUpdate(actor, 0x164, pair.a[*(u8 *)((char *)state + 0x63)], (void *)state[3]);
            }
            *(u8 *)((char *)state + 0x63) ^= 1;
        }
    }
    if (*(short *)(actor + 0x21a) != 0) {
        return 1;
    }
    *(u8 *)(*state + 0x1c7) = 3;
    return 1;
}
