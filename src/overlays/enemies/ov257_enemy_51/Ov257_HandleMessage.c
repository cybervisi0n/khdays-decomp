/* Message handler of the ov257 enemy (+0x1c). Kind-5 messages drive its effects, kept as
 * resource/handle pairs in the +0x400 table: 0 starts the wing effect on the +0x3d8 part
 * (Ov257_SpawnChildStoreTwoArgs, pair 4), 1 the trail between pairs 2 and 3 on the +0x3d4 part
 * (Ov257_SpawnTrailHelper), 2 stops the trail, 3/4/5 play pairs 0, 1 and 5 (kind 0x17) and 6 pair
 * 7 (kind 5) at the message point, 8 starts the partner effect (Ov257_SpawnChild10AndBackLink, pair 9) and
 * attaches pair 6 to the +0x3d4 part, 9 and 0xa play pairs 10 and 11 (kind 5), 0xb starts pair 8
 * at the message point (Ov257_SpawnHelper2, unpacked from its three 24-bit coordinates), 0xc
 * starts reaction +0x408 mode 0xd on the +0xa0 pose (+0x40c) and 0xd ends it. Every message then
 * goes to the common handler. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Pair { int res; int handle; };

extern int Ov257_SpawnChildStoreTwoArgs(char *self, int res, void *at);
extern int Ov257_SpawnTrailHelper(char *self, int res, int res2, void *at);
extern void TaskList_FinishByTag(int model, int handle);
extern int Ov107_CreateNodeXformTaskFx24(int model, int res, int kind, int arg, int scale, void *pos);
extern int Ov107_CreateNodeBodyTask(int model, int res, int kind, void *at, int a, int b);
extern int Ov257_SpawnHelper2(char *self, int res, VecFx32 *at);
extern int Ov257_SpawnChild10AndBackLink(char *self);
extern int Ov107_CreateSpawnTask(char *self, int id, int mode, int flag, void *pose);
extern void Ov107_UnlinkNodeFromOwner(int handle);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

#define PAIRS (*(struct Pair **)(self + 0x400))

void Ov257_HandleMessage(char *self, u8 *msg, int arg)
{
    if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
            PAIRS[4].handle = Ov257_SpawnChildStoreTwoArgs(self, PAIRS[4].res, (void *)(*(int *)(self + 0x3d8) + 4));
            break;
        case 1:
            PAIRS[2].handle = Ov257_SpawnTrailHelper(self, PAIRS[2].res, PAIRS[3].res, (void *)(*(int *)(self + 0x3d4) + 4));
            break;
        case 2:
            if (PAIRS[2].handle != 0) {
                TaskList_FinishByTag(*(int *)(self + 0x3c), PAIRS[2].handle);
                PAIRS[2].handle = 0;
            }
            break;
        case 3:
            PAIRS[0].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[0].res, 0x17, 0, 0x1000, msg + 5);
            break;
        case 4:
            PAIRS[1].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[1].res, 0x17, 0, 0x1000, msg + 5);
            break;
        case 5:
            PAIRS[5].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[5].res, 0x17, 0, 0x1000, msg + 5);
            break;
        case 6:
            PAIRS[7].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[7].res, 5, 0, 0x1000, msg + 5);
            break;
        case 8:
            PAIRS[9].handle = Ov257_SpawnChild10AndBackLink(self);
            PAIRS[6].handle = Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), PAIRS[6].res, 4, (void *)(*(int *)(self + 0x3d4) + 4), 0, 0);
            break;
        case 9:
            PAIRS[10].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[10].res, 5, 0, 0x1000, msg + 5);
            break;
        case 10:
            PAIRS[11].handle = Ov107_CreateNodeXformTaskFx24(*(int *)(self + 0x3c), PAIRS[11].res, 5, 0, 0x1000, msg + 5);
            break;
        case 11: {
            VecFx32 pos;
            int rawZ;
            int rawY;
            int rawX;

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
            PAIRS[8].handle = Ov257_SpawnHelper2(self, PAIRS[8].res, &pos);
            break;
        }
        case 12:
            *(int *)(self + 0x40c) = Ov107_CreateSpawnTask(self, (short)*(int *)(self + 0x408), 0xd, 0, self + 0xa0);
            break;
        case 13:
            Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x40c));
            *(int *)(self + 0x40c) = 0;
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
