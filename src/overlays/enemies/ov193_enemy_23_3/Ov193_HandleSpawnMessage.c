/* Message handler of the ov191 enemy (x3: ov191/192/193). A "spawned" message (kind 5): sub 0
 * attaches the first two models of the +0x3a0 set to the actor's +0x3ac placement (mode 0x17);
 * sub 1 starts the third from the packet payload at weight 0x1000; sub 2 decodes the packed
 * 24-bit position in bytes 5..13 into a fresh transform, concatenates the actor's +0xa0
 * placement and starts the fourth model from it. The base handler always runs.
 * 020c08cc takes six arguments (see Ov120_Actor_HandleEvent); the set pointer is reloaded for each
 * store (see Ov178_HandleSpawnMessage). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern void Srt_SetRotationQuat(SrtTransform *transform, void *placement);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int zero, SrtTransform *transform);
extern int Ov107_CreateNodeXformTaskFx24(int model, int parent, int kind, int zero, int weight, void *payload);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);

void Ov193_HandleSpawnMessage(int owner, unsigned char *command, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    union {
        int words[3];
        unsigned char bytes[12];
    } packed;

    if (command[2] == 5) {
        switch (command[3]) {
        case 0:
            (*(int **)(owner + 0x3a0))[1] = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3a0))[0], 0x17, (void *)(owner + 0x3ac), 0, 0);
            (*(int **)(owner + 0x3a0))[3] = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3a0))[2], 0x17, (void *)(owner + 0x3ac), 0, 0);
            break;
        case 1:
            (*(int **)(owner + 0x3a0))[5] = Ov107_CreateNodeXformTaskFx24(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3a0))[4], 0x17, 0, 0x1000, command + 5);
            break;
        case 2:
            SrtTransform_SetIdentity(&transform);
            packed.bytes[3] = command[5];
            packed.bytes[2] = command[6];
            packed.bytes[1] = command[7];
            translation.x = packed.words[0] >> 8;
            packed.bytes[7] = command[8];
            packed.bytes[6] = command[9];
            packed.bytes[5] = command[0xa];
            translation.y = packed.words[1] >> 8;
            packed.bytes[11] = command[0xb];
            packed.bytes[10] = command[0xc];
            packed.bytes[9] = command[0xd];
            translation.z = packed.words[2] >> 8;
            Srt_SetTranslation(&transform, &translation);
            Srt_SetRotationQuat(&transform, (void *)(owner + 0xa0));
            (*(int **)(owner + 0x3a0))[7] = Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3a0))[6], 0x17, 0, &transform);
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
