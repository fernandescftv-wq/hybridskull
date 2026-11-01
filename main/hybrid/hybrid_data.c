/* hybrid_data.c - catalogo gerado de ~/HybridSkull_UI/data.js/app.js
 * AMPS (31), PEDAIS (52) + categorias, GCATS, PRESETS_DEMO, PRESETS_EXTRA.
 * Paletas (AMP_PAL, CH_COLORS, LETTER_COLORS) do app.js — valores literais.
 */
#include "hy_types.h"
#include "hybrid_data.h"
#include <string.h>
#include <stdio.h>

const HyAmp hy_amps[] = {
    { .slug = "british_800", .name = "British 800", .cat = "lead", .ch = 2 },
    { .slug = "british_lead", .name = "British Lead", .cat = "lead", .ch = 2 },
    { .slug = "briton", .name = "Briton", .cat = "crunch", .ch = 1 },
    { .slug = "treadplate", .name = "Treadplate", .cat = "lead", .ch = 2 },
    { .slug = "california_mk_iv", .name = "California MK IV", .cat = "lead", .ch = 3 },
    { .slug = "ecstasy_100", .name = "Ecstasy 100", .cat = "crunch", .ch = 3 },
    { .slug = "vh_4", .name = "VH-4", .cat = "lead", .ch = 4 },
    { .slug = "slo", .name = "SLO", .cat = "lead", .ch = 2 },
    { .slug = "archon_lead", .name = "Archon Lead", .cat = "lead", .ch = 2 },
    { .slug = "insanity_iii", .name = "Insanity III", .cat = "lead", .ch = 3 },
    { .slug = "tone_star_lead", .name = "Tone Star Lead", .cat = "lead", .ch = 2 },
    { .slug = "harmonic_halo", .name = "Harmonic Halo", .cat = "lead", .ch = 2 },
    { .slug = "match_30", .name = "Match 30", .cat = "clean", .ch = 2 },
    { .slug = "tangerine_30", .name = "Tangerine 30", .cat = "crunch", .ch = 2 },
    { .slug = "tangerine_rock", .name = "Tangerine Rock", .cat = "crunch", .ch = 2 },
    { .slug = "route_66", .name = "Route 66", .cat = "crunch", .ch = 2 },
    { .slug = "pg_crunch", .name = "PG Crunch", .cat = "crunch", .ch = 1 },
    { .slug = "be_101", .name = "BE-101", .cat = "crunch", .ch = 2 },
    { .slug = "dumbel_50", .name = "Dumbel 50", .cat = "clean", .ch = 1 },
    { .slug = "blackface_63", .name = "'63 Blackface", .cat = "clean", .ch = 2 },
    { .slug = "twostone_studio", .name = "TwoStone Studio", .cat = "clean", .ch = 2 },
    { .slug = "mk_mini_ii", .name = "MK Mini II", .cat = "clean", .ch = 2 },
    { .slug = "sv_blue", .name = "SV Blue", .cat = "clean", .ch = 2 },
    { .slug = "pg_clean", .name = "PG Clean", .cat = "clean", .ch = 1 },
    { .slug = "jazz_clean", .name = "Jazz Clean", .cat = "clean", .ch = 1 },
    { .slug = "ac_chime", .name = "AC Chime", .cat = "clean", .ch = 2 },
    { .slug = "lo_fi_box", .name = "Lo-Fi Box", .cat = "clean", .ch = 1 },
    { .slug = "tone_mallet", .name = "Tone Mallet", .cat = "crunch", .ch = 2 },
    { .slug = "are_acoustic", .name = "ARE Acoustic", .cat = "acoustic", .ch = 1 },
    { .slug = "flat_acoustic", .name = "Flat Acoustic", .cat = "acoustic", .ch = 1 },
    { .slug = "acoustic_sim", .name = "Acoustic Sim", .cat = "acoustic", .ch = 1 },
};
const int hy_n_amps = 31;

