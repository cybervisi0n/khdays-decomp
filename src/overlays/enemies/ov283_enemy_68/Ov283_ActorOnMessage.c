/* Message handler of the ov283 actor: a spawn message (kind 5) unpacks its position into a transform
 * and starts the +0x3ec effect pair of the sub id: 0 and 1 at the position scaled 2.0, 2 and 4 on
 * the left hand (+0x394), 3 and 5 on the right hand (+0x398). The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;
struct EffectPair { int res; int handle; };
struct Ov283Effects { char pad[0x3ec]; struct EffectPair pair[6]; };

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern void Srt_SetScaleUniform(SrtTransform *transform, int scale);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int arg, SrtTransform *transform);
extern int Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

static inline void UnpackPosition(VecFx32 *out, const u8 *msg)
{
    union {
        int words[3];
        u8 bytes[12];
    } packed;

    packed.bytes[3] = msg[5];
    packed.bytes[2] = msg[6];
    packed.bytes[1] = msg[7];
    out->x = packed.words[0] >> 8;
    packed.bytes[7] = msg[8];
    packed.bytes[6] = msg[9];
    packed.bytes[5] = msg[0xa];
    out->y = packed.words[1] >> 8;
    packed.bytes[11] = msg[0xb];
    packed.bytes[10] = msg[0xc];
    packed.bytes[9] = msg[0xd];
    out->z = packed.words[2] >> 8;
}

int Ov283_ActorOnMessage(char *self, u8 *msg, int arg)
{
    SrtTransform transform;
    VecFx32 translation;

    if (msg[2] == 5) {
        UnpackPosition(&translation, msg);
        SrtTransform_SetIdentity(&transform);
        Srt_SetTranslation(&transform, &translation);
        switch (msg[3]) {
        case 0:
        case 1:
            SrtTransform_SetIdentity(&transform);
            Srt_SetScaleUniform(&transform, 0x2000);
            UnpackPosition(&translation, msg);
            Srt_SetTranslation(&transform, &translation);
            ((struct Ov283Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeXformTask(
                *(int *)(self + 0x3c), ((struct Ov283Effects *)self)->pair[msg[3]].res, 0x17, 0, &transform);
            break;
        case 2:
        case 3:
            if (msg[3] == 2) {
                ((struct Ov283Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeBodyTask(
                    *(int *)(self + 0x3c), ((struct Ov283Effects *)self)->pair[msg[3]].res, 5,
                    (void *)(*(int *)(self + 0x394) + 4), 0, 0);
            } else {
                ((struct Ov283Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeBodyTask(
                    *(int *)(self + 0x3c), ((struct Ov283Effects *)self)->pair[msg[3]].res, 5,
                    (void *)(*(int *)(self + 0x398) + 4), 0, 0);
            }
            break;
        case 4:
        case 5:
            if (msg[3] == 4) {
                ((struct Ov283Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeBodyTask(
                    *(int *)(self + 0x3c), ((struct Ov283Effects *)self)->pair[msg[3]].res, 5,
                    (void *)(*(int *)(self + 0x394) + 4), 0, 0);
            } else {
                ((struct Ov283Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeBodyTask(
                    *(int *)(self + 0x3c), ((struct Ov283Effects *)self)->pair[msg[3]].res, 5,
                    (void *)(*(int *)(self + 0x398) + 4), 0, 0);
            }
            break;
        }
    }
    return Ov107_AiState_OnMessage(self, msg, arg);
}
