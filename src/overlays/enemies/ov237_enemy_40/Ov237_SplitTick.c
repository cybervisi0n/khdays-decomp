/* Split tick of the ov237 actor: the +0x30 clock runs up at the frame rate; a single +0x55 hold drops,
 * setting flag 0 of the +0x488 rig. Alone, from 0x88 to 0x7f8 the +0x3c aim follows the partner rig at
 * data_ov237_020d1bdc; linked, from 0x2398 to 0x2b90 at data_ov237_020d1b40. From 0x2398 a linked pair
 * with the two-handed hold splits: effect 0x12 and the split sound (0x12d variant 0x10) play and the
 * partner (+0x4a4) appears at the +0x38 point - each side at 0 health takes the other's (clamped), the
 * actor's +0x494 grab time is a fifth of its maximum and is shared, the partner is marked busy (+0x4b0),
 * the rig flags swap, the partner's +0x60 high byte gets bit 0 and loses bits 1, 2, 6 and 7, its
 * +0x1ae bit 0 clears and it enters move 10. Once the +4 rig is idle bit 1 of the +0x60 high byte
 * clears and the next move is 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;

extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, int at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_ov237_020d1b40;
extern const VecFx32 data_ov237_020d1bdc;

/* The partner link at +0x4a4 is read through the typed member; read as a raw int at an offset
 * the statement block after 020c5c54 is scheduled differently (the constant 10 sinks). */
struct Ov237Actor { char pad[0x4a4]; int partner; };

void Ov237_SplitTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 aimAlone;
    VecFx32 aimLinked;
    VecFx32 pos;

    state[0xc] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x55) == 1) {
        (*((u8 *)state + 0x55))--;
        ((B8 *)(*(int *)(*state + 0x488) + 8))->f |= 1;
    }
    if (state[0xc] >= 0x88 && state[0xc] < 0x7f8 && *(int *)(*state + 0x4ac) == 0) {
        aimAlone = data_ov237_020d1bdc;
        *(VecFx32 *)(state + 0xf) = Ov237_RotateByActorHeading(node, &aimAlone);
    }
    if (state[0xc] >= 0x2398 && state[0xc] < 0x2b90 && *(int *)(*state + 0x4ac) != 0) {
        aimLinked = data_ov237_020d1b40;
        *(VecFx32 *)(state + 0xf) = Ov237_RotateByActorHeading(node, &aimLinked);
    }
    if (state[0xc] >= 0x2398 && *((u8 *)state + 0x55) == 2 && *(int *)(*state + 0x4ac) != 0) {
        (*((u8 *)state + 0x55))--;
        func_ov107_020c0b90(*state, 0x12, *(VecFx32 *)state[0xe], 0);
        Ov107_BuildAndSendUpdate(*state, 0x12d, 0x10, state[0xe]);
        if (((struct Ov237Actor *)*state)->partner != 0) {
            pos = *(VecFx32 *)state[0xe];
            {
                char *actor = (char *)*state;

                if (*(short *)(actor + 0x21a) == 0) {
                    int hp = *(short *)(((struct Ov237Actor *)actor)->partner + 0x21a);

                    *(short *)(actor + 0x21a) = hp < 0 ? 0 : (hp > *(short *)(actor + 0x218) ? *(short *)(actor + 0x218) : hp);
                }
            }
            {
                char *actor = (char *)*state;
                char *partner = (char *)((struct Ov237Actor *)actor)->partner;

                if (*(short *)(partner + 0x21a) == 0) {
                    int hp = *(short *)(actor + 0x21a);

                    *(short *)(partner + 0x21a) = hp < 0 ? 0 : (hp > *(short *)(partner + 0x218) ? *(short *)(partner + 0x218) : hp);
                }
            }
            *(int *)(*state + 0x494) = *(short *)(*state + 0x218) / 5;
            *(int *)(((struct Ov237Actor *)*state)->partner + 0x4b0) = 1;
            *(int *)(((struct Ov237Actor *)*state)->partner + 0x494) = *(int *)(*state + 0x494);
            Ov107_MoveNodeAndRelayout((Actor *)(((struct Ov237Actor *)*state)->partner), &pos);
            ((B8 *)(*(int *)(*state + 0x488) + 8))->f &= ~1;
            {
                u16 hw = *(u16 *)(((struct Ov237Actor *)*state)->partner + 0x60);

                *(u16 *)(((struct Ov237Actor *)*state)->partner + 0x60) = (hw & ~0xff00) |
                    ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
            }
            {
                u16 hw = *(u16 *)(((struct Ov237Actor *)*state)->partner + 0x60);

                *(u16 *)(((struct Ov237Actor *)*state)->partner + 0x60) = (hw & ~0xff00) |
                    (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0xc6) << 0x18) >> 0x10);
            }
            *(u16 *)(((struct Ov237Actor *)*state)->partner + 0x1ae) &= ~1;
            ((B8 *)(*(int *)(((struct Ov237Actor *)*state)->partner + 0x488) + 8))->f |= 1;
            *(signed char *)(((struct Ov237Actor *)*state)->partner + 0x1c6) = 10;
            *(signed char *)(((struct Ov237Actor *)*state)->partner + 0x1c7) = 10;
        }
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);

        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~2) << 0x18) >> 0x10);
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
