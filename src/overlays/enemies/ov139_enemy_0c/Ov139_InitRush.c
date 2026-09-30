/* AI step: posts pose 9, aims the rush at the target (or along its stored direction), sends the
 * rush update (0x11f, mode 8) and continues with the rush tick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Ov139RushState {
    void *pOwner;
    char pad004[0x1c];
    VecFx32 vInitialDirection;
    VecFx32 vDirection;
    int nSpeed;
    int nTimer;
    char pad040[4];
    void *pTarget;
    VecFx32 *pPosition;
    VecFx32 *pEffectPosition;
    u8 pad050[4];
    u8 nPhase;
};

extern void VEC_Subtract(void *a, void *b, void *out);
extern int VEC_Normalize(void *source, void *dest);
extern void Ov107_BuildAndSendUpdate(void *actor, int reaction, int mode,
                                VecFx32 *position);
extern void SetIndexedSlot(int *node, int slot, void *callback);
extern const VecFx32 data_02042258;
extern void Ov139_RushTick(void);

void Ov139_InitRush(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)*state, 9, 0);
    if (state[0x11] != 0) {
        VEC_Subtract((void *)state[0x12],
                     (void *)(state[0x11] + 0x190),
                     state + 0xb);
        state[0xc] = 0;
    } else {
        *(VecFx32 *)(state + 0xb) = *(VecFx32 *)(state + 8);
        state[0xc] = 0;
    }
    if (VEC_Normalize(state + 0xb, state + 0xb) == 0) {
        *(VecFx32 *)(state + 0xb) = data_02042258;
    }
    state[0xe] = 0x800;
    *(u8 *)((char *)state + 0x54) = 0;
    state[0xf] = 0;
    Ov107_BuildAndSendUpdate((void *)*state, 0x11f, 8, (VecFx32 *)state[0x13]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20),
                  Ov139_RushTick);
}
