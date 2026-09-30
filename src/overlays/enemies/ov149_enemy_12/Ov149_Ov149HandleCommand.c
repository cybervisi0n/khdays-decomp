/* Handle type-5 commands for this enemy, then forward every command to the common
 * handler, exactly like Ov281_HandleActorCommand.
 *
 * Action 1 re-binds two render nodes onto the shared transform and opens effect 7;
 * action 2 re-binds a third; action 0 sweeps the scene's actor list, collecting up
 * to sixteen ids of live actors whose surface distance is within 0x8000, halving
 * their stagger timer when this enemy is in mode 1, firing reaction 0x14e at each,
 * and handing the collected list to Ov149_bindSubitemsByTypeId. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"

struct Command {
    u8 pad00[2];
    u8 type02;
    u8 action03;
};

struct Slots {
    int field00;
    int field04;
    int field08;
    int field0c;
    char pad10[8];
    int field18;
    int field1c;
    int field20;
    int field24;
};

struct Other {
    char pad00[2];
    u16 id02;
    char pad04[0x5c];
    struct Hw60 { u16 lo : 8, hi : 8; } flags60;
    char pad62[0x12];
    VecFx32 vPos74;
    int nRadius80;
    char pad84[0x17c];
    u16 flags200_18;
    char pad202[0x16];
    short nStaggerMax218;
    short nStagger21a;
};

struct Self {
    Actor base;                  /* 0x000 */
    u8 pad38c[0x4];
    struct Slots *pSlots390;
    int *pTransform394;
    char pad398[4];
    int aTransform39c[11];
    char pad3c8[8];
    int nEffect3d0;
};

extern int Ov107_CreateNodeBodyTask(int resource, int node, int kind, void *transform,
                               int flags, int enabled);
extern int Ov107_CreateSpawnTask(struct Self *self, int id, int kind, int enabled,
                               void *transform);
extern void **List_First(void *list);
extern void **List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b,
                         VecFx32 *dst);
extern int VEC_Mag(const VecFx32 *v);
extern void Ov107_BuildAndSendUpdate(struct Self *self, int id, int kind,
                                VecFx32 *at);
extern int Ov149_bindSubitemsByTypeId(struct Self *self, short *ids);
extern void Ov107_AiState_OnMessage(struct Self *self, struct Command *cmd, int arg2);

void Ov149_Ov149HandleCommand(struct Self *self, struct Command *cmd, int arg2)
{
    struct Other *other;
    void **it;
    int count;
    int n;
    int cap;

    if (cmd->type02 == 5) {
        switch (cmd->action03) {
        case 1:
            self->pSlots390->field0c =
                Ov107_CreateNodeBodyTask(self->base.taskList, self->pSlots390->field08,
                                    0x17, self->pTransform394 + 1, 0, 0);
            self->pSlots390->field1c =
                Ov107_CreateNodeBodyTask(self->base.taskList, self->pSlots390->field18,
                                    0x17, self->aTransform39c, 0, 0);
            self->nEffect3d0 =
                Ov107_CreateSpawnTask(self, 0x14e, 7, 0,
                                    self->pTransform394 + 1);
            break;
        case 2:
            self->pSlots390->field24 =
                Ov107_CreateNodeBodyTask(self->base.taskList, self->pSlots390->field20,
                                    0x17, self->pTransform394 + 1, 0, 0);
            break;
        case 0: {
            int list = ((int)self->base.pScene);
            short ids[16] = {0};
            VecFx32 delta;

            count = 0;
            it = List_First((void *)(list + 0x80));
            other = (it == 0) ? 0 : (struct Other *)*it;
            while (other != 0) {
                if ((other->flags60.lo & 1) != 0
                    && (*(u16 *)((char *)other + 0x1ac) & 4) == 0) {
                    VEC_Subtract(&other->vPos74, &self->base.sphere.center, &delta);
                    if (VEC_Mag(&delta) - (other->nRadius80 + self->base.sphere.radius)
                        <= 0x8000) {
                        ids[count++] = other->id02;
                        if (self->base.mode == 1) {
                            cap = other->nStaggerMax218;
                            n = other->nStagger21a + cap / 2;
                            if (n < 0) {
                                cap = 0;
                            } else if (n <= cap) {
                                cap = n;
                            }
                            other->nStagger21a = (short)cap;
                            Ov107_BuildAndSendUpdate(self, 0x14e, 8, &other->vPos74);
                        }
                        if (count >= 0x10) {
                            break;
                        }
                    }
                }
                it = List_Next((void *)(list + 0x80));
                other = (it == 0) ? 0 : (struct Other *)*it;
            }
            self->pSlots390->field04 = Ov149_bindSubitemsByTypeId(self, ids);
            break;
        }
        }
    }

    Ov107_AiState_OnMessage(self, cmd, arg2);
}
