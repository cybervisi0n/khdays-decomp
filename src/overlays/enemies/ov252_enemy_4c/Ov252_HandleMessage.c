/* Message handler of the ov252 actor. A snapshot (kind 0, the 0x44-byte report of 020cca68) restores
 * each armour piece's shown bit, record nibble and slot byte and the three masks. A spawn message
 * (kind 5) unpacks its position into a transform and, by sub id, starts the +0x63c effect pair: posed
 * at the transform (0x17, variant byte 4), at a fist, arm, the last shown piece's anchor or the actor
 * pose (then attached through 020c09a0), from the payload (0xb), or spawns a shard, gem or bomb into
 * the first free pair of its range; 0x32 knocks the actor back, 0x33 plays the owner's sound at the
 * position and 0x34 starts the +0x580 sound 0x148/6. The base handler always runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;
typedef struct { int b0 : 1; int b1 : 1; } Bits;
typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;
typedef struct { unsigned f : 8; } B8;
struct Ov252Msg {
    u8 kind;
    u8 sub;
    u8 b2;
    u8 pad[0x21];
    int shown[4];
    u8 armour[4];
    u16 masks[3];
    signed char slots[4];
    u8 pad42[2];
};
struct Ov252Body {
    char pad[0x38c];
    int models[4];
    char pad39c[0x4f4 - 0x39c];
    int shapes[4];
    char pad504[0x518 - 0x504];
    NibblePair *records[4];
};
/* Spawn work area: the sound message colour bytes (0x33) and the unpacked position. */
struct Ov252MsgWork {
    u8 colour[6];
    VecFx32 pos;
};
struct EffectPair { int res; int handle; };
struct Ov252Effects { char pad[0x63c]; struct EffectPair pair[0x31]; };

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int arg, SrtTransform *transform);
extern int Ov107_CreateNodeXformTaskFx24(int model, int parent, int kind, void *at, int weight, void *payload);
extern int Ov107_CreateNodeBodyTask(int model, int parent, int kind, void *at, int a, int b);
extern int Ov252_SpawnShard(char *self, int owner, VecFx32 *vel, signed char slot);
extern int Ov252_SpawnGem(char *self, int owner, VecFx32 *pos, signed char slot, u8 kind);
extern int Ov252_SpawnBomb(char *self, int owner, VecFx32 *pos, signed char slot, u8 kind);
extern void Ov107_ForwardVisibleEvent(char *self, int a);
extern int Ov002_GetSlotTableByte(void *scene);
extern void Ov002_SpawnAllDrops(u8 *colour, int id, VecFx32 *vec);
extern int Ov107_CreateSpawnTask(char *self, int id, int mode, void *at, void *pose);
extern void Ov107_AiState_OnMessage(char *self, u8 *msg, int arg);

/* The message carries x, y, z at +5 as big-endian signed 24-bit values. */
static inline void UnpackPosition(VecFx32 *out, const u8 *msg)
{
    union {
        int words[3];
        u8 bytes[12];
    } packed;

    packed.bytes[3] = msg[5];
    packed.bytes[2] = msg[6];
    packed.bytes[1] = msg[7];
    out->x = packed.words[0] >> 8;
    packed.bytes[7] = msg[8];
    packed.bytes[6] = msg[9];
    packed.bytes[5] = msg[0xa];
    out->y = packed.words[1] >> 8;
    packed.bytes[11] = msg[0xb];
    packed.bytes[10] = msg[0xc];
    packed.bytes[9] = msg[0xd];
    out->z = packed.words[2] >> 8;
}

