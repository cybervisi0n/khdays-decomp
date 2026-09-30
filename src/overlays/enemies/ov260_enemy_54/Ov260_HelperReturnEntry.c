/* Return entry of an ov260 helper: the flat distance from its +0x18 anchor to the owner's +0x3a4
 * aim sets the +0x44 flight speed ((dist - 5.0) x 0.066, at least 0.3125), the +8 facing turns from
 * the rest axis to the owner's +0x3b0 aim, the owner's +0x38c link clears, bit 0 of its +0x60 high
 * byte is set and bits 2 and 7 drop, the +0x388 shape shows, the +0x48 flag and +0x40 clear, the
 * +0x28 velocity rests, +0x1c starts at the +0x390 part's position and the node moves on to 020d13ac. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int x, y, z, w; } Quat;
typedef struct { unsigned f : 8; } B8;

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_HelperFlightTick(void);
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;

void Ov260_HelperReturnEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;

    VEC_Subtract((VecFx32 *)(*state + 0x3a4), (VecFx32 *)state[6], &d);
    d.y = 0;
    {
        int over = VEC_Normalize(&d, &d) - 0x5000;

        state[0x11] = over * 0x88 / 0x800;
    }
    if (state[0x11] < 0x500) {
        state[0x11] = 0x500;
    }
    Quat_FromTwoVectors((Quat *)(state + 2), &data_02042258, (VecFx32 *)(*state + 0x3b0));
    *(int *)(*state + 0x38c) = 0;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~0x84) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x388) + 8))->f |= 1;
    *((u8 *)state + 0x48) = 0;
    state[0x10] = 0;
    *(VecFx32 *)(state + 10) = data_02041dc8;
    *(VecFx32 *)(state + 7) = *(VecFx32 *)(*(int *)(*state + 0x390) + 0x74);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_HelperFlightTick);
}
