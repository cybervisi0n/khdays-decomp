/* While the event bit is set, integrates the decaying rate into the height, clamps it to 0..cap and
 * syncs the actor; at 0 the local player sends record 2 once. With flag 0x80 hands over to
 * Ov014_ActorStepProgress. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    char pad00[0xe0];
    VecFx32 sourceVec;
    char pad_ec[0x1b1 - 0xec];
    unsigned char flags;
    unsigned char useAltRate;
    unsigned char callbackState;
    int cap;
    int rateA;
    int rateB;
    int source;
    int remaining;
    int position;
} Ov014State;

static inline int fx_mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern int Ov002_GetModuleScale(void);
extern int Ov002_RecordElementHit(Ov014State *, unsigned char *, int);
extern void Ov014_SetFlag2RunTwoSubActionsIfFlag4(Ov014State *, int);
extern int Ov014_ActorStepProgress(void);

int Ov014_TickHeightMotion(Ov014State *self)
{
    int delta;
    int rate;
    unsigned int event;
    unsigned char rec[4];
    VecFx32 vec;

    delta = Ov002_GetModuleScale();
    rate = self->source;
    event = GameState_GetField(*(unsigned short *)((char *)self + 0x14),
                           *(unsigned char *)((char *)self + 0x16));
    event &= 0xfffe;
    event <<= 15;
    event >>= 16;
    if ((event & 1) == 0)
        return 0;
    if ((self->flags & 0x80) == 0) {
        {
            if (self->remaining > 0) {
                if (self->useAltRate)
                    rate -= fx_mul(self->rateB, delta);
                else
                    rate -= fx_mul(self->rateA, delta);
                self->remaining -= delta;
                if (self->remaining < 0) self->remaining = 0;
            }
            self->position += rate;
        }
        if (self->position < 0) {
            self->position = 0;
            if (Session_GetLocalPlayerIndex() == 0 && (self->flags & 1) == 0) {
                rec[0] = 2;
                if (Ov002_RecordElementHit(self, rec, 4))
                    self->flags |= 1;
            }
        } else if (self->cap < self->position) {
            self->position = self->cap;
        }
        vec = self->sourceVec;
        vec.y = self->position;
        Actor_SetVecAndSyncChild((int *)((char *)self + 0x38), &vec);
        *(VecFx32 *)((char *)self + 0x1c) = vec;
    } else {
        if (*(short *)((char *)self + 0x1d0) >= 0) {
            self->callbackState = 1;
            Ov014_SetFlag2RunTwoSubActionsIfFlag4(self, 1);
            return (int)Ov014_ActorStepProgress;
        }
    }
    return 0;
}
