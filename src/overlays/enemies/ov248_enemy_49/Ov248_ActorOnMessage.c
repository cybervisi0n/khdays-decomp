/* Message handler of the ov248 actor: a "spawned" message (kind 5) carries a packed 24-bit position in
 * bytes 5..13 which becomes the translation of a fresh transform; sub-kind 0 attaches slot 0's model
 * to it (mode 0x17, message +4), sub-kind 1 places slot 1's model on the +0xa0 node (message +4, flag
 * when it is 2); the effect lands in the slot's +4. The base handler always runs. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;
struct Slot { int model; int effect; };
struct Ov248Actor { char pad[0x388]; struct Slot slots[2]; };

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int zero, SrtTransform *transform);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);

void Ov248_ActorOnMessage(int owner, unsigned char *command, int arg)
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
        case 0:
            ((struct Ov248Actor *)owner)->slots[command[3]].effect =
                Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), ((struct Ov248Actor *)owner)->slots[command[3]].model, 0x17, command[4], &transform);
            break;
        case 1:
            ((struct Ov248Actor *)owner)->slots[command[3]].effect =
                Ov107_CreateNodeBodyTask(*(int *)(owner + 0x3c), ((struct Ov248Actor *)owner)->slots[command[3]].model, 0x17, (void *)(owner + 0xa0), command[4], command[4] == 2);
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
