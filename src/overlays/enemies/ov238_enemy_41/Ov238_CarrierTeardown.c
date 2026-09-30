/* Teardown hook of the ov238 actor's carrier: its +0x3dc model takes the +0x3ec anchor's transform and
 * mirrors it onto the +0x38c set's first model; the data_ov238_020d3668 offset turned by the +0x3f8
 * part's rotation is added to that part's +0x14 position. Outside move 8 the +0x438 effect stops; then
 * the base teardown runs. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[11]; } SrtTransform;
struct Posed { char pad[0x10]; SrtTransform srt; };
struct Anchor { char pad[4]; SrtTransform srt; };

extern void Vec3TransformViaTempMtx(VecFx32 *out, void *rotation, const VecFx32 *in);
extern void TaskList_FinishByTag(int model, int handle);
extern const VecFx32 data_ov238_020d3668;

void Ov238_CarrierTeardown(char *self)
{
    VecFx32 off;

    (*(struct Posed **)(self + 0x3dc))->srt = (*(struct Anchor **)(self + 0x3ec))->srt;
    (**(struct Posed ***)(self + 0x38c))->srt = (*(struct Posed **)(self + 0x3dc))->srt;
    off = data_ov238_020d3668;
    Vec3TransformViaTempMtx(&off, (void *)(*(int *)(self + 0x3f8) + 4), &off);
    *(int *)(*(int *)(self + 0x3f8) + 0x1c) += off.z;
    *(int *)(*(int *)(self + 0x3f8) + 0x14) += off.x;
    *(int *)(*(int *)(self + 0x3f8) + 0x18) += off.y;
    if (*(signed char *)(self + 0x1c6) != 8 && *(int *)(self + 0x438) != 0) {
        TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(self + 0x438));
        *(int *)(self + 0x438) = 0;
    }
    Ov107_AiState_PostTickBase(self);
}
