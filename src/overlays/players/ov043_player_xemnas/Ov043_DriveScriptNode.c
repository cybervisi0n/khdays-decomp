/* Drives the mission enemy's +0x2c2c script node from the model's track-0 frame. Mode 0x11
 * (windup) marks the node live, clears its held bit, binds channel 2 to the +0xe0 table and
 * winds it to the frame, then pins the node to the actor's position and heading. Mode 0x12
 * (release) marks it live, drops the held bit once the +0x7b0 timer reaches the +0x2aba rate,
 * re-pins the node while unheld (raising the held bit), binds channel 2 in mode 1, winds it and
 * latches bit 0 once the frame passes 0xc000. Whenever the node is live it is re-registered
 * for a flagged actor (+0x694 bit 0), and outside those two modes it is stepped at 0x1800
 * (20 fps) or 0x1000 until it completes, which retires it. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct b2 { u8 b0 : 1, b1 : 1; };

extern void BindAnimTrack(void *animation, int track, void *table, short mode);   /* BindAnimTrack */
extern void Anim_SetFrameWrapped(void *animation, int track, int frame);              /* Anim_SetFrameWrapped */
extern void Scene_DrawNode(void *node);
extern unsigned short Sequence_UpdateTracks(void *node, int tick);

void Ov043_DriveScriptNode(char *self)
{
    char *pNode = self + 0x2c + 0x2c00;
    int nFrame = Anim_GetFrame(*(char **)(self + 0x20) + 4, 0);
    int bActive = 0;

    switch (*(int *)(self + 0x6bc)) {
    case 0x11:
        *(u8 *)(pNode + 0x108) = 1;
        *(u8 *)(pNode + 0x109) &= ~2;
        BindAnimTrack(pNode, 2, pNode + 0xe0, 0);
        Anim_SetFrameWrapped(pNode, 2, nFrame);
        *(VecFx32 *)(pNode + 0xa4) = *(VecFx32 *)(self + 0x8c + 0x400);
        bActive = 1;
        *(u16 *)(pNode + 0x7c) = (u16)(*(u16 *)(*(char **)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
        *(u16 *)pNode |= 0x20;
        break;
    case 0x12:
        *(u8 *)(pNode + 0x108) = 1;
        if (*(int *)(self + 0x7b0) == *(short *)(self + 0x2a00 + 0xba)) {
            *(u8 *)(pNode + 0x109) &= ~2;
        }
        if (((struct b2 *)(pNode + 0x109))->b1 == 0) {
            *(VecFx32 *)(pNode + 0xa4) = *(VecFx32 *)(self + 0x8c + 0x400);
            *(u16 *)(pNode + 0x7c) = (u16)(*(u16 *)(*(char **)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
            *(u16 *)pNode |= 0x20;
            ((struct b2 *)(pNode + 0x109))->b0 = 0;
            ((struct b2 *)(pNode + 0x109))->b1 = 1;
        }
        BindAnimTrack(pNode, 2, pNode + 0xe0, 1);
        Anim_SetFrameWrapped(pNode, 2, nFrame);
        if (nFrame >= 0xc000) {
            if (((struct b2 *)(pNode + 0x109))->b0 == 0) {
                ((struct b2 *)(pNode + 0x109))->b0 = 1;
            }
        }
        bActive = 1;
        break;
    }
    if (*(signed char *)(pNode + 0x108) != 1) {
        return;
    }
    if (((struct b2 *)(self + 0x694))->b0 != 0) {
        Scene_DrawNode(pNode);
    }
    if (bActive != 0) {
        return;
    }
    if (Sequence_UpdateTracks(pNode, GetFrameRateMode() == 1 ? 0x1800 : 0x1000) != 0) {
        *(u8 *)(pNode + 0x108) = 0;
        *(u8 *)(pNode + 0x109) &= ~2;
    }
}
