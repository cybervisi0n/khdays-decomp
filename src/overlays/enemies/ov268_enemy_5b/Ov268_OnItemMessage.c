/* Item message handler of the ov208 enemy (x3 with ov209/ov268). A kind-0 message, outside owner
 * mode 1, copies its 0x24 flag into bit 1 of the +0x384 sub-item's +0x5c. A "spawned" message
 * (kind 5) attaches the +0x398 pair named by its slot: slots 0/1 anchor it on the +0x38c/+0x390
 * part's +4 point (kind 0x17, flag 1); slots 2/3 take the +0x394 part's +0xa0 transform moved
 * 2.0 along its forward axis. The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int m[4]; VecFx32 trans; int pad[4]; } SrtTransform;
struct b2 { int b0 : 1, b1 : 1; };
struct Pair { int res; int handle; };

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Srt_SetTranslation(SrtTransform *t, const VecFx32 *v);
extern int Ov107_CreateNodeXformTask(int model, int res, int kind, int zero, SrtTransform *t);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

void Ov268_OnItemMessage(char *self, u8 *msg, int arg)
{
    SrtTransform t;
    VecFx32 v;

    if (msg[2] == 0) {
        if (*(int *)(self + 0x50) != 1) {
            ((struct b2 *)(*(int *)(self + 0x384) + 0x5c))->b1 = msg[0x24];
        }
    } else if (msg[2] == 5) {
        switch (msg[3]) {
        case 0:
        case 1:
            ((struct Pair *)(self + 0x398))[msg[3]].handle =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), ((struct Pair *)(self + 0x398))[msg[3]].res, 0x17,
                                    (msg[3] == 0 ? (void *)(*(int *)(self + 0x38c) + 4) : (void *)(*(int *)(self + 0x390) + 4)), 0, 1);
            break;
        case 2:
        case 3:
            t = *(SrtTransform *)(*(int *)(self + 0x394) + 0xa0);
            v.x = 0;
            v.y = 0;
            v.z = 0x2000;
            Vec3TransformViaTempMtx(&v, &t, &v);
            VEC_Add(&v, &t.trans, &v);
            Srt_SetTranslation(&t, &v);
            ((struct Pair *)(self + 0x398))[msg[3]].handle =
                Ov107_CreateNodeXformTask(*(int *)(self + 0x3c), ((struct Pair *)(self + 0x398))[msg[3]].res, 0x17, 0, &t);
            break;
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
