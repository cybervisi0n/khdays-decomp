/* Ov253_MsgHookB -- message hook: a kind 0 message stores its four +0x24..+0x2a halfwords
 * into +0x398 / +0x38c / +0x390 / +0x394; a kind 5 message aimed at slot 0 spawns the +0x3e8
 * block's first effect child (020c09a0, kind 5, at the actor's +0xa0) into its +4 and the
 * position child at +0xb0 (020d31a0) into +0x3d0, aimed at slot 1 spawns the second one
 * (020c08cc, kind 5, scale 1.0, payload at byte 5) into its +0xc. Then the base hook (020c7500). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_CreateNodeXformTaskFx24(int list, int parent, int kind, int a, int scale, unsigned char *payload);
extern int Ov107_AiState_OnMessage(int self, unsigned char *msg, int extra);
extern int Ov253_SpawnPosChild(int self, const VecFx32 *pos);

struct Ov253Pair { int pEffect; int pChild; };

int Ov253_MsgHookB(int self, unsigned char *msg, int extra) {
    if (msg[2] == 0) {
        *(int *)(self + 0x398) = *(short *)(msg + 0x24);
        *(int *)(self + 0x38c) = *(short *)(msg + 0x26);
        *(int *)(self + 0x390) = *(short *)(msg + 0x28);
        *(int *)(self + 0x394) = *(short *)(msg + 0x2a);
    } else if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
            (*(struct Ov253Pair **)(self + 0x3e8))[0].pChild =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), (*(struct Ov253Pair **)(self + 0x3e8))[0].pEffect, 5, (void *)(self + 0xa0), 0, 0);
            *(int *)(self + 0x3d0) = Ov253_SpawnPosChild(self, (VecFx32 *)(self + 0xb0));
            break;
        case 1:
            (*(struct Ov253Pair **)(self + 0x3e8))[1].pChild =
                Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), (*(struct Ov253Pair **)(self + 0x3e8))[1].pEffect, 5, 0, 0x1000, msg + 5);
            break;
        }
    }
    return Ov107_AiState_OnMessage(self, msg, extra);
}
