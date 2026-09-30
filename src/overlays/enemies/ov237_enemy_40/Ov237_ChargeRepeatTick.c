/* Charge-repeat tick of the ov237 actor: the charge release check runs (020cf2b0); each time the
 * +4 rig finishes, a remaining charge step (+0x34) replays the charge sound (0x12d variant 10) with
 * pose 0x17 and effect 0xb, and the last one clears the +0x30 clock, plays pose 0x18 with effect 0xc
 * (both effects skipped in +0x49e mode 3) and waits on 020cf784. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov237_ChargeRelease(int *node);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, int at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_SweepTick(void);

void Ov237_ChargeRepeatTick(int *node)
{
    int *state = (int *)node[1];

    Ov237_ChargeRelease(node);
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0xd] == 0) {
        state[0xc] = 0;
        Ov107_PostTagUpdate((Actor *)(*state), 0x18, 0);
        if (*(u8 *)(*state + 0x49e) != 3) {
            func_ov107_020c0b90(*state, 0xc, *(VecFx32 *)state[0xe], 0);
        }
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_SweepTick);
        return;
    }
    state[0xd]--;
    Ov107_BuildAndSendUpdate(*state, 0x12d, 10, state[0xe]);
    Ov107_PostTagUpdate((Actor *)(*state), 0x17, 0);
    if (*(u8 *)(*state + 0x49e) == 3) {
        return;
    }
    func_ov107_020c0b90(*state, 0xb, *(VecFx32 *)state[0xe], 0);
}
