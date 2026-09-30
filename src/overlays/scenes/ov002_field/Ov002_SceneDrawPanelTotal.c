/*
 * Ov002_SceneDrawPanelTotal - draw the panel's running total.
 *
 * Nothing is drawn while the number has no digits. Otherwise the shared digit
 * widget is placed and drawn once per digit, most significant first, each one
 * a fixed step further left so the number always ends on the same right edge.
 *
 * ARM.
 */

#include "nitro/fx_types.h"

typedef struct {
    char pad000[0x970];
    VecFx32 vDigitPos;
    char pad97c[0x3e4];
    int nTotalShown;
} Ov002CounterScene;

extern int data_ov002_0207f628;

extern int Ov002_CountDecimalDigits_2(int nValue);
extern int Ov002_ExtractDecimalDigit(int nIndex, int nValue);
extern void Ov002_BindWidgetSubNode3(void *pWidget, int nDigit);
extern void Ov002_DrawAndStepNode(void *pWidget);

void Ov002_SceneDrawPanelTotal(void)
{
    VecFx32 v;
    int i;
    int nValue;
    Ov002CounterScene *s;

    s = *(Ov002CounterScene **)&data_ov002_0207f628;
    nValue = s->nTotalShown;
    for (i = 0; i < Ov002_CountDecimalDigits_2(nValue); i++) {
        v.x = (Ov002_CountDecimalDigits_2(nValue) - i) * 0x6a4 - 0x4394;
        v.y = 0x348a;
        v.z = 0;
        s->vDigitPos = v;
        Ov002_BindWidgetSubNode3((char *)s + 0x8cc,
                            Ov002_ExtractDecimalDigit(i, nValue));
        Ov002_DrawAndStepNode((char *)s + 0x8cc);
    }
}
