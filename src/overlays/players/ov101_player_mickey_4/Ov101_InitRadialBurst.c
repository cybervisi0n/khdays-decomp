/* Initialize an effect part. Phase 2 binds three slots at 120-degree offsets around vPosition;
 * phase 1 can arm the local pending trigger. When the scene spawn bit is set, spawn slot id 0xd6.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct MtxFx33 {
    int m[3][3];
} MtxFx33;

typedef struct Ov101RadialBurstFrame {
    MtxFx33 mRotation;
    VecFx32 vSpawnPosition;
    VecFx32 vRadialOffset;
    VecFx32 vOrigin;
} Ov101RadialBurstFrame;

typedef struct Ov101HeadingSource {
    u8 pad00[0x80];
    u16 nHeading;
} Ov101HeadingSource;

typedef struct Ov101EffectPart {
    u8 pad00[2];
    s8 nState;
    u8 pad03;
    int nLifetime;
    u8 pad08[0x22];
    short nPhase;
    u8 pad2c[0xa0];
    VecFx32 vPosition;
} Ov101EffectPart;

typedef struct Ov101SceneFlags {
    u8 bSlotSpawnEnabled : 1;
    u8 rest : 7;
} Ov101SceneFlags;

typedef struct Ov101SceneState {
    u32 nFlags;
    u8 pad04[0x1c];
    Ov101HeadingSource *pHeadingSource;
    u8 pad24[0x456];
    u8 nPendingMode;
    u8 nPendingIndex;
    u8 pad47c[0x218];
    Ov101SceneFlags slotSpawnFlags;
    u8 pad695[0x25eb];
    u8 aSlotPool[4];
} Ov101SceneState;

extern Ov101SceneState *data_ov101_020bc0e0;
extern const short data_0203d210[];

extern void MTX_RotY33_(MtxFx33 *m, int nSin, int nCos);
extern void MTX_MultVec33(VecFx32 *src, MtxFx33 *m, VecFx32 *dst);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *dst);
extern void Ov101_BindFreeSlotAtPos(int slotPool, VecFx32 *position);
extern int Session_GetLocalPlayerIndex(void);
extern void Slot_Spawn(int slotId, int variant, VecFx32 *position,
                          int flags);

int Ov101_InitRadialBurst(void *pContext, Ov101EffectPart *pPart, int nDelta)
{
    const short *pSinCosTable;
    MtxFx33 *pRotation;
    Ov101RadialBurstFrame f;
    int nSpawnVariant;
    int nAngle;
    Ov101SceneState *pScene;

    pScene = data_ov101_020bc0e0;
    pPart->nState = 4;
    pPart->nLifetime = 0x3000;
    nSpawnVariant = 0;
    if (pPart->nPhase == 2) {
        f.vOrigin = pPart->vPosition;
        pSinCosTable = data_0203d210;
        nAngle = (u16)(pScene->pHeadingSource->nHeading - 0x8000);
        pRotation = &f.mRotation;
        do {
            int nTrigIndex = nAngle >> 4;

            f.vRadialOffset.x = 0;
            f.vRadialOffset.y = 0;
            f.vRadialOffset.z = 0x3000;
            MTX_RotY33_(pRotation,
                        -pSinCosTable[nTrigIndex * 2],
                        -pSinCosTable[nTrigIndex * 2 + 1]);
            MTX_MultVec33(&f.vRadialOffset, pRotation, &f.vRadialOffset);
            VEC_Add(&f.vRadialOffset, &f.vOrigin, &f.vSpawnPosition);
            Ov101_BindFreeSlotAtPos((int)pScene->aSlotPool, &f.vSpawnPosition);
            nAngle = (u16)(nAngle + 0x5555);
            nSpawnVariant++;
        } while (nSpawnVariant < 3);
        Session_GetLocalPlayerIndex();
        nSpawnVariant = 1;
    } else if (pPart->nPhase == 1
               && Session_GetLocalPlayerIndex() == 0
               && (int)(*(long long *)pScene & 0x10000) == 0) {
        pScene->nPendingMode = 3;
        pScene->nPendingIndex = nSpawnVariant;
    }
    if (pScene->slotSpawnFlags.bSlotSpawnEnabled != 0) {
        Slot_Spawn(0xd6, nSpawnVariant, &pPart->vPosition, 0);
    }
    return 0;
}

