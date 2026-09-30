/* Dive tick of the ov125 enemy: reseeds the +0x3c counter with 3 times the owner's rate
 * (30/10) and acquires the target with its squared distance -- none sends it to sub-state 2.
 * The gap (root distance less both radii) rebuilds the look-at at +0x68 and sets the blend
 * t = (4.0 - gap) / 4.0 clamped to +-1.0; the step at +8 is the look-at's forward (turned by
 * data_02042258) scaled by -t plus the sideways component (up x forward, up being the +0x78
 * orbit sense) scaled by 1.0 - |t|, both at 0x280, with a fixed -0x180 fall speed. While the
 * +0x54 count is spent a 0x65 roll re-arms it to a random value between the actor's +0x224 and
 * +0x228 and picks sub-state 6 (under 20 with an idle aim node), 7 (under 80) or 5; otherwise
 * the 020cd27c state takes over once the +0x13c height drops under 0x2000.
 * `+ (v - v)` is the documented copy artifact of RandNextScaled (`add r4,r0,#0`). */

#include "nitro/fx_types.h"

static inline void VEC_Set(VecFx32 *v, int x, int y, int z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

extern int Ov107_FindNearestObject(int obj, int *pSqDist);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern int FX_Sqrt(int a);
extern void Mtx33_LookAt(void *out, void *a, int b, void *c);
extern void Quat_FromMtx33(void *a, void *b);
extern int FX_Div(int num, int den);
extern void Vec3TransformViaTempMtx(void *out, void *pose, void *k);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int RandNextScaled(int bound);
extern int Ov126_IsField34Nibble1(int node);
extern int data_02042264;
extern int data_02042258;
extern void Ov126_ChooseMove(void);

void Ov126_DiveTick(int *node)
{
    int owner;
    int *state = (int *)node[1];
    int buf[9];
    VecFx32 up;
    VecFx32 sideN;
    VecFx32 side;
    VecFx32 fwd;
    int sq;
    int actor;
    int target;
    int t;
    int v;
    int lo;
    int diff;

    owner = *state;
    state[0xf] = *(int *)(node[0] + 0x2c) * 30 / 10;
    target = state[1] = Ov107_FindNearestObject(*state, &sq);
    if (target == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
        return;
    }
    actor = *state;
    sq = FX_Sqrt(sq) - *(int *)(target + 0x80) - *(int *)(actor + 0x80);
    Mtx33_LookAt(buf, (void *)(state[1] + 0x74), state[9], &data_02042264);
    Quat_FromMtx33(state + 0x1a, buf);
    t = FX_Div(0x4000 - sq, 0x4000);
    if (t < -0x1000) t = -0x1000;
    if (t > 0x1000) t = 0x1000;
    Vec3TransformViaTempMtx(&fwd, state + 0x1a, &data_02042258);
    VEC_Set(&up, 0, state[0x1e] << 12, 0);
    VEC_CrossProduct(&up, &fwd, &sideN);
    ScaleVec3Fx12(0x1000 - (t < 0 ? -t : t), &sideN, &sideN);
    ScaleVec3Fx12(-t, &fwd, &side);
    ScaleVec3Fx12(0x280, &sideN, &sideN);
    ScaleVec3Fx12(0x280, &side, &side);
    VEC_Add(&sideN, &side, (VecFx32 *)(state + 2));
    state[3] = -0x180;
    if (state[0x15] <= 0) {
        int r = RandNextScaled(0x65) + (v - v);
        lo = *(int *)(*state + 0x224);
        diff = *(int *)(*state + 0x228) - lo;
        if (diff < 0) {
            diff = -diff;
        }
        state[0x15] = lo + RandNextScaled(diff + 1);
        if (r < 0x14 && Ov126_IsField34Nibble1(*(int *)(*state + 0x390)) == 0) {
            *(unsigned char *)(*state + 0x1c7) = 6;
            SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
            return;
        }
        if (r < 0x50) {
            *(unsigned char *)(*state + 0x1c7) = 7;
            SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
            return;
        }
        *(unsigned char *)(*state + 0x1c7) = 5;
        SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), 0);
        return;
    }
    if (*(int *)(owner + 0x13c) >= 0x2000) {
        return;
    }
    SetIndexedSlot((int)node, *(signed char *)((int)node + 0x20), &Ov126_ChooseMove);
}
