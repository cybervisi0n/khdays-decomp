/*
 * Ov119_FaceTargetLookAt -- x3. AI-state tick: fire, then face the target with a look-at orientation.
 * Fire attack 2 (020c9264, flag 0). If the target state[0x16] is null, just dispatch. Else
 * dir = normalise(flatten_y(target(+0x190) - state[0x12])); if the direction is degenerate (normalise
 * returns 0) fall back to the const forward data_02042258. Build the orientation state[7..10] via
 * 0202ed60 (look-at from data_02042258 toward dir) + 0202f4a4, then copy it down to state[3..6].
 * Always hand off via 0203c634 to the 020cd8a8 state.
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct q4 { int a, b, c, d; };
extern void VEC_Subtract(void *a, void *b, void *c);
extern int  VEC_Normalize(void *a, void *b);
extern void Quat_FromTwoVectors(void *out, void *fwd, void *dir);
extern VecFx32 data_02042258;
extern void SetIndexedSlot(int self, int idx, int cb);
extern void Ov119_DecayOffsetGiveUpLatch(void);

void Ov119_FaceTargetLookAt(int *self) {
    int *state = (int *)self[1];
    int target;
    VecFx32 v;

    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    target = state[0x16];
    if (target != 0) {
        VEC_Subtract((void *)(target + 0x190), (void *)state[0x12], &v);
        v.y = 0;
        if (VEC_Normalize(&v, &v) == 0) {
            v = data_02042258;
        }
        Quat_FromTwoVectors((void *)(state + 7), &data_02042258, &v);
        Vec4_Normalize((void *)(state + 7), (void *)(state + 7));
        *(struct q4 *)(state + 3) = *(struct q4 *)(state + 7);
    }
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (int)&Ov119_DecayOffsetGiveUpLatch);
}
