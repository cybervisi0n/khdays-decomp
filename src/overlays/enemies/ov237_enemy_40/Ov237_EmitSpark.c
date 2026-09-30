/* Emit one ov237 particle from a ring emitter: the next record (0x38 bytes) of the owner's +0x90 ring
 * takes kind 0x10, the position and the given +0x1c value (+0x18 / +0x20 cleared), the owner's +0x5c
 * bit 1 is cleared and the cursor advances modulo the +0x8c ring size. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int kind; char pad4[0x14]; int a; int value; int c; char pad24[8]; VecFx32 pos; } Particle;
typedef struct { char *owner; int next; } Emitter;

void Ov237_EmitSpark(Emitter *emitter, VecFx32 *pos, int value)
{
    Particle *p;

    RandNextScaled(10);
    p = (Particle *)(*(int *)(emitter->owner + 0x90) + emitter->next * 0x38);
    *(int *)(emitter->owner + 0x5c) &= ~2;
    p->kind = 0x10;
    p->pos = *pos;
    p->a = 0;
    p->value = value;
    p->c = 0;
    emitter->next = (emitter->next + 1) % *(int *)(emitter->owner + 0x8c);
}
