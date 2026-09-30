/*
 * Game_UpdateObjectMotion - per-frame transform/motion update for a gameplay object, dispatched on
 * the object's motion-state word at obj+0xf8. Called from the Game_RunActionScript frame tail.
 *
 *   state -1 : nothing (just returns).
 *   state  0 : commit the object's base transform block (Camera_CommitMatrices(obj+4)).
 *   state  1 : Obj_PrepAltTransform(), then commit the alternate transform block at obj+0x4c.
 *   default  : step the motion (Obj_StepMotionTransform). If the "moved" flag at obj+0x100 is clear, just
 *              commit obj+4; otherwise build a delta transform on the stack (CamAnim_EvalDelta into a
 *              0x38-byte scratch from obj+0xec), copy the object's transform there, add the three
 *              delta vectors (obj+0x18/0x24/0x30 + delta -> scratch), add the two scalar fields,
 *              and commit the composed transform (Camera_CommitMatrices(scratch)).
 *
 * THUMB. The motion-state test is a switch (case -1/0/1/default) so it emits as the original's
 * cmp/beq decision tree with separate case blocks. The stack scratch is an int array (not a
 * char[]) so the two scalar accumulations address it sp-relative ([sp,#n]) instead of through a
 * materialised pointer, matching the original.
 */

#include "nitro/types.h"

extern void Camera_CommitMatrices(void *xform);
extern void Obj_PrepAltTransform(void *);
extern void Obj_StepMotionTransform(void *obj, int a, int b, int c, int d);
extern void CamAnim_EvalDelta(void *src, void *dst, int arg);
extern void MI_CpuCopy8(void *src, void *dst, int n);
extern void VEC_Add(const void *a, const void *b, void *dst);

void Game_UpdateObjectMotion(char *obj)
{
    int buf[0x1c];

    switch (*(int *)(obj + 0xf8)) {
    case -1:
        break;
    case 0:
        Camera_CommitMatrices(obj + 4);
        return;
    case 1:
        Obj_PrepAltTransform(obj);
        Camera_CommitMatrices(obj + 0x4c);
        return;
    default:
        Obj_StepMotionTransform(obj, 0, 0, 0, 0);
        if (*(int *)(obj + 0x100) == 0) {
            Camera_CommitMatrices(obj + 4);
            return;
        }
        CamAnim_EvalDelta(obj + 0x3c, &buf[0xe], *(int *)(obj + 0xec));
        MI_CpuCopy8(obj + 4, buf, 0x38);
        VEC_Add(obj + 0x24, &buf[0x16], &buf[8]);
        VEC_Add(obj + 0x18, &buf[0x13], &buf[5]);
        VEC_Add(obj + 0x30, &buf[0x19], &buf[0xb]);
        buf[0] += buf[0xe];
        buf[1] += buf[0xf];
        Camera_CommitMatrices(buf);
        break;
    }
}
