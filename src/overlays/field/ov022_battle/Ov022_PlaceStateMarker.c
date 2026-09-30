/* Ov022_PlaceStateMarker -- put an actor's state marker over a world point.
 *
 * Kind 0 uses the first marker panel with the state as it is; kind 1 uses the
 * second and remaps the state to its marker index. State 0 hides the slot.
 * Otherwise the point, lifted by the panel's y offset, is projected; a point
 * behind the camera hides the slot, and any other lands the marker chosen by
 * the state at the projection shifted by the panel's x offset, shows the
 * slot, records the state and restarts its timer.
 */

/* Ov022ScreenPos: an fx32 screen position */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct ScreenPos {
    int x;
    int y;
};

/* Ov022MarkerSprite: one of the panel's 0x30-byte sprites */
struct MarkerSprite {
    u8 pad00[0x10];
    struct ScreenPos pos;        /* 0x10 */
    u8 pad18[0x18];
};

/* Ov022MarkerSlot: one actor's marker */
struct MarkerSlot {
    struct MarkerSprite *pSprites; /* 0x00 one per state */
    u8 bShown;                   /* 0x04 */
    u8 nState;                   /* 0x05 */
    u8 pad06[2];
    int nTimer;                  /* 0x08 */
};

/* Ov022MarkerPanel */
struct MarkerPanel {
    u8 nKind;                    /* 0x00 */
    u8 nCount;                   /* 0x01 sprites per slot */
    u8 pad02[2];
    int nYLift;                  /* 0x04 added to the world y */
    int nXShift;                 /* 0x08 added to the screen x */
    struct MarkerSlot aSlots[4]; /* 0x0c one per actor id */
};

#define MARKER_KIND_STATE 0
#define MARKER_KIND_MENU 1

extern struct MarkerPanel *data_ov022_020b2ec0[2];   /* the two marker panels by kind */

extern int Ov022_MenuStateToMarker(int nState);          /* menu state -> marker index */
extern int Ov022_ProjectPointToScreen(VecFx32 *pWorld, struct ScreenPos *pScreen);

void Ov022_PlaceStateMarker(int nKind, int nId, int nState, VecFx32 *pPos)
{
    VecFx32 vecWorld;
    struct ScreenPos posScreen;
    struct MarkerPanel *pPanel;
    struct MarkerSlot *pSlot;
    struct MarkerSprite *pSprite;

    switch (nKind) {
    case MARKER_KIND_STATE:
        pPanel = data_ov022_020b2ec0[0];
        break;
    case MARKER_KIND_MENU:
        pPanel = data_ov022_020b2ec0[1];
        nState = Ov022_MenuStateToMarker(nState);
        break;
    }
    pSlot = &pPanel->aSlots[nId];
    if (nState == 0) {
        pSlot->nState = 0;
        pSlot->bShown = 0;
        return;
    }
    vecWorld = *pPos;
    vecWorld.y += pPanel->nYLift;
    if (Ov022_ProjectPointToScreen(&vecWorld, &posScreen) < 0) {
        pSlot->bShown = 0;
        return;
    }
    posScreen.x += pPanel->nXShift;
    pSprite = &pSlot->pSprites[nState - 1];
    pSprite->pos = posScreen;
    pSlot->nState = nState;
    pSlot->bShown = 1;
    pSlot->nTimer = 0;
}
