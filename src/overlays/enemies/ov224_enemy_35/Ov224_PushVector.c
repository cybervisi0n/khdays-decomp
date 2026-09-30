/* Thin forwarder: push a caller-supplied vector to the owner (*state) via ov107 c0b90 with
 * mode 0; the flag is set when the caller passes a zero fourth argument. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, unsigned char flag);
void Ov224_PushVector(int *state, VecFx32 v, int bTarget) {
    func_ov107_020c0b90(*state, 0, v, bTarget == 0 ? 1 : 0);
}
