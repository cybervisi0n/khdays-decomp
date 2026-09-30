/* Circling tick of the ov254 boss. The +0x44 clock runs; in sub-state 5 the +0x48 clock also fires
 * reaction 0x16d modes 4 and 0x12 once each (at 0.17 and 0.4, +0x70 bits 0/1). The +0xc velocity is
 * the +0x30 heading scaled by the +0x430 rig's +0x38 speed and the +0x58 ease (which decays by +0x5c),
 * and its height steers towards the ground found by 020cd840 (1/32 of the gap). Once the +4 rig is
 * idle: after 1.75 the +0x4c lap count grows (heading +1.57 into +0x38) unless +0x84 is set; the third
 * lap ends the circle (pose 6 / 3, rig motion 5 / 2, sub-state 5 extras), keeps the target height and
 * the gap to the ground and hands over to 020d0448; otherwise every 1.75 the +0x84 dash toggles
 * (ease 0.94 or back to 1.0) and pose 5 / 2 with rig motion 4 / 1 restart. While dashing, a ground
 * probe along the +0x38 heading ahead of the target ends the dash (+0x70 bit 7, heading kept in
 * +0x34) when it hits far enough, or the heading turns on; before the third lap 020cd128 runs. Not
 * dashing, sub-state 5 lands (020cd474, helper mode 2) or restarts the circle (020cd3c4). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct CollisionResult { int pad00; int pad04; int field08; int nAlong; };

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, int at);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov254_PanelYForPhase(int *state, int a);
extern void Ov107_PostTagUpdate(int owner, int pose, int loop);
extern void Ov107_StartAnim(int rig, int motion, int mode);
extern void Ov254_KnockbackAtFeet(int actor, int side);
extern void Ov254_ForwardToAiTaskWhenReady(int helper, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern struct CollisionResult *Collision_CastSphere(int collision, VecFx32 *from, VecFx32 *step, int radius);
extern int Ov254_TrackTargetFlatDistance(int *node);
extern int Ov254_LedgeCheck(int *state);
extern int Ov254_TargetInReach(int *state, int a);
extern void Ov254_SwoopTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov254_CircleTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 p;
    VecFx32 d;
    int world = *(int *)(*state + 4);
    unsigned int idx;
    u8 f;
    struct CollisionResult *hit;

    state[0x11] += *(int *)(node[0] + 0x2c);
    if (*(signed char *)(*state + 0x1c6) == 5) {
        state[0x12] += *(int *)(node[0] + 0x2c);
        f = *((u8 *)state + 0x70);
        if ((f & 1) == 0 && state[0x12] >= 0x2a8) {
            *((u8 *)state + 0x70) = f | 1;
            Ov107_BuildAndSendUpdate(*state, 0x16d, 4, state[2]);
        }
        f = *((u8 *)state + 0x70);
        if ((f & 2) == 0 && state[0x12] >= 0x660) {
            *((u8 *)state + 0x70) = f | 2;
            Ov107_BuildAndSendUpdate(*state, 0x16d, 0x12, state[2]);
        }
    }
    idx = ANG2IDX(state[0xc]);
    state[3] = data_0203d210[idx * 2];
    state[4] = 0;
    state[5] = data_0203d210[idx * 2 + 1];
    ScaleVec3Fx12(*(int *)(*(int *)(*state + 0x430) + 0x38), (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    ScaleVec3Fx12(state[0x16], (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[0x16] = FX_MUL(state[0x16], state[0x17]);
    state[4] = FX_MUL(Ov254_PanelYForPhase(state, -1) - ((VecFx32 *)state[2])->y, 0x80);
    if (*(u8 *)(state[1] + 0xad) == 0) {
        if (state[0x11] >= 0x1c00 && state[0x21] == 0) {
            state[0x13]++;
            state[0xe] = state[0xc] + 0x1922;
        }
        if (state[0x13] >= 3) {
            Ov107_PostTagUpdate(*state, *(signed char *)(*state + 0x1c6) == 5 ? 6 : 3, 0);
            Ov107_StartAnim(*(int *)(*state + 0x430), *(signed char *)(*state + 0x1c6) == 5 ? 5 : 2, 0);
            if (*(signed char *)(*state + 0x1c6) == 5) {
                Ov254_KnockbackAtFeet(*state, -1);
                Ov254_ForwardToAiTaskWhenReady(*(int *)(*state + 0x460), 4);
            }
            state[0x11] = 0;
            state[0x14] = ((VecFx32 *)state[2])->y;
            state[0x15] = Ov254_PanelYForPhase(state, 0xb) - state[0x14];
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_SwoopTick);
            return;
        }
        if (state[0x11] >= 0x1c00) {
            state[0x11] = 0;
            if ((state[0x21] ^= 1) != 0) {
                state[0x17] = 0xf00;
            } else {
                state[0x16] = 0x1000;
                state[0x17] = 0x1000;
            }
        }
        Ov107_PostTagUpdate(*state, *(signed char *)(*state + 0x1c6) == 5 ? 5 : 2, 0);
        Ov107_StartAnim(*(int *)(*state + 0x430), *(signed char *)(*state + 0x1c6) == 5 ? 4 : 1, 0);
        if (*(signed char *)(*state + 0x1c6) == 5) {
            Ov254_ForwardToAiTaskWhenReady(*(int *)(*state + 0x460), 5);
            state[0x12] = 0;
            *((u8 *)state + 0x70) &= ~3;
        }
    }
    if (state[0x21] != 0) {
        if (state[0x13] + 1 >= 3) {
            if ((*((u8 *)state + 0x70) & 0x80) != 0) {
                return;
            }
            {
                VecFx32 *t = (VecFx32 *)state[2];

                VecSet(&p, t->x, 0x2000, t->z);
            }
            idx = ANG2IDX(state[0xe]);
            d.x = data_0203d210[idx * 2];
            d.y = 0;
            d.z = data_0203d210[idx * 2 + 1];
            ScaleVec3Fx12(0x164cf, &d, &d);
            hit = Collision_CastSphere(*(int *)(world + 0x7c), &p, &d, 0xf662);
            if (hit != 0 && hit->field08 == 0 &&
                (int)(((long long)hit->nAlong * 0x164cf) >> 27) >= 0x1f000 - 0x164cf) {
                *((u8 *)state + 0x70) |= 0x80;
                state[0x11] = 0;
                state[0xd] = state[0xe];
                return;
            }
            state[0xe] += 0x14b296;
            return;
        }
        Ov254_TrackTargetFlatDistance(node);
        return;
    }
    if (*(signed char *)(*state + 0x1c6) != 5) {
        return;
    }
    if (Ov254_LedgeCheck(state) != 0) {
        Ov254_ForwardToAiTaskWhenReady(*(int *)(*state + 0x460), 2);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (Ov254_TargetInReach(state, 0) == 0) {
        return;
    }
    state[0x17] = 0xf00;
    state[0x13] = 3;
}
