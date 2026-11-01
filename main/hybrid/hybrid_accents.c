/* auto: cor dominante por amp (gerada por gen_assets.py) */
#include "hybrid_accents.h"
#include <string.h>
#include <stdint.h>

typedef struct { const char *slug; uint32_t hex; } AmAcc;
static const AmAcc ACC[] = {
    { "ac_chime", 0x572d3a },
    { "acoustic_sim", 0xc6b19d },
    { "archon_lead", 0xd9a24b },
    { "are_acoustic", 0xd9a24b },
    { "be_101", 0xd9a24b },
    { "blackface_63", 0xd9a24b },
    { "british_800", 0x6a533c },
    { "british_lead", 0xb9986b },
    { "briton", 0xd9a24b },
    { "california_mk_iv", 0x47191c },
    { "dumbel_50", 0x8c4a38 },
    { "ecstasy_100", 0xd9b27e },
    { "flat_acoustic", 0xa46a4d },
    { "harmonic_halo", 0xd9a24b },
    { "insanity_iii", 0xd9a24b },
    { "jazz_clean", 0xd9a24b },
    { "lo_fi_box", 0x8d5838 },
    { "match_30", 0x802329 },
    { "mk_mini_ii", 0x906621 },
    { "pg_clean", 0xd9a24b },
    { "pg_crunch", 0x0f7474 },
    { "route_66", 0xa3886b },
    { "slo", 0xd9a24b },
    { "sv_blue", 0xd9a24b },
    { "tangerine_30", 0xbd5837 },
    { "tangerine_rock", 0x826954 },
    { "tone_mallet", 0xd9a24b },
    { "tone_star_lead", 0xd9a24b },
    { "treadplate", 0xd9a24b },
    { "twostone_studio", 0xd9a24b },
    { "vh_4", 0xd9a24b },
};

uint32_t hy_amp_accent_hex(const char *slug)
{
    for (unsigned i = 0; i < sizeof(ACC)/sizeof(ACC[0]); i++)
        if (!strcmp(ACC[i].slug, slug)) return ACC[i].hex;
    return 0xd9a24b;
}
