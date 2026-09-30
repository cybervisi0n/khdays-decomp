/* Message handler of the ov238 actor: a spawn message (kind 5) unpacks its position into a transform
 * and starts the +0x404 effect pair of the sub id: 0-4 and 8 from the payload (kind 5, weight 1.0);
 * 5-7 replace the +0x438 effect and attach to the +0x3f8 part's +4 placement (sub 6 variant 1); 0xa
 * knocks the actor back. The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;
struct EffectPair { int res; int handle; };
struct Ov238Effects { char pad[0x404]; struct EffectPair pair[9]; };

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTaskFx24(int model, int parent, int kind, int arg, int weight, void *payload);
extern void TaskList_FinishByTag(int model, int handle);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

void Ov238_OnSpawnMessage(char *self, u8 *msg, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    union {
        int words[3];
        u8 bytes[12];
    } packed;

    if (msg[2] == 5) {
        packed.bytes[3] = msg[5];
        packed.bytes[2] = msg[6];
        packed.bytes[1] = msg[7];
        translation.x = packed.words[0] >> 8;
        packed.bytes[7] = msg[8];
        packed.bytes[6] = msg[9];
        packed.bytes[5] = msg[0xa];
        translation.y = packed.words[1] >> 8;
        packed.bytes[11] = msg[0xb];
        packed.bytes[10] = msg[0xc];
        packed.bytes[9] = msg[0xd];
        translation.z = packed.words[2] >> 8;
        SrtTransform_SetIdentity(&transform);
        Srt_SetTranslation(&transform, &translation);
        switch (msg[3]) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 8:
            ((struct Ov238Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeXformTaskFx24(
                *(int *)(self + 0x3c), ((struct Ov238Effects *)self)->pair[msg[3]].res, 5, 0, 0x1000, msg + 5);
            break;
        case 5:
        case 6:
        case 7:
            if (*(int *)(self + 0x438) != 0) {
                TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(self + 0x438));
                *(int *)(self + 0x438) = 0;
            }
            ((struct Ov238Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeBodyTask(
                *(int *)(self + 0x3c), ((struct Ov238Effects *)self)->pair[msg[3]].res, 5,
                (void *)(*(int *)(self + 0x3f8) + 4), 0, msg[3] == 6);
            break;
        case 0xa:
            Ov107_ForwardVisibleEvent(self, 1);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
