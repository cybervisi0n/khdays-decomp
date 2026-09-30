/* Anchors the orbit camera at the origin, recomputes its position from angle and radius and
 * re-aims. */

#include "nitro/fx_types.h"

typedef struct {
    int yOffset;
    int field04;
    int field08;
} Ov002MotionPreset;

typedef struct {
    char pad00[0x14];
    VecFx32 base;                  /* +0x14 */
    VecFx32 position;              /* +0x20 */
    char pad2c[0x18];
    int presetIndex;               /* +0x44 */
    char pad48[0x0c];
    int radius;                    /* +0x54 */
    int angle;                     /* +0x58 */
    char pad5c[4];
    int verticalOffset;            /* +0x60 */
    VecFx32 computed;              /* +0x64 */
    VecFx32 anchor;                /* +0x70 */
} Ov002MotionState;

typedef struct {
    char pad00[0x20];
    Ov002MotionState *motion;       /* +0x20 */
} Ov002Actor;

extern short data_0203d210[];
extern Ov002MotionPreset data_ov002_0207e768[];

extern void Ov002_TickCameraTransition(Ov002Actor *actor);
extern void Ov002_ReaimActor(Ov002Actor *actor, int reaim);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov002_Camera_SetAnchor(Ov002Actor *actor, const VecFx32 *origin)
{
    Ov002MotionState *motion = actor->motion;
    VecFx32 anchor;
    int angleIndex;
    int computedX;
    int computedY;
    int computedZ;

    anchor.x = origin->x;
    anchor.y = origin->y + motion->verticalOffset;
    anchor.z = origin->z;
    motion->anchor = anchor;
    motion->base = motion->anchor;

    angleIndex = motion->angle >> 4;
    computedZ = motion->base.z +
                FX_Mul(data_0203d210[angleIndex * 2 + 1], motion->radius);
    computedX = motion->base.x +
                FX_Mul(data_0203d210[angleIndex * 2], motion->radius);
    computedY = motion->base.y +
                data_ov002_0207e768[motion->presetIndex].yOffset;
    motion->computed.x = computedX;
    motion->computed.y = computedY;
    motion->computed.z = computedZ;
    motion->position = motion->computed;

    Ov002_TickCameraTransition(actor);
    Ov002_ReaimActor(actor, 1);
}
