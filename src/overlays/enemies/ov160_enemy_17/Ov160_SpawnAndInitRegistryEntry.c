/* Spawn a registry entry (CreateRegistryEntry, size 0x3c, cbs ce190/ce234) into *entry, stash the
 * 4 spawn args in entry[0..3], copy the owner's two facing vectors (owner->f398+0x14 and
 * owner+0xb0, the latter bumped +0x300 on Y) into entry[4..6]/[7..9], and transform each
 * against its target (Srt_SetTranslation into param_2+4 / param_3+4). Returns the spawn result. */

#include "nitro/fx_types.h"

extern int CreateRegistryEntry(int list, int a, int b, void *cb2, void *cb1, int **out);
extern void Srt_SetTranslation(int dst, int *src);
extern void Ov160_ReleaseAction(void);
extern void Ov160_Reaction_ResetTwoSubObjects(void);
int Ov160_SpawnAndInitRegistryEntry(int param_1, int param_2, int param_3, int param_4) {
    int *entry;
    int r = CreateRegistryEntry(*(int *)(param_1 + 0x3c), 100, 0x3c,
                          &Ov160_Reaction_ResetTwoSubObjects, &Ov160_ReleaseAction, &entry);
    entry[0] = param_1;
    entry[1] = param_2;
    entry[2] = param_3;
    entry[3] = param_4;
    *(VecFx32 *)(entry + 4) = *(VecFx32 *)(*(int *)(*entry + 0x398) + 0x14);
    Srt_SetTranslation(entry[1] + 4, entry + 4);
    *(VecFx32 *)(entry + 7) = *(VecFx32 *)(*entry + 0xb0);
    entry[8] += 0x300;
    Srt_SetTranslation(entry[2] + 4, entry + 7);
    return r;
}
