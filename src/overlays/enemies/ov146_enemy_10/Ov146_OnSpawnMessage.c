/* Message handler of the ov146 actor: a spawn message (kind 5) unpacks its position into a transform
 * and starts the +0x3c4 effect pair of the sub id: 0 at the transform scaled 2.0, 1 and 2 at the
 * partner chain's (+0x3b8 of +0x3b8) +0x14 point scaled 2.0 (variant 0 / 2), 4 attached to the actor
 * pose (+0xa0). The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;
struct EffectPair { int res; int handle; };
struct Ov146Effects { char pad[0x3c4]; struct EffectPair pair[8]; };

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern void Srt_SetScaleUniform(SrtTransform *transform, int scale);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int arg, SrtTransform *transform);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

void Ov146_OnSpawnMessage(char *self, u8 *msg, int arg)
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
            Srt_SetScaleUniform(&transform, 0x2000);
            ((struct Ov146Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeXformTask(
                *(int *)(self + 0x3c), ((struct Ov146Effects *)self)->pair[msg[3]].res, 0x17, 0, &transform);
            break;
        case 1:
        case 2:
            Srt_SetTranslation(&transform, (VecFx32 *)(*(int *)(*(int *)(self + 0x3b8) + 0x3b8) + 0x14));
            Srt_SetScaleUniform(&transform, 0x2000);
            ((struct Ov146Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeXformTask(
                *(int *)(self + 0x3c), ((struct Ov146Effects *)self)->pair[msg[3]].res, 0x17,
                (u8)(msg[3] == 1 ? 0 : 2), &transform);
            break;
        case 4:
            ((struct Ov146Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeBodyTask(
                *(int *)(self + 0x3c), ((struct Ov146Effects *)self)->pair[msg[3]].res, 0x17, self + 0xa0, 0, 0);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
