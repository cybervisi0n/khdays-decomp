/* Hit filter of the ov265 enemy (variant of ov231/ov232's with 30/15 dodge odds), active
 * while the +0x21a stock is positive (a d100 is drawn first either way). In sub-state 7 every hit
 * only sets bit 0 of the result. A 1|0x10 hit in sub-state 8 requests sub-state 0xa. In sub-states
 * 2/4 and unguarded (+0x1c4 bit 1 clear), a 0x20 hit on a d100 below 30, or any hit on a d100 below
 * 15, is dodged: the +0x24 timer clears, +0x49 records on which side of the +0x3c facing the
 * owner's forward axis lies, and sub-state 0xb is requested. Otherwise the damage is resolved into
 * +0x28, the stock drains (clamped to 0..+0x218), an empty owner stock requests sub-state 3 and a
 * damaging hit that is not the 0x88-flagged 0x80 kind fires reaction +0x50 at the +8 anchor with
 * the {0,1} / {2,3} (0x22 hits) mode picked by the +0x4e parity, which then flips. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct HitWord { unsigned int lo : 16, hi : 16; };

extern int VEC_DotProduct(const void *a, const VecFx32 *b);
extern int Ov107_CalcHitDamage(char *self, unsigned int *hit);
extern void Ov107_BuildAndSendUpdate(char *self, int id, int mode, void *at);
extern const VecFx32 data_02042258;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

int Ov265_FilterHit(char *self, int src, unsigned int *hit)
{
    unsigned char modes[2][2] = { { 0, 1 }, { 2, 3 } };
    int *state = *(int **)(self + 0x214);
    VecFx32 fwd;
    int stock;
    int limit;

    RandNextScaled(100);
    if (*(short *)(self + 0x200 + 0x1a) <= 0) {
        return 0;
    }
    if (*(signed char *)(*state + 0x100 + 0xc6) == 7) {
        ((struct HitWord *)(hit + 9))->lo = 1;
        return 1;
    }
    if ((((struct HitWord *)hit)->lo & 1) != 0 && (((struct HitWord *)hit)->lo & 0x10) != 0
        && *(signed char *)(*state + 0x100 + 0xc6) == 8) {
        *(unsigned char *)(*state + 0x1c7) = 0xa;
        return 0;
    }
    if ((*(signed char *)(*state + 0x100 + 0xc6) == 2 || *(signed char *)(*state + 0x100 + 0xc6) == 4)
        && (*(unsigned char *)(*state + 0x1c4) & 2) == 0) {
        if (((((struct HitWord *)hit)->lo & 0x20) != 0 && (unsigned int)RandNextScaled(100) < 0x1e)
            || (unsigned int)RandNextScaled(100) < 0xf) {
            state[9] = 0;
            Vec3TransformViaTempMtx(&fwd, (void *)(*state + 0xa0), &data_02042258);
            VEC_DotProduct(state + 0xf, &fwd);
            *((unsigned char *)state + 0x49) = FX_MUL(state[0xf], fwd.z) - FX_MUL(state[0x11], fwd.x) < 0;
            *(unsigned char *)(*state + 0x1c7) = 0xb;
            return 0;
        }
    }
    hit[10] = Ov107_CalcHitDamage(self, hit);
    stock = *(short *)(self + 0x200 + 0x1a) - hit[10];
    if (stock < 0) {
        limit = 0;
    } else {
        limit = *(short *)(self + 0x200 + 0x18);
        if (stock <= limit) {
            limit = stock;
        }
    }
    *(short *)(self + 0x200 + 0x1a) = limit;
    if (*(short *)(*state + 0x200 + 0x1a) == 0) {
        *(unsigned char *)(*state + 0x1c7) = 3;
    }
    if ((int)hit[10] > 0) {
        if (!((((struct HitWord *)hit)->lo & 8) != 0 && (((struct HitWord *)hit)->lo & 0x80) != 0 && ((struct HitWord *)hit)->hi == 0x80)) {
            Ov107_BuildAndSendUpdate(self, *(short *)((char *)state + 0x50),
                                modes[(((struct HitWord *)hit)->lo & 0x22) != 0][*((unsigned char *)state + 0x4e)], (void *)state[2]);
            *((unsigned char *)state + 0x4e) ^= 1;
        }
    }
    return 1;
}
