/* Message handler of the ov260 actor: a "spawned" message (kind 5) carries a packed 24-bit position
 * in bytes 5..13 which becomes the translation of a fresh transform; the sub-kind (message byte 3)
 * attaches an effect into its +0x478 / +0x47c slot pair (message byte 4 as the variant, +0x3c owner):
 * 0, 2-4, 6, 7 at the transform (mode 0x17; 9 also turned by the +0xa0 rotation, mode 0x1f), 2 and 6
 * also flag the actor (020c0b14); 1 and 8 on the +0x3e4 node (mode 0x1f, 8 looped); 5 and 0xa on the
 * +0x3b8 node; 0xb on the +0x424 part; 0xd releases the effect in slot byte 4. The base handler
 * always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern void Srt_SetRotationQuat(SrtTransform *transform, void *quat);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int variant, SrtTransform *transform);
extern int Ov107_CreateNodeBodyTask(int model, int parent, int kind, void *at, int a, int b);
extern void Ov107_ForwardVisibleEvent(int owner, int flag);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);

void Ov260_ActorMessageHandler(int owner, unsigned char *command, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    union {
        int words[3];
        unsigned char bytes[12];
    } packed;

    if (command[2] == 5) {
        u8 kind = 0x17;

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
        SrtTransform_SetIdentity(&transform);
        Srt_SetTranslation(&transform, &translation);
        switch (command[3]) {
        case 9:
            kind |= 8;
            Srt_SetRotationQuat(&transform, (void *)(owner + 0xa0));
            /* fall through */
        case 0:
        case 2:
        case 3:
        case 4:
        case 6:
        case 7:
            *(int *)(owner + command[3] * 8 + 0x47c) = Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x478), kind, command[4], &transform);
            if (!(command[3] != 2 && command[3] != 6)) {
                Ov107_ForwardVisibleEvent(owner, 1);
            }
            break;
        case 1:
        case 8:
            *(int *)(owner + command[3] * 8 + 0x47c) = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x478), 0x1f, (void *)(owner + 0x3e4), command[4],
                command[3] == 8);
            break;
        case 5:
        case 0xa:
            *(int *)(owner + command[3] * 8 + 0x47c) = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x478), kind, (void *)(owner + 0x3b8), command[4], 0);
            break;
        case 0xb:
            *(int *)(owner + command[3] * 8 + 0x47c) = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x478), kind, (void *)(*(int *)(owner + 0x424) + 4),
                command[4], 0);
            break;
        case 0xd:
            if (*(void **)(owner + command[4] * 8 + 0x47c) != 0) {
                TaskList_FinishByTag(*(void **)(owner + 0x3c), *(void **)(owner + command[4] * 8 + 0x47c));
                *(void **)(owner + command[4] * 8 + 0x47c) = 0;
            }
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
