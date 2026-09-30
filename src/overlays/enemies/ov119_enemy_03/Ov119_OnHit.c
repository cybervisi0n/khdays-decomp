/* Hit handler of the ov119 enemy (x3 with ov272/ov279): ignored while the +0x21a stamina is spent.
 * The source is kept at +0x58 and the hit point at +0x3c; sub-state 7 forces hit mode 0 and 0xa
 * mode 1, and the damage is resolved. A 0x4000-flagged hit only sets result 1 and requests
 * sub-state 4. Otherwise the stamina drops by the damage (clamped to the +0x218 maximum); spent
 * stamina requests sub-state 3 and a 0x8000 hit sub-state 4. A damaging hit that is not the
 * 8|0x80/0x80 special flips the +0x71 parity and fires reaction 0x121 at the +0x4c point with mode
 * 2/3 (bits 1/5) or 0/1 picked by it. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct ActorHitEvent {
    unsigned int uFlagsLo : 16;
    unsigned int uFlagsHi : 16;
    VecFx32 vPoint;
    int nDamage10;
    unsigned char pad014[0xc];
    unsigned int uMode20;
    unsigned int uResultLo : 16;
    unsigned int uResultHi : 16;
    int nDamage;
};
struct Parity { u8 b0 : 1; };

extern int Ov107_CalcHitDamage(char *actor, struct ActorHitEvent *hit);
extern void Ov107_BuildAndSendUpdate(char *actor, int a, u16 id, void *at);

int Ov119_OnHit(char *actor, int other, struct ActorHitEvent *hit)
{
    int *state = *(int **)(actor + 0x214);
    int delta;
    int rem;

    if (*(short *)(actor + 0x21a) <= 0) {
        return 0;
    }
    state[0x16] = other;
    *(VecFx32 *)(state + 0xf) = hit->vPoint;
    switch (*(signed char *)(*state + 0x1c6)) {
    case 7:
        hit->uMode20 = 0;
        break;
    case 0xa:
        hit->uMode20 = 1;
        break;
    }
    hit->nDamage = Ov107_CalcHitDamage(actor, hit);
    if ((hit->uFlagsLo & 0x4000) != 0) {
        hit->uResultLo = 1;
        *(u8 *)(*state + 0x1c7) = 4;
        return 1;
    }
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
    if (*(short *)(actor + 0x21a) == 0) {
        *(u8 *)(*state + 0x1c7) = 3;
    } else if ((hit->uFlagsLo & 0x8000) != 0) {
        *(u8 *)(*state + 0x1c7) = 4;
    }
    if ((short)hit->nDamage10 > 0) {
        if ((hit->uFlagsLo & 8) == 0 || (hit->uFlagsLo & 0x80) == 0 || hit->uFlagsHi != 0x80) {
            if ((hit->uFlagsLo & 0x22) != 0) {
                ((struct Parity *)((u8 *)state + 0x71))->b0++;
                Ov107_BuildAndSendUpdate(actor, 0x121, (((struct Parity *)((u8 *)state + 0x71))->b0 & 1) ? 2 : 3, (void *)state[0x13]);
            } else {
                ((struct Parity *)((u8 *)state + 0x71))->b0++;
                Ov107_BuildAndSendUpdate(actor, 0x121, (((struct Parity *)((u8 *)state + 0x71))->b0 & 1) ? 0 : 1, (void *)state[0x13]);
            }
        }
    }
    return 1;
}
