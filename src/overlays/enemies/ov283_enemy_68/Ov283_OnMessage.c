/* Message handler of the ov283 actor: a spawn message (kind 5) unpacks its position into a transform
 * and, for sub 0, starts the +0x390 effect pair there (kind 0xf). The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;
struct EffectPair { int res; int handle; };
struct Ov283Effects { char pad[0x390]; struct EffectPair pair[1]; };

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int arg, SrtTransform *transform);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

void Ov283_OnMessage(char *self, u8 *msg, int arg)
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
        if (msg[3] == 0) {
            ((struct Ov283Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeXformTask(
                *(int *)(self + 0x3c), ((struct Ov283Effects *)self)->pair[msg[3]].res, 0xf, 0, &transform);
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
