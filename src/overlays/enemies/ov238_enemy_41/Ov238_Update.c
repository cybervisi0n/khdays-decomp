/* Update of the ov238 actor: in phase 1 without a target, while its +0x398 partner carries a rider
 * (+0x3f0), it rides along: placed at the rider's +0x14 point (020c5c54) and oriented by the rider's
 * +4 rotation after the up-to-normal tilt. The base update runs, the +0xa0 pose is copied to the
 * +0x38c model and mirrored onto the +0x388 set's first model. */

#include "nitro/fx_types.h"

typedef struct { int x, y, z, w; } Quat;
typedef struct { int w[11]; } SrtTransform;
struct Posed { char pad[0x10]; SrtTransform srt; };
struct Ov238Actor { char pad[0xa0]; SrtTransform pose; };

extern void Ov107_MoveNodeAndRelayout(char *self, void *at);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetRotationQuat(void *srt, const Quat *rot);
extern void Ov107_ProcessObjectTick(void *obj, int arg2);
extern const VecFx32 data_02042264;

void Ov238_Update(char *self, int arg)
{
    VecFx32 normal;
    Quat q;
    int ride = 0;

    if (*(int *)(self + 0x50) == 1) {
        if (*(int *)(self + 0x390) == 0) {
            ride = *(int *)(*(int *)(self + 0x398) + 0x3f0);
        }
        if (ride != 0) {
            Ov107_MoveNodeAndRelayout(self, (void *)(ride + 0x14));
            Quat_FromTwoVectors(&q, &data_02042264, &normal);
            Quat_Multiply(&q, (Quat *)(ride + 4), &q);
            Srt_SetRotationQuat(self + 0xa0, &q);
        }
    }
    Ov107_ProcessObjectTick(self, arg);
    (*(struct Posed **)(self + 0x38c))->srt = ((struct Ov238Actor *)self)->pose;
    (**(struct Posed ***)(self + 0x388))->srt = (*(struct Posed **)(self + 0x38c))->srt;
}
