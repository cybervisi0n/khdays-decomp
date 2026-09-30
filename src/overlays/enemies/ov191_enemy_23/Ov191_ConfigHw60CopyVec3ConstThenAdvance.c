/* State step: clears bit 0 and sets bits 0x82 in the high byte of the actor's flags (+0x60), clears
 * bit 0 of its model's flag byte (+8), resets the step vector from the constant default at
 * data_02041dc8 and installs the queue-action-when-active step. */

#include "nitro/fx_types.h"
#include "game/ai_task.h"

extern int SetIndexedSlot();
extern int Ov191_AiStep_QueueAction1IfActive();

extern const VecFx32 data_02041dc8;

struct D {
    char pad0[8];
    unsigned int f8 : 8;
};

struct C {
    char pad0[0x60];
    unsigned short f60;
    char pad62[0x326];
    struct D *f388;
};

struct B {
    struct C *p0;
    char pad4[4];
    VecFx32 v8;
};

struct A {
    AI_TASK_FIELDS(struct B)
};

void Ov191_ConfigHw60CopyVec3ConstThenAdvance(struct A *a)
{
    struct B *b = a->pState;
    struct C *c;
    struct D *d;
    unsigned int x;

    c = b->p0;
    x = c->f60;
    c->f60 = (x & ~0xff00) |
             (((unsigned int)(unsigned short)(((x << 16) >> 24) & ~1) << 24) >> 16);

    c = b->p0;
    x = c->f60;
    c->f60 = (x & ~0xff00) | (((((x << 16) >> 24) | 0x82) << 24) >> 16);

    d = b->p0->f388;
    d->f8 = d->f8 & ~1;

    b->v8 = data_02041dc8;

    SetIndexedSlot(a, a->slot, Ov191_AiStep_QueueAction1IfActive);
}
