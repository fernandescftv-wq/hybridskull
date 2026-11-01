/* hy_types.h - HybridSkull v2 no ESP32-P4 (800x480), fiel ao ~/HybridSkull_UI
 *
 * Arquivos NOVOS (nada do palco.c/amp_*.c/telas existentes e tocado):
 *   hy_types.h           header central + estado + helpers de estilo
 *   hybrid_data.c        catalogo (31 amps, 52 pedais), paletas, presets demo
 *   hybrid_assets.c/.h   JPEG do SPIFFS -> lv_img_dsc_t (PSRAM), caches
 *   hybrid_ui.c/.h       casco: fundo, top bar, nav, hw BANCO 1-8, roteamento
 *   hybrid_view_signal.c palco (amp + cadeia + base8/loops)
 *   hybrid_view_amp.c    aba AMP (nome, skin, CH1-4, picker de 31 skins)
 *   hybrid_view_presets.c aba PRESETS (8 chips de banco + 8 slots)
 *   hybrid_view_gear.c   aba GEAR (rail de categorias + grade de pedais)
 *   hybrid_view_auto.c   aba AUTO (Digital7, intervalo, tocar/parar)
 *   hybrid_view_config.c aba CONFIG (REDE/SKIN/SISTEMA/MIDI)
 *
 * Escala web 1440x820 -> ESP 800x480 = 0,555 (blocos); fontes piso 10px.
 */
#ifndef HY_TYPES_H
#define HY_TYPES_H

#include "lvgl.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── fontes geradas de Cinzel 700/900 + Digital7 (tools: fonttools + lv_font_conv) */
extern const lv_font_t hy_fonte_c10;   /* micro-capas (sub, slots, labels)   */
extern const lv_font_t hy_fonte_c12;   /* abas, chips, botões                */
extern const lv_font_t hy_fonte_c16;   /* nameplate, presets, srow           */
extern const lv_font_t hy_fonte_c20;   /* ampName, zoom                      */
extern const lv_font_t hy_fonte_d31;   /* Digital7 31px (AUTO .big)          */
extern const lv_img_dsc_t hy_gem;      /* diamante ◆ 10x10 (hy_gem.h)        */

#include "hy_theme.h"

/* geometria (escala 0,555 de 1440x820) */
#define HY_W           800
#define HY_H           480
#define HY_TOP_H       40
#define HY_NAV_H       30
#define HY_VIEW_Y      HY_TOP_H                              /* abas fora: 40 */
#define HY_HW_H        42
#define HY_VIEW_H      (HY_H - HY_VIEW_Y - HY_HW_H)          /* 398 */

/* ── catálogo (hybrid_data.c) ────────────────────────────────────────────────── */
typedef struct {
    const char *slug, *name, *cat;   /* cat: lead|crunch|clean|acoustic */
    int ch;                          /* canais CH n/N */
} HyAmp;

typedef struct {
    const char *id, *name, *panel, *cat;   /* cat: dyn|drive|mod|filter|delay|reverb|eq */
} HyPedal;

typedef struct {
    const char *name, *amp;          /* amp = slug ou NULL */
    int chan;
    const char *chain[8];            /* ids de pedal; NULL termina */
} HyPreset;

extern const HyAmp    hy_amps[];       /* 31 */
extern const int      hy_n_amps;
extern const HyPedal  hy_pedais[];     /* 52 */
extern const int      hy_n_pedais;
extern const HyPreset hy_presets_demo[8];
extern const HyPreset hy_presets_extra[2];

/* categorias do gear browser (window.CATS) */
typedef struct { const char *id, *label; } HyGearCat;
extern const HyGearCat hy_gear_cats[7];
extern const int hy_n_gear_cats;

/* LETTER_COLORS (SISTEMA › Cor das letras) */
extern const uint32_t hy_letter_colors[];
extern const char    *hy_letter_names[];
extern const int      hy_n_letter_colors;

const HyAmp   *hy_amp_slug(const char *slug);
const HyPedal *hy_pedal_id(const char *id);
lv_color_t     hy_amp_accent(const char *slug, const char *cat); /* AMP_PAL hash */
lv_color_t     hy_chan_cor(int chan);                            /* CH1..4      */

/* ── estado (espelho do S do app.js) ────────────────────────────────────────── */
enum { HY_V_SIGNAL, HY_V_PRESETS, HY_V_GEAR, HY_V_AMP, HY_V_AUTO, HY_V_CONFIG, HY_V_N };

typedef struct {
    char name[28];
    int  amp;        /* índice em hy_amps, -1 = vazio */
    int  chan;
    char chain[8][12];  /* ids de pedal; "" = slot vazio */
} HySlot;

typedef struct {
    int      view;                 /* enum acima */
    int      cur_bank, cur_slot;   /* 0-7 */
    int      chan;                 /* canal atual CH1..4 */
    int      amp;                  /* índice do amp vigente (~S.amp) */
    bool     stomp, stop;
    bool     menu_fb;              /* false=abas embaixo / true=esquerda */
    int      cor_letras;           /* índice em LETTER_COLORS */
    int      cfg_estilo;           /* 0=Manuscrita 1=Playfair 2=Cinzel 3=Robusta */
    int      cfg_brilho;           /* 0-100 */
    int      skin_fundo;           /* -1: amp vigente, -2: video, >=0: índice amp */
    int      skin_vel;             /* 0 lento 1 normal 2 rápido */
    int      gear_cat;             /* íндice em hy_gear_cats */
    int      auto_run, auto_iv;    /* AUTO */
    int      midi, midi_clock, midi_scenes;
    bool     rede_wifi, rede_pareando;
    HySlot   banks[8][8];
    int      undo_n, redo_n;       /* pilhas apenas para dot habilitado */
} HyState;

extern HyState hy;

const char *hy_view_id(int v);       /* 'signal'...'config' (hash/style key) */

/* ── helpers de estilo (latão, placas, textos) ───────────────────────────────── */
#include "hybrid_components.h"

/* ── casco ──────────────────────────────────────────────────────────────────── */
void hybridskull_boot(void);            /* cria a tela, fundo, top/nav/hw e views */

/* atualizações globais chamados pelas views */
void hy_apply_bg(void);                 /* (re)decodifica bg_<amp>.jpg */
void hy_apply_accent(void);             /* --acc segue o amp (recoloriza abas/glow) */
void hy_apply_chan(void);               /* faixa de cor do canal + chips CH        */
void hy_set_view(int v);                /* troca de aba (nav + .view.on)           */
void hy_set_amp(int idx);               /* troca o amp vigente + bg + picker       */
void hy_set_chan(int n);                /* CH1..4                                  */

/* refs de objetos do casco (usados pelas views p/ recolorir com o accent) */
extern lv_obj_t *hy_tab_btn[6];         /* nav tabs */
extern lv_obj_t *hy_view_box[6];        /* containers das views */
extern lv_obj_t *hy_bg_img;             /* fundo 800x480 */
extern lv_color_t hy_acc;               /* acento atual (segue o amp) */

#ifdef __cplusplus
}
#endif
#endif /* HY_TYPES_H */

/* estado inicial (banco 1 = presets demo) */
void hy_init(void);
