/* Ov245_GrabCheck -- grab check: raises bit 1 of the actor's +0x60 high byte and, while a
 * +0x3b4 owner exists whose +0x3b4 target has bit 0 of its +8 low byte set, offers the owner a
 * hit packet (flags 0x2004, the owner's +0x2a2 strength, the actor's +0x258 kind, the target)
 * through 020c5cfc; when it lands, effect 0 plays at the +0x3b0 item's +0x14 offset, reaction
 * 0/0x53 fires at the state's +0xc anchor, bit 0 of +0x1ae is raised, pose 3 is set and the
 * node moves to 020d47e0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct w8 { unsigned int lo : 8, rest : 24; };

struct HitPacket {
    u32 flagsLo : 16;
    u32 flagsHi : 16;
    VecFx32 normal;
    int field_10 : 16;
    int field_12 : 16;
    int field_14 : 16;
    int field_16 : 16;
    void *field_18;
    signed char field_1c;
    u8 pad01d[3];
    int field_20;
    u32 flags24Lo : 16;
    u32 flags24Hi : 16;
    int field_28;
};

extern int Ov107_AiState_ApplyHit(int other, int source, struct HitPacket *packet);
extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_FourShape_AiStep_QueueAction0OnAnimEndB(void);

void Ov245_GrabCheck(int *node) {
    int *state = (int *)node[1];
    int owner;
    int target;

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 2) << 0x18) >> 0x10);
    }
    owner = *(int *)(*state + 0x3b4);
    if (owner != 0) {
        struct HitPacket packet = {0};
        target = *(int *)(owner + 0x3b4);
        packet.flagsLo = 0x2004;
        packet.field_10 = *(u16 *)(*(int *)(*state + 0x3b4) + 0x200 + 0xa2);
        packet.field_14 = *(int *)(*state + 0x258);
        packet.field_18 = (void *)target;
        if ((((struct w8 *)(target + 8))->lo & 1) == 0) {
            return;
        }
        if (Ov107_AiState_ApplyHit(owner, 0, &packet) == 0) {
            return;
        }
        func_ov107_020c0b90(*(int *)(*state + 0x3b4), 0, *(VecFx32 *)(*(int *)(*state + 0x3b0) + 0x14), 0);
        Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[3]);
        *(u16 *)(*state + 0x100 + 0xae) |= 1;
        Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_FourShape_AiStep_QueueAction0OnAnimEndB);
    }
}
