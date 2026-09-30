/* Place the widget: re-bind it to the new owner with the caller's style, copy the
 * three-word position into +0xa4, and push the new depth down the chain at
 * +0xb8 / +0xb4 / +0xb0.
 *
 * The position is a whole-struct assignment -- that is what produces the ldm/stm
 * pair -- while the depth chain is three separate re-reading assignments, same as
 * Ov002_RetargetWidget. */

#include "nitro/fx_types.h"

extern void RegisterSeqAndInit(void *self, int owner, int flags, int style);

typedef struct {
    char pad0000[0xa4];
    VecFx32 vecPos;     /* +0xa4 */
    char pad00b0[0];
    int nDepth0;        /* +0xb0 */
    int nDepth1;        /* +0xb4 */
    int nDepth2;        /* +0xb8 */
} Ov002Widget;

void Ov106_PlaceWidget(Ov002Widget *self, int owner, const VecFx32 *pos,
                         int depth, int style) {
    RegisterSeqAndInit(self, owner, 1, style);

    self->vecPos = *pos;

    self->nDepth2 = depth;
    self->nDepth1 = self->nDepth2;
    self->nDepth0 = self->nDepth1;
}
