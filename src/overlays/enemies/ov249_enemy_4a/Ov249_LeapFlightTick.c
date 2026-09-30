/* Flight tick of the ov249 enemy's leap: within 3.0 of travel (+0x24) the +0xc velocity is the +0x18
 * direction at 0.5; later, while the +0x28 progress has not passed the +0x2c range, the direction is
 * swung by up to 1.22 rad along a cosine of the progress and the progress advances by the swung
 * forward step (clamped to the range while short of it). The owner's +0x74 sphere is swept (kind 3,
 * pushed away from the owner on the ground plane): on any hit effect 8 spawns on the +0x38c partner,
 * effect 0 at the owner with reaction 0x145 mode 0xe; hitting a wall along the velocity or landing
 * (+0x17a bit 1) ends with effect 0 and mode 0x11; else the travel grows by the speed and past 21.0
 * effect 0 ends it. Every end clears the travel and hands over to 020d48a4. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int radius; } Sphere;
typedef struct { int m[9]; } Mtx33;
struct Bits17a { unsigned char b0 : 1, b1 : 1; };

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *out);
extern int func_02020400(int a, int b);
extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern int FX_Div(int a, int b);
extern int Ov107_CollectSphereOverlaps(int actor, Sphere *sphere, int *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Collision_CastRay(int collision, VecFx32 *start, VecFx32 *ray);
extern int VEC_Mag(const VecFx32 *v);
extern void Ov249_AiTimerQueueAction0(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov249_LeapFlightTick(int *node)
{
    int *state = (int *)node[1];
    Sphere sphere;
    int hits[4];
    Mtx33 mtx;
    VecFx32 local;
    VecFx32 dir;
    int item;
    int world;
    long i;
    long n;
    int hitAny = 0;

    world = *(int *)(*state + 4);
    if (state[9] <= 0x3000) {
        ScaleVec3Fx12(0x800, (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
    } else {
        int range = state[0xb];
        int progress = state[0xa];

        if (progress <= range && range != 0) {
            ScaleVec3Fx12(0x800, (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
            {
                int idx = ANG2IDX(data_0203d210[(func_02020400(progress << 15, range) >> 4) * 2 + 1] * 0x138c / 0x1000);

                MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
            }
            MTX_MultVec33((VecFx32 *)(state + 3), &mtx, (VecFx32 *)(state + 3));
            local.x = 0;
            local.y = 0;
            local.z = 0x800;
            MTX_MultVec33(&local, &mtx, &local);
            ScaleVec3Fx12(FX_Div(*(int *)(node[0] + 0x2c), 0x88), &local, &local);
            if (state[0xa] < state[0xb]) {
                state[0xa] += local.z;
                if (state[0xa] > state[0xb]) {
                    state[0xa] = state[0xb];
                }
            } else {
                state[0xa] += local.z;
            }
        }
    }
    sphere = *(Sphere *)(*state + 0x74);
    n = Ov107_CollectSphereOverlaps(*(int *)(*state + 0x38c), &sphere, hits);
    for (i = 0; i < n; i++) {
        VEC_Subtract((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)(*state + 0x74), &dir);
        dir.y = 0;
        VEC_Normalize(&dir, &dir);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x38c), 3, &dir, 0) != 0) {
            hitAny = 1;
        }
    }
    if (hitAny) {
        func_ov107_020c0b90(*(int *)(*state + 0x38c), 8, *(VecFx32 *)(*state + 0x74), 0);
        func_ov107_020c0b90(*state, 0, *(VecFx32 *)(*state + 0x74), 1);
        Ov107_BuildAndSendUpdate(*state, 0x145, 0xe, (void *)state[2]);
        state[9] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov249_AiTimerQueueAction0);
        return;
    }
    if (Collision_CastRay(*(int *)(world + 0x7c), (VecFx32 *)state[2], (VecFx32 *)(state + 3)) != 0) {
        item = *state;
        func_ov107_020c0b90(item, 0, *(VecFx32 *)(item + 0x74), 1);
        Ov107_BuildAndSendUpdate(*state, 0x145, 0x11, (void *)state[2]);
        state[9] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov249_AiTimerQueueAction0);
        return;
    }
    item = *state;
    if (((struct Bits17a *)(item + 0x17a))->b1 != 0) {
        func_ov107_020c0b90(item, 0, *(VecFx32 *)(item + 0x74), 1);
        Ov107_BuildAndSendUpdate(*state, 0x145, 0x11, (void *)state[2]);
        state[9] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov249_AiTimerQueueAction0);
        return;
    }
    state[9] += VEC_Mag((VecFx32 *)(state + 3));
    if (state[9] < 0x15000) {
        return;
    }
    item = *state;
    func_ov107_020c0b90(item, 0, *(VecFx32 *)(item + 0x74), 1);
    state[9] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov249_AiTimerQueueAction0);
}
