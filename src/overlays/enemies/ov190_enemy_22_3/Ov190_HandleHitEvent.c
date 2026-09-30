/* Applies an actor hit event: selects the reaction mode, computes and clamps damage, updates hit
 * points, emits the alternating reaction effect and requests the next actor state. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"

struct Ov190ReactionModes {
    u8 normal[2];
    u8 special[2];
};

struct Ov190HitFlags {
    unsigned int low : 16;
    unsigned int kind : 16;
};

struct Ov190Hit {
    unsigned int flags;
    VecFx32 position;
    char pad10[0x10];
    int reaction20;
    unsigned int resultFlags24;
    int damage28;
};

struct Ov190ReactionObject {
    char pad000[0x1c6];
    signed char state1c6;
    u8 nextState1c7;
};

struct Ov190ReactionWork {
    struct Ov190ReactionObject *object00;
    char pad004[0x08];
    int reactionContext0c;
    char pad010[0x1c];
    VecFx32 position2c;
    int source38;
    char pad03c[0x02];
    u8 facing3e;
};

extern const struct Ov190ReactionModes data_ov190_020d5cd8;
extern int Ov107_CalcHitDamage(Actor *self,
                               struct Ov190Hit *hit);
extern void Ov107_BuildAndSendUpdate(Actor *self,
                                int reactionId, u8 mode, int context);

int Ov190_HandleHitEvent(Actor *self, int source,
                        struct Ov190Hit *hit)
{
    u8 normalModes[2] = {
        data_ov190_020d5cd8.normal[0],
        data_ov190_020d5cd8.normal[1]
    };
    u8 specialModes[2] = {
        data_ov190_020d5cd8.special[0],
        data_ov190_020d5cd8.special[1]
    };
    struct Ov190ReactionWork *work = self->pAiState;
    int delta;
    int remaining;
    struct Ov190HitFlags *flags;

    if (self->field_1ac & 1) {
        return 0;
    }
    if (self->hitPoints <= 0) {
        return 0;
    }

    switch (work->object00->state1c6) {
    case 6:
        hit->reaction20 = 0;
        break;
    case 7:
        hit->reaction20 = 1;
        break;
    case 9:
        hit->reaction20 = 2;
        break;
    }

    work->position2c = hit->position;
    hit->damage28 = Ov107_CalcHitDamage(self, hit);

    if (((u16)hit->flags & 0x4000) != 0) {
        hit->resultFlags24 = (hit->resultFlags24 & 0xffff0000) | 1;
        work->object00->nextState1c7 = 5;
        return 1;
    }

    delta = self->hitPoints - hit->damage28;
    if (delta < 0) {
        remaining = 0;
    } else {
        remaining = self->hitPointsCap;
        if (delta <= remaining) {
            remaining = delta;
        }
    }
    self->hitPoints = (short)remaining;
    work->source38 = source;

    if (hit->damage28 > 0) {
        flags = (struct Ov190HitFlags *)hit;
        if ((flags->low & 8) == 0 || (flags->low & 0x80) == 0 ||
            flags->kind != 0x80) {
            if ((flags->low & 0x22) != 0) {
                work->facing3e = work->facing3e + 1;
                Ov107_BuildAndSendUpdate(self, 0x12f,
                                    specialModes[work->facing3e & 1],
                                    work->reactionContext0c);
            } else {
                work->facing3e = work->facing3e + 1;
                Ov107_BuildAndSendUpdate(self, 0x12f,
                                    normalModes[work->facing3e & 1],
                                    work->reactionContext0c);
            }
        }
    }

    if (self->hitPoints == 0) {
        work->object00->nextState1c7 = 3;
        return 1;
    }

    if ((work->object00->state1c6 == 7 ||
         work->object00->state1c6 == 9) &&
        (((u16)hit->flags & 1) != 0) &&
        (((u16)hit->flags & 0x10) != 0)) {
        work->object00->nextState1c7 = 8;
        return 1;
    }

    if (((u16)hit->flags & 0x8000) != 0) {
        work->object00->nextState1c7 = 5;
    }
    return 1;
}

