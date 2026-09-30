/* Hover tick of the ov221 enemy. Airborne outside sub-state 6 the +0x50 heading turns towards
 * +0x58 at 0x96 (0x28 without a +0x78 target) per 100 of rate (0203d040), and the owner's
 * +0xa0 pose takes the heading about data_02042264. Sub-state 6 drains the +0x68 timer. The
 * +0x14 step is copied out and, in sub-states 2/4/5/6 with a target, its y becomes half the
 * gap from the +8 point to the target's +0x194 plus 2.0 (less whatever the owner's +0x13c
 * exceeds 9.0 by, never below the target itself), clamped to [-0x100, 0x60]; in sub-state 9 a
 * cast of the owner's +0x80 radius along the step from its +0x74 clips the step to the first
 * blocking hit. The step lands in the owner's +0xf0 and the +0x14 velocity decays by 0x300 per
 * 0x88 of rate. A +0x60 bit-0 owner then runs the jump handler (ov221 1044) and, once it has
 * launched (+0x400 bit 7), accumulates +0x41c up to 0xc38: within it the strike sweep (ov221
 * 0a2c mode 6) runs from the +0x410 landing with the three axes and a reach growing from 0x1933
 * to 0x4333 across the window. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int q[4]; } Quat;

struct Ov221SweepParams {
    VecFx32 aim;
    VecFx32 v0c;
    VecFx32 v18;
    VecFx32 v24;
    int nReach;
    int bFlag;
};

struct CollisionHit {
    int pad00;
    int pad04;
    int nBlocked;
    int nAlong;
};

struct b1 { unsigned char b0 : 1; };
struct hw60 { unsigned short lo : 8, hi : 8; };

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern int Angle_TurnToward(int cur, int target, int step, int flag);
extern void QuatFromAxisAngle(Quat *out, const VecFx32 *axis, int angle);
extern void Srt_SetRotationQuat(void *pose, Quat *q);
extern int Ov107_FindNearestObject(int owner, int flag);
extern struct CollisionHit *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern void ScaleVec3Fixed27(int scale, VecFx32 *in, VecFx32 *out);
extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov221_RunSubStateScript(int self, int *node);
extern void Ov221_StrikeSweepEntities(int *state, int mode, struct Ov221SweepParams *params);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;

void Ov221_HoverTick(int *node)
{
    int *state = (int *)node[1];
    Quat q;
    VecFx32 step;
    VecFx32 origin;
    struct Ov221SweepParams params;
    int rem;
    int nStep;
    int target;
    int h;
    int d;
    int t;
    struct CollisionHit *hit;

    step = *(VecFx32 *)(state + 5);
    if (((struct b1 *)(*state + 0x17a))->b0 == 0 && *(signed char *)(*state + 0x1c6) != 6) {
        state[0x14] = Angle_TurnToward(state[0x14], state[0x16],
                                    *(int *)(*node + 0x2c) * (state[0x1e] != 0 ? 0x96 : 0x28) / 100, 0);
    }
    QuatFromAxisAngle(&q, &data_02042264, state[0x14]);
    Srt_SetRotationQuat((void *)(*state + 0xa0), &q);
    if (*(signed char *)(*state + 0x1c6) == 6) {
        state[0x1a] -= *(int *)(*node + 0x2c);
        if (state[0x1a] < 0) {
            state[0x1a] = 0;
        }
    }
    switch (*(signed char *)(*state + 0x1c6)) {
    case 2:
    case 4:
    case 5:
    case 6:
        if (state[0x1e] != 0 && (target = Ov107_FindNearestObject(*state, 0)) != 0) {
            h = *(int *)(target + 0x194) + 0x2000;
            if (*(int *)(*state + 0x13c) > 0x9000) {
                h -= *(int *)(*state + 0x13c) - 0x9000;
            }
            if (h < *(int *)(target + 0x194)) {
                h = *(int *)(target + 0x194);
            }
            d = (h - *(int *)(state[2] + 4)) / 2;
            step.y = d;
            if (d > 0x60) {
                d = 0x60;
            } else if (d < -0x100) {
                d = -0x100;
            }
            step.y = d;
        }
        break;
    case 9:
        origin = *(VecFx32 *)(*state + 0x74);
        hit = Collision_CastSphereEx(*(void **)(*(int *)(*state + 4) + 0x7c), &origin, &step, *(int *)(*state + 0x80), 0);
        if (hit != 0 && hit->nBlocked == 0) {
            ScaleVec3Fixed27(hit->nAlong, &step, &step);
        }
        break;
    }
    *(VecFx32 *)(*state + 0xf0) = step;
    for (rem = *(int *)(*node + 0x2c); rem > 0; rem -= 0x88) {
        nStep = rem <= 0x88 ? rem : 0x88;
        ScaleVec3Fx12(0x1000 - FX_Mul(FX_Div(nStep, 0x88), 0x300), (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
    }
    if ((((struct hw60 *)(*state + 0x60))->lo & 1) == 0) {
        return;
    }
    Ov221_RunSubStateScript(*state, node);
    if ((*(u8 *)(*state + 0x400) & 0x80) == 0) {
        return;
    }
    *(int *)(*state + 0x41c) += *(int *)(*node + 0x2c);
    if (*(int *)(*state + 0x41c) > 0xc38) {
        return;
    }
    params.aim = *(VecFx32 *)(*state + 0x410);
    t = *(int *)(*state + 0x41c);
    if (t > 0xc38) {
        t = 0xc38;
    } else if (t < 0) {
        t = 0;
    }
    params.nReach = 0x1900 + (t * 0x2a00 / 0xc38 + 0x33);
    params.v0c = data_02042270;
    params.v18 = data_02042258;
    params.v24 = data_02042264;
    params.bFlag = 1;
    Ov221_StrikeSweepEntities(state, 6, &params);
}
