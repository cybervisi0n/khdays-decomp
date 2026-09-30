/* Rise decision of the ov210 enemy (x3 with ov211/ov282). With any of bits 0-3 of the owner's
 * +0x1c4 set and a target re-acquired into +0x10, the gap to it beyond both radii picks the next
 * sub-state: 0xf within 1.0, 0xa from 8.0, otherwise a d100 roll picks 0xa/0xd/0xc/0xb/9 at
 * 25/50/75/99, and the action ends. Otherwise it performs the rise entry: bits 2, 3 and 6 of the
 * +0x60 high byte and bit 0 of +0x1ae are raised, bit 0 of the +0x3b0 body's +8 low byte clears,
 * animation 6 plays, the +0x2c/+0x30 timers clear, the +4 point is kept at +0x34 and sent packed
 * in the overlay's 14-byte message (data_ov211_020d651c, flag 0) to the owner's +0x24 hook; the
 * +0x60 timer and +0x66 byte clear and the tick hands over to Ov211_RiseTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int value; } Fx32;
typedef struct { u16 h[7]; } Cmd14;

struct hw60 { unsigned short lo : 8, hi : 8; };
struct w8 { unsigned int lo : 8, rest : 24; };

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

extern int Ov107_FindNearestObject(int owner, int *out);
extern int FX_Sqrt(int v);

static inline int RandRange(int lo, int hi) { return (int)RandNextScaled(hi - lo + 1) + lo; }
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov211_020d651c;
extern void Ov211_RiseTick(int *node);

void Ov211_RiseDecision(int *node)
{
    int *state = (int *)node[1];
    Cmd14 msg;
    int dist;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    Fx32 *pPos;
    int owner;
    int target;
    int roll;
    u16 v;

    if ((*(u8 *)(*state + 0x1c4) & 0xf) != 0) {
        target = state[4] = Ov107_FindNearestObject(*state, &dist);
        if (target != 0) {
            owner = *state;
            dist = FX_Sqrt(dist) - (*(int *)(owner + 0x80) + *(int *)(target + 0x80));
            if (dist <= 0x1000) {
                *(u8 *)(*state + 0x1c7) = 0xf;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
            if (dist >= 0x8000) {
                *(u8 *)(*state + 0x1c7) = 0xa;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
            roll = RandRange(0, 100);
            if (roll < 0x19) {
                *(u8 *)(*state + 0x1c7) = 0xa;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
            if (roll < 0x32) {
                *(u8 *)(*state + 0x1c7) = 0xd;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
            if (roll < 0x4b) {
                *(u8 *)(*state + 0x1c7) = 0xc;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
            if (roll < 0x63) {
                *(u8 *)(*state + 0x1c7) = 0xb;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
            *(u8 *)(*state + 0x1c7) = 9;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    v = *(u16 *)(*state + 0x60);
    *(u16 *)(*state + 0x60) = (u16)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 0x4c) << 0x18) >> 0x10));
    *(u16 *)(*state + 0x1ae) |= 1;
    ((struct w8 *)(*(int *)(*state + 0x3b0) + 8))->lo &= ~1;
    Ov107_PostTagUpdate((Actor *)(*state), 6, 0);
    state[0xb] = 0;
    state[0xc] = 0;
    *(VecFx32 *)(state + 0xd) = *(VecFx32 *)state[1];
    msg = data_ov211_020d651c;
    pPos = (Fx32 *)state[1];
    PACK(msg, scratchX, pPos[0], 5);
    PACK(msg, scratchY, pPos[1], 8);
    PACK(msg, scratchZ, pPos[2], 11);
    ((u8 *)&msg)[4] = 0;
    if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
        (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
    }
    state[0x18] = 0;
    *(u8 *)((char *)state + 0x66) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov211_RiseTick);
}
