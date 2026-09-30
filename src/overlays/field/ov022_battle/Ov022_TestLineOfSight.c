/* Whether the local player has line of sight to a position (no collision in the way of a sphere
 * cast from just above the player). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov022Actor {
    char pad_0000[0x66];
    u16 group66;
} Ov022Actor;

typedef struct CollisionHit {
    char pad_0000[0x0c];
    int distance0c;
} CollisionHit;

extern VecFx32 *func_ov022_020881f8(int index);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern CollisionHit *EntityMgr_RunSphereCastSimple(int group, const VecFx32 *origin,
                                    const VecFx32 *direction, int mask);
extern int VEC_Mag(const VecFx32 *vector);

int Ov022_TestLineOfSight(int index, const VecFx32 *position)
{
    VecFx32 raisedOrigin;
    VecFx32 direction;
    VecFx32 hitPosition;
    VecFx32 hitDelta;
    Ov022Actor *actor;
    VecFx32 *origin;
    CollisionHit *hit;
    int result = 1;

    actor = (Ov022Actor *)GetEntryField20ByIndex(QueryActiveStateOrDelegate());
    origin = func_ov022_020881f8(QueryActiveStateOrDelegate());
    VEC_Subtract(position, origin, &direction);
    raisedOrigin = *origin;
    raisedOrigin.y += 0x1000;
    hit = EntityMgr_RunSphereCastSimple(actor->group66, &raisedOrigin, &direction, 0xcd);
    if (hit != 0) {
        Vec3ScaleAddQ27(hit->distance0c, &direction, &raisedOrigin,
                      &hitPosition);
        VEC_Subtract(&hitPosition, origin, &hitDelta);
        if (VEC_Mag(&direction) > VEC_Mag(&hitDelta)) {
            result = 0;
        }
    }
    return result;
}

