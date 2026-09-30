/* Spawn timer tick of the ov299 boss shell. The +8 timer accumulates frame time up to the
 * actor's +0x390 interval; then the first idle +0x384 part (its +0x38c clear) is launched by
 * d3d68 at the position of the best target among the scene's +0xa8 list: ready actors (bit 1 of
 * +0x40, bit 0 of +0x60) whose +0x1b4 kind flag bit 16 is clear, taking the first one and then
 * the closest or the farthest to the actor's +0x74 position depending on a coin toss (3eb4).
 * The timer restarts from zero. */

#include "nitro/fx_types.h"

struct Bits40 { int b0 : 1, b1 : 1; };
struct hw60 { unsigned short lo : 8, hi : 8; };
struct Ov299Actor { char pad000[0x384]; int parts[3]; };

extern int RandNextScaled(int range);
extern int *List_First(int list);
extern int *List_Next(int list);
extern long long *GetEntryField20ByIndex(int kind);
extern void VEC_Subtract(const VecFx32 *a, const void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void Ov299_RunSetupThenForwardIfState1(int part, int target, VecFx32 *pos, int actor);
extern const VecFx32 data_02041dc8;

void Ov299_SpawnTimerTick(int *node)
{
    int *state = (int *)node[1];
    int actor = *state;
    int bestActor;
    VecFx32 best;
    VecFx32 d;
    VecFx32 pos;
    int bestDist;
    long i;
    int pick;
    int scene;
    int *entry;
    int other;
    int len;

    if (state[2] < *(int *)(actor + 0x390)) {
        state[2] += *(int *)(*node + 0x2c);
        return;
    }
    scene = *(int *)(actor + 4);
    best = data_02041dc8;
    bestActor = 0;
    bestDist = 0x7fffffff;
    pick = RandNextScaled(2);
    for (i = 0; i < 3; i++) {
        if (*(int *)(((struct Ov299Actor *)*state)->parts[i] + 0x38c) == 0) {
            entry = List_First(scene + 0xa8);
            other = entry == 0 ? 0 : *entry;
            while (other != 0) {
                pos = *(VecFx32 *)(other + 0x74);
                if (((struct Bits40 *)(other + 0x40))->b1 != 0 && (((struct hw60 *)(other + 0x60))->lo & 1) != 0 &&
                    (*GetEntryField20ByIndex(*(unsigned char *)(other + 0x1b4)) & 0x10000) == 0) {
                    VEC_Subtract(&pos, (void *)(actor + 0x74), &d);
                    len = VEC_Normalize(&d, &d);
                    if (bestDist >= 0x7fffffff || (pick != 0 && len < bestDist) || (pick == 0 && len > bestDist)) {
                        bestDist = len;
                        best = *(VecFx32 *)(other + 0x74);
                        bestActor = other;
                    }
                }
                entry = List_Next(scene + 0xa8);
                other = entry == 0 ? 0 : *entry;
            }
            Ov299_RunSetupThenForwardIfState1(((struct Ov299Actor *)*state)->parts[i], state[1], &best, bestActor);
            state[2] = 0;
            return;
        }
    }
}
