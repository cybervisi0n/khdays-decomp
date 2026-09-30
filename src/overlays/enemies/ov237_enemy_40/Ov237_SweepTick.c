/* Sweep tick of the ov237 actor: the +0x30 clock and the +0x1c hit clock run up at the frame rate
 * (the hit clock wrapping past 0x2a8 with the +0x57 mask cleared). Between 0x88 and 0x2398, below
 * +0x49e mode 3, the arm sweeps: the data_ov237_020d1b58 direction leans back, across and forward in
 * three phases (the +0x14 heading 1.57 left of, then right of, then on +0x18) and together with the
 * data_ov237_020d1b4c push is turned by the +0x10 heading; a 1.0 sphere 6.5 out from the +0x444 rig and
 * a box 3.5 out along the direction (0.27 / 0.27 / 3.5) hit once each (effect 1, kind 2) and the +0x55
 * hold drops. Once the +4 rig is idle +0x58 is set and the next move is 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;
typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 pos; int radius; } Sphere;
typedef struct { VecFx32 pos; VecFx32 axis[3]; int ext[3]; } Box;

extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov237_AttackHitTest(int *node, void *sphere, void *box, void *segment, VecFx32 *push, int once, unsigned short effect, int kind);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_ov237_020d1b4c;
extern const VecFx32 data_ov237_020d1b58;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042270;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov237_SweepTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 push;
    Mtx33 rot;
    Sphere sphere;
    Box box;
    VecFx32 reach;
    VecFx32 ax;
    VecFx32 ay;
    VecFx32 az;
    Quat q;

    state[0xc] += *(int *)(node[0] + 0x2c);
    state[7] += *(int *)(node[0] + 0x2c);
    if (state[7] >= 0x2a8) {
        state[7] = 0;
        *((u8 *)state + 0x57) = 0;
    }
    if (*(u8 *)(*state + 0x49e) < 3 && state[0xc] >= 0x88 && state[0xc] < 0x2398) {
        dir = data_ov237_020d1b58;
        push = data_ov237_020d1b4c;
        VEC_Normalize(&dir, &dir);
        if (state[0xc] < 0xd48) {
            state[5] = state[6] - 0x1922;
            dir.x = -(state[0xc] - 0x88) * 6 / 10;
        } else if (state[0xc] < 0xd48 * 2) {
            state[5] = state[6] + 0x1922;
            dir.x = state[0xc] - 0x1531;
        } else {
            state[5] = state[6];
            dir.x = 0x550 - (state[0xc] - 0x1a90) / 3;
        }
        VEC_Normalize(&dir, &dir);
        {
            int idx = ANG2IDX(state[4]) * 2;

            MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
        }
        MTX_MultVec33(&dir, &rot, &dir);
        MTX_MultVec33(&push, &rot, &push);
        ScaleVec3Fx12(0x6800, &dir, &sphere.pos);
        VEC_Add(&sphere.pos, (VecFx32 *)(*(int *)(*state + 0x444) + 0x14), &sphere.pos);
        sphere.radius = 0x1000;
        Ov237_AttackHitTest(node, &sphere, 0, 0, &push, 1, 1, 2);
        reach = dir;
        Quat_FromTwoVectors(&q, &data_02042258, &dir);
        Vec3TransformViaTempMtx(&ax, &q, &data_02042270);
        Vec3TransformViaTempMtx(&ay, &q, &data_02042264);
        Vec3TransformViaTempMtx(&az, &q, &data_02042258);
        ScaleVec3Fx12(0x3800, &reach, &reach);
        VEC_Add((VecFx32 *)(*(int *)(*state + 0x444) + 0x14), &reach, &box.pos);
        box.axis[0] = ax;
        box.axis[1] = ay;
        box.axis[2] = az;
        box.ext[0] = 0x440;
        box.ext[1] = 0x440;
        box.ext[2] = 0x3800;
        Ov237_AttackHitTest(node, 0, &box, 0, &push, 1, 1, 2);
        if (*((u8 *)state + 0x55) != 0) {
            *((u8 *)state + 0x55) = 0;
        }
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    state[0x16] = 1;
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
