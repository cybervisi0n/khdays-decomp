/* Message handler: a "spawned" message (kind 5) with slot byte 0 starts the +0x3b0 sub-item
 * mapped by data_ov236_020d627c[cmd[4]] (kind 0x17, weight 1.0, payload at cmd+5); slot 1
 * spawns the first sub-item at the actor's +0x74 position (y = 0x100) from an identity
 * transform and pushes pose 1; slots 2 / 3 start the +0x30 / +0x38 sub-items; slot 4 stores
 * the 020d5f34 result at +0x44; slot 5 starts effect 0x127 (kind 0xf) on the +0x39c item's
 * transform into +0x3c4. The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } Srt;
struct Ov236SlotMap { u8 b[4]; };

extern int Ov107_CreateNodeXformTaskFx24(int list, int parent, int kind, int a, int scale, u8 *payload);
extern void SrtTransform_SetIdentity(Srt *srt);
extern void Srt_SetTranslation(Srt *srt, const VecFx32 *t);
extern int Ov107_CreateNodeXformTask(int owner, int slot, int kind, int a4, const Srt *srt);
extern void Ov107_ForwardVisibleEvent(char *self, int a2);
extern int Ov236_SpawnReactionTaskFromHit();
extern int Ov107_CreateSpawnTask(char *self, int id, int a3, int a4, int xform);
extern void Ov107_AiState_OnMessage(char *self, u8 *cmd, void *arg3);
extern const struct Ov236SlotMap data_ov236_020d627c;

void Ov236_HandleMessage(char *self, u8 *cmd, void *arg3)
{
    Srt srt;
    VecFx32 pos;
    struct Ov236SlotMap map;

    if (cmd[2] == 5) {
        switch (cmd[3]) {
        case 0:
            map = data_ov236_020d627c;
            *(int *)(*(int *)(self + 0x3b0) + map.b[cmd[4]] * 8 + 4) =
                Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3b0) + map.b[cmd[4]] * 8),
                                    0x17, 0, 0x1000, cmd + 5);
            break;
        case 1:
            SrtTransform_SetIdentity(&srt);
            pos = *(VecFx32 *)(self + 0x74);
            pos.y = 0x100;
            Srt_SetTranslation(&srt, &pos);
            *(int *)(*(int *)(self + 0x3b0) + 4) =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3b0)), 0x17, 0, &srt);
            Ov107_ForwardVisibleEvent(self, 1);
            break;
        case 2:
            *(int *)(*(int *)(self + 0x3b0) + 0x34) =
                Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3b0) + 0x30), 0x17, 0, 0x1000, cmd + 5);
            break;
        case 3:
            *(int *)(*(int *)(self + 0x3b0) + 0x3c) =
                Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3b0) + 0x38), 0x17, 0, 0x1000, cmd + 5);
            break;
        case 4:
            *(int *)(*(int *)(self + 0x3b0) + 0x44) = Ov236_SpawnReactionTaskFromHit(self);
            break;
        case 5:
            *(int *)(self + 0x3c4) = Ov107_CreateSpawnTask(self, 0x127, 0xf, 1, *(int *)(self + 0x39c) + 4);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, cmd, arg3);
}
