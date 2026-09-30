/* Message hook of the ov259 enemy. A kind-5 message spawns, by byte 3, into the +0x430 item/handle
 * pair table: slots 0/6/10/11 effect 7 at the packed position (bytes 5..0xd, three big-endian signed
 * 24-bit values), slots 9/12 effect 0x17 scaled 3.0 there (byte 4 as flag), slot 14 effect 0x15 into
 * pair 2 at the packed position oriented by the +0x384 body's +0x124 axis; slots 1 and 4 switch the
 * body's glow (020d1764 on / off) and anchor effect 0xf on the +0xa0 pose, slots 2/3 effect 0x15 there;
 * slots 5 and 7 anchor effects 1 / 7 on the +0x410 bone, slot 8 a looping effect 7 on the +0xa0 pose;
 * slot 15 raises the owner flag (020c0b14), and slots 16/17 hide / show the +0x390 wing rig (+0x42c).
 * The base hook always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int w[4]; } Quat;
typedef struct { int w[11]; } SrtTransform;
typedef union { int words[3]; u8 bytes[12]; } Packed;
struct Pair { int res; int handle; };
struct Ov259 { char pad[0x430]; struct Pair pairs[13]; };

extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *forward, const VecFx32 *direction);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetRotationQuat(SrtTransform *t, const Quat *q);
extern void SrtTransform_SetIdentity(SrtTransform *t);
extern void Srt_SetTranslation(SrtTransform *t, const VecFx32 *pos);
extern void Srt_SetScaleUniform(SrtTransform *t, int scale);
extern void Srt_SetScaleXYZ(void *pose, int x, int y, int z);
extern int Ov107_CreateNodeXformTask(int model, int res, int kind, int flag, void *at);
extern void Ov259_SwapShells(int body, int on);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);
extern const VecFx32 data_02042264;

void Ov259_OnMessage(char *self, u8 *msg, int arg)
{
    SrtTransform t;
    VecFx32 v;
    Quat spin;
    Quat q;
    Packed packedA;
    Packed packedB;
    Packed packedC;

    if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
        case 6:
        case 10:
        case 11:
            packedA.bytes[3] = msg[5];
            packedA.bytes[2] = msg[6];
            packedA.bytes[1] = msg[7];
            v.x = packedA.words[0] >> 8;
            packedA.bytes[7] = msg[8];
            packedA.bytes[6] = msg[9];
            packedA.bytes[5] = msg[0xa];
            v.y = packedA.words[1] >> 8;
            packedA.bytes[11] = msg[0xb];
            packedA.bytes[10] = msg[0xc];
            packedA.bytes[9] = msg[0xd];
            v.z = packedA.words[2] >> 8;
            SrtTransform_SetIdentity(&t);
            Srt_SetTranslation(&t, &v);
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 7, 0, &t);
            break;
        case 1:
            Ov259_SwapShells(*(int *)(self + 0x384), 1);
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 0xf, msg[4], self + 0xa0);
            break;
        case 4:
            Ov259_SwapShells(*(int *)(self + 0x384), 0);
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 0xf, msg[4], self + 0xa0);
            break;
        case 9:
        case 12:
            SrtTransform_SetIdentity(&t);
            Srt_SetScaleUniform(&t, 0x3000);
            packedB.bytes[3] = msg[5];
            packedB.bytes[2] = msg[6];
            packedB.bytes[1] = msg[7];
            v.x = packedB.words[0] >> 8;
            packedB.bytes[7] = msg[8];
            packedB.bytes[6] = msg[9];
            packedB.bytes[5] = msg[0xa];
            v.y = packedB.words[1] >> 8;
            packedB.bytes[11] = msg[0xb];
            packedB.bytes[10] = msg[0xc];
            packedB.bytes[9] = msg[0xd];
            v.z = packedB.words[2] >> 8;
            Srt_SetTranslation(&t, &v);
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 0x17, msg[4], &t);
            break;
        case 2:
        case 3:
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 0x15, msg[4], self + 0xa0);
            break;
        case 14:
            packedC.bytes[3] = msg[5];
            packedC.bytes[2] = msg[6];
            packedC.bytes[1] = msg[7];
            v.x = packedC.words[0] >> 8;
            packedC.bytes[7] = msg[8];
            packedC.bytes[6] = msg[9];
            packedC.bytes[5] = msg[0xa];
            v.y = packedC.words[1] >> 8;
            packedC.bytes[11] = msg[0xb];
            packedC.bytes[10] = msg[0xc];
            packedC.bytes[9] = msg[0xd];
            v.z = packedC.words[2] >> 8;
            SrtTransform_SetIdentity(&t);
            Srt_SetTranslation(&t, &v);
            QuatFromAxisAngle(&spin, &data_02042264, 0);
            Quat_FromTwoVectors(&q, &data_02042264, (VecFx32 *)(*(int *)(self + 0x384) + 0x124));
            Quat_Multiply(&q, &q, &spin);
            Srt_SetRotationQuat(&t, &q);
            ((struct Ov259 *)self)->pairs[2].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[2].res, 0x15, msg[4], &t);
            break;
        case 5:
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 1,
                                    (void *)(*(int *)(self + 0x410) + 4), 0, 0);
            break;
        case 7:
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 7,
                                    (void *)(*(int *)(self + 0x410) + 4), 0, 0);
            break;
        case 8:
            ((struct Ov259 *)self)->pairs[msg[3]].handle =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), ((struct Ov259 *)self)->pairs[msg[3]].res, 7,
                                    self + 0xa0, 0, 1);
            break;
        case 15:
            Ov107_ForwardVisibleEvent(self, 1);
            break;
        case 16:
            Srt_SetScaleXYZ((void *)(*(int *)(self + 0x390) + 4), 0, 0, 0);
            *(int *)(self + 0x42c) = 0;
            break;
        case 17:
            Srt_SetScaleXYZ((void *)(*(int *)(self + 0x390) + 4), 0x1000, 0x1000, 0x1000);
            *(int *)(self + 0x42c) = 1;
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
