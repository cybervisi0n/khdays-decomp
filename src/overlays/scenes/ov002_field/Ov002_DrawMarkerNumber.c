/*
 * Ov002_DrawMarkerNumber - draw a signed number above the local player.
 *
 * The marker is anchored on the player's projected position, lifted by the
 * caller's offset and shifted right by half the number's width so the digits
 * end up centred. The marker's list level falls with the caller's alpha.
 *
 * A positive number takes one palette and a trailing glyph 10; a negative one
 * takes the other palette and glyph 11. The digits are then drawn least
 * significant first, walking left, and the trailing glyph closes the run.
 *
 * ARM.
 */

#include "nitro/fx_types.h"

typedef struct {
    char pad000[0x78];
    int nList;
    char pad07c[0x8c];
} Ov002Widget;

typedef struct {
    char pad000[0x1068];
    Ov002Widget markerWidget;
} Ov002MarkerScene;

extern int data_ov002_0207f628;

extern void NNS_G3dMdlSetMdlPolygonIDAll(int nList, int nValue);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);

extern int Ov002_CountDecimalDigits_2(int nValue);
extern void Ov002_EmitLocalPlayerBox(int *pOut, int nArg);
extern void Ov002_PlaceWidget_2(char *pWidget, const VecFx32 *pPos, int nTag,
                                int nValue);

void Ov002_DrawMarkerNumber(int nArg, int nValue, int nAlpha, int nLift)
{
    Ov002MarkerScene *s;
    int nTag;
    VecFx32 vPos;
    int nGlyph;
    int nSteps;
    int nDigits;
    int i;

    nSteps = nAlpha >> 12;
    s = *(Ov002MarkerScene **)&data_ov002_0207f628;
    nDigits = Ov002_CountDecimalDigits_2(nValue < 0 ? -nValue : nValue);
    nGlyph = 10;
    Ov002_EmitLocalPlayerBox(&vPos.x, nArg);

    vPos.x += (nDigits + 1) * 0x7000 / 2;
    vPos.y += nLift;

    NNS_G3dMdlSetMdlPolygonIDAll(s->markerWidget.nList, 0x37);
    NNS_G3dMdlSetMdlAlphaAll(s->markerWidget.nList,
                  0x1f - nSteps * 0x1f / 100);

    if (nValue < 0) {
        nTag = 0x315f;
        nGlyph = 0xb;
    } else {
        nTag = 0x3fdf;
    }
    if (nValue < 0) {
        nValue = -nValue;
    }

    for (i = 0; i < nDigits; i++) {
        Ov002_PlaceWidget_2((char *)&s->markerWidget, &vPos, nTag,
                            (nValue % 10) << 12);
        vPos.x -= 0x7000;
        nValue /= 10;
    }
    Ov002_PlaceWidget_2((char *)&s->markerWidget, &vPos, nTag, nGlyph << 12);
}
