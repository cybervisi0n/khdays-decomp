/* Message handler of the ov223 enemy. A kind-0 message, outside owner mode 1, unpacks its
 * 24-bit position (bytes 0x24..0x2c) into the +0xbc anchor and offsets the +0xa0 pose by it
 * (0203ca74). A "spawned" message (kind 5) with slot 0 unpacks the 0x5 position into a fresh
 * transform and starts the +0x398 sub-item under the +0x3c owner with kind 0x17 into +0x39c;
 * with slot 2 the 0x14 position goes to the +0x390 ring's writer (ov223 4e70) along with the
 * message's +4 pose. The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;
typedef struct { int q[4]; } Quat;
typedef union { int words[3]; u8 bytes[12]; } Packed;

extern void Srt_SetScaleVec(void *pose, const VecFx32 *offset);
extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int zero, SrtTransform *transform);
extern void Ov223_WriteRingSlot(int *ring, const VecFx32 *pPoint, const Quat *pPose);
extern void Ov107_AiState_OnMessage(int owner, u8 *msg, int arg);

void Ov223_HandleMessage(int owner, u8 *msg, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    Packed anchor;
    Packed spawn;
    Packed ring;

    if (msg[2] == 0) {
        if (*(int *)(owner + 0x50) != 1) {
            anchor.bytes[3] = msg[0x24];
            anchor.bytes[2] = msg[0x25];
            anchor.bytes[1] = msg[0x26];
            *(int *)(owner + 0xbc) = anchor.words[0] >> 8;
            anchor.bytes[7] = msg[0x27];
            anchor.bytes[6] = msg[0x28];
            anchor.bytes[5] = msg[0x29];
            *(int *)(owner + 0xc0) = anchor.words[1] >> 8;
            anchor.bytes[11] = msg[0x2a];
            anchor.bytes[10] = msg[0x2b];
            anchor.bytes[9] = msg[0x2c];
            *(int *)(owner + 0xc4) = anchor.words[2] >> 8;
            Srt_SetScaleVec((void *)(owner + 0xa0), (VecFx32 *)(owner + 0xbc));
        }
    } else if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
            spawn.bytes[3] = msg[5];
            spawn.bytes[2] = msg[6];
            spawn.bytes[1] = msg[7];
            translation.x = spawn.words[0] >> 8;
            spawn.bytes[7] = msg[8];
            spawn.bytes[6] = msg[9];
            spawn.bytes[5] = msg[0xa];
            translation.y = spawn.words[1] >> 8;
            spawn.bytes[11] = msg[0xb];
            spawn.bytes[10] = msg[0xc];
            spawn.bytes[9] = msg[0xd];
            translation.z = spawn.words[2] >> 8;
            SrtTransform_SetIdentity(&transform);
            Srt_SetTranslation(&transform, &translation);
            *(int *)(owner + msg[3] * 8 + 0x39c) =
                Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c), *(int *)(owner + msg[3] * 8 + 0x398), 0x17, 0, &transform);
            break;
        case 2:
            ring.bytes[3] = msg[0x14];
            ring.bytes[2] = msg[0x15];
            ring.bytes[1] = msg[0x16];
            translation.x = ring.words[0] >> 8;
            ring.bytes[7] = msg[0x17];
            ring.bytes[6] = msg[0x18];
            ring.bytes[5] = msg[0x19];
            translation.y = ring.words[1] >> 8;
            ring.bytes[11] = msg[0x1a];
            ring.bytes[10] = msg[0x1b];
            ring.bytes[9] = msg[0x1c];
            translation.z = ring.words[2] >> 8;
            Ov223_WriteRingSlot(*(int **)(owner + 0x390), &translation, (const Quat *)(msg + 4));
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, msg, arg);
}
