/* Publish the landing: copy the pending anchor into the live slot and ask the placement helper
 * where the object ends up. If it reports a spot, send the canned 4-byte event to the object's
 * notify hook, latch the returned height and go to action 7. If it does not, wait unless the
 * ready bit at +0x17a is set, in which case clear the counter, drop 0x06 from the hw60 high byte
 * and continue in the next step.
 *
 * Matched byte-exact 2026-07-23, first compile. The event pair is copied HIGH halfword first
 * through a pointer bound to the destination -- a struct assignment reads the low one first and
 * four instructions come out different.
 *
 * One of four byte-identical siblings. */

#include "nitro/fx_types.h"

typedef struct { unsigned short a, b; } Ev;

extern Ev data_ov186_020d34c0[];
extern int Ov107_CollectSphereOverlaps(int obj, void *a, void *out);
extern void SetIndexedSlot(void *node, int idx, void *cb);
extern void Ov186_AiReaimStrafe(void);

struct hw60 { unsigned short lo : 8, hi : 8; };
struct Bit0 { unsigned char b0 : 1; };

void Ov186_PublishLanding(int *node) {
    int *state = (int *)node[1];
    int out[4];
    Ev ev;
    void (*cb)(int, Ev *, int);

    *(VecFx32 *)(state + 0xb) = *(VecFx32 *)(state + 0x19);
    if (Ov107_CollectSphereOverlaps(state[0], (void *)(state[0] + 0x74), out) != 0) {
        Ev *pe = &ev;
        pe->b = data_ov186_020d34c0[2].b;
        pe->a = data_ov186_020d34c0[2].a;
        cb = *(void (**)(int, Ev *, int))(state[0] + 0x24);
        if (cb != 0) {
            cb(state[0], pe, 4);
        }
        state[0x1d] = out[0];
        *(char *)(state[0] + 0x1c7) = 7;
        SetIndexedSlot(node, *(signed char *)((int)node + 0x20), 0);
        return;
    }
    if (((struct Bit0 *)(state[0] + 0x17a))->b0 == 0) {
        return;
    }
    state[0xe] = 0;
    ((struct hw60 *)(state[0] + 0x60))->hi &= ~6;
    SetIndexedSlot(node, *(signed char *)((int)node + 0x20), Ov186_AiReaimStrafe);
}
