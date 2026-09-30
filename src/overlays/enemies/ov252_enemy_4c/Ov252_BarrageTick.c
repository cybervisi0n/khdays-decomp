/* Barrage tick of the ov252 actor: +0x6c accumulates the frame rate and it faces the target
 * (020cdfe8 with the offset); unguarded it backs off under 10.0 and closes in beyond 64.0 at 0.3125.
 * +0x64 and +0x68 accumulate too. Every 20th shot (+0x60) with a volley running (+0x88) the volley
 * restarts: +0x89 cue, the +0x588 flag clears, the +0x64c muzzle model hides and the shot timer is
 * set back 1.0. Each 0.125 a shot fires: with a target the owner fires effect 0x13 from the +0x570
 * muzzle along the +0x54 heading (spread +/-0xd6, raised up to 0.16 over the aim height); the muzzle
 * shows while a volley runs, a cue fires effect 0x34 at the +0x560 point with +0x588 set, and the shot
 * counters advance. Once the partner holds no queued move it turns toward the origin (020cdb88) and,
 * after 10.0 (15.0 guarded), clears +0x588, plays pose 0x1c and moves on to 020d0220; else pose 0x1b. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int Ov252_HeadingDelta(int *node, VecFx32 *v, int angle, int wantAbs);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_IdleDecide(void);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov252_BarrageTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 delta;
    VecFx32 muzzle;
    VecFx32 aim;
    VecFx32 dir;
    VecFx32 d;
    VecFx32 p;
    int gap;
    int spread;

    state[0x1b] += *(int *)(node[0] + 0x2c);
    gap = Ov252_CheckTarget(node, &delta, 1);
    if (state[0x2b] == 0) {
        if (gap < 0xa000) {
            ScaleVec3Fx12(-0x500, &delta, (VecFx32 *)(state + 3));
        }
        if (gap > 0x40000) {
            ScaleVec3Fx12(0x500, &delta, (VecFx32 *)(state + 3));
        }
    }
    state[0x19] += *(int *)(node[0] + 0x2c);
    state[0x1a] += *(int *)(node[0] + 0x2c);
    if (state[0x18] % 20 == 0 && *((unsigned char *)state + 0x88) != 0) {
        *((unsigned char *)state + 0x88) = 0;
        *((unsigned char *)state + 0x89) = 1;
        *(int *)(*state + 0x588) = 0;
        *(int *)(*(int *)(*state + 0x64c) + 0x5c) |= 2;
        state[0x19] -= 0x1000;
    }
    if (state[0x19] >= 0x200) {
        muzzle = *(VecFx32 *)(*(int *)(*state + 0x570) + 0x14);
        if (*(int *)(*state + 0x4e4) != 0) {
            VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x4e4) + 0x190), &muzzle, &aim);
            VEC_Normalize(&aim, &dir);
            spread = RandNextScaled(0x1ad) - 0xd6;
            {
                int idx = ANG2IDX(state[0x15] + spread) * 2;

                aim.y = 0;
                aim.x = data_0203d210[idx];
                aim.z = data_0203d210[idx + 1];
            }
            {
                int lift = RandNextScaled(0x281) + (spread - spread);

                aim.y = dir.y + lift;
            }
            func_ov107_020c0b90(*state, 0x13, aim, 0);
        }
        if (*((unsigned char *)state + 0x88) == 0) {
            *(int *)(*(int *)(*state + 0x64c) + 0x5c) &= ~2;
        }
        if (*((unsigned char *)state + 0x89) != 0) {
            *((unsigned char *)state + 0x89) = 0;
            *(int *)(*state + 0x588) = 1;
            func_ov107_020c0b90(*state, 0x34, *(VecFx32 *)(*(int *)(*state + 0x560) + 0x14), 0);
        }
        *((unsigned char *)state + 0x88) += 1;
        state[0x18]++;
        state[0x19] = 0;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    p = *(VecFx32 *)state[2];
    p.y = 0;
    VEC_Subtract(&data_02041dc8, &p, &d);
    VEC_Normalize(&d, &d);
    Ov252_HeadingDelta(node, &d, state[0x15], 1);
    if ((state[0x2b] == 0 && state[0x1a] >= 0xa000) || (state[0x2b] != 0 && state[0x1a] >= 0xf000)) {
        *(int *)(*state + 0x588) = 0;
        Ov107_PostTagUpdate((Actor *)(*state), 0x1c, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_IdleDecide);
    } else {
        Ov107_PostTagUpdate((Actor *)(*state), 0x1b, 0);
    }
}
