/* Grab-slot flight of the ov261 enemy (and its byte-identical twin): flies from the +4 position
 * towards the +0x2c point of the +0x48 grab slot. On the first tick a direction within 0xc00 of
 * vertical replaces the +0x74 axis with the up vector and the direction with the forward one
 * (unit length 0x800). After 0x2000 of accumulated frame-time the +0x1c anchor faces the point,
 * the list's first entry is released (slot +0x3ad, not carried), the +0x2c word points at the
 * slot's point and the state ends with sub-state 2. Otherwise the +0x30 velocity is 0x800 along
 * the axis-cross-direction, blended back towards the direction beyond 0x1800 and towards its
 * reverse under 0x800 of distance; the +0x3c rate is the frame-time * 30 / 5 and the anchor
 * faces the velocity. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void Ov262_SetFacingAnchor(void *anchor, VecFx32 *dir, const VecFx32 *pos);
extern void Ov015_SpotArrive(int piece, int slot, int carried, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void VEC_CrossProduct(void *a, void *b, void *d);
extern int FX_Div(int a, int b);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void VEC_Add(void *a, void *b, void *d);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;

void Ov262_GrabSlotFlight(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 side;
    int slot;
    int len;
    int t;

    slot = *(int *)(*state + 0x3a0) + 0x18 + *(u8 *)(state + 0x12) * 0x24;
    VEC_Subtract((void *)(slot + 0x14), (void *)state[1], &dir);
    len = VEC_Normalize(&dir, &dir);
    if (state[0x10] == 0) {
        t = VEC_DotProduct(&dir, &data_02042264);
        if (t < 0) {
            t = -t;
        }
        if (t > 0xc00) {
            *(VecFx32 *)(state + 0x1d) = data_02042264;
            dir = data_02042258;
            len = 0x800;
        }
    }
    state[0x10] += *(int *)(*node + 0x2c);
    if (state[0x10] >= 0x2000) {
        Ov262_SetFacingAnchor(state + 7, &dir, &data_02041dc8);
        Ov015_SpotArrive(*(int *)(*(int *)(*state + 0x3a0)), *(u8 *)(*state + 0x3ad), 0, 0);
        state[0xb] = slot + 0x14;
        *(u8 *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    VEC_CrossProduct(state + 0x1d, &dir, &side);
    if (len >= 0x1800) {
        t = FX_Div(len - 0x1800, 0x800);
        if (t > 0x1000) {
            t = 0x1000;
        } else if (t < 0) {
            t = 0;
        }
        ScaleVec3Fx12(0x1000 - t, &side, &side);
        ScaleVec3Fx12(t, &dir, &dir);
        VEC_Add(&side, &dir, &side);
    } else if (len < 0x800) {
        t = FX_Div(0x800 - len, 0x800);
        if (t > 0x1000) {
            t = 0x1000;
        } else if (t < 0) {
            t = 0;
        }
        ScaleVec3Fx12(-0x1000, &dir, &dir);
        ScaleVec3Fx12(0x1000 - t, &side, &side);
        ScaleVec3Fx12(t, &dir, &dir);
        VEC_Add(&side, &dir, &side);
    }
    ScaleVec3Fx12(0x800, &side, state + 0xc);
    state[0xf] = *(int *)(*node + 0x2c) * 30 / 5;
    Ov262_SetFacingAnchor(state + 7, &side, &data_02041dc8);
}
