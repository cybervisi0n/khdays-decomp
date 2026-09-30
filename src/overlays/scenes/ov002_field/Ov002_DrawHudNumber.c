/*
 * Ov002_DrawHudNumber - draw a number with the panel's HUD widget.
 *
 * The palette says how urgent the number is: over ten takes the plain one,
 * three or under the warning one, and anything between the middle one. The
 * digits are drawn least significant first from the right-hand edge, stepping
 * left, so a zero still draws one digit.
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
    char pad000[0x1214];
    Ov002Widget hudWidget;
} Ov002HudScene;

extern int data_ov002_0207f628;

extern void NNS_G3dMdlSetMdlPolygonIDAll(int nList, int nValue);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);

extern void Ov002_PlaceWidget_2(char *pWidget, const VecFx32 *pPos, int nTag,
                                int nValue);

void Ov002_DrawHudNumber(int nX, int nY, int nValue)
{
    VecFx32 v;
    int nTag;
    Ov002HudScene *s;

    s = *(Ov002HudScene **)&data_ov002_0207f628;
    NNS_G3dMdlSetMdlPolygonIDAll(s->hudWidget.nList, 0x36);
    NNS_G3dMdlSetMdlAlphaAll(s->hudWidget.nList, 0x1f);

    if (nValue <= 10) {
        if (nValue <= 3) {
            nTag = 0x315f;
        } else {
            nTag = 0x3fdf;
        }
    } else {
        nTag = 0x7fff;
    }

    v.x = nX + 0x12000;
    v.y = nY;
    v.z = 0;
    do {
        v.x -= 0xa99a;
        Ov002_PlaceWidget_2((char *)&s->hudWidget, &v, nTag,
                            (nValue % 10) << 12);
        nValue /= 10;
    } while (nValue > 0);
}
