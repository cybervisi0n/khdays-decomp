/* Approach tick of the ov279 enemy (x2 with ov119). Gives up (sub-state 2, tick cleared) once
 * the +8 target leaves the owner's scene, loses its +0x40 bit 1 or has bit 0 of its +0x60 low
 * byte clear. Otherwise the flattened unit direction from the +0x4c point to the target's +0x74
 * is scaled into the +0x30 step by the gap between the two collision radii (0x80 when closer than
 * 2.0, at most 0x300), the +0x34 climb follows the height difference clamped to +/-0x80 and the
 * +0x1c facing aims at the target. Within 2.0 effect 3 spawns at the origin, animation 7 plays,
 * +0x70/+0x50 clear, the facing is committed to +0xc and the tick hands over to
 * Ov279_SwipeHitWindow. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[4]; } Quat;
struct Bits40 { int b0 : 1, b1 : 1; };
struct hw60 { unsigned short lo : 8, hi : 8; };

extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(void *dst, const int *src);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;
extern void Ov279_SwipeHitWindow(int *node);

void Ov279_TickApproachTarget(int *node)
{
    int target;
    int *state = (int *)node[1];
    int owner;
    VecFx32 d;
    int mtx[9];
    int t;
    int o;
    int gap;
    int speed;

    owner = state[0];
    target = state[2];
    if (*(int *)(target + 4) != *(int *)(owner + 4) || ((struct Bits40 *)(target + 0x40))->b1 == 0
        || (((struct hw60 *)(target + 0x60))->lo & 1) == 0) {
        *(unsigned char *)(owner + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    VEC_Subtract((void *)(target + 0x74), (void *)state[0x13], &d);
    d.y = 0;
    t = state[2];
    o = state[0];
    gap = VEC_Normalize(&d, &d) - *(int *)(t + 0x80) - *(int *)(o + 0x80);
    speed = gap;
    if (gap < 0x2000) {
        speed = 0x80;
    }
    if (speed > 0x300) {
        speed = 0x300;
    }
    ScaleVec3Fx12(speed, &d, (VecFx32 *)(state + 0xc));
    state[0xd] = *(int *)(target + 0x78) - *(int *)(state[0x13] + 4);
    if (state[0xd] > 0x80) {
        state[0xd] = 0x80;
    }
    if (state[0xd] < -0x80) {
        state[0xd] = -0x80;
    }
    Mtx33_LookAt(mtx, (VecFx32 *)(target + 0x74), (VecFx32 *)state[0x13], &data_02042264);
    Quat_FromMtx33(state + 7, mtx);
    if (!(gap >= 0x2000)) {
        func_ov107_020c0b90(*state, 3, data_02041dc8, 0);
        Ov107_PostTagUpdate((Actor *)(*state), 7, 0);
        *((unsigned char *)state + 0x70) = 0;
        state[0x14] = 0;
        *(Quat *)(state + 3) = *(Quat *)(state + 7);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov279_SwipeHitWindow);
        return;
    }
}
