/* Per-frame update of the ov039 enemy (x4: ov039/058/078/095): reads the node's animation frame
 * and drives the first secondary channel (+0xda8 from the config at +0x2c2c); by mode: 0x2f and
 * 0x32 refresh both attack anchors on the emitter's context (slot 5) while it is idle, 0x33
 * clears them, 0x17-0x19 and 0x1e-0x2d raise bit 0 of the +0xf0c channel flags when the local
 * flag bit 12 / bit 4 is set and the +0xf0d byte is non-zero; then the second channel (+0xf0c
 * from +0x2c80) is driven, the attachment stepped, an idle attachment flags the local player's
 * bit 16 in both words, and the common post-update runs. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov002_WidgetScrollCommit(char *channel, char *config, int heading, int frame);
extern int func_ov022_020923dc(char *emitter, int slot);
extern void Ov039_GetAttackAnchor(char *self, int side, VecFx32 *out);
extern void func_ov022_02089478(int context, int slot, VecFx32 *at);
extern void func_ov022_02089524(int context, int slot);
extern int Ov022_GetGlobal34(void);
extern void Ov022_ForwardToNodeHandler(void *attach, int tick);
extern void Ov022_InvokeCallback24IfBit0(void *attach);
extern int Ov022_AreStreamsIdle(void *attach);
extern void func_ov022_020ad588(char *self);

void Ov039_UpdateAnchorsAndChannels(char *self)
{
    VecFx32 at;
    int frame = Anim_GetFrame(*(char **)(self + 0x20) + 4, 0);
    int i;
    int context;

    Ov002_WidgetScrollCommit(self + 0x1a8 + 0xc00, self + 0x2c + 0x2c00, *(short *)(self + 0x2aba), frame);
    switch (*(int *)(self + 0x6bc)) {
    case 0x17:
    case 0x18:
    case 0x19:
        if ((*(unsigned long long *)(self + 0x464) & 0x1000) != 0 && *(signed char *)(self + 0xf0d) != 0) {
            *(unsigned char *)(self + 0xf0c) |= 1;
        }
        break;
    case 0x2f:
    case 0x32:
        if (func_ov022_020923dc(self + 0x248 + 0x2400, 5) == 0) {
            context = *(int *)(self + 0x2000 + 0x65c);
            for (i = 0; i < 2; i++) {
                Ov039_GetAttackAnchor(self, i, &at);
                func_ov022_02089478(context, i, &at);
            }
        }
        break;
    case 0x33:
        if (func_ov022_020923dc(self + 0x248 + 0x2400, 5) == 0) {
            context = *(int *)(self + 0x2000 + 0x65c);
            for (i = 0; i < 2; i++) {
                func_ov022_02089524(context, i);
            }
        }
        break;
    default:
        if (*(int *)(self + 0x6bc) >= 0x1e && *(int *)(self + 0x6bc) < 0x2e) {
            if ((*(unsigned long long *)(self + 0x464) & 0x10) != 0 && *(signed char *)(self + 0xf0d) != 0) {
                *(unsigned char *)(self + 0xf0c) |= 1;
            }
        }
        break;
    }
    Ov002_WidgetScrollCommit(self + 0x30c + 0xc00, self + 0x2c80, *(short *)(self + 0x2aba), frame);
    Ov022_ForwardToNodeHandler(*(void **)(self + 0x2644), Ov022_GetGlobal34());
    Ov022_InvokeCallback24IfBit0(*(void **)(self + 0x2644));
    if (Ov022_AreStreamsIdle(*(void **)(self + 0x2644)) == 0) {
        if (Session_GetLocalPlayerIndex() == 0) {
            *(unsigned long long *)(self + 0x464) |= 0x10000;
        }
        if (Session_GetLocalPlayerIndex() == 0) {
            *(unsigned long long *)(self + 0x46c) |= 0x10000;
        }
    }
    func_ov022_020ad588(self);
}
