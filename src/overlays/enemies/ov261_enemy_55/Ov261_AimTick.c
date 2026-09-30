/* Hover aim tick of the ov261 enemy (and its byte-identical twin): the speed is thirty frame-times
 * over fifteen, the anchor follows the partner's position (+0x3a8 part, +0x74), and when the
 * orientation's forward vector faces the partner (dot above 0xf00) the clock resets, reaction
 * 0x179 mode 4 fires at the actor's position and the tick hands off to the hover entry.
 * Codegen: the partner position is held as a pointer variable (`pos`); a plain `int part` local
 * leaves the /15 quotient in ip instead of the ROM's r6. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int x, y, z, w; } Quat;

typedef struct {
    int actor;
    VecFx32 *pos;
    int pad08;
    Quat orient;
    VecFx32 anchor;
    int pad28[2];
    VecFx32 jitter;
    int speed;
    int clock;
} HoverState;

extern void Ov261_SetFacingAnchor(VecFx32 *anchor, VecFx32 *pos, VecFx32 *target);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *d);
extern int VEC_Normalize(const VecFx32 *a, VecFx32 *d);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void Ov107_BuildAndSendUpdate(int actor, int reaction, int mode, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;
extern void Ov261_HoverEntryTick(int *node);

void Ov261_AimTick(int *node)
{
    HoverState *state = (HoverState *)node[1];
    VecFx32 fwd;
    VecFx32 dir;
    VecFx32 *pos;

    state->speed = *(int *)(*node + 0x2c) * 30 / 15;
    pos = (VecFx32 *)(*(int *)(state->actor + 0x3a8) + 0x74);
    Ov261_SetFacingAnchor(&state->anchor, pos, state->pos);
    Vec3TransformViaTempMtx(&fwd, &state->orient, &data_02042258);
    VEC_Subtract(pos, state->pos, &dir);
    VEC_Normalize(&dir, &dir);
    if (VEC_DotProduct(&fwd, &dir) > 0xf00) {
        state->clock = 0;
        Ov107_BuildAndSendUpdate(state->actor, 0x179, 4, state->pos);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov261_HoverEntryTick);
    }
}
