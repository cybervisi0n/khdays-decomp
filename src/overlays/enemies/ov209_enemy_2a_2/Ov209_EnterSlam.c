/* Slam entry of the ov208 enemy (x3 with ov209/ov268): sends the owner command 3 with the zero
 * vector, fires reaction 0x154 mode 8 at the +0x3c4 part's +4 point, clears the +0x2c timer and
 * the +0x49 byte and hands the tick over to Ov209_AiDriveWindup. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov209_AiDriveWindup(int *node);

void Ov209_EnterSlam(int *node)
{
    int *state = (int *)node[1];

    func_ov107_020c0b90(*state, 3, data_02041dc8, 0);
    Ov107_BuildAndSendUpdate(*state, 0x154, 8, (void *)(*(int *)(*state + 0x3c4) + 4));
    state[0xb] = 0;
    *(unsigned char *)((char *)state + 0x49) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov209_AiDriveWindup);
}
