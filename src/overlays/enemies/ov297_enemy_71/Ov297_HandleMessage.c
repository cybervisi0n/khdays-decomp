/* Message handler of the ov297 enemy: a "spawned" message (kind 5): payload 0 builds a fresh
 * transform scaled by 2.0 at the packet's 24-bit position and starts the +0x398 sub-item of
 * that slot under the +0x3c owner (kind 0x17, the packet's blend) into +0x39c; payload 1 does the
 * same unscaled with kind 7 and no blend. The base handler
 * always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetScaleUniform(SrtTransform *transform, int scale);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int blend,
                                     SrtTransform *transform);
extern void Ov107_AiState_OnMessage(int owner, u8 *msg, int arg);

void Ov297_HandleMessage(int owner, u8 *msg, int arg)
{
    SrtTransform transform;
    VecFx32 translation;

    if (msg[2] == 5) {
        switch (msg[3]) {
        case 0: {
            union {
                int words[3];
                u8 bytes[12];
            } packed;
            SrtTransform_SetIdentity(&transform);
            Srt_SetScaleUniform(&transform, 0x2000);
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
            Srt_SetTranslation(&transform, &translation);
            *(int *)(owner + msg[3] * 8 + 0x39c) =
                Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), *(int *)(owner + msg[3] * 8 + 0x398), 0x17, msg[4], &transform);
            break;
        }
        case 1: {
            union {
                int words[3];
                u8 bytes[12];
            } packed;
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
            *(int *)(owner + msg[3] * 8 + 0x39c) =
                Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), *(int *)(owner + msg[3] * 8 + 0x398), 7, 0, &transform);
            break;
        }
        }
    }
    Ov107_AiState_OnMessage(owner, msg, arg);
}
