/* Thin forwarder: push a caller-supplied vector to the owner (*param_1) via
 * func_ov107_020c0b90 with mode 0 / flag 0. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
void Ov223_ForwardVecToOwner(int *param_1, VecFx32 v) {
    func_ov107_020c0b90(*param_1, 0, v, 0);
}
