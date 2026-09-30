/* Message handler of the ov273 enemy (x2 with ov273). A kind-0 message copies its +0x24 flag into
 * bit 1 of the +0x388 tail rig's +0x5c and re-inits it. Kind-5 messages drive the +0x430 effect
 * table: 0 plays pair 0 (kind 0x17, the message's scale byte and point); 1 attaches it at +0x3f8
 * and, when the +4 target is armed (+0x40 bits 0-1), spawns slot effect 0x162/4 at the message's
 * unpacked 24-bit point, plus pair 1 when the scale byte is 1; 2 plays pair 4 (scale 3.0) and
 * raises the ground flag (020c0b14); 3 attaches pairs 5 / 6 at the +0x3f0 / +0x3f4 bones; 4 drops
 * each idle +0x3e4 child (sub-state 0) to the ground below the point at the message's per-child
 * angle and distance around the actor and runs its +0x1cc hook; 5 attaches pair 7 at the body
 * rig's +0x30; 6 plays pair 2 at the unpacked point; 7 / 9 / 8 register effect 0x162 (modes
 * 0xc / 5 / 0xa) on the +0xa0 pose into +0x424 / +0x428 / +0x42c. The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } Srt;
struct Pair { int res; int handle; };
struct Bit0 { int b0 : 1; };
struct Bits5c { unsigned int b0 : 1; unsigned int b1 : 1; };
struct Bits40 { int b0 : 1; int b1 : 1; };

extern void RefreshObjectCallbacks(int item, int a);
extern int Ov107_CreateNodeXformTaskFx24(int model, int res, int kind, int arg, int scale, void *pos);
extern int Ov107_CreateNodeBodyTask(int model, int res, int kind, void *at, int a, int b);
extern int Slot_Spawn(int slot, int id, VecFx32 *pos, unsigned int flags);
extern void Ov107_ForwardVisibleEvent(char *self, int a);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Collision_CastRay(int grid, VecFx32 *pos, VecFx32 *ray);
extern void VEC_Add(const void *a, const void *b, void *out);
extern void Srt_SetTranslation(void *srt, VecFx32 *pos);
extern void SrtTransform_SetIdentity(Srt *srt);
extern int Ov107_CreateSpawnTask(char *self, int id, int mode, int flag, void *pose);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);
extern const short data_0203d210[];
extern const VecFx32 data_02042240;

#define PAIRS (*(struct Pair **)(self + 0x430))
#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov273_HandleMessage(char *self, u8 *msg, int arg)
{
    VecFx32 pos;
    Srt srt;
    VecFx32 at;
    VecFx32 ray;
    int atZ;
    int atY;
    int atX;
    int rawZ;
    int rawY;
    int rawX;
    int actor;
    int hit;
    int i;

    if (msg[2] == 0) {
        ((struct Bits5c *)(*(int *)(self + 0x388) + 0x5c))->b1 = ((struct Bit0 *)(msg + 0x24))->b0;
        RefreshObjectCallbacks(*(int *)(self + 0x388), 0);
    } else if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
            PAIRS[0].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[0].res, 0x17, msg[4], 0x1000, msg + 5);
            break;
        case 1:
            PAIRS[0].handle = Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), PAIRS[0].res, 0x17, self + 0x3f8, msg[4], 0);
            if (*(int *)(self + 4) != 0 && ((struct Bits40 *)(*(int *)(self + 4) + 0x40))->b0 &&
                ((struct Bits40 *)(*(int *)(self + 4) + 0x40))->b1) {
                ((char *)&atX)[3] = msg[5];
                ((char *)&atX)[2] = msg[6];
                ((char *)&atX)[1] = msg[7];
                at.x = atX >> 8;
                ((char *)&atY)[3] = msg[8];
                ((char *)&atY)[2] = msg[9];
                ((char *)&atY)[1] = msg[10];
                at.y = atY >> 8;
                ((char *)&atZ)[3] = msg[11];
                ((char *)&atZ)[2] = msg[12];
                ((char *)&atZ)[1] = msg[13];
                at.z = atZ >> 8;
                Slot_Spawn(0x162, 4, &at, 0);
            }
            if (msg[4] == 1) {
                PAIRS[1].handle = Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), PAIRS[1].res, 0x17, self + 0x3f8, 0, 0);
            }
            break;
        case 2:
            PAIRS[4].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[4].res, 0x17, 0, 0x3000, msg + 5);
            Ov107_ForwardVisibleEvent(self, 1);
            break;
        case 3:
            PAIRS[5].handle = Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), PAIRS[5].res, 0x17, (void *)(*(int *)(self + 0x3f0) + 4), 0, 0);
            PAIRS[6].handle = Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), PAIRS[6].res, 0x17, (void *)(*(int *)(self + 0x3f4) + 4), 0, 0);
            break;
        case 4: {
            actor = *(int *)(self + 4);
            for (i = 0; i < 8; i++) {
                if (*(signed char *)((*(int **)(self + 0x3e4))[i] + 0x1c6) != 0) {
                    continue;
                }
                pos.x = *(int *)(self + 0xb0) + FX_Mul(data_0203d210[ANG2IDX(((int *)(msg + 4))[i]) * 2], ((int *)(msg + 0x24))[i]);
                pos.y = *(int *)(self + 0xb4) + 0x8000;
                pos.z = *(int *)(self + 0xb8) + FX_Mul(data_0203d210[ANG2IDX(((int *)(msg + 4))[i]) * 2 + 1], ((int *)(msg + 0x24))[i]);
                ScaleVec3Fx12(0x10000, &data_02042240, &ray);
                hit = Collision_CastRay(*(int *)(actor + 0x7c), &pos, &ray);
                if (hit == 0 || *(int *)(hit + 8) != 0) {
                    continue;
                }
                ray.y = (int)(((long long)*(int *)(hit + 0xc) * ray.y) >> 27);
                VEC_Add(&pos, &ray, &pos);
                pos.y += 0x100;
                Srt_SetTranslation((void *)((*(int **)(self + 0x3e4))[i] + 0xa0), &pos);
                if (*(void (**)(int, int))((*(int **)(self + 0x3e4))[i] + 0x1cc) != 0) {
                    (*(void (**)(int, int))((*(int **)(self + 0x3e4))[i] + 0x1cc))((*(int **)(self + 0x3e4))[i], 0);
                }
            }
            break;
        }
        case 5:
            PAIRS[7].handle = Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), PAIRS[7].res, 0x17, (void *)(*(int *)(self + 0x384) + 0x30), 0, 0);
            break;
        case 6: {
            SrtTransform_SetIdentity(&srt);
            ((char *)&rawX)[3] = msg[5];
            ((char *)&rawX)[2] = msg[6];
            ((char *)&rawX)[1] = msg[7];
            pos.x = rawX >> 8;
            ((char *)&rawY)[3] = msg[8];
            ((char *)&rawY)[2] = msg[9];
            ((char *)&rawY)[1] = msg[10];
            pos.y = rawY >> 8;
            ((char *)&rawZ)[3] = msg[11];
            ((char *)&rawZ)[2] = msg[12];
            ((char *)&rawZ)[1] = msg[13];
            pos.z = rawZ >> 8;
            Srt_SetTranslation(&srt, &pos);
            PAIRS[2].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[2].res, 0x17, 0, 0x1000, msg + 5);
            break;
        }
        case 7:
            *(int *)(self + 0x424) = Ov107_CreateSpawnTask(self, 0x162, 0xc, 0, self + 0xa0);
            break;
        case 9:
            *(int *)(self + 0x428) = Ov107_CreateSpawnTask(self, 0x162, 5, 0, self + 0xa0);
            break;
        case 8:
            *(int *)(self + 0x42c) = Ov107_CreateSpawnTask(self, 0x162, 0xa, 0, self + 0xa0);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
