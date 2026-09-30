/* Computes where an actor approaches a target from: the distance and its facing relative to the
 * direction to the target, within the scan distance (larger with ability 0x55). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov022ActorNode {
    char pad_0000[0x80];
    u16 angle80;
} Ov022ActorNode;

typedef struct Ov022Actor {
    char pad_0000[9];
    u8 index9;
    char pad_000a[0x16];
    Ov022ActorNode *node20;
    char pad_0024[0x454];
    s16 angle478;
    char pad_047a[0x12];
    VecFx32 position48c;
} Ov022Actor;

extern const s16 data_0203d210[];

extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *vector);
extern void VEC_Normalize(const VecFx32 *source, VecFx32 *destination);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);

void Ov022_ComputeApproachPoint(Ov022Actor *actor, const VecFx32 *targetPosition,
                          VecFx32 *result, int mode)
{
    VecFx32 direction;
    VecFx32 facing;
    int distance;
    int angle;
    int facingDot;
    int bestDistance;
    int alternateDot;
    const s16 *table;
    const VecFx32 *actorPosition;

    result->x = 0x7fffffff;
    result->y = -1;
    result->z = mode;

    actorPosition = &actor->position48c;
    distance = VEC_Distance(targetPosition, actorPosition);
    VEC_Subtract(targetPosition, actorPosition, &direction);
    if (VEC_Mag(&direction) != 0) {
        VEC_Normalize(&direction, &direction);
    }

    direction.y = 0;
    angle = (u16)(actor->node20->angle80 - 0x8000);
    angle >>= 4;
    table = data_0203d210;
    facing.x = -table[angle << 1];
    facing.z = -table[(angle << 1) + 1];
    facing.y = 0;
    facingDot = VEC_DotProduct(&facing, &direction);

    bestDistance = 0x9000;
    if (Slot_EvalPackedParam(actor->index9, 0x55) != 0) {
        bestDistance = 0xd800;
    }

    angle = actor->angle478 >> 4;
    table = data_0203d210;
    facing.x = -table[angle << 1];
    facing.z = -table[(angle << 1) + 1];
    facing.y = 0;
    alternateDot = VEC_DotProduct(&facing, &direction);

    if (distance <= 0x3000) {
        result->x = distance;
        if (facingDot >= 0x0ab8) {
            result->y = 3;
            return;
        }
        result->y = 2;
        return;
    }

    if (distance <= 0x5000) {
        result->x = distance;
        if (alternateDot >= 0x0ab8) {
            result->y = 1;
            return;
        }
        result->y = 0;
        return;
    }

    if (distance <= bestDistance) {
        result->x = distance;
        result->y = 0;
        return;
    }

    if (distance <= 0x1e000) {
        result->x = distance;
        result->y = -1;
    }
}

