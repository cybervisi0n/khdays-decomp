/* Dash tick of the ov228 enemy (x2 with ov229). The +0x24 speed grows by 0.1875 per 0x88 step of
 * the owner's rate (clamped to [0xc0, 0x800]) and scales the +0x14 direction into the +8 step.
 * While the +0x38 weave flag holds, the +0x2c phase advances by 21 per unit of rate until it
 * passes 0xffff; afterwards it advances by 42 (wrapped to 16 bits) and the +0x30 weave amplitude
 * decays by 1/16 per step. The sideways weave (amplitude x sine of the phase), turned by the
 * +0x28 heading, is added to the step. A blocked step (Ov228_ScanNearbyEntitiesReact) spawns the body's
 * effect 8 at the owner's +0x74 point and fires reaction 0x12b mode 0xe at the +4 point; a wall
 * contact (+0x17a bit 1) fires the reaction and clears the step; otherwise the travelled length
 * accumulates in +0x20 until 21.0. Ending: effect 0 (flag 1) at the owner, animation 2, the length
 * restarts and the tick hands over to Ov228_AiScanReactTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;
struct Bits17a { unsigned char b0 : 1, b1 : 1; };

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

extern int FX_Div(int a, int b);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *m, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov228_ScanNearbyEntitiesReact(int *state);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int VEC_Mag(const VecFx32 *v);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;
extern void Ov228_AiScanReactTick(int *node);

void Ov228_DashTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 weave;
    Mtx33 mtx;
    int step;
    int speed;
    int item;

    step = *(int *)(*node + 0x2c);
    while (step > 0) {
        state[9] = (int)(((long long)state[9]
                              * ((int)(((long long)FX_Div(step <= 0x88 ? step : 0x88, 0x88) * 0x300LL + 0x800LL) >> 12)
                                 + 0x1000)
                          + 0x800LL)
                         >> 12);
        step -= 0x88;
    }
    speed = state[9];
    if (speed > 0x800) {
        speed = 0x800;
    } else if (speed < 0xc0) {
        speed = 0xc0;
    }
    state[9] = speed;
    ScaleVec3Fx12(speed, (VecFx32 *)(state + 5), (VecFx32 *)(state + 2));
    if (state[0xe] != 0) {
        state[0xb] += *(int *)(*node + 0x2c) * 0x15000 / 0x1000;
        if (state[0xb] > 0xffff) {
            state[0xe] = 0;
        }
    }
    if (state[0xe] == 0) {
        state[0xb] += *(int *)(*node + 0x2c) * 0x2a000 / 0x1000;
        state[0xb] = (unsigned short)state[0xb];
        step = *(int *)(*node + 0x2c);
        while (step > 0) {
            state[0xc] = (int)(((long long)state[0xc]
                                    * (0x1000 - (int)(((long long)FX_Div(step <= 0x88 ? step : 0x88, 0x88) * 0x100LL + 0x800LL) >> 12))
                                + 0x800LL)
                               >> 12);
            step -= 0x88;
        }
    }
    weave.x = state[0xc] * data_0203d210[(state[0xb] >> 4) * 2] / 0x1000;
    weave.y = 0;
    weave.z = 0;
    MTX_RotY33_(&mtx, data_0203d210[ANG2IDX(state[0xa]) * 2], data_0203d210[ANG2IDX(state[0xa]) * 2 + 1]);
    MTX_MultVec33(&weave, &mtx, &weave);
    VEC_Add((VecFx32 *)(state + 2), &weave, (VecFx32 *)(state + 2));
    if (Ov228_ScanNearbyEntitiesReact(state) != 0) {
        item = *state;
        func_ov107_020c0b90(*(int *)(item + 0x38c), 8, *(VecFx32 *)(item + 0x74), 0);
        Ov107_BuildAndSendUpdate(state[0], 0x12b, 0xe, (void *)state[1]);
    } else if (((struct Bits17a *)(*state + 0x17a))->b1 != 0) {
        Ov107_BuildAndSendUpdate(*state, 0x12b, 0xe, (void *)state[1]);
        *(VecFx32 *)(state + 2) = data_02041dc8;
    } else {
        state[8] += VEC_Mag((VecFx32 *)(state + 2));
        if (state[8] < 0x15000) {
            return;
        }
    }
    item = *state;
    func_ov107_020c0b90(item, 0, *(VecFx32 *)(item + 0x74), 1);
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    state[8] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov228_AiScanReactTick);
}
