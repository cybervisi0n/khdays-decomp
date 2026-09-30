/* Ov263_ChaseTick -- chase tick of the ov263 enemy. The +0x18
 * heading turns towards the +0x1c target heading (8x the owner's rate when +0x4a is 1, else 3x)
 * and orients the owner's +0xa0 pose. Without a target (+0x14, re-acquired by
 * Ov263_AcquireTarget) sub-state 2 is requested and the tick ends. The +0x24 delay runs down in
 * sub-states 2/4. In those sub-states a 7.0 floor probe (Ov263_ProbeGround) is cast; with a
 * target 7.0 or more below the +8 point sub-state 0xe is requested (unless +0x58 is set), and one
 * 3.0 or more above arms the +0x2c climb timer (0xd00). When the probe hits, the climb timer raises
 * the +0x34 lift by 0x200 per tick while it runs down, and the distance from the +0xc point to the
 * contact raises it below 1.07 or lowers it above 2.67 (once the climb is over). Finally the +0x30
 * velocity goes to the owner's +0xf0 and is scaled by 0.25. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Srt_SetRotationQuat(void *pose, void *q);
extern void Ov263_AcquireTarget(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Ov263_ProbeGround(int *node, VecFx32 *dir, int direct);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02042264;
extern const VecFx32 data_ov263_020d3698;

void Ov263_ChaseTick(int *node)
{
    int *state = (int *)node[1];
    int q[4];
    VecFx32 probe;
    int len;
    int d;

    state[6] = Angle_TurnToward(state[6], state[7],
                             *((unsigned char *)state + 0x4a) != 1 ? *(int *)(*node + 0x2c) * 3 : *(int *)(*node + 0x2c) << 3, 0);
    QuatFromAxisAngle(q, &data_02042264, state[6]);
    Srt_SetRotationQuat((void *)(*state + 0xa0), q);
    if (state[5] == 0) {
        Ov263_AcquireTarget(node);
        if (state[5] == 0) {
            *(unsigned char *)(*state + 0x1c7) = 2;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    if (state[9] > 0) {
        if (*(signed char *)(*state + 0x100 + 0xc6) == 2 || *(signed char *)(*state + 0x100 + 0xc6) == 4) {
            state[9] -= *(int *)(*node + 0x2c);
        }
        if (state[9] < 0) {
            state[9] = 0;
        }
    }
    if (*(signed char *)(*state + 0x100 + 0xc6) == 2 || *(signed char *)(*state + 0x100 + 0xc6) == 4) {
        probe = data_ov263_020d3698;
        probe.y = Ov263_ProbeGround(node, &probe, 1);
        if (state[5] != 0) {
            d = *(int *)(state[2] + 4) - *(int *)(state[5] + 0x194);
            if (d >= 0x7000) {
                if (state[0x16] == 0) {
                    *(unsigned char *)(*state + 0x1c7) = 0xe;
                }
            } else if (*(int *)(state[5] + 0x194) - *(int *)(state[2] + 4) >= 0x3000) {
                state[0xb] = 0xd00;
            }
        }
        if (probe.y != 0) {
            VEC_Add(&probe, (VecFx32 *)state[3], &probe);
            VEC_Subtract(&probe, (VecFx32 *)state[3], &probe);
            len = VEC_Normalize(&probe, &probe);
            if (state[0xb] != 0) {
                state[0xd] += 0x200;
                state[0xb] -= *(int *)(*node + 0x2c);
                if (state[0xb] < 0) {
                    state[0xb] = 0;
                }
            }
            if (len < 0x189e) {
                state[0xd] += 0x200;
            } else if (len > 0x3d8b) {
                if (state[0xb] == 0) {
                    state[0xd] -= 0x200;
                }
            }
        }
    }
    {
        VecFx32 *vel = (VecFx32 *)(state + 0xc);
        *(VecFx32 *)(*state + 0xf0) = *vel;
        ScaleVec3Fx12(0x400, vel, vel);
    }
}
