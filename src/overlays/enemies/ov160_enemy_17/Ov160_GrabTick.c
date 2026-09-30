/* Grab tick of the ov160 enemy. With a held body (+0x2c) that carries flag bit 15 of its
 * +0x464 word pair, the body's +0x20 object is pulled to the +0x38c item's +0xb0 point
 * (0202b450), the body's +0x4ec attachment is moved there too (its +0x190 point, c5c54, and
 * its +0xc hook with 0 when its +0x40 bit 1 is set), the body is released (ov022 ad8e0, 0),
 * the item is placed at (0.5, 5.0, 0) and notified the same way, and the overlay's 14-byte
 * message (data_ov160_020cf7d6) carries the object's +0xa8 point, lifted 1.0, packed as
 * 24-bit values to the item's +0x24 hook; reaction 0x151 mode 8 fires at the +4 point, the
 * sub-state clears and the state ends. Without such a body the +0x30 timer accumulates the
 * rate and past 0x2a8 the state ends the same way. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;
typedef struct { u8 hi, mid, lo; } Fx24;

typedef struct {
    u16 id;             /* +0x0 */
    u8 kind;            /* +0x2 */
    u8 cmd;             /* +0x3 */
    u8 flag;            /* +0x4 */
    Fx24 pos[3];        /* +0x5 */
} PosMsg;

struct Ov160Flags464 { char pad[0x464]; unsigned long long flags; };
struct Ov160Bits40 { int b0 : 1, b1 : 1; };

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

extern void Actor_SetVecAndSyncChild(int object, VecFx32 *at);
extern void Ov107_MoveNodeAndRelayout(int ent, VecFx32 *at);
extern void Ov022_ToggleBit13ByMode(int body, int a);
extern void Ov107_BuildAndSendUpdate(int item, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const PosMsg data_ov160_020cf7d6;

void Ov160_GrabTick(int *node)
{
    int *state = (int *)node[1];
    PosMsg msg;
    VecFx32 raw;
    FxVec vContact;
    int e;
    int item;

    if (state[0xb] != 0 && (((struct Ov160Flags464 *)state[0xb])->flags & 0x8000) != 0) {
        msg = data_ov160_020cf7d6;
        Actor_SetVecAndSyncChild(*(int *)(state[0xb] + 0x20), (VecFx32 *)(*(int *)(*state + 0x38c) + 0xb0));
        e = *(int *)(state[0xb] + 0x4ec);
        if (e != 0) {
            *(VecFx32 *)(e + 0x190) = *(VecFx32 *)(*(int *)(*state + 0x38c) + 0xb0);
            Ov107_MoveNodeAndRelayout(e, (VecFx32 *)(e + 0x190));
            e = *(int *)(state[0xb] + 0x4ec);
            if (((struct Ov160Bits40 *)(e + 0x40))->b1 != 0 && *(void (**)(int, int))(e + 0xc) != 0) {
                (*(void (**)(int, int))(e + 0xc))(e, 0);
            }
        }
        Ov022_ToggleBit13ByMode(state[0xb], 0);
        raw.x = 0x2000;
        raw.y = 0x5000;
        raw.z = 0;
        Ov107_MoveNodeAndRelayout(*(int *)(*state + 0x38c), &raw);
        item = *(int *)(*state + 0x38c);
        if (((struct Ov160Bits40 *)(item + 0x40))->b1 != 0 && *(void (**)(int, int))(item + 0xc) != 0) {
            (*(void (**)(int, int))(item + 0xc))(item, 0);
        }
        raw = *(VecFx32 *)(*(int *)(state[0xb] + 0x20) + 0xa8);
        raw.y += 0x1000;
        vContact.x = *(Fx32 *)&raw.x;
        PackFx24(&msg.pos[0], vContact.x.value);
        vContact.y = *(Fx32 *)&raw.y;
        PackFx24(&msg.pos[1], vContact.y.value);
        vContact.z = *(Fx32 *)&raw.z;
        PackFx24(&msg.pos[2], vContact.z.value);
        if (*(void (**)(int, PosMsg *, int))(*(int *)(*state + 0x38c) + 0x24) != 0) {
            (*(void (**)(int, PosMsg *, int))(*(int *)(*state + 0x38c) + 0x24))(*(int *)(*state + 0x38c), &msg, 0xe);
        }
        Ov107_BuildAndSendUpdate(*(int *)(*state + 0x38c), 0x151, 8, (void *)state[1]);
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0xc] += *(int *)(node[0] + 0x2c);
    if (state[0xc] < 0x2a8) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
