/*
 * MsgQueue_Post - take a free send slot, copy the command payload into it and
 * hand back the slot's freshly stamped handle.
 *
 * The send pool (base = *data_0204c230) holds its queue context at base+0xc:
 * a 32-bit free-slot bitmask (SET bit = free, bit 31 = slot 0), 32 six-byte
 * slot entries, 32 state blocks of 0x34 bytes each (four-byte header, then the
 * payload area) and, immediately after the last state block, the running u16
 * sequence counter that supplies every handle. Same object as
 * MsgQueue_ResendForPeer.c/MsgQueue_Contains.c/MsgQueue_MarkSlotReceived.c.
 *
 * Returns 0xffff when no slot is free. Otherwise the highest-order free slot is
 * taken, its type and payload size recorded, nSize bytes of pCmd copied into its
 * state block, and the counter advanced -- wrapping back to 0 before it could
 * ever hand out the 0xffff "no slot" sentinel as a real handle.
 *
 * The clz leading-zero count has no mwcc 3.0 intrinsic (every spelling lowers to
 * an external call), so it is the project's one-line inline-asm helper, exactly
 * as in the sibling MsgQueue_Contains.c.
 */

#include "nitro/mi.h"

static inline unsigned int Clz(unsigned int x) {
    #ifdef SDK_BUILD_ARM
     asm { clz x, x } return x; 
    #else
    //TODO
    return 0;
    #endif
    }

extern char *data_0204c230;

typedef struct {
    unsigned short handle;
    unsigned short size;
    unsigned char type;
    unsigned char peer;
} MsgSlot;

typedef struct {
    unsigned int field_00;
    unsigned char payload[0x30];
} MsgState;

typedef struct {
    unsigned int freeMask;
    MsgSlot slots[32];
    MsgState states[32];
    unsigned short seq;
} MsgCtx;

unsigned short MsgQueue_Post(int nKind, const void *pCmd, unsigned short nSize)
{
    MsgCtx *ctx = (MsgCtx *)(data_0204c230 + 0xc);
    MsgSlot *slot = 0;
    MsgState *state;
    unsigned int freeMask = ctx->freeMask;
    int idx = Clz(freeMask);
    unsigned short handle;

    if (idx == 0x20) {
        return 0xffff;
    }

    ctx->freeMask = freeMask & ~(0x80000000U >> idx);
    slot = &ctx->slots[idx];
    slot->size = nSize + 4;
    slot->type = (unsigned char)nKind;

    state = &ctx->states[idx];
    handle = ctx->seq;
    ctx->seq = handle + 1;
    slot->handle = handle;
    slot->peer = 0;
    MI_CpuCopy8(pCmd, state->payload, nSize);

    if (ctx->seq >= 0xffff) {
        ctx->seq = 0;
    }
    return slot->handle;
}