const HyPedal hy_pedais[] = {
    { .id = "comp", .name = "Compressor", .panel = "DSP.Compressor.Pedal.BG.png", .cat = "dyn" },
    { .id = "optcomp", .name = "Opto Comp", .panel = "DSP.BBEOpticalComp.Pedal.BG.png", .cat = "dyn" },
    { .id = "bluecomp", .name = "Blue Comp", .panel = "DSP.BlueComp.Pedal.BG.png", .cat = "dyn" },
    { .id = "boost", .name = "Booster", .panel = "DSP.Booster.Pedal.BG.png", .cat = "drive" },
    { .id = "distx", .name = "Distortion X", .panel = "DSP.Distortion.BG.png", .cat = "drive" },
    { .id = "tubedrv", .name = "Tube Driver", .panel = "DSP.BKTubeDriver.Pedal.BG.png", .cat = "drive" },
    { .id = "autowah", .name = "Auto Wah", .panel = "DSP.AutoWah01.Pedal.BG.png", .cat = "filter" },
    { .id = "chorus", .name = "Chorus", .panel = "DSP.ChorusStereo.Pedal.BG.png", .cat = "mod" },
    { .id = "ce1", .name = "CE-1 Chorus", .panel = "DSP.CE1.Pedal.BG.png", .cat = "mod" },
    { .id = "chorusola", .name = "Chorus Ola", .panel = "DSP.ChorusOla.Pedal.BG.png", .cat = "mod" },
    { .id = "flanger", .name = "Flanger", .panel = "DSP.Flanger.BG.png", .cat = "mod" },
    { .id = "acchorus", .name = "Acoustic Chorus", .panel = "DSP.AcousticChorus.Pedal.BG.png", .cat = "mod" },
    { .id = "delay", .name = "Digital Delay", .panel = "DSP.Delay.Pedal.BG.png", .cat = "delay" },
    { .id = "delaydm3", .name = "Analog Delay", .panel = "DSP.DelayDM3.Pedal.BG.png", .cat = "delay" },
    { .id = "delayola", .name = "Delay Ola", .panel = "DSP.DelayOla.Pedal.BG.png", .cat = "delay" },
    { .id = "drev", .name = "Digital Reverb", .panel = "DSP.DigitalReverb.Pedal.BG.png", .cat = "reverb" },
    { .id = "drev2", .name = "Digital Reverb Pro", .panel = "DSP.DigitalReverb.BG.png", .cat = "reverb" },
    { .id = "spring", .name = "'60 Spring", .panel = "DSP.63Spring.Pedal.BG.png", .cat = "reverb" },
    { .id = "ambrev", .name = "Ambient Reverb", .panel = "DSP.AmbientReverb.Pedal.BG.png", .cat = "reverb" },
    { .id = "acrev", .name = "Acoustic Reverb", .panel = "DSP.AcousticReverb.Pedal.BG.png", .cat = "reverb" },
    { .id = "acEq", .name = "Acoustic EQ", .panel = "DSP.AcousticEQ.Pedal.BG.png", .cat = "eq" },
    { .id = "hx_c3band", .name = "3-Band Comp", .panel = "HX.ThreeBandComp.Pedal.BG.png", .cat = "dyn" },
    { .id = "hx_redsq", .name = "Red Squeeze", .panel = "HX.RedSqueeze.Pedal.BG.png", .cat = "dyn" },
    { .id = "hx_lacomp", .name = "LA Studio Comp", .panel = "HX.LAStudioComp.Pedal.BG.png", .cat = "dyn" },
    { .id = "hx_alpaca", .name = "Alpaca Rouge", .panel = "HX.AlpacaRouge.Pedal.BG.png", .cat = "drive" },
    { .id = "hx_scream", .name = "Scream 808", .panel = "HX.Scream808.Pedal.BG.png", .cat = "drive" },
    { .id = "hx_rat", .name = "Ratatouille Dist", .panel = "HX.Ratatouille.Pedal.BG.png", .cat = "drive" },
    { .id = "hx_minotaur", .name = "Minotaur", .panel = "HX.Minotaur.Pedal.BG.png", .cat = "drive" },
    { .id = "hx_bighorn", .name = "Bighorn Fuzz", .panel = "HX.BighornFuzz.Pedal.BG.png", .cat = "drive" },
    { .id = "hx_wahchrome", .name = "Wah Chrome", .panel = "HX.WahChrome.Pedal.BG.png", .cat = "filter" },
    { .id = "hx_autofilt", .name = "Auto Filter", .panel = "HX.AutoFilter.Pedal.BG.png", .cat = "filter" },
    { .id = "hx_chor70", .name = "70 Chorus", .panel = "HX.Chorus70.Pedal.BG.png", .cat = "mod" },
    { .id = "hx_phaserdx", .name = "Deluxe Phaser", .panel = "HX.DeluxePhaser.Pedal.BG.png", .cat = "mod" },
    { .id = "hx_trem60", .name = "60s Bias Trem", .panel = "HX.BiasTrem60.Pedal.BG.png", .cat = "mod" },
    { .id = "hx_tape", .name = "Tape Echo", .panel = "HX.TapeEcho.Pedal.BG.png", .cat = "delay" },
    { .id = "hx_bucket", .name = "Bucket Brigade", .panel = "HX.BucketBrigade.Pedal.BG.png", .cat = "delay" },
    { .id = "hx_hotsprings", .name = "Hot Springs", .panel = "HX.HotSprings.Pedal.BG.png", .cat = "reverb" },
    { .id = "hx_dtank", .name = "Double Tank", .panel = "HX.DoubleTank.Pedal.BG.png", .cat = "reverb" },
    { .id = "hx_eq10", .name = "10-Band Graphic", .panel = "HX.TenBandGraphic.Pedal.BG.png", .cat = "eq" },
    { .id = "hx_eqpar", .name = "Parametric EQ", .panel = "HX.ParametricEQ.Pedal.BG.png", .cat = "eq" },
    { .id = "tx_greenbox", .name = "Greenbox", .panel = "TX.Greenbox.Pedal.BG.png", .cat = "drive" },
    { .id = "tx_orangebox", .name = "Orangebox", .panel = "TX.Orangebox.Pedal.BG.png", .cat = "drive" },
    { .id = "tx_pugilist", .name = "Pugilist", .panel = "TX.Pugilist.Pedal.BG.png", .cat = "drive" },
    { .id = "tx_royal", .name = "Royal Tone", .panel = "TX.RoyalTone.Pedal.BG.png", .cat = "drive" },
    { .id = "tx_comp", .name = "Pedal Comp", .panel = "TX.Compressor.Pedal.BG.png", .cat = "dyn" },
    { .id = "tx_studiocomp", .name = "Studio Comp", .panel = "TX.StudioComp.Pedal.BG.png", .cat = "dyn" },
    { .id = "tx_phaser90", .name = "Phaser 90", .panel = "TX.Phaser90.Pedal.BG.png", .cat = "mod" },
    { .id = "tx_chorus", .name = "Analog Chorus", .panel = "TX.AnalogChorus.Pedal.BG.png", .cat = "mod" },
    { .id = "tx_vibe", .name = "Universe Vibe", .panel = "TX.UniverseVibe.Pedal.BG.png", .cat = "mod" },
    { .id = "tx_memory", .name = "Memory Delay", .panel = "TX.MemoryDelay.Pedal.BG.png", .cat = "delay" },
    { .id = "tx_plate", .name = "Large Plate", .panel = "TX.LargePlate.Pedal.BG.png", .cat = "reverb" },
    { .id = "tx_autowah", .name = "Auto Wah TX", .panel = "TX.AutoWah.Pedal.BG.png", .cat = "filter" },
};
const int hy_n_pedais = 52;

