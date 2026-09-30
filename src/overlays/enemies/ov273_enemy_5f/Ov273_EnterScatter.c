/* Scatter entry: builds the 0x44-byte spread message from the data_ov273_020d6b24 template
 * (kind 5/4), fills its eight offsets with rand(0x6489) - 0x3244 and its eight heights with
 * rand(0x3001) + 0x1000, sends it through the actor's +0x24 hook (when set), plays pose 0x10,
 * spawns effect 8 at the zero vector unless the actor is being torn down (+0x1c4 & 0xa), and
 * moves the node to 020cf5fc. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct SpreadMsg { int w[17]; };
typedef void (*MsgHook)(int actor, struct SpreadMsg *m, int size);
extern void func_ov107_020c0b90(int obj, int mode, VecFx32 v, int flag);
extern void SetIndexedSlot(int self, int idx, int cb);
extern const struct SpreadMsg data_ov273_020d6b24;
extern VecFx32 data_02041dc8;
extern void Ov273_AiEnterFastSink(void);

void Ov273_EnterScatter(int *self) {
    int *state = (int *)self[1];
    struct SpreadMsg msg = data_ov273_020d6b24;
    int i;

    for (i = 0; i < 8; i++) {
        msg.w[1 + i] = RandNextScaled(0x6489) - 0x3244;
        msg.w[9 + i] = RandNextScaled(0x3001) + 0x1000;
    }
    {
        MsgHook hook = *(MsgHook *)(*state + 0x24);
        if (hook != 0) {
            hook(*state, &msg, 0x44);
        }
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x10, 0);
    {
        int actor = *state;
        if ((*(unsigned char *)(actor + 0x1c4) & 0xa) == 0) {
            func_ov107_020c0b90(actor, 8, data_02041dc8, 0);
        }
    }
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (int)&Ov273_AiEnterFastSink);
}
