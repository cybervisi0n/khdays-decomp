/* Message handler of the ov134 enemy (x3: ov134/135/136), ported from the matched ov191 sibling. A
 * "spawned" message (kind 5): sub 0 attaches the +0x3a4 set's entry 0 to a transform built from
 * the message's packed position, the actor's +0xa0 quaternion and a unit scale (mode 0x15); sub 1
 * starts entry 2 from the payload (mode 0x15, weight 0xb33); sub 2 fixes entry 4 to the +0x394
 * item's +4 placement (mode 1). The base handler always runs. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern void Srt_SetRotationQuat(SrtTransform *transform, void *placement);
extern void Srt_SetScaleUniform(SrtTransform *transform, int scale);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int zero, SrtTransform *transform);
extern int Ov107_CreateNodeXformTaskFx24(int model, int parent, int kind, int zero, int weight, void *payload);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);

void Ov136_HandleSpawnMessage(int owner, unsigned char *command, int arg)
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
            Srt_SetScaleUniform(&transform, 0x1000);
            (*(int **)(owner + 0x3a4))[1] = Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3a4))[0], 0x15, 0, &transform);
            break;
        case 1:
            (*(int **)(owner + 0x3a4))[3] = Ov107_CreateNodeXformTaskFx24(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3a4))[2], 0x15, 0, 0xb33, command + 5);
            break;
        case 2:
            (*(int **)(owner + 0x3a4))[5] = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3a4))[4], 1, (void *)(*(int *)(owner + 0x394) + 4), 0, 1);
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
