/* Bounce entry of the ov204 enemy's ball (and its byte-identical twin): plays animation 9, aims
 * the travel direction from the launch position at the target's +0x190 point (or copies the
 * default direction), flattens and normalises it (falling back to the shared forward vector),
 * sets the speed to 0x1000, clears the phase and the clock, fires reaction 0x132 mode 8 at the
 * reaction point and hands off to the bounce tick. The flattening store is written at the end
 * of both branches: the tail-merged block keeps it ahead of the normalise argument. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    int actor;
    int pad04;
    VecFx32 vel;
    VecFx32 defaultDir;
    VecFx32 *from;
    VecFx32 *at;
    u8 *busy;
    int clock;
    int pad30[4];
    int target;
    u8 pad44;
    u8 phase;
    u8 pad46[2];
    VecFx32 dir;
    int speed;
} BallState;

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *d);
extern int VEC_Normalize(const VecFx32 *a, VecFx32 *d);
extern void Ov107_BuildAndSendUpdate(int actor, int reaction, int mode, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;
extern void Ov205_BounceTick(int *node);

void Ov205_EnterBounce(int *node)
{
    BallState *state = (BallState *)node[1];

    Ov107_PostTagUpdate((Actor *)state->actor, 9, 0);
    if (state->target != 0) {
        VEC_Subtract(state->from, (VecFx32 *)(state->target + 0x190), &state->dir);
        state->dir.y = 0;
    } else {
        state->dir = state->defaultDir;
        state->dir.y = 0;
    }
    if (VEC_Normalize(&state->dir, &state->dir) == 0) {
        state->dir = data_02042258;
    }
    state->speed = 0x1000;
    state->phase = 0;
    state->clock = 0;
    Ov107_BuildAndSendUpdate(state->actor, 0x132, 8, state->at);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov205_BounceTick);
}
