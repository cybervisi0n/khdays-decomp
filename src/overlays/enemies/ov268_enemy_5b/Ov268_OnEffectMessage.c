/* Effect message hook of the ov208 enemy (x3 with ov209/ov268). A "spawned" message (kind 5)
 * carries three big-endian signed 24-bit coordinates in bytes 5 .. 0xd, unpacked into the
 * translation of a fresh transform; byte 3 then picks the +0x40c pair: slot 0 spawns pair 0 on
 * the +0x3e0 point (kind 0x17, param 3), slots 2/3 spawn pair 2 on the actor's +0xa0 pose (flag
 * set for slot 3), slots 1/4/5 spawn pair 1/3/4 at the transform, and slot 5 also sets pose 1.
 * The base hook always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;
struct Pair { int res; int handle; };

extern void SrtTransform_SetIdentity(SrtTransform *t);
extern void Srt_SetTranslation(SrtTransform *t, const VecFx32 *pos);
extern int Ov107_CreateNodeXformTask(int model, int res, int kind, int zero, SrtTransform *t);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

void Ov268_OnEffectMessage(char *self, u8 *msg, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    union {
        int words[3];
        u8 bytes[12];
    } packed;
    u8 slot;

    if (msg[2] == 5) {
        SrtTransform_SetIdentity(&transform);
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
        switch (msg[3]) {
        case 0:
            (*(struct Pair **)(self + 0x40c))[0].handle =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), (*(struct Pair **)(self + 0x40c))[0].res, 0x17, self + 0x3e0, 3, 0);
            break;
        case 2:
        case 3:
            (*(struct Pair **)(self + 0x40c))[2].handle =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), (*(struct Pair **)(self + 0x40c))[2].res, 0x17, self + 0xa0, (u8)(msg[3] == 3), 0);
            break;
        case 1:
        case 4:
        case 5:
            slot = msg[3] == 1 ? 1 : (msg[3] == 4 ? 3 : 4);
            (*(struct Pair **)(self + 0x40c))[slot].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), (*(struct Pair **)(self + 0x40c))[slot].res, 0x17, 0, &transform);
            if (msg[3] == 5) {
                Ov107_ForwardVisibleEvent(self, 1);
            }
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
