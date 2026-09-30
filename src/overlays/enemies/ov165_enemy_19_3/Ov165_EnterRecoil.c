/* Enter the recoil state, unless the gate byte at state[0x16] says otherwise: play the
 * canned 4-byte event with the object's own id patched into its first halfword, put the
 * object into animation 7, clear bit 0 of the flags at +0x3cc, reset the stored direction
 * at state[9] to the zero vector, and clear the two counters before re-registering the
 * think callback.
 *
 * The 4-byte event buffer is a STRUCT ASSIGNMENT from the template followed by an
 * overwrite of its first halfword; that is what makes mwcc emit the two loads before the
 * two stores, in descending address order, as the ROM does. Written field by field the
 * loads pair up with their stores and the order flips. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void SetIndexedSlot(void *node, int idx, void *cb);
extern void Ov165_TickRecoil(void);
extern unsigned short data_ov165_020d4aa8[];
extern int data_02041dc8[];

typedef struct { unsigned short a, b; } Ov163Pair;

void Ov165_EnterRecoil(int *node) {
    int *state = (int *)node[1];
    Ov163Pair buf;

    if (*(unsigned char *)state[0x16] != 0) {
        return;
    }
    buf = *(Ov163Pair *)(data_ov165_020d4aa8 + 2);
    buf.a = *(unsigned short *)(state[0] + 2);
    MsgQueue_Post(4, &buf, 4);
    Ov107_PostTagUpdate((Actor *)state[0], 7, 1);
    *(int *)(state[0] + 0x3cc) &= ~1;
    *(VecFx32 *)(state + 9) = *(VecFx32 *)data_02041dc8;
    state[0x1a] = 0;
    state[0xc] = 0;
    SetIndexedSlot(node, *(signed char *)((int)node + 0x20), Ov165_TickRecoil);
}
