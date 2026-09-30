/* Sweep tick of an ov254 helper's attack: the +0xc timer accumulates the frame rate and pose 1
 * loops once the +4 item's +0xad byte clears. Until armed (+0x20, set at 0x1980) nothing else
 * happens. Armed, the +0x388 shape's box (lowered to a half-height of 8.0) is swept through the
 * +0x390 owner's hit list (020c8df0); each hit part k (< 4) whose +0x10 cooldown ran out is pushed
 * away from the helper (flattened, 3.0 across, 1.0 up, +x when on top) through 020ca918 and, if
 * that lands, knocks the owner back at the part (mode 0), restarts the cooldown (0x198) and flags
 * the owner's +0x4e0 hit. The other cooldowns run down. With a hit, reaction 0x16d/0xc (strong,
 * +0xa8 of the item) or 0/0x50 fires at the box. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 axis[3]; int ext[3]; } Box;

extern void Ov107_PostTagUpdate(int actor, int pose, int loop);
extern int Ov107_CollectCapsuleOverlaps(int owner, Box *box, int *hits);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern const VecFx32 data_02042258;

void Ov254_AttackSweepTick(int *node)
{
    int *state = (int *)node[1];
    Box box;
    int hits[4];
    VecFx32 d;
    VecFx32 fallback;
    int nHits;
    long i;
    unsigned int k;
    u8 bit;
    u8 hitMask = 0;

    state[3] += *(int *)(node[0] + 0x2c);
    if (*(u8 *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate(*state, 1, 1);
    }
    if (*((u8 *)state + 0x20) == 0) {
        if (state[3] >= 0x1980) {
            *((u8 *)state + 0x20) = 1;
        }
        return;
    }
    box = *(Box *)(**(int **)(*state + 0x388) + 0x94);
    box.pos.y += box.ext[1] - 0x8000;
    box.ext[1] = 0x8000;
    nHits = Ov107_CollectCapsuleOverlaps(*(int *)(*state + 0x390), &box, hits);
    i = 0;
    if (nHits > 0) {
        fallback = data_02042258;
        do {
            k = *(u16 *)(hits[i] + 2);
            bit = 1 << k;
            if (k < 4 && state[4 + k] <= 0) {
                VEC_Subtract((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)(*state + 0x74), &d);
                d.y = 0;
                if (VEC_Normalize(&d, &d) == 0) {
                    d = fallback;
                }
                ScaleVec3Fx12(0x3000, &d, &d);
                d.y = 0x1000;
                if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x390), 4, &d, 0) != 0) {
                    func_ov107_020c0b90(*(int *)(*state + 0x390), 0, *(VecFx32 *)(hits[i] + 0x74), 0);
                    state[4 + k] = 0x198;
                    hitMask |= bit;
                    *(int *)(*(int *)(*state + 0x390) + 0x4e0) = 1;
                }
            }
        } while (++i < nHits);
    }
    for (i = 0; i < 4; i++) {
        if ((hitMask & (1 << i)) == 0) {
            state[4 + i] -= *(int *)(node[0] + 0x2c);
            if (state[4 + i] < 0) {
                state[4 + i] = 0;
            }
        }
    }
    if (*(int *)(*(int *)(*state + 0x390) + 0x4e0) == 0) {
        return;
    }
    if (*(u8 *)(state[1] + 0xa8) != 0) {
        Ov107_BuildAndSendUpdate(*state, 0x16d, 0xc, &box);
    } else {
        Ov107_BuildAndSendUpdate(*state, 0, 0x50, &box);
    }
}
