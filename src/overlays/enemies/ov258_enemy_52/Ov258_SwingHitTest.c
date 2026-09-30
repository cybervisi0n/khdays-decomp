/* Swing hit test of the ov258 actor: after re-acquiring the target (020cd028) a 96.0-long capsule of
 * radius 12.6 stands upright at the +0x1c point (0.25 high); it hits with a random flat push of up
 * to 1.0 each way (kind 4, effect 1) through 020cd104. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;

extern int Ov258_AcquireTarget(int *node, int face);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov258_AttackHitTest(int *node, void *sphere, void *box, void *capsule, void *segment, VecFx32 *push, int once, int effect, int kind);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;

void Ov258_SwingHitTest(int *node)
{
    int *state = (int *)node[1];
    Capsule cap;
    VecFx32 push;

    Ov258_AcquireTarget(node, 0);
    cap.pos = *(VecFx32 *)(state + 7);
    cap.pos.y = 0x400;
    cap.axis = data_02042264;
    cap.length = 0x60000;
    cap.radius = 0xc900;
    VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x454) + 0x190), (VecFx32 *)(state + 7), &push);
    VEC_Normalize(&push, &push);
    push = data_02041dc8;
    push.x = RandNextScaled(0x2001) - 0x1000;
    push.z = RandNextScaled(0x2001) - 0x1000;
    Ov258_AttackHitTest(node, 0, 0, &cap, 0, &push, 0, 1, 4);
}