void Ov252_HandleMessage(char *self, u8 *msg, int arg)
{
    SrtTransform transform;
    struct Ov252MsgWork work;
    signed char i;
    int node;
    signed char j;
    u8 *colour;
    void *at;

    if (msg[2] == 0) {
        for (i = 0; i < 4; i++) {
            ((Bits *)(((struct Ov252Body *)self)->models[i] + 0x5c))->b1 = ((struct Ov252Msg *)msg)->shown[i];
            ((struct Ov252Body *)self)->records[i]->hi = msg[i + 0x34];
            *(signed char *)(self + i + 0x39c) = *(signed char *)(msg + i + 0x3e);
        }
        *(u16 *)(self + 0x57a) = ((struct Ov252Msg *)msg)->masks[0];
        *(u16 *)(self + 0x57c) = ((struct Ov252Msg *)msg)->masks[1];
        *(u16 *)(self + 0x57e) = ((struct Ov252Msg *)msg)->masks[2];
    } else if (msg[2] == 5) {
        at = 0;
        i = 0;
        UnpackPosition(&work.pos, msg);
        SrtTransform_SetIdentity(&transform);
        Srt_SetTranslation(&transform, &work.pos);
        switch (msg[3]) {
        case 0:
        case 1:
        case 6:
        case 8:
        case 9:
        case 0xa:
        case 0x11:
            ((struct Ov252Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeXformTask(
                *(int *)(self + 0x3c), ((struct Ov252Effects *)self)->pair[msg[3]].res, 0x17, msg[4], &transform);
            break;
        case 0x12:
            at = self + 0xa0;
            break;
        case 2:
        case 3:
        case 7:
            at = (void *)(*(int *)(self + 0x560) + 4);
            break;
        case 4:
        case 5:
            {
                int k;

                for (k = 3; k >= 0; k--) {
                    if (((B8 *)(((struct Ov252Body *)self)->shapes[k] + 8))->f & 1) {
                        at = self + 0x58c + k * 0x2c;
                        break;
                    }
                }
            }
            break;
        case 0xb:
            ((struct Ov252Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeXformTaskFx24(
                *(int *)(self + 0x3c), ((struct Ov252Effects *)self)->pair[msg[3]].res, 0x17, at, 0x1000, msg + 5);
            break;
        case 0xc:
            at = (void *)(*(int *)(self + 0x558) + 4);
            if (msg[3] == 0xd) {
                i = 1;
            }
            break;
        case 0xd:
            at = (void *)(*(int *)(self + 0x554) + 4);
            if (msg[3] == 0xd) {
                i = 1;
            }
            break;
        case 0xe:
            at = (void *)(*(int *)(self + 0x564) + 4);
            break;
        case 0xf:
            at = (void *)(*(int *)(self + 0x568) + 4);
            break;
        case 0x10:
            at = self + 0xa0;
            break;
        case 0x13:
            for (j = 0; j < 8; j++) {
                if (((struct Ov252Effects *)self)->pair[j + 0x13].handle == 0) {
                    break;
                }
            }
            if (j != 8) {
                ((struct Ov252Effects *)self)->pair[j + 0x13].handle = Ov252_SpawnShard(
                    self, ((struct Ov252Effects *)self)->pair[j + 0x13].res, &work.pos, j);
            }
            break;
        case 0x1b:
            for (j = 0; j < 0xc; j++) {
                if (((struct Ov252Effects *)self)->pair[j + 0x1b].handle == 0) {
                    break;
                }
            }
            if (j != 0xc) {
                ((struct Ov252Effects *)self)->pair[j + 0x1b].handle = Ov252_SpawnGem(
                    self, ((struct Ov252Effects *)self)->pair[j + 0x1b].res, &work.pos, j, msg[4]);
            }
            break;
        case 0x27:
            for (j = 0; j < 0xa; j++) {
                if (((struct Ov252Effects *)self)->pair[j + 0x27].handle == 0) {
                    break;
                }
            }
            if (j != 0xa) {
                ((struct Ov252Effects *)self)->pair[j + 0x27].handle = Ov252_SpawnBomb(
                    self, ((struct Ov252Effects *)self)->pair[j + 0x27].res, &work.pos, j, msg[4]);
            }
            break;
        case 0x32:
            Ov107_ForwardVisibleEvent(self, 1);
            break;
        case 0x33:
            node = *(int *)(self + 4);
            /* cleared through a byte pointer to the work area, then byte 2 set */
            colour = (u8 *)&work;
            colour[0] = 0;
            colour[1] = 0;
            colour[2] = 0;
            colour[3] = 0;
            colour[4] = 0;
            colour[5] = 0;
            work.colour[2] = 8;
            Ov002_SpawnAllDrops(work.colour, (u16)(Ov002_GetSlotTableByte(*(void **)(node + 0x78))),
                                &work.pos);
            break;
        case 0x34:
            *(int *)(self + 0x580) = Ov107_CreateSpawnTask(self, 0x148, 6, at, (void *)(*(int *)(self + 0x560) + 4));
            break;
        }
        if (at != 0) {
            ((struct Ov252Effects *)self)->pair[msg[3]].handle = Ov107_CreateNodeBodyTask(
                *(int *)(self + 0x3c), ((struct Ov252Effects *)self)->pair[msg[3]].res, 0x17, at, msg[4], i);
        }
    }
    Ov107_AiState_OnMessage(self, msg, arg);
}
