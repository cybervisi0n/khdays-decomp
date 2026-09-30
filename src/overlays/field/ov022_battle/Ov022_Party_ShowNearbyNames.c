/* Shows the name tags of the live party members within range of the camera owner. */

#include "nitro/fx_types.h"

typedef struct Ov022Root {
    char padding000[0x34];
    unsigned char entryCount34;
} Ov022Root;

typedef struct Ov022Actor {
    char padding000[9];
    unsigned char entryId09;
    char padding00a[0x5c];
    short field066;
    char padding068[0x424];
    VecFx32 position48c;
    char padding498[0x1fc];
    unsigned char stateFlags694;
    char padding695[0x2023];
    int verticalOffset26b8;
} Ov022Actor;

extern Ov022Root *NNSi_FndGetCurrentRootHeap(void);
extern int func_ov022_02083f0c(void);
extern int Ov002_GetBit0OfField38IfValid(int p);
extern int Ov002_PollSession(void);
extern VecFx32 *Ov002_GetWordAt0x20Plus0x20(int owner);
extern int VEC_Distance(VecFx32 *a, VecFx32 *b);
extern int Ov022_GetGlobal34(void);
extern void Ov022_DrawStateMarker(int kind, int entryId,
                                VecFx32 *position, int value);

void Ov022_Party_ShowNearbyNames(void)
{
    Ov022Root *root = NNSi_FndGetCurrentRootHeap();
    int owner = func_ov022_02083f0c();
    VecFx32 actorPosition;
    VecFx32 targetPosition;
    int index;
    Ov022Actor *actor;
    char *entryCursor;
    int minusOne;

    if (Ov002_GetBit0OfField38IfValid(owner) == 0) {
        return;
    }

    index = 0;
    if (index >= root->entryCount34) {
        return;
    }

    entryCursor = (char *)root;
    minusOne = -1;
    do {
        if (Ov002_PollSession() != 0) {
            actor = *(Ov022Actor **)(*(int *)(entryCursor + 4) + 0x20);
            if (actor->field066 != minusOne &&
                ((unsigned int)(actor->stateFlags694 << 31) >> 31) != 0) {
                actorPosition = actor->position48c;
                targetPosition = *Ov002_GetWordAt0x20Plus0x20(owner);
                if (VEC_Distance(&actorPosition, &targetPosition) <= 0x3c000) {
                    Ov022_DrawStateMarker(0, actor->entryId09,
                        &actorPosition, Ov022_GetGlobal34());
                    actorPosition.y += actor->verticalOffset26b8;
                    Ov022_DrawStateMarker(1, actor->entryId09,
                        &actorPosition, Ov022_GetGlobal34());
                }
            }
        }
        index++;
        entryCursor += 0xc;
    } while (index < root->entryCount34);
}

