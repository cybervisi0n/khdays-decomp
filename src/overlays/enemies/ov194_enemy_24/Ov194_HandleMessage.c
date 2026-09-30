/* Message handler of the ov194 enemy (x3: ov194/195/196). A "spawned" message (kind 5): sub 0
 * decodes the packed 24-bit position in bytes 5..13 into a fresh transform, scales it by 2.0,
 * rotates it about the world Y axis by the packet's +0x10 angle and starts the first model of
 * the +0x3d4 set from it (mode 0x15); sub 1 starts the second from the payload at weight 0x1784;
 * sub 2 attaches the third (mode 1) to the actor's +0x3a4 placement (0, 1). The base handler always
 * runs. (020c08cc takes six arguments -- see Ov120_Actor_HandleEvent; the packed bytes are
 * assembled through a byte-addressed union as in Ov178_HandleSpawnMessage.) */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern void Srt_SetScaleUniform(SrtTransform *transform, int scale);
extern void Srt_SetRotationAxisAngle(SrtTransform *transform, const VecFx32 *axis, int angle);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int zero, SrtTransform *transform);
extern int Ov107_CreateNodeXformTaskFx24(int model, int parent, int kind, int zero, int weight, void *payload);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);
extern const VecFx32 data_02042264;

void Ov194_HandleMessage(int owner, unsigned char *command, int arg)
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
            Srt_SetScaleUniform(&transform, 0x2000);
            Srt_SetRotationAxisAngle(&transform, &data_02042264, *(int *)(command + 0x10));
            (*(int **)(owner + 0x3d4))[1] = Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3d4))[0], 0x15, 0, &transform);
            break;
        case 1:
            (*(int **)(owner + 0x3d4))[3] = Ov107_CreateNodeXformTaskFx24(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3d4))[2], 0x15, 0, 0x1784, command + 5);
            break;
        case 2:
            (*(int **)(owner + 0x3d4))[5] = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x3d4))[4], 1, (void *)(owner + 0x3a4), 0, 1);
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
