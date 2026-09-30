/* Ov245_SpinTick -- spin tick: advances the state's +0x18 angle towards +0x1c at the +0x20
 * rate (0203d040), builds the rotation about the world up axis (0202f188 on 02042264) and the
 * one that aligns the actor's +0x124 facing (0202ed60), combines them (0202ef54) into the
 * actor's +0xa0 placement, then pushes the +0x24 velocity to the actor's +0xf0 and clears it.
 * Codegen: the shared +0x24 address is a named pointer so the copy evaluates its destination
 * address first (ip) and the source (lr) second. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Quat_FromTwoVectors(void *out, const VecFx32 *a, const VecFx32 *b);
extern void Quat_Multiply(void *out, void *a, void *b);
extern void Srt_SetRotationQuat(int placement, void *rotation);
extern char data_02042264[];
extern const VecFx32 data_02041dc8;

void Ov245_SpinTick(int *node) {
    int *state = (int *)node[1];
    int spin[4];
    int face[4];

    state[6] = Angle_TurnToward(state[6], state[7], state[8], 0);
    QuatFromAxisAngle(spin, data_02042264, state[6]);
    Quat_FromTwoVectors(face, (const VecFx32 *)data_02042264, (VecFx32 *)(*state + 0x124));
    Quat_Multiply(face, face, spin);
    Srt_SetRotationQuat(*state + 0xa0, face);
    {
        VecFx32 *push = (VecFx32 *)(state + 9);
        VecFx32 *dst = (VecFx32 *)(*state + 0xf0);
        *dst = *push;
        *push = data_02041dc8;
    }
}
