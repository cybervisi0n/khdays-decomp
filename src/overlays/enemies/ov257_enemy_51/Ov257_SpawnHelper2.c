/* Spawn of the second ov257 helper (0x18-byte state, kind 100; tick Ov257_StartHelper2, second
 * callback Ov257_TaskTeardown_FlagPart): it keeps the enemy and the part, stores the start point at +0xc,
 * places the part there and records at +8 whether the enemy's +0x50 mode is 1. */

#include "nitro/fx_types.h"

extern int CreateRegistryEntry(int scene, int kind, int size, void *cb, void *cb2, int **out);
extern void Srt_SetTranslation(void *srt, const VecFx32 *pos);
extern void Ov257_StartHelper2(void);
extern void Ov257_TaskTeardown_FlagPart(void);

int Ov257_SpawnHelper2(char *self, int part, VecFx32 *pos)
{
    int *out;

    int rc = CreateRegistryEntry(*(int *)(self + 0x3c), 100, 0x18, Ov257_StartHelper2, Ov257_TaskTeardown_FlagPart, &out);
    out[0] = (int)self;
    out[1] = part;
    *(VecFx32 *)(out + 3) = *pos;
    Srt_SetTranslation((void *)(out[1] + 4), pos);
    out[2] = *(int *)(out[0] + 0x50) == 1;
    return rc;
}
