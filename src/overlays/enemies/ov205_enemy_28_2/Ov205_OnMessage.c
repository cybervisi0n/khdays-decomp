/* Message handler of the ov205 enemy (twin of ov204 020cfef0): a "spawned" message (kind 5)
 * starts the +0x394 part's sub-items under the +0x3c owner (kind 0x17, weight 0x1000): payload 0
 * the +0x20 item into +0x24 and payload 2 the +0x30 item into +0x34 at the packet's point,
 * payload 1 the +0x28 item into +0x2c at the actor's position lowered by its +0x13c height
 * (packed as 24-bit fixed point), payload 3 the first of the overlay's four listed +4 slots
 * whose handle is not alive. The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u8 hi, mid, lo; } Fx24;   /* sign + 23-bit magnitude, big-endian */
typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;
struct SlotList { int a[4]; };

extern int Ov107_CreateNodeXformTaskFx24(void *taskList, void *subitem, int mode, int blend, int weight, void *payload);
extern int FindListEntryByField1c(void *taskList, int handle);
extern void Ov107_AiState_OnMessage(int actor, u8 *msg, int param);
extern const struct SlotList data_ov205_020d7210;

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

struct Ov204Slot { int item; int handle; };

void Ov205_OnMessage(int actor, u8 *msg, int param)
{
    VecFx32 pos;
    Fx24 packed[3];
    struct SlotList slots;
    FxVec vDead;
    int i;
    int k;

    if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
            *(int *)(*(int *)(actor + 0x394) + 0x24) =
                Ov107_CreateNodeXformTaskFx24(*(void **)(actor + 0x3c), *(void **)(*(int *)(actor + 0x394) + 0x20), 0x17, 0, 0x1000, msg + 5);
            break;
        case 1:
            pos = *(VecFx32 *)(actor + 0x74);
            pos.y -= *(int *)(actor + 0x13c);
            vDead.x = *(Fx32 *)&pos.x;
            PackFx24(&packed[0], vDead.x.value);
            vDead.y = *(Fx32 *)&pos.y;
            PackFx24(&packed[1], vDead.y.value);
            vDead.z = *(Fx32 *)&pos.z;
            PackFx24(&packed[2], vDead.z.value);
            *(int *)(*(int *)(actor + 0x394) + 0x2c) =
                Ov107_CreateNodeXformTaskFx24(*(void **)(actor + 0x3c), *(void **)(*(int *)(actor + 0x394) + 0x28), 0x17, 0, 0x1000, packed);
            break;
        case 2:
            *(int *)(*(int *)(actor + 0x394) + 0x34) =
                Ov107_CreateNodeXformTaskFx24(*(void **)(actor + 0x3c), *(void **)(*(int *)(actor + 0x394) + 0x30), 0x17, 0, 0x1000, msg + 5);
            break;
        case 3:
            slots = data_ov205_020d7210;
            for (i = 0; i < 4; i++) {
                k = slots.a[i];
                if (FindListEntryByField1c(*(void **)(actor + 0x3c), ((struct Ov204Slot *)*(int *)(actor + 0x394))[k].handle) == 0) {
                    *(int *)(*(int *)(actor + 0x394) + k * 8 + 4) =
                        Ov107_CreateNodeXformTaskFx24(*(void **)(actor + 0x3c), *(void **)(*(int *)(actor + 0x394) + k * 8), 0x17, 0, 0x1000, msg + 5);
                    break;
                }
            }
            break;
        }
    }
    Ov107_AiState_OnMessage(actor, msg, param);
}
