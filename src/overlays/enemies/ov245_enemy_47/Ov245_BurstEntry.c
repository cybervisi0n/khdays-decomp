/* Ov245_BurstEntry -- burst entry: raises bit 4 of the actor's +0x1ae, sets pose 2, plays
 * effect 0 and reaction 0x15a/7 at the +8 anchor, then for each of the +0x394 owner's three
 * +0x43c parts still idle (bit 0 of +0x60 clear) launches it (020d5478, value 0) from the anchor
 * offset by half the actor's +0x70 scale along the direction 2*pi*i/3 (sine table); the node
 * moves to 020d1d98. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct Ov245Owner { char pad[0x43c]; int parts[3]; };

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void Ov245_InvokeHookAndRearm_3(int part, VecFx32 *pos, int value);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_AiStep_QueueAction0OnAnimEnd(void);
extern const short data_0203d210[];

static inline int FX_Mul(int v1, int v2)
{
    return (int)(((long long)v1 * v2 + 0x800LL) >> 12);
}

void Ov245_BurstEntry(int *node) {
    int *state = (int *)node[1];
    struct Ov245Owner *owner = (struct Ov245Owner *)*(int *)(*state + 0x394);
    int i;
    int idx;
    int angle;
    VecFx32 ofs;
    VecFx32 pos;
    *(unsigned short *)(*state + 0x100 + 0xae) |= 0x10;
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(*state, 0x15a, 7, (void *)state[2]);
    angle = 0;
    i = 0;
    do {
        if ((((struct hw60 *)(owner->parts[i] + 0x60))->lo & 1) == 0) {
            pos = *(VecFx32 *)state[2];
            idx = (unsigned short)((0x28BE60DB9391LL * (angle / 3) + 0x80000000000LL) >> 44);
            ofs.x = FX_Mul(data_0203d210[(idx >> 4) << 1], *(int *)(*state + 0x70) / 2);
            pos.x += ofs.x;
            ofs.z = FX_Mul(data_0203d210[((idx >> 4) << 1) + 1], *(int *)(*state + 0x70) / 2);
            pos.z += ofs.z;
            Ov245_InvokeHookAndRearm_3(owner->parts[i], &pos, 0);
        }
        angle += 0x6488;
    } while (++i < 3);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_AiStep_QueueAction0OnAnimEnd);
}
