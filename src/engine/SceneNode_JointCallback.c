/* G3D node callback installed by Scene_DrawNode: for every child of the scene node being drawn
 * (data_0204c1f4) attached to the joint the renderer is on (the render state's current node when
 * its node-valid flag 0x10 is set), the child's matrix is multiplied in at model scale (the
 * state's posScale, then back with invPosScale), billboarded first when the child asks (flag 8). */

#include "nitro/types.h"
#include "game/engine.h"
#include "nitro/fx/fx.h"

typedef struct SceneNode SceneNode;

struct SceneNode {
    u16 flags;
    char _002[0x7e];
    MtxFx43 matrix;         /* 0x80 */
    char _0b0[0x10];
    SceneNode *child;       /* 0xc0 */
    SceneNode *next;        /* 0xc4 */
    s16 jointId;            /* 0xc8 */
};

typedef struct NNSG3dRS {
    char _000[8];
    unsigned int flag;      /* 0x08 */
    char _00c[0xa2];
    u8 currentNode;         /* 0xae */
    char _0af[0x31];
    s32 posScale;           /* 0xe0 */
    s32 invPosScale;        /* 0xe4 */
} NNSG3dRS;

extern SceneNode *data_0204c1f4;
extern void GX_SendFifoWords(unsigned int command, const void *src, unsigned int words);
extern void NNS_G3dGetCurrentMtx(MtxFx43 *matrix, int a);

void SceneNode_JointCallback(NNSG3dRS *rs)
{
    SceneNode *child = data_0204c1f4->child;

    if (child == 0) {
        return;
    }
    do {
        if (child->jointId == ((rs->flag & 0x10) ? rs->currentNode : -1)) {
            s32 scale[3];
            s32 inv[3];
            s32 posScale = rs->posScale;
            s32 invScale = rs->invPosScale;

            scale[0] = posScale;
            scale[1] = posScale;
            scale[2] = posScale;
            GX_SendFifoWords(0x1b, scale, 3);
            NNS_G3dGetCurrentMtx(&child->matrix, 0);
            if (child->flags & 8) {
                Mtx33_ApplyFixedRotation(&child->matrix);
            }
            inv[0] = invScale;
            inv[1] = invScale;
            inv[2] = invScale;
            GX_SendFifoWords(0x1b, inv, 3);
        }
        child = child->next;
    } while (child != 0);
}
