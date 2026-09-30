/* Per-frame step: folds the motion the shared sampler returns into the actor's
 * position on the ground plane, then asks the actor's own hook whether it is
 * still active and latches the answer.
 *
 * Becoming active shows the node, raises the actor's own flag and, on the local
 * player only, raises one more. The two 64-bit flag words are written as whole
 * long longs, which is why one half reads and writes itself unchanged.
 *
 * The sample is copied into the step whole and only then flattened, and the
 * sample is declared before the step: the other order costs five instructions
 * of shuffling.
 */

#include "nitro/fx_types.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_StepAnchorDelta(int self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SceneNode_Enable(int node);
extern int Session_GetLocalPlayerIndex(void);

int Ov030_StepActor(int self) {
    int r;
    VecFx32 sample;
    VecFx32 step;

    *(int *)(self + 0x24) &= ~0x80;
    Ov022_StepAnchorDelta(self, &sample);
    *(int *)(self + 0x58) = sample.y;

    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x498), &step, (VecFx32 *)(self + 0x498));

    r = (*(int (**)(int))(self + 0x668))(self);
    ((Flags *)(self + 0x694))->b1 = (unsigned char)r;
    if (((Flags *)(self + 0x694))->b1) {
        int *p;

        *(unsigned long long *)self |= 0x2000000000000ULL;
        p = *(int **)(self + 0x20);
        if ((*(unsigned int *)p & 0x20) == 0) {
            SceneNode_Enable((int)p + 4);
        }
        if (Session_GetLocalPlayerIndex() == 0) {
            *(unsigned long long *)((char *)self + 0x464) |= 2;
        }
    }
    return 0;
}
