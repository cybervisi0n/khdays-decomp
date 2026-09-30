/* Grab entry of the ov146 actor: the partner (+8) is grabbed (020ce2b4); both get bits 1 and 7 of their
 * +0x60 high byte and bit 0 of +0x1ae set, the partner's +0x3ac shape takes bit 1, the actor's bits 2-3
 * clear and its +0x3ac shape hides. A point 0.75 above the actor, turned by its +0xa0 rotation and
 * offset from the +0xc point, gets effect 0; sound 0/0x48 plays at the actor, +0x3c clears and the node
 * moves on to 020ccfe8. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned f : 8; } B8;

extern int Ov146_ForwardToAiTaskWhenReady(int partner);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov146_WakeTick(void);

void Ov146_GrabEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;

    Ov146_ForwardToAiTaskWhenReady(state[2]);
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x82) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(state[2] + 0x60);
        *(u16 *)(state[2] + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x82) << 0x18) >> 0x10);
    }
    *(u16 *)(*state + 0x1ae) |= 1;
    *(u16 *)(state[2] + 0x1ae) |= 1;
    ((B8 *)(*(int *)(state[2] + 0x3ac) + 8))->f |= 2;
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~0xc) << 0x18) >> 0x10);
    }
    ((B8 *)(*(int *)(*state + 0x3ac) + 8))->f &= ~1;
    at.x = 0;
    at.y = 0xc00;
    at.z = 0;
    Vec3TransformViaTempMtx(&at, (void *)(*state + 0xa0), &at);
    VEC_Add(&at, (VecFx32 *)state[3], &at);
    func_ov107_020c0b90(*state, 0, at, 0);
    Ov107_BuildAndSendUpdate(*state, 0, 0x48, (void *)(*state + 0x74));
    state[0xf] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov146_WakeTick);
}
