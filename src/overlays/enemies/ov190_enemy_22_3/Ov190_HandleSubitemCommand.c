/* Handles type-5 placement commands for the named entity actor: decodes a signed 24-bit position,
 * creates or transforms subitems for actions 0-3, creates the special resource for action 5,
 * releases it for action 6, then forwards to the base actor handler. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Ov190Transform {
    int words[11];
};

struct Ov190Command {
    u8 pad00[2];
    u8 type02;
    u8 action03;
    u8 target04;
    u8 packedPosition05[9];
};

struct Ov190NodeSlot {
    int node;
    int handle;
};

struct Ov190Actor {
    char pad000[0x3c];
    int resource03c;
    char pad040[0x60];
    struct Ov190Transform transform0a0;
    char pad0cc[0x2f8];
    int specialHandle3c4;
    struct Ov190NodeSlot nodes3c8[4];
};

extern void SrtTransform_SetIdentity(struct Ov190Transform *transform);
extern void Srt_SetTranslation(struct Ov190Transform *transform,
                          const VecFx32 *position);
extern int Ov107_CreateNodeXformTask(int resource, int node, int kind, int flags,
                               struct Ov190Transform *transform);
extern int Ov107_CreateNodeBodyTask(int resource, int node, int kind,
                               struct Ov190Transform *transform, int flags,
                               int enabled);
extern int Ov107_CreateSpawnTask(struct Ov190Actor *self, int resourceId,
                               int kind, int enabled,
                               struct Ov190Transform *transform);
extern void TaskList_FinishByTag(int resource, int handle);
extern void Ov107_UnlinkNodeFromOwner(int handle);
extern void Ov107_AiState_OnMessage(struct Ov190Actor *self,
                                struct Ov190Command *command, int arg2);

void Ov190_HandleSubitemCommand(struct Ov190Actor *self,
                         struct Ov190Command *command, int arg2)
{
    struct Ov190Transform transform;
    VecFx32 position;
    union {
        int words[3];
        u8 bytes[12];
    } packed;

    if (command->type02 == 5) {
        SrtTransform_SetIdentity(&transform);

        packed.bytes[3] = command->packedPosition05[0];
        packed.bytes[2] = command->packedPosition05[1];
        packed.bytes[1] = command->packedPosition05[2];
        position.x = packed.words[0] >> 8;
        packed.bytes[7] = command->packedPosition05[3];
        packed.bytes[6] = command->packedPosition05[4];
        packed.bytes[5] = command->packedPosition05[5];
        position.y = packed.words[1] >> 8;
        packed.bytes[11] = command->packedPosition05[6];
        packed.bytes[10] = command->packedPosition05[7];
        packed.bytes[9] = command->packedPosition05[8];
        position.z = packed.words[2] >> 8;

        Srt_SetTranslation(&transform, &position);

        switch (command->action03) {
        case 0:
            self->nodes3c8[command->action03].handle =
                Ov107_CreateNodeXformTask(self->resource03c,
                                    self->nodes3c8[command->action03].node,
                                    5, 0, &transform);
            break;
        case 1:
        case 2:
            transform = self->transform0a0;
            self->nodes3c8[command->action03].handle =
                Ov107_CreateNodeXformTask(self->resource03c,
                                    self->nodes3c8[command->action03].node,
                                    5, 0, &transform);
            break;
        case 3:
            self->nodes3c8[command->action03].handle =
                Ov107_CreateNodeBodyTask(self->resource03c,
                                    self->nodes3c8[command->action03].node,
                                    5, &self->transform0a0, 0, 1);
            break;
        case 4:
            break;
        case 5:
            self->specialHandle3c4 =
                Ov107_CreateSpawnTask(self, 0x12f, 8, 1,
                                    &self->transform0a0);
            break;
        case 6:
            if (self->nodes3c8[3].handle != 0) {
                TaskList_FinishByTag(self->resource03c,
                              self->nodes3c8[3].handle);
                self->nodes3c8[3].handle = 0;
            }
            if (self->specialHandle3c4 != 0) {
                Ov107_UnlinkNodeFromOwner(self->specialHandle3c4);
                self->specialHandle3c4 = 0;
            }
            break;
        }
    }

    Ov107_AiState_OnMessage(self, command, arg2);
}

