/* ov022: build the wind effect for one actor.
 *
 * Registers the effect's animation against its own descriptor, opens the
 * actor's own resource container and loads the file the actor's index selects
 * out of it, binds that file to the animation and drops the container again.
 * The pose table then gives the row for this wind strength, clamped to nine,
 * and the table is freed straight away because only that row is wanted.
 * Finally the effect points its render object back at itself and arms the node
 * hook, so the walker calls Ov022_PlaceWindNode while drawing the model.
 */

/* One row of the pose table: the pair the wind strength selects. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct PoseRow {
    int nA;
    int nB;
};

/* The SDK render object the walker drives, embedded in the animation. */
struct RenderObj {
    u8 pad0000[0x2c];
    void *pOwner;                 /* 0x002c, the SDK's spare user slot */
    u8 pad0030[0x1c];
};

struct Anim {
    u8 pad0000[0x20];
    struct RenderObj obj;         /* 0x0020 */
    u8 pad006c[0x9c];
};

struct WindEffect {
    u8 nFlags;                    /* 0x0000 */
    u8 pad0001[3];
    struct Anim anim;             /* 0x0004 */
    u8 blkBind[0x24];             /* 0x010c */
    void *pFile;                  /* 0x0130 */
    u8 nSlotId;                   /* 0x0134 */
    u8 pad0135[3];
    VecFx32 vecPos;               /* 0x0138 */
    u8 pad0144[0x2c];
    int nField170;                /* 0x0170, the pair the tick owns */
    int nField174;                /* 0x0174 */
    struct PoseRow pose;          /* 0x0178 */
};

struct Actor {
    u8 pad0000[9];
    u8 nId;                       /* 0x0009 */
    u8 pad000a[2];
    u32 nContainerIndex;          /* 0x000c */
};

extern char data_ov022_020b2b94[];
extern char data_ov022_020b2ba4[];
extern char data_ov022_020b2bb0[];

extern void Ov022_PlaceWindNode(void *pState);

extern void RegisterSeqAndInit(struct Anim *pAnim, char *pszDescriptor, int nA,
                          int nB);
extern void *Msg_OpenContainerAndReadHeader(char *pszName, int nHeap);
extern void *Archive_LoadFile(u32 nFile, int nSlot);
extern void Resource_BindFileToSlot(void *pBind, struct Anim *pAnim, void *pFile,
                          int nSlot);
extern void ZeroHalfThenFree(void *pContainer);
extern void BindAnimTrack(struct Anim *pAnim, int nTrack, void *pBind,
                          int nGroup);
extern void NNSi_FndFreeFromDefaultHeap(void *pBlock);
extern void NNS_G3dRenderObjSetCallBack(void *pObj, void *pfnHook, int nA, int nCommand,
                          int nTiming);

void Ov022_SetUpWindEffect(struct WindEffect *pEffect, int nCount,
                         struct Actor *pActor)
{
    struct PoseRow *pRow;
    void *pContainer;
    struct PoseRow *pTable;
    u32 nMask;

    if (nCount <= 0) {
        return;
    }
    pEffect->nSlotId = pActor->nId;
    RegisterSeqAndInit(&pEffect->anim, data_ov022_020b2b94, 1, 5);
    pContainer = Msg_OpenContainerAndReadHeader(data_ov022_020b2ba4, 6);
    /* The mask is one value: the ROM loads 0xfffffc once and shifts it right
     * by fifteen for the index mask. */
    nMask = 0xfffffc;
    pEffect->pFile = Archive_LoadFile(
            ((((u32)pContainer + 0x8000) & nMask) << 7) | 0x80000000
            | (pActor->nContainerIndex & (nMask >> 15)),
            pEffect->nSlotId + 7);
    Resource_BindFileToSlot(pEffect->blkBind, &pEffect->anim, pEffect->pFile,
                  pEffect->nSlotId + 7);
    ZeroHalfThenFree(pContainer);
    BindAnimTrack(&pEffect->anim, 0, pEffect->blkBind, 0);
    pTable = Archive_LoadFile((u32)data_ov022_020b2bb0, 6);
    if (nCount > 9) {
        nCount = 9;
    }
    pRow = pTable;
    pRow += nCount - 1;
    pEffect->pose = *pRow;
    NNSi_FndFreeFromDefaultHeap(pTable);
    pEffect->anim.obj.pOwner = pEffect;
    NNS_G3dRenderObjSetCallBack(&pEffect->anim.obj, (void *)Ov022_PlaceWindNode, 0, 6, 3);
    pEffect->nFlags |= 1;
    pEffect->nField170 = 0;
    pEffect->nField174 = 0;
}