const HyGearCat hy_gear_cats[] = {
    { "drive", "DRIVE" },
    { "dyn", "DYNAMICS" },
    { "mod", "MOD" },
    { "filter", "FILTER" },
    { "delay", "DELAY" },
    { "reverb", "REVERB" },
    { "eq", "EQ" },
};
const int hy_n_gear_cats = 7;

const HyPreset hy_presets_demo[8] = {
    { .name = "Clean Studio", .amp = "twostone_studio", .chan = 1,
      .chain = { "comp", "acEq", "chorus", "acrev", NULL } },
    { .name = "Crunch Rhythm", .amp = "tangerine_rock", .chan = 2,
      .chain = { "boost", "chorus", "delay", NULL } },
    { .name = "High Gain Lead", .amp = "vh_4", .chan = 3,
      .chain = { "tubedrv", "distx", "delay", "drev", NULL } },
    { .name = "Fuzz Wall", .amp = "british_800", .chan = 3,
      .chain = { "bigmuff", "chorus", "delayola", NULL } },
    { .name = "Ambient Pad", .amp = "match_30", .chan = 1,
      .chain = { "comp", "chorusola", "delay", "ambrev", NULL } },
    { .name = "Old Rock", .amp = "route_66", .chan = 2,
      .chain = { "boost", "tubedrv", "delaydm3", "spring", NULL } },
    { .name = "Neo Soul", .amp = "be_101", .chan = 2,
      .chain = { "optcomp", "bluecomp", "tubedrv", "drev", NULL } },
    { .name = "Surf Tank", .amp = "jazz_clean", .chan = 1,
      .chain = { "spring", "autowah", "ce1", NULL } },
};

const HyPreset hy_presets_extra[2] = {
    { .name = "Boutique Drive", .amp = "mk_mini_ii", .chan = 1,
      .chain = { "hx_minotaur", "hx_alpaca", "hx_chor70", "hx_tape", "hx_hotsprings", NULL } },
    { .name = "ToneX Drive", .amp = "tangerine_rock", .chan = 2,
      .chain = { "tx_comp", "tx_greenbox", "tx_phaser90", "tx_memory", "tx_plate", NULL } },
};

