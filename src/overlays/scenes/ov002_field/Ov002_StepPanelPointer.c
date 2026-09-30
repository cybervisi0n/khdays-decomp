/*
 * Ov002_StepPanelPointer - walk the panel's pointer widget towards a world
 * position.
 *
 * The animation is advanced by a step that depends on the transition currently
 * running, and the caller is told whether it has finished. While it has not,
 * the widget is moved to the projected position and refreshed; once it has, it
 * is left where it stands.
 *
 * ARM.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    char pad000[0xa4];
    VecFx32 vPos;
    char pad0b0[0x58];
} Ov002Widget;

typedef struct {
    char pad000[0x132c];
    Ov002Widget pointerWidget;
} Ov002PointerScene;

extern int data_ov002_0207f628;

extern unsigned short Sequence_UpdateTracks(void *pWidget, int nStep);
extern void Scene_DrawNode(void *pWidget);

extern void Ov002_ProjectWorldToPanel(VecFx32 *pOut, const VecFx32 *pIn,
                                const void *pCam);

unsigned int Ov002_StepPanelPointer(const void *pCam, const VecFx32 *pIn)
{
    Ov002PointerScene *s;
    unsigned int nDone;
    int nStep;
    VecFx32 v;

    s = *(Ov002PointerScene **)&data_ov002_0207f628;
    if (GetFrameRateMode() == 1) {
        nStep = 0x1800;
    } else {
        nStep = 0x1000;
    }
    Ov002_ProjectWorldToPanel(&v, pIn, pCam);

    nDone = Sequence_UpdateTracks(&s->pointerWidget, nStep);
    if (nDone == 0) {
        s->pointerWidget.vPos = v;
        Scene_DrawNode(&s->pointerWidget);
    }
    return nDone;
}
