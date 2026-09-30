/* Drop reward of the ov252 actor: the last armour piece whose shape (+0x4f4) is still shown gives the
 * point (its +0x59c record) where the reward appears; sound 0x148/8 plays at the +8 point and the owner
 * spawns effect 0x1b there with `param`. Returns 1 (2 when no piece was picked). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned f : 8; } B8;
struct Ov252Pieces {
    char pad[0x4f4];
    int shapes[4];
    char pad504[0x98];
    struct { VecFx32 at; char pad[0x20]; } rec[4];
};

extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);

int Ov252_DropReward(int *node, int param)
{
    int *state = (int *)node[1];
    short i;
    VecFx32 at;

    for (i = 3; i >= 0; i--) {
        if (((B8 *)(((struct Ov252Pieces *)*state)->shapes[i] + 8))->f & 1) {
            at = ((struct Ov252Pieces *)*state)->rec[i].at;
            break;
        }
    }
    if (i >= 4) {
        return 2;
    }
    RandNextScaled(0x3245);
    Ov107_BuildAndSendUpdate(*state, 0x148, 8, (void *)state[2]);
    func_ov107_020c0b90(*state, 0x1b, at, param);
    return 1;
}
