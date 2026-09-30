/*
 * MsgQueue_ResendForPeer - re-queue every pending send slot owned by peer `param_1`.
 *
 * The send pool (base = *data_0204c230) keeps a 32-bit free-slot bitmask at +0xc
 * (SET bit = free, bit 31 = slot 0), 6-byte slot entries at +0x10 (u16 handle,
 * u16 size, u8 type, u8 peer) and a per-slot state block at +0xd0, stride 0x34.
 *
 * We scan the OCCUPIED slots (complement of the free mask) top-bit-down with clz,
 * clearing each visited bit. For each occupied slot whose peer byte maps
 * (IsKind1Or4) to the target peer id, we re-stamp the slot's state byte -
 * a packed bitfield: bits0-1 = 1 (queued), bits2-4 = local player index
 * (Session_GetLocalPlayerIndex), bits5-7 = 7 - copy the peer/handle into the state header and
 * re-send it via MsgQueue_SendGate (type 0xb).
 *
 * The clz leading-zero count has no mwcc 3.0 intrinsic (every spelling lowers to
 * an external call), so it is written as the project's one-line inline-asm helper.
 */

#include "game/engine.h"

static inline unsigned int Clz(unsigned int x) { 
    #ifdef SDK_BUILD_ARM
    asm { clz x, x } return x; 
    #else
    return 0;
    #endif
}

extern int *data_0204c230;

typedef struct {
    unsigned char state:2, lpi:3, top:3;
    unsigned char peer;
    unsigned short handle;
} SlotState;

void MsgQueue_ResendForPeer(int param_1)
{
    int *base = data_0204c230;
    unsigned char *states;
    unsigned char *entries;
    unsigned char *entry;
    unsigned int occupied;
    int idx;
    SlotState *state;

    if (*(unsigned int *)((char *)base + 0xc) == 0xffffffff) return;
    occupied = ~*(unsigned int *)((char *)base + 0xc);
    idx = Clz(occupied);
    entries = (unsigned char *)base + 0x10;
    states = (unsigned char *)base + 0xd0;
    while (idx < 0x20) {
        entry = entries + idx * 6;
        if (param_1 != IsKind1Or4(entry[4])) {
            occupied = occupied & ~(0x80000000U >> idx);
            idx = Clz(occupied);
        } else {
            state = (SlotState *)(states + idx * 0x34);
            state->state = 1;
            state->lpi = (unsigned char)Session_GetLocalPlayerIndex();
            state->top = 7;
            state->peer = entry[4];
            state->handle = *(unsigned short *)entry;
            MsgQueue_SendGate(0xb, (unsigned char *)state, *(unsigned short *)(entry + 2));
            occupied = occupied & ~(0x80000000U >> idx);
            idx = Clz(occupied);
        }
    }
}
