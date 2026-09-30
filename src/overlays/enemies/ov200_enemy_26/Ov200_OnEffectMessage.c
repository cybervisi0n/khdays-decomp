/* Effect message hook of the ov200 enemy (x3 with ov201/ov271). A "spawned" message (kind 5) picks
 * by byte 3: slot 0 spawns pair 0 (kind 5) at the packed position (bytes 5..0xd, big-endian
 * signed 24-bit); slot 7 does the same with the transform scaled 1.5 and kind 0x15 into pair 2,
 * or pair 3 while pair 2's effect is still alive (0203c6e0); slots 1/3 enable the first (1) or the
 * two other (3) +0x390 parts (Ov200_SetNodeActiveState) and start reaction 0x157 mode 7 on the +0xa0 pose
 * into +0x3b0, slots 2/4 disable them again; slots 5/6 start modes 4/5 (looping) into +0x3b4/
 * +0x3b8. The base hook always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;
struct Pair { int res; int handle; };

extern void SrtTransform_SetIdentity(SrtTransform *t);
extern void Srt_SetTranslation(SrtTransform *t, const VecFx32 *pos);
extern void Srt_SetScaleUniform(SrtTransform *t, int scale);
extern int Ov107_CreateNodeXformTask(int model, int res, int kind, int zero, SrtTransform *t);
extern int FindListEntryByField1c(int model, int handle);
extern void Ov200_SetNodeActiveState(int part, int on);
extern int Ov107_CreateSpawnTask(char *self, int id, int mode, int flag, void *pose);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

void Ov200_OnEffectMessage(char *self, u8 *msg, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    union {
        int words[3];
        u8 bytes[12];
    } packedA;
    union {
        int words[3];
        u8 bytes[12];
    } packedB;
    int i;

    if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
            SrtTransform_SetIdentity(&transform);
            packedA.bytes[3] = msg[5];
            packedA.bytes[2] = msg[6];
            packedA.bytes[1] = msg[7];
            translation.x = packedA.words[0] >> 8;
            packedA.bytes[7] = msg[8];
            packedA.bytes[6] = msg[9];
            packedA.bytes[5] = msg[0xa];
            translation.y = packedA.words[1] >> 8;
            packedA.bytes[11] = msg[0xb];
            packedA.bytes[10] = msg[0xc];
            packedA.bytes[9] = msg[0xd];
            translation.z = packedA.words[2] >> 8;
            Srt_SetTranslation(&transform, &translation);
            (*(struct Pair **)(self + 0x3a4))[0].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), (*(struct Pair **)(self + 0x3a4))[0].res, 5, 0, &transform);
            break;
        case 7:
            SrtTransform_SetIdentity(&transform);
            Srt_SetScaleUniform(&transform, 0x1800);
            packedB.bytes[3] = msg[5];
            packedB.bytes[2] = msg[6];
            packedB.bytes[1] = msg[7];
            translation.x = packedB.words[0] >> 8;
            packedB.bytes[7] = msg[8];
            packedB.bytes[6] = msg[9];
            packedB.bytes[5] = msg[0xa];
            translation.y = packedB.words[1] >> 8;
            packedB.bytes[11] = msg[0xb];
            packedB.bytes[10] = msg[0xc];
            packedB.bytes[9] = msg[0xd];
            translation.z = packedB.words[2] >> 8;
            Srt_SetTranslation(&transform, &translation);
            if (FindListEntryByField1c(*(int *)(self + 0x3c), (*(struct Pair **)(self + 0x3a4))[2].handle) == 0) {
                (*(struct Pair **)(self + 0x3a4))[2].handle =
                    Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), (*(struct Pair **)(self + 0x3a4))[2].res, 0x15, 0, &transform);
            } else {
                (*(struct Pair **)(self + 0x3a4))[3].handle =
                    Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), (*(struct Pair **)(self + 0x3a4))[3].res, 0x15, 0, &transform);
            }
            break;
        case 1:
            Ov200_SetNodeActiveState(*(int *)(self + 0x390), 1);
            *(int *)(self + 0x3b0) = Ov107_CreateSpawnTask(self, 0x157, 7, 0, self + 0xa0);
            break;
        case 2:
            Ov200_SetNodeActiveState(*(int *)(self + 0x390), 0);
            break;
        case 3:
            for (i = 1; i < 3; i++) {
                Ov200_SetNodeActiveState(((int *)(self + 0x390))[i], 1);
            }
            *(int *)(self + 0x3b0) = Ov107_CreateSpawnTask(self, 0x157, 7, 0, self + 0xa0);
            break;
        case 4:
            for (i = 1; i < 3; i++) {
                Ov200_SetNodeActiveState(((int *)(self + 0x390))[i], 0);
            }
            break;
        case 5:
            *(int *)(self + 0x3b4) = Ov107_CreateSpawnTask(self, 0x157, 4, 1, self + 0xa0);
            break;
        case 6:
            *(int *)(self + 0x3b8) = Ov107_CreateSpawnTask(self, 0x157, 5, 1, self + 0xa0);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
