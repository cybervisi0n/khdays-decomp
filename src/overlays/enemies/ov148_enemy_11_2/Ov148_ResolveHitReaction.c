/* Hit handler: when the actor still has hit points (+0x21a), records the hit direction and source,
 * applies the damage clamped to the maximum (+0x218), plays the hit reaction (alternating sides, a
 * separate set for flagged hits) and queues defeat (action 3) at zero or the knockback action (6)
 * for a flagged hit; returns 1 when it handled the hit. */

#include "nitro/fx_types.h"

struct ReactionModes {
    unsigned char flagged[2];
    unsigned char normal[2];
};

struct HitFlags {
    unsigned int low : 16;
    unsigned int kind : 16;
};

struct HitDescriptor {
    unsigned int flags;
    VecFx32 direction;
    char pad10[0x18];
    int damage;
};

struct ReactionNode {
    unsigned char *actor;
    char pad04[4];
    int source;
    int reactionContext;
    char pad10[0x0c];
    VecFx32 direction;
    char pad28[0x1c];
    unsigned char facing;
};

extern const struct ReactionModes data_ov148_020d24e0;
extern int Ov107_CalcHitDamage(int owner, struct HitDescriptor *hit);
extern void Ov107_BuildAndSendUpdate(int owner, int reactionId,
                                unsigned char mode, int context);

int Ov148_ResolveHitReaction(int owner, int source, struct HitDescriptor *hit)
{
    struct ReactionModes modes;
    struct ReactionNode *node = *(struct ReactionNode **)(owner + 0x214);
    int damage;
    int delta;
    int remaining;
    struct HitFlags *flags;

    modes.normal[0] = data_ov148_020d24e0.normal[0];
    modes.normal[1] = data_ov148_020d24e0.normal[1];
    modes.flagged[0] = data_ov148_020d24e0.flagged[0];
    modes.flagged[1] = data_ov148_020d24e0.flagged[1];

    if (*(short *)(owner + 0x21a) <= 0) {
        return 0;
    }

    node->direction = hit->direction;
    damage = Ov107_CalcHitDamage(owner, hit);
    hit->damage = damage;

    delta = *(short *)(owner + 0x21a) - damage;
    if (delta < 0) {
        remaining = 0;
    } else {
        remaining = *(short *)(owner + 0x218);
        if (delta <= remaining) {
            remaining = delta;
        }
    }
    *(short *)(owner + 0x21a) = (short)remaining;
    node->source = source;

    if (hit->damage > 0) {
        flags = (struct HitFlags *)hit;
        if ((flags->low & 8) == 0 || (flags->low & 0x80) == 0 ||
            flags->kind != 0x80) {
            if ((flags->low & 0x22) != 0) {
                Ov107_BuildAndSendUpdate(owner, 0x126,
                                    modes.flagged[node->facing],
                                    node->reactionContext);
            } else {
                Ov107_BuildAndSendUpdate(owner, 0x126,
                                    modes.normal[node->facing],
                                    node->reactionContext);
            }
            node->facing ^= 1;
        }
    }

    if (*(short *)(owner + 0x21a) == 0) {
        node->actor[0x1c7] = 3;
    } else if (((unsigned short)hit->flags & 0x8000) != 0) {
        node->actor[0x1c7] = 6;
    }

    return 1;
}

