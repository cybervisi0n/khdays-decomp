/* Hit callback: once, launches away from the hit (or back along the velocity) at 0x600 plus 0x500
 * upward. */

#include "nitro/fx_types.h"

extern int VEC_Normalize(const VecFx32 *source, VecFx32 *dest);
extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *src, VecFx32 *dst);

struct State {
    char pad00[4];
    VecFx32 *pos;
    char pad08[4];
    VecFx32 velocity;
    char pad18[0x18];
    int done;
};

struct Task {
    char pad00[0x214];
    struct State *state;
};

int Ov299_BounceOnHit(struct Task *task, void *target, void *aim) {
    struct State *state = task->state;
    VecFx32 buf;
    int ok;

    if (state->done != 0) {
        return 0;
    }

    ok = VEC_Normalize((VecFx32 *)((char *)aim + 4), &buf);
    if (ok == 0) {
        if (target != 0) {
            VEC_Subtract(state->pos, (VecFx32 *)((char *)target + 0x190), &buf);
            buf.y = 0;
            ok = VEC_Normalize(&buf, &buf);
        }
        if (ok == 0) {
            int negZ = -state->velocity.z;
            int negX = -state->velocity.x;
            buf.x = negX;
            buf.y = 0;
            buf.z = negZ;
            VEC_Normalize(&buf, &buf);
        }
    }

    ScaleVec3Fx12(0x600, &buf, &state->velocity);
    state->velocity.y += 0x500;
    state->done = 1;
    return 1;
}
