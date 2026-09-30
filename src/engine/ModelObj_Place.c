/* Places a model object: the position is `pos`, or when none is given the object's own +0xa8 point
 * (or, for a bone-driven object (flag 0x20), the point of its +0x110 transform). Unless frozen
 * (flag 0x10) the +0x110 transform takes the position and is attached to the owner's scene; a free
 * object keeps the position in +0xa8. The object is marked placed (flag 8) and remembers `owner`. */

#include "nitro/fx_types.h"

typedef struct {
    int field_00;
    char pad_04[0xa4];
    VecFx32 pos_a8;
    char pad_b4[0x5c];
    char sub_110[0x48];
} Obj;

extern VecFx32 *Obj_GetSub2c(void *xf);
extern void Node_SetPosAndNotify(void *xf, VecFx32 *pos);
extern void QuadTree_InsertObject(int scene, void *xf);

void ModelObj_Place(int owner, Obj *obj, VecFx32 *pos)
{
    if (pos == 0) {
        if (!(obj->field_00 & 0x20)) {
            pos = &obj->pos_a8;
        } else {
            pos = Obj_GetSub2c(obj->sub_110);
        }
    }
    if (!(obj->field_00 & 0x10)) {
        Node_SetPosAndNotify(obj->sub_110, pos);
        QuadTree_InsertObject(**(int **)(owner + 4), obj->sub_110);
    }
    if (!(obj->field_00 & 0x20)) {
        obj->pos_a8 = *pos;
    }
    obj->field_00 |= 8;
    *(int *)((char *)obj + 0x10c) = owner;
}
