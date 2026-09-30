/* Message handler of the ov256 actor: a "spawned" message (kind 5) attaches an effect by its sub-kind
 * (byte 3) into the +0x46c / +0x470 slot pair under the +0x3c owner: 14 at the message's own spot
 * (mode 0xf), 0, 2, 4-7 and 9 likewise (mode 7), 8 spawns a shard at the packed 24-bit position in
 * bytes 5..13 (020d0c8c with the +0x4ac model, into +0x4b0), 10 / 12 on the +0x434 claw and 11 / 13 on
 * the +0x438 claw, 1 and 15 on the actor's +0xa0 node (byte 4 as the variant). The base handler always
 * runs. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_CreateNodeXformTaskFx24(int model, int parent, int kind, int zero, int scale, void *spot);
extern int Ov256_SpawnShard(int owner, int model, VecFx32 *pos);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);

void Ov256_MessageHandler(int owner, unsigned char *command, int arg)
{
    VecFx32 pos;
    union {
        int words[3];
        unsigned char bytes[12];
    } packed;

    if (command[2] == 5) {
        switch (command[3]) {
        case 14:
            *(int *)(owner + command[3] * 8 + 0x470) = Ov107_CreateNodeXformTaskFx24(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x46c), 0xf, 0, 0x1000, command + 5);
            break;
        case 0:
        case 2:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
            *(int *)(owner + command[3] * 8 + 0x470) = Ov107_CreateNodeXformTaskFx24(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x46c), 7, 0, 0x1000, command + 5);
            break;
        case 8:
            packed.bytes[3] = command[5];
            packed.bytes[2] = command[6];
            packed.bytes[1] = command[7];
            pos.x = packed.words[0] >> 8;
            packed.bytes[7] = command[8];
            packed.bytes[6] = command[9];
            packed.bytes[5] = command[0xa];
            pos.y = packed.words[1] >> 8;
            packed.bytes[11] = command[0xb];
            packed.bytes[10] = command[0xc];
            packed.bytes[9] = command[0xd];
            pos.z = packed.words[2] >> 8;
            *(int *)(owner + 0x4b0) = Ov256_SpawnShard(owner, *(int *)(owner + 0x4ac), &pos);
            break;
        case 10:
        case 12:
            *(int *)(owner + command[3] * 8 + 0x470) = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x46c), 7, (void *)(*(int *)(owner + 0x434) + 0xa0), command[4], 0);
            break;
        case 11:
        case 13:
            *(int *)(owner + command[3] * 8 + 0x470) = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x46c), 7, (void *)(*(int *)(owner + 0x438) + 0xa0), command[4], 0);
            break;
        case 1:
        case 15:
            *(int *)(owner + command[3] * 8 + 0x470) = Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c),
                *(int *)(owner + command[3] * 8 + 0x46c), 7, (void *)(owner + 0xa0), command[4], 0);
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
