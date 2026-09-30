/* Ov174_HandleSpawnMessage: spawn-message handler of the ov173 enemy (x2), variant of the matched ov166 sibling: subs 2/4 start entry 2 from the transform (two separate bodies, laid out 3-4-2-5), sub 3 fixes entries 6 and 4 to the +0x38c/+0x390 placements, sub 5 starts entry 0, and sub 8 registers effect 0x141 (modes 4/5; kind 2 is an empty case). */
/* Message handler of the ov173 enemy (and its byte-identical twins). A "spawned" message (kind 5) carries
 * a packed 24-bit position in bytes 5..13 which becomes the translation of a fresh transform;
 * the sub-kind then attaches the effect models of the +0x39c set: sub 0 fixes entry 0 to the
 * +0x390 item's +4 placement (mode 7), subs 1 and 2 start entries 4 and 2 from the transform;
 * sub 8 registers effect 0x114 (kind 7 or 8 by the message's +4 slot) or effect 0x13e (kind 4)
 * on the +0xa0 node into the +0x3a0 slot. The base handler always runs. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int zero, SrtTransform *transform);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);

void Ov174_HandleSpawnMessage(int owner, unsigned char *command, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    union {
        int words[3];
        unsigned char bytes[12];
    } packed;

    if (command[2] == 5) {
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
        switch (command[3]) {
        case 3:
            (*(int **)(owner + 0x39c))[7] = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x39c))[6], 7, (void *)(*(int *)(owner + 0x38c) + 4), 0, 0);
            (*(int **)(owner + 0x39c))[5] = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x39c))[4], 7, (void *)(*(int *)(owner + 0x390) + 4), 0, 0);
            break;
        case 4:
            (*(int **)(owner + 0x39c))[3] = Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x39c))[2], 7, 0, &transform);
            break;
        case 2:
            (*(int **)(owner + 0x39c))[3] = Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x39c))[2], 7, 0, &transform);
            break;
        case 5:
            (*(int **)(owner + 0x39c))[1] = Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), (*(int **)(owner + 0x39c))[0], 7, 0, &transform);
            break;
        case 8:
            switch (command[4]) {
            case 0:
                *(int *)(owner + command[4] * 4 + 0x3a0) = Ov107_CreateSpawnTask(owner, 0x141, 4, 0, (void *)(owner + 0xa0));
                break;
            case 1:
                *(int *)(owner + command[4] * 4 + 0x3a0) = Ov107_CreateSpawnTask(owner, 0x141, 5, 0, (void *)(owner + 0xa0));
                break;
            case 2:
                break;
            }
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
