/* Effect message hook of the ov248 enemy. A "spawned" message (kind 5) carries
 * three big-endian signed 24-bit coordinates in bytes 5 .. 0xd, unpacked into the translation of a
 * fresh transform; byte 3 picks the +0x4a8 pair: slot 0 (transform scaled to 1.0, flag 1) and
 * slots 1/6/7/8 (flag byte 4) spawn kind 0x17 at the transform, slot 6 also sets pose 1; slot 2
 * anchors on the +0x40c point, slots 3/9 on the actor's +0xa0 pose, slot 4 on the +0x438 point
 * (looping when byte 4 is 1) and slot 5 on the +0x3e0 point, all with byte 4; slot 0xb starts
 * reaction 0x146 mode 9 on the +0xa0 pose into +0x4a4. The base hook always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } Srt;

extern void SrtTransform_SetIdentity(Srt *srt);
extern void Srt_SetTranslation(Srt *srt, const VecFx32 *t);
extern void Srt_SetScaleUniform(Srt *srt, int scale);
extern void *Ov107_CreateNodeXformTask(void *owner, void *slot, int kind, int flag, const Srt *srt);
extern void Ov107_ForwardVisibleEvent(void *self, int a2);
extern void *Ov107_CreateNodeBodyTask(void *owner, void *slot, int kind, void *xform, int a5, int a6);
extern void *Ov107_CreateSpawnTask(void *self, int id, int a3, int a4, void *xform);
extern void Ov107_AiState_OnMessage(void *self, void *cmd, void *arg3);

void Ov248_OnEffectMessage(char *self, u8 *cmd, void *arg3)
{
    int rawZ;
    int rawY;
    int rawX;
    Srt srt;
    VecFx32 pos;

    if (cmd[2] == 5) {
        ((char *)&rawX)[3] = cmd[5];
        ((char *)&rawX)[2] = cmd[6];
        ((char *)&rawX)[1] = cmd[7];
        pos.x = rawX >> 8;
        ((char *)&rawY)[3] = cmd[8];
        ((char *)&rawY)[2] = cmd[9];
        ((char *)&rawY)[1] = cmd[10];
        pos.y = rawY >> 8;
        ((char *)&rawZ)[3] = cmd[11];
        ((char *)&rawZ)[2] = cmd[12];
        ((char *)&rawZ)[1] = cmd[13];
        pos.z = rawZ >> 8;
        SrtTransform_SetIdentity(&srt);
        Srt_SetTranslation(&srt, &pos);
        switch (cmd[3]) {
        case 0:
            Srt_SetScaleUniform(&srt, 0x1000);
        case 1:
        case 6:
        case 7:
        case 8:
            *(void **)(self + (cmd[3] << 3) + 0x4ac) =
                Ov107_CreateNodeXformTask(*(void **)(self + 0x3c),
                                          *(void **)(self + (cmd[3] << 3) + 0x4a8), 0x17,
                                          (u8)(cmd[3] == 0 ? 1 : cmd[4]), &srt);
            if (cmd[3] == 6) {
                Ov107_ForwardVisibleEvent(self, 1);
            }
            break;
        case 2:
            *(void **)(self + (cmd[3] << 3) + 0x4ac) =
                Ov107_CreateNodeBodyTask(*(void **)(self + 0x3c),
                                    *(void **)(self + (cmd[3] << 3) + 0x4a8),
                                    0x17, self + 0x40c, cmd[4], 0);
            break;
        case 3:
        case 9:
            *(void **)(self + (cmd[3] << 3) + 0x4ac) =
                Ov107_CreateNodeBodyTask(*(void **)(self + 0x3c),
                                    *(void **)(self + (cmd[3] << 3) + 0x4a8),
                                    0x17, self + 0xa0, cmd[4], 0);
            break;
        case 4:
            *(void **)(self + (cmd[3] << 3) + 0x4ac) =
                Ov107_CreateNodeBodyTask(*(void **)(self + 0x3c),
                                    *(void **)(self + (cmd[3] << 3) + 0x4a8),
                                    0x17, self + 0x438, cmd[4], cmd[4] == 1);
            break;
        case 5:
            *(void **)(self + (cmd[3] << 3) + 0x4ac) =
                Ov107_CreateNodeBodyTask(*(void **)(self + 0x3c),
                                    *(void **)(self + (cmd[3] << 3) + 0x4a8),
                                    0x17, self + 0x3e0, cmd[4], 0);
            break;
        case 0xb:
            *(void **)(self + 0x4a4) = Ov107_CreateSpawnTask(self, 0x146, 9, 0, self + 0xa0);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, cmd, arg3);
}
