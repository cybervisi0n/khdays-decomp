/* Spawn an ov258 beam effect: an 0x50-byte node (0203c5c0 on the owner's +0x3c model, start 020d0eb4,
 * end 020d0fc8) holds `rig`, the owner's +0x464 effect record 0x26 (kind 0x19) or 0x27, the owner and
 * its nearest target (020cab14; none aborts with 0). The rig moves to `pos` (kept in +0x10), the kind
 * is stored in +0x49 and +0x4a marks the second variant. Returns the node handle. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct EffectPair { int res; int handle; };
struct Ov258Effects { char pad[0x464]; struct EffectPair pair[0x30]; };

extern int CreateRegistryEntry(int model, int a, int size, void *start, void *end, int **out);
extern int Ov107_FindNearestObject(int actor, int kind);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void Ov258_BeamStart(void);
extern void Ov258_TaskTeardown_FlagOwner_3(void);

enum { BEAM_MAIN = 0, BEAM_SIDE = 1 };

int Ov258_SpawnBeam(char *self, int rig, VecFx32 *pos, int kind)
{
    int *state;
    VecFx32 at;
    int handle;

    handle = CreateRegistryEntry(*(int *)(self + 0x3c), 100, 0x50, Ov258_BeamStart, Ov258_TaskTeardown_FlagOwner_3, &state);
    state[2] = (int)self;
    state[0] = rig;
    state[1] = ((struct Ov258Effects *)state[2])->pair[kind == 0x19 ? 0x26 : 0x27].res;
    state[3] = Ov107_FindNearestObject(state[2], 0);
    if (state[3] == 0) {
        return 0;
    }
    at = *pos;
    *(VecFx32 *)(state + 4) = *pos;
    Srt_SetTranslation((void *)(state[0] + 4), &at);
    *((u8 *)state + 0x49) = kind;
    *((u8 *)state + 0x4a) = kind == 0x19 ? BEAM_MAIN : BEAM_SIDE;
    return handle;
}
