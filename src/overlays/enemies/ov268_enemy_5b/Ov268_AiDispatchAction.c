/* Ov268_AiDispatchAction -- ov208's move dispatcher, with an orientation update in front.
 *
 * ★ That update runs BEFORE the "nothing queued" check, so it happens on every tick whether or not
 * a move is pending: the angle at ctx+0x30 is turned into a quaternion about the constant axis
 * data_02042264 and pushed into the sub-object at ctx[0]+0xa0. The other dispatchers do their work
 * only when a move is queued.
 *
 * The "nothing queued" case then RETURNS outright (a predicated `popeq`) rather than falling into
 * the shared -1 store, as in ov213.
 *
 * The reset drops 0xce from the hw60 hi-byte and bit 0 of the halfword at +0x1ae, then sets bit 0
 * on ctx[0]+0x3b8 and bits 0-1 on ctx[0]+0x3b4.
 *
 * The hw60 write HAS the lsl#0x10/lsr#0x10 trunc pair -> bitfield form; the +8 fields are
 * byte-in-word. See codegen-cracks.md. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    int x;
    int y;
    int z;
    int w;
} Quaternion;

typedef struct {
    unsigned short lo : 8;
    unsigned short hi : 8;
} Hw60;

typedef struct {
    unsigned f : 8;
} B8;

extern void Srt_SetRotationQuat(int obj, const Quaternion *q);
extern void SetIndexedSlot(int self, int slot, void (*cb)(void));
extern VecFx32 data_02042264;
extern void Ov268_AiLockAndPostUpdate(void);
extern void Ov268_SpawnEffect48TwoNodeClear(void);
extern void Ov268_SetPose1ThenAdvanceSlot(void);
extern void Ov268_AiEnterAim(void);
extern void Ov268_FaceTargetFireGate(void);
extern void Ov268_KickFixedMotion(void);
extern void Ov268_AiEnterChargeBreak(void);
extern void Ov268_EnterSlam(void);
extern void Ov268_AiEnterIdle(void);
extern void Ov268_AiEnterThrow(void);
extern void Ov268_AiEnterBounceShot(void);
extern void Ov268_SpawnEffect4dTwoNodeClear(void);
extern void Ov268_SpawnEffect49KickMotion(void);

void Ov268_AiDispatchAction(int self) {
    int *ctx;
    Quaternion q;

    ctx = *(int **)(self + 4);
    QuatFromAxisAngle(&q, &data_02042264, ctx[0xc]);
    Srt_SetRotationQuat(ctx[0] + 0xa0, &q);

    if (*(signed char *)(ctx[0] + 0x1c7) == -1) {
        return;
    }

    *(signed char *)(ctx[0] + 0x1c6) = *(signed char *)(ctx[0] + 0x1c7);
    ((Hw60 *)(ctx[0] + 0x60))->hi &= ~0xce;
    *(unsigned short *)(ctx[0] + 0x1ae) &= ~1;
    ((B8 *)(*(int *)(ctx[0] + 0x3b8) + 8))->f |= 1;
    ((B8 *)(*(int *)(ctx[0] + 0x3b4) + 8))->f |= 3;

    switch (*(signed char *)(ctx[0] + 0x1c6)) {
    case 0:
        SetIndexedSlot(self, 1, Ov268_AiLockAndPostUpdate);
        break;
    case 1:
        SetIndexedSlot(self, 1, Ov268_SpawnEffect48TwoNodeClear);
        break;
    case 2:
        SetIndexedSlot(self, 1, Ov268_SetPose1ThenAdvanceSlot);
        break;
    case 4:
        SetIndexedSlot(self, 1, Ov268_AiEnterAim);
        break;
    case 5:
        SetIndexedSlot(self, 1, Ov268_FaceTargetFireGate);
        break;
    case 6:
        SetIndexedSlot(self, 1, Ov268_KickFixedMotion);
        break;
    case 7:
        SetIndexedSlot(self, 1, Ov268_AiEnterChargeBreak);
        break;
    case 3:
        SetIndexedSlot(self, 1, Ov268_SpawnEffect4dTwoNodeClear);
        break;
    case 8:
        SetIndexedSlot(self, 1, Ov268_EnterSlam);
        break;
    case 9:
        SetIndexedSlot(self, 1, Ov268_AiEnterIdle);
        break;
    case 10:
        SetIndexedSlot(self, 1, Ov268_AiEnterThrow);
        break;
    case 11:
        SetIndexedSlot(self, 1, Ov268_AiEnterBounceShot);
        break;
    case 12:
        SetIndexedSlot(self, 1, Ov268_SpawnEffect49KickMotion);
        break;
    }

    *(signed char *)(ctx[0] + 0x1c7) = -1;
}
