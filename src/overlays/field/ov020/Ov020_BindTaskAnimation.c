/* Update handler: binds the task's scene node to the animation the parent task
 * is named after, resets the pose and hides the node again.
 *
 * An unnamed parent means there is nothing to bind, so the whole body is
 * skipped.
 *
 * Two codegen notes. The offset vector is assigned to itself -- a twelve-byte
 * load and store back to the same address -- and the field has to be volatile
 * for that to survive: a plain self-assignment, a copy through an ordinary
 * pointer, through a temporary and through a cast alias are all deleted.
 *
 * The scale is one chained assignment rather than three statements. That
 * keeps the constant live across all three stores, so the flag update below
 * cannot have the register it was held in and moves to the fourth scratch
 * register -- which is what lets its store be scheduled after the call
 * argument is computed, exactly as the original does.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct Ov020ScriptTask {
    char pad00[8];
    struct Ov020ScriptTask *pParent08;      /* 0x08 */
    char pad0c[6];
    unsigned short hFlags12;                /* 0x12 */
    char pad14[8];
    unsigned short aSceneNode1c[1];         /* 0x1c */
    char pad1e[0x3a];
    char aName58[0x10];                     /* 0x58 */
    char pad68[0x58];
    volatile VecFx32 vOffsetC0;         /* 0xc0 */
    VecFx32 vScaleCC;                   /* 0xcc */
};

extern void *Ov002_LookupChannelEntry(const char *name);
extern void Ov002_RebindAnimTracks(unsigned short *node, int a, int b);

void Ov020_BindTaskAnimation(struct Ov020ScriptTask *task)
{
    struct Ov020ScriptTask *parent = task->pParent08;
    void *anim;

    if (parent->aName58[0] == 0) {
        return;
    }

    anim = Ov002_LookupChannelEntry(parent->aName58);
    RegisterSeqAndInit(task->aSceneNode1c, anim, 1, 4);

    task->vOffsetC0 = task->vOffsetC0;
    task->vScaleCC.x = task->vScaleCC.y = task->vScaleCC.z = 0x6000;
    task->hFlags12 |= 4;

    Ov002_RebindAnimTracks(task->aSceneNode1c, 0, 0);
    SceneNode_Disable(task->aSceneNode1c);
}
