/* Ov253_ThrowRelease -- throw: unless the +0x3bc target's +0x18c rider carries flag 0x2000 (then
 * it is just detached, 020ad8e0), bits 7 and 1 of the target's +0x60 high byte clear, the +0x384
 * item's +0x296 power takes 35 % of the rider's +0x12 stat with +0x293 = 2 and +0x294 = 1, and
 * the target takes reaction 1 from the item with a 2.0 push along data_02042264 (020ca918); the
 * node then moves to 020d0884. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, const VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov022_ToggleBit13ByMode(int target, int a);
extern const VecFx32 data_02042264;
extern void Ov253_AiEnterDrop(void);

void Ov253_ThrowRelease(int *node) {
    int *state = (int *)node[1];
    int rider = *(int *)(*(int *)(*state + 0x3bc) + 0x18c);
    VecFx32 push;

    if ((*(unsigned long long *)rider & 0x2000ULL) == 0) {
        ScaleVec3Fx12(0x2000, &data_02042264, &push);
        {
            int target = *(int *)(*state + 0x3bc);
            u16 hw = *(u16 *)(target + 0x60);
            *(u16 *)(target + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0x80) << 0x18) >> 0x10);
        }
        {
            int target = *(int *)(*state + 0x3bc);
            u16 hw = *(u16 *)(target + 0x60);
            *(u16 *)(target + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~2) << 0x18) >> 0x10);
        }
        *(u16 *)(*(int *)(*state + 0x384) + 0x200 + 0x96) =
            *(u16 *)(*(int *)(*(int *)(*state + 0x3bc) + 0x18c) + 0x12) * 0x23 / 100;
        *(unsigned char *)(*(int *)(*state + 0x384) + 0x293) = 2;
        *(unsigned char *)(*(int *)(*state + 0x384) + 0x294) = 1;
        Ov107_InvokeHitCallback(*(int *)(*state + 0x3bc), *state, *(int *)(*state + 0x384), 1, &push, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov253_AiEnterDrop);
        return;
    }
    Ov022_ToggleBit13ByMode(rider, 0);
}
