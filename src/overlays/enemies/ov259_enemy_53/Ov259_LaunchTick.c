/* Launch tick of the ov259 actor: it keeps facing its target (020cd5d4) while the +0x68 timer runs
 * up to +0x80. Each time it expires the next helper slot (+0x98) is thrown when idle: the throw
 * direction is the rest vector turned by the +0x78 heading and the helper launches from the +0x10
 * point (020d26e0). With the first throw done a d100 roll is drawn, +0x84 = 0x3fc0, the next move is
 * 2 and the node ends. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;

extern void Ov259_FaceTarget(int *node);
extern void Ov259_LaunchHelper(int helper, VecFx32 *pos, VecFx32 *dir, int heading);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

void Ov259_LaunchTick(int *node)
{
    int *state = (int *)node[1];
    int expired = 0;
    VecFx32 dir;
    Quat q;

    Ov259_FaceTarget(node);
    state[0x1a] += *(int *)(node[0] + 0x2c);
    if (state[0x1a] > state[0x20]) {
        expired = 1;
    }
    if (expired == 0) {
        return;
    }
    if (state[0x26] == 0) {
        int helper = *(int *)(*state + state[0x26] * 4 + 0x388);

        if (*(int *)(helper + 0x388) == 0) {
            QuatFromAxisAngle(&q, &data_02042264, state[0x1e]);
            Vec3TransformViaTempMtx(&dir, &q, &data_02042258);
            Ov259_LaunchHelper(helper, (VecFx32 *)state[4], &dir, state[0x1e]);
        }
    } else {
        RandNextScaled(0x65);
        state[0x21] = 0x3fc0;
        *(signed char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0x1a] = 0;
    state[0x26]++;
}
