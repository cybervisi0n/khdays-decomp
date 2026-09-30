/* Facing update: when a heading word is supplied, rebuild the actor's +0xa0 orientation about
 * world Y from it scaled by pi/180 (x 0x3244 / 180). */

#include "nitro/fx_types.h"

extern void Srt_SetRotationAxisAngle(void *quat, const VecFx32 *axis, int angle);
extern const VecFx32 data_02042264;

void Ov277_SetHeadingDegrees(char *actor, int unused, int *pHeading) {
    if (pHeading == 0) return;
    Srt_SetRotationAxisAngle(actor + 0xa0, &data_02042264, *pHeading * 0x3244 / 180);
}
