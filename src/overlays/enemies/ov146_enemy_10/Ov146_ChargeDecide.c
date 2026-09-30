/* Charge decision of the ov146 actor: it looks for a live, unshielded entity of the scene list (other
 * than itself and its partner) within 48.0. Without one bit 0 of +0x1ae clears, the next move is 2 and
 * the node ends. With one the charge timer (+0x3c) and stage (+0x50) reset, both play pose 3, the
 * partner is grabbed (020ce2b4), effect 4 plays at the actor, sound 0x125/4 at its +0xc point and the
 * node moves on to 020cdaa4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { u16 lo : 8; u16 hi : 8; } flags16;

extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Ov146_ForwardToAiTaskWhenReady(int partner);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void Ov146_ChargeTick(void);

void Ov146_ChargeDecide(int *node)
{
    int *state = (int *)node[1];
    int found = 0;
    int grid = *(int *)(*state + 4);
    int *it;
    char *e;
    VecFx32 d;

    it = List_First((void *)(grid + 0x80));
    e = it == 0 ? 0 : (char *)*it;
    while (e != 0) {
        if (e != (char *)*state && e != (char *)state[2] && (((flags16 *)(e + 0x60))->lo & 1) &&
            !(*(u16 *)(e + 0x1ac) & 6)) {
            VEC_Subtract((VecFx32 *)(*state + 0x74), (VecFx32 *)(e + 0x74), &d);
            if (VEC_Normalize(&d, &d) <= 0x30000) {
                found = 1;
                break;
            }
        }
        it = List_Next((void *)(grid + 0x80));
        e = it == 0 ? 0 : (char *)*it;
    }
    if (found == 0) {
        *(u16 *)(*state + 0x1ae) &= ~1;
        *(u8 *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0xf] = 0;
    *((u8 *)state + 0x50) = 0;
    Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
    Ov107_PostTagUpdate((Actor *)state[2], 3, 0);
    Ov146_ForwardToAiTaskWhenReady(state[2]);
    func_ov107_020c0b90(*state, 4, *(VecFx32 *)(*state + 0x74), 0);
    Ov107_BuildAndSendUpdate(*state, 0x125, 4, (void *)state[3]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov146_ChargeTick);
}
