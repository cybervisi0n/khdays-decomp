/* Launch an ov259 helper while it is live (+0x50 == 1): its +0xa0 pose turns from the rest axis
 * (data_02042240) to `dir`, it is placed at `pos` (020c5c54), the +0x0c handler is told (when +0x40
 * bit 1 allows it), +0x388 marks it busy and its +0x214 flight takes the direction and heading
 * (020d2904). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;
struct Flags40 { int b0 : 1; int b1 : 1; };

extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Srt_SetRotationQuat(char *srt, Quat *q);
extern void Ov259_HelperStartFlight(int flight, VecFx32 *dir, int heading);
extern const VecFx32 data_02042240;

void Ov259_LaunchHelper(char *self, VecFx32 *pos, VecFx32 *dir, int heading)
{
    Quat q;

    if (*(int *)(self + 0x50) != 1) {
        return;
    }
    Quat_FromTwoVectors(&q, &data_02042240, dir);
    Vec4_Normalize(&q, &q);
    Srt_SetRotationQuat(self + 0xa0, &q);
    Ov107_MoveNodeAndRelayout((Actor *)self, pos);
    if (((struct Flags40 *)(self + 0x40))->b1 && *(void (**)(char *, int))(self + 0xc) != 0) {
        (*(void (**)(char *, int))(self + 0xc))(self, 0);
    }
    *(int *)(self + 0x388) = 1;
    Ov259_HelperStartFlight(*(int *)(self + 0x214), dir, heading);
}