/* AMP_PAL (paleta que segue o amp): familia -> 3 variantes por hash slug */
lv_color_t hy_amp_accent(const char *slug, const char *cat)
{
    static const uint32_t pal_lead[]    = { 0xE04A3F, 0xC7452F, 0xD9773A };
    static const uint32_t pal_crunch[]  = { 0xFF8A3C, 0xFFA030, 0xE8681C };
    static const uint32_t pal_clean[]   = { 0x5FB0E6, 0x7FA8CC, 0x46C2B8 };
    static const uint32_t pal_acoustic[] = { 0x4CD964, 0x8FC64A, 0xB9C64E };
    const uint32_t *pal = pal_lead;
    if (cat && !strcmp(cat, "crunch"))        pal = pal_crunch;
    else if (cat && !strcmp(cat, "clean"))    pal = pal_clean;
    else if (cat && !strcmp(cat, "acoustic")) pal = pal_acoustic;
    uint32_t h = 0;
    for (; slug && *slug; slug++) h = h * 31 + (uint8_t)*slug;
    return lv_color_hex(pal[h % 3]);
}

/* CH_COLORS do palco.c */
lv_color_t hy_chan_cor(int chan)
{
    switch (chan) {
    case 1:  return HY_CH1;
    case 2:  return HY_CH2;
    case 3:  return HY_CH3;
    case 4:  return HY_CH4;
    default: return HY_CH1;
    }
}

const HyAmp *hy_amp_slug(const char *slug)
{
    for (int i = 0; i < hy_n_amps; i++)
        if (!strcmp(hy_amps[i].slug, slug)) return &hy_amps[i];
    return 0;
}

const HyPedal *hy_pedal_id(const char *id)
{
    for (int i = 0; i < hy_n_pedais; i++)
        if (!strcmp(hy_pedais[i].id, id)) return &hy_pedais[i];
    return 0;
}

/* LETTER_COLORS (SISTEMA > Cor das letras) */
const uint32_t hy_letter_colors[] = {
    0xF3C969, 0xC9922B, 0xB87333, 0xFFA030, 0xEDE0C8,
    0x8A6420, 0x4CD964, 0xE04A3F, 0x7FA8CC
};
const char *hy_letter_names[] = { "Ouro", "Latao", "Cobre", "Ambar", "Marfim",
                                  "Bronze", "Verde LED", "Vermelho", "Azul Aco" };
const int hy_n_letter_colors = 9;

/* estado inicial — ver rodapé deste arquivo */

/* ── estado global (espelho do S de app.js, defaults) ───────────────────────── */
HyState hy;

void hy_init(void)
{
    memset(&hy, 0, sizeof(hy));
    hy.view       = HY_V_SIGNAL;
    hy.cur_bank   = 0;            /* BANCO 1 */
    hy.cur_slot   = 0;
    hy.chan       = 1;
    hy.skin_fundo = -1;           /* 'amp' (fundo segue o amp vigente) */
    hy.skin_vel   = 1;            /* normal */
    hy.cor_letras = 0;            /* Ouro */
    hy.cfg_estilo = 2;            /* Cinzel */
    hy.cfg_brilho = 100;
    hy.gear_cat   = 0;            /* drive */
    hy.auto_iv    = 1;            /* 4s (índice 1 em [2,4,8,16]) */
    hy.midi       = 1;
    hy.rede_wifi  = true;

    /* banco 1 semas os presets demo; demais bancos vazios */
    for (int i = 0; i < 8; i++) {
        const HyPreset *p = &hy_presets_demo[i];
        HySlot *s = &hy.banks[0][i];
        snprintf(s->name, sizeof(s->name), "%s", p->name);
        const HyAmp *a = hy_amp_slug(p->amp);
        s->amp  = a ? (int)(a - hy_amps) : -1;
        s->chan = p->chan;
        for (int j = 0; j < 8 && p->chain[j]; j++)
            snprintf(s->chain[j], sizeof(s->chain[j]), "%s", p->chain[j]);
    }
}

const char *hy_view_id(int v)
{
    switch (v) {
    case HY_V_PRESETS: return "presets";
    case HY_V_GEAR:    return "gear";
    case HY_V_AMP:     return "amp";
    case HY_V_AUTO:    return "auto";
    case HY_V_CONFIG:  return "config";
    default:           return "signal";
    }
}
