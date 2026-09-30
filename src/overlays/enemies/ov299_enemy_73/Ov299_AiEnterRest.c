/* Clears the hit link and stance bit 0, sets bit 7, stops and installs the queue-action-1 step. */

#include "nitro/fx_types.h"
#include "game/ai_task.h"

extern const VecFx32 data_02041dc8;
extern void SetIndexedSlot();
extern void Ov299_AiStep_QueueAction1IfActive(void);

struct D { char pad0[8]; unsigned int f8 : 8; };
struct C { char pad0[0x60]; unsigned short f60; char pad62[0x326]; struct D *f388; };
struct B { struct C *p0; char pad4[8]; VecFx32 vc; };
struct A {
    AI_TASK_FIELDS(struct B)
};

void Ov299_AiEnterRest(struct A *a) {
    struct B *b = a->pState;
    struct C *c;
    struct D *d;
    unsigned int x;

    *(int *)((char *)b->p0 + 0x38c) = 0;

    c = b->p0;
    x = c->f60;
    c->f60 = (x & ~0xff00) |
             (((unsigned int)(unsigned short)(((x << 16) >> 24) & ~1) << 24) >> 16);

    c = b->p0;
    x = c->f60;
    c->f60 = (x & ~0xff00) | (((((x << 16) >> 24) | 0x80) << 24) >> 16);

    d = b->p0->f388;
    d->f8 = d->f8 & ~1;

    b->vc = data_02041dc8;

    SetIndexedSlot(a, a->slot, Ov299_AiStep_QueueAction1IfActive);
}
