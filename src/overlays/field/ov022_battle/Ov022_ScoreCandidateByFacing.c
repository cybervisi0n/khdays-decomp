/* Scores the panel targets as lock-on candidates by the local actor's facing and distance; returns
 * the best distance. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov022ActorNode {
    char pad_0000[0x80];
    u16 angle80;
} Ov022ActorNode;

typedef struct Ov022Actor {
    char pad_0000[0x20];
    Ov022ActorNode *node20;
} Ov022Actor;

typedef struct Ov022Target {
    VecFx32 position;
} Ov022Target;

typedef struct Ov022TargetEntry {
    char pad_0000[4];
    struct Ov022TargetEntry *next4;
    char pad_0008[0x0a];
    u16 flags12;
} Ov022TargetEntry;

extern const short data_0203d210[];

extern void *NNSi_FndGetCurrentRootHeap(void);
extern VecFx32 *func_ov022_020881f8(int index);
extern int Ov022_GetEntryField66(int index);
extern int Ov002_GetSlotTableByte(int group);
extern Ov022TargetEntry *Ov002_List_GetWord(int value);
extern Ov022Target *Ov002_TriggerEntryActive(Ov022TargetEntry *entry);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern int Ov002_TriggerEntrySecondary(Ov022TargetEntry *entry);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *vector);
extern void VEC_Normalize(const VecFx32 *source, VecFx32 *destination);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void Ov022_ComputeApproachPoint(Ov022Actor *actor, Ov022Target *target,
                                VecFx32 *result, int mode);
extern int func_ov022_02085cc0(u32 *state, const VecFx32 *result);
extern void func_ov022_02084810(u32 *selectionFlags,
                                Ov022TargetEntry *entry,
                                const VecFx32 *result);

int Ov022_ScoreCandidateByFacing(u32 *selectionFlags, int index, int bestDistance)
{
    VecFx32 direction;
    VecFx32 facing;
    VecFx32 resultPosition;
    VecFx32 targetHorizontal;
    VecFx32 originHorizontal;
    VecFx32 *origin;
    Ov022TargetEntry *entry;
    Ov022Target *target;
    Ov022Actor *actor;

    NNSi_FndGetCurrentRootHeap();
    origin = func_ov022_020881f8(index);
    actor = (Ov022Actor *)GetEntryField20ByIndex(index);
    entry = Ov002_List_GetWord(
                               (u16)Ov002_GetSlotTableByte(Ov022_GetEntryField66(QueryActiveStateOrDelegate())));

    while (entry != 0) {
        int distance;
        int angle;
        int horizontalDistance;
        const short *table;

        target = Ov002_TriggerEntryActive(entry);
        if (target != 0 && (entry->flags12 & 8) != 0) {
            distance = VEC_Distance(&target->position, origin);
            if (distance <= Ov002_TriggerEntrySecondary(entry)) {
                VEC_Subtract(&target->position, origin, &direction);
                if (VEC_Mag(&direction) != 0) {
                    VEC_Normalize(&direction, &direction);
                }

                angle = (u16)(actor->node20->angle80 - 0x8000);
                angle >>= 4;
                table = data_0203d210;
                facing.x = -table[angle << 1];
                facing.z = -table[(angle << 1) + 1];
                direction.y = 0;
                facing.y = 0;

                originHorizontal = *origin;
                targetHorizontal = target->position;
                originHorizontal.y = targetHorizontal.y = 0;
                horizontalDistance = VEC_Distance(
                    &targetHorizontal, &originHorizontal);

                if (VEC_DotProduct(&facing, &direction) >= 0x800 ||
                    horizontalDistance <= 0x1000) {
                    Ov022_ComputeApproachPoint(actor, target, &resultPosition, 2);
                    if (func_ov022_02085cc0(&selectionFlags[7],
                                            &resultPosition) != 0) {
                        func_ov022_02084810(selectionFlags, entry,
                                            &resultPosition);
                        bestDistance = resultPosition.x;
                    }
                }
            }
        }
        entry = entry->next4;
    }

    return bestDistance;
}

