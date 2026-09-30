/* Ov216_HandleSpawnMessage: ported from a matched sibling family (same shape, constants and offsets adjusted). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } Srt;

extern void SrtTransform_SetIdentity(Srt *srt);
extern void Srt_SetTranslation(Srt *srt, const VecFx32 *t);
extern void Srt_SetScaleUniform(Srt *srt, int scale);
extern void *Ov107_CreateNodeBodyTask(void *owner, void *slot, int flags, void *xform, int a5,
                                      int a6);
extern void *Ov107_CreateNodeXformTask(void *owner, void *slot, int kind, int a4,
                                 const Srt *srt);
extern void Ov107_ForwardVisibleEvent(void *self, int a2);
extern void *Ov107_CreateSpawnTask(void *self, int id, int a3, int a4, void *xform);
extern void Ov216_releaseHandles(void *self);
extern void Ov216_initEntrySlotAdvanceIndex(void *node, const VecFx32 *pos);
extern void Ov107_AiState_OnMessage(void *self, void *cmd, void *arg3);

void Ov216_HandleSpawnMessage(char *self, u8 *cmd, void *arg3)
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
            *(void **)(self + (cmd[3] << 3) + 0x444) =
                Ov107_CreateNodeBodyTask(*(void **)(self + 0x3c),
                                         *(void **)(self + (cmd[3] << 3) + 0x440), 0x17,
                                         self + 0x3f4, 3, 0);
            break;
        case 1:
            Srt_SetScaleUniform(&srt, 0x199a);
            *(void **)(self + (cmd[3] << 3) + 0x444) =
                Ov107_CreateNodeXformTask(*(void **)(self + 0x3c),
                                    *(void **)(self + (cmd[3] << 3) + 0x440),
                                    0x15, 0, &srt);
            break;
        case 3:
            Srt_SetScaleUniform(&srt, 0x199a);
            *(void **)(self + (cmd[3] << 3) + 0x444) =
                Ov107_CreateNodeXformTask(*(void **)(self + 0x3c),
                                    *(void **)(self + (cmd[3] << 3) + 0x440),
                                    0x15, 0, &srt);
            Ov107_ForwardVisibleEvent(self, 1);
            break;
        case 2:
        case 4:
            *(void **)(self + (cmd[3] << 3) + 0x444) =
                Ov107_CreateNodeBodyTask(*(void **)(self + 0x3c),
                                         *(void **)(self + (cmd[3] << 3) + 0x440),
                                         (u8)(((cmd[3] == 2) ? 0 : 4) | 1), self + 0x3c8, 0, 1);
            if (cmd[3] == 2) {
                *(void **)(self + 0x43c) =
                    Ov107_CreateSpawnTask(self, 0x144, 5, 1, self + 0x3c8);
            }
            break;
        case 5:
            break;
        case 6:
            Ov216_releaseHandles(self);
            break;
        case 7:
            Ov216_initEntrySlotAdvanceIndex(*(void **)(self + 0x3c0), &pos);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, cmd, arg3);
}
