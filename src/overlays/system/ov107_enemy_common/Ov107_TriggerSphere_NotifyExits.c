/* Notifies each active actor that has left the trigger sphere (calls its exit callback with 2). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct {
    unsigned pad0 : 1;
    int flag1 : 1;
    int flag2 : 1;
    unsigned rest : 29;
} Flags40;

typedef struct {
    u16 lo : 8;
    u16 hi : 8;
} SplitU16;

extern char **List_First(void *listHead);
extern char **List_Next(void *listHead);
extern int Ov107_FindNearestObject(void *node, int *out);
extern void VEC_Subtract(int *a, int *b, int *out);
extern fx32 VEC_Mag(const int *v);

void Ov107_TriggerSphere_NotifyExits(char *self) {
    char *base = *(char **)self;
    char *sub = *(char **)(base + 4);
    char *listHead = sub + 0x80;
    char **iter;
    char *node;

    iter = List_First(listHead);
    node = (iter == 0) ? 0 : *iter;
    if (node == 0) {
        return;
    }

    do {
        if (((Flags40 *)(node + 0x40))->flag1) {
            if (((Flags40 *)(node + 0x40))->flag2) {
                u16 b = ((SplitU16 *)(node + 0x60))->lo;
                if (b & 1) {
                    int tmp0;
                    if (Ov107_FindNearestObject(node, &tmp0)) {
                        int diff[3];
                        VEC_Subtract((int *)(self + 0x1c), (int *)(node + 0x74), diff);
                        if (VEC_Mag(diff) > *(int *)(self + 0x28) + *(int *)(node + 0x80)) {
                            void (*cb)(void *, int) = *(void (**)(void *, int))(node + 0x1cc);
                            if (cb != 0) {
                                cb(node, 2);
                            }
                        }
                    }
                }
            }
        }

        iter = List_Next(listHead);
        node = (iter == 0) ? 0 : *iter;
    } while (node != 0);
}
