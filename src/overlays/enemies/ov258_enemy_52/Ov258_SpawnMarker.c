/* Spawn an ov258 marker effect: a 0x18-byte node (0203c5c0 on the +0x3c model, start 020d1564, end
 * 020d15f4) holds the owner, the effect rig and the position; the rig is moved there (0203ca30).
 * Returns the node handle. */

#include "nitro/fx_types.h"

extern int CreateRegistryEntry(int model, int a, int size, void *start, void *end, int **out);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void Ov258_MarkerStart(void);
extern void Ov258_ResetEntryMarkDirty(void);

int Ov258_SpawnMarker(char *self, int rig, VecFx32 *pos)
{
    int *state;
    int handle;

    handle = CreateRegistryEntry(*(int *)(self + 0x3c), 100, 0x18, Ov258_MarkerStart, Ov258_ResetEntryMarkDirty, &state);
    state[0] = (int)self;
    state[1] = rig;
    *(VecFx32 *)(state + 2) = *pos;
    Srt_SetTranslation((void *)(state[1] + 4), pos);
    return handle;
}
