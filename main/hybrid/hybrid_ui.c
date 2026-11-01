/* hybrid_ui.c - casco HybridSkull: fundo, top bar, nav, hw BANCO 1-8
 * Réplica de #screen (1440x820) + #hw na escala 0,555 -> 800x480.
 * A tela antiga do palco NÃO é tocada (fica viva como referência).
 */
#include "hybrid_ui.h"
#include "hybrid_assets.h"
#include "video_loop.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static void hy_set_amp_guard(int amp);
void hb_bank_on(int idx);

lv_obj_t   *hy_tela = NULL, *hy_bg_img = NULL;
lv_obj_t   *hy_top_box, *hy_nav_box, *hy_hw_box;
lv_obj_t   *hy_view_box[HY_V_N];
lv_obj_t   *hy_tab_btn[HY_V_N];             /* botões de aba */
lv_obj_t   *hy_lbl_preset, *hy_lbl_amp_sig;
lv_color_t  hy_acc;
lv_obj_t   *hy_amp_name, *hy_amp_all;
lv_obj_t   *hy_amp_chunks[4];
lv_obj_t   *hy_amp_picker_box;
lv_obj_t   *hy_presets_row;
lv_obj_t   *hy_bank_chip[8];
lv_obj_t   *hy_prow[8][8];                  /* preenchido por presets */

static lv_obj_t *s_chan_strip;              /* faixa de cor do canal */
static lv_obj_t *s_dsp_fill;
static bool s_limpo = false;                /* CENÁRIO (palco limpo) */

#define HY_TAB_QTD 5     /* AUTO saiu da nav (agora e funcao na CONFIG) */
static const char *HY_TAB_TXT[HY_TAB_QTD] =
    { "SIGNAL", "AMP", "PRESETS", "GEAR", "CONFIG" };
static const int HY_TAB_VIEW[HY_TAB_QTD] =
    { HY_V_SIGNAL, HY_V_AMP, HY_V_PRESETS, HY_V_GEAR, HY_V_CONFIG };

/* ─────────────────────────────────────────────────────────────── fundo ──── */
void hy_apply_bg(void)
{
    const HyAmp *a = &hy_amps[hy.amp];
    lv_img_dsc_t *d = hy_assets_bg(a->slug);
    if (!d) return;
    if (!hy_bg_img) {
        hy_bg_img = lv_img_create(hy_tela);
        lv_obj_clear_flag(hy_bg_img, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(hy_bg_img, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(hy_bg_img, HY_W, HY_H);
        lv_obj_align(hy_bg_img, LV_ALIGN_TOP_LEFT, 0, 0);
    }
    lv_img_set_src(hy_bg_img, d);
    lv_obj_move_background(hy_bg_img);
}

/* ─────────────────────────────────────────────────────────────── accent ─── */
/* --acc: borda inferior da aba ativa, glow da skin, wires, FADING */
void hy_apply_accent(void)
{
    const HyAmp *a = &hy_amps[hy.amp];
    hy_acc = hy_amp_accent(a->slug, a->cat);
    for (int i = 0; i < HY_V_N; i++)
        if (hy_tab_btn[i]) {
            if (i == hy.view) {
                lv_obj_set_style_border_color(hy_tab_btn[i], hy_acc, 0);
            } else {
                lv_obj_set_style_border_color(hy_tab_btn[i],
                                              lv_color_hex(0x241A10), 0);
            }
        }
    hy_apply_chan();               /* faixa de canal tem cor própria CHn */
    hy_view_amp_refresh();
}

void hy_apply_chan(void)
{
    lv_color_t c = hy_chan_cor(hy.chan);
    if (s_chan_strip) {
        lv_obj_set_style_bg_color(s_chan_strip, c, 0);
        lv_obj_set_style_bg_opa(s_chan_strip, LV_OPA_COVER, 0);
    }
    for (int i = 0; i < 4; i++)
        if (hy_amp_chunks[i]) hy_chip_on(hy_amp_chunks[i], i + 1 == hy.chan, c);
}

void hy_set_amp(int idx)
{
    if (idx < 0 || idx >= hy_n_amps || idx == hy.amp) return;
    hy.amp = idx;
    hy_assets_limpar_amp();
    hy_apply_bg();
    hy_apply_accent();
    hy_view_signal_novo_amp();
    hy_view_amp_refresh();
}

void hy_set_chan(int n)
{
    if (n < 1) n = 1;
    if (n > 4) n = 4;
    hy.chan = n;
    hy_apply_chan();
    hy_view_amp_refresh();
}

void hy_set_slot(int bank, int slot)
{
    if (bank < 0 || bank > 7 || slot < 0 || slot > 7) return;
    hy.cur_bank = bank;
    hy.cur_slot = slot;
    HySlot *s = &hy.banks[bank][slot];
    if (s->name[0]) {
        if (hy_lbl_preset) lv_label_set_text(hy_lbl_preset, s->name);
        if (s->amp >= 0) hy_set_amp_guard(s->amp);
    }
    for (int i = 0; i < 8; i++)
        if (hy_bank_chip[i]) hy_chip_on(hy_bank_chip[i], i == bank, HY_OURO);
    hb_bank_on(bank);
    hy_view_presets_refresh();
    hy_view_amp_refresh();
    hy_view_signal_novo_amp();
}

/* se slot aponta para um amp válido, partilha com hy_set_amp */
static void hy_set_amp_guard(int amp)
{
    if (amp < 0 || amp >= hy_n_amps) return;
    if (amp == hy.amp) { hy_apply_accent(); hy_view_signal_novo_amp(); return; }
    hy_set_amp(amp);
    hy_set_chan(hy.banks[hy.cur_bank][hy.cur_slot].chan);
}

static void ev_nav(lv_event_t *e)
{
    hy_set_view((int)(intptr_t)lv_event_get_user_data(e));
}

static lv_obj_t *s_voltar;           /* botao de retorno do modo limpo */

static void ev_voltar(lv_event_t *e)
{
    (void)e;
    if (!s_limpo) return;
    s_limpo = false;
    if (hy_top_box) lv_obj_clear_flag(hy_top_box, LV_OBJ_FLAG_HIDDEN);
    for (int i = 0; i < HY_V_N; i++)
        if (hy_view_box[i] && (i == hy.view))
            lv_obj_clear_flag(hy_view_box[i], LV_OBJ_FLAG_HIDDEN);
    if (s_voltar) lv_obj_add_flag(s_voltar, LV_OBJ_FLAG_HIDDEN);
    hy_set_view(hy.view);
}

static int s_prev_view = HY_V_SIGNAL;     /* X: volta pra de onde veio */

static void ev_tap_vazio(lv_event_t *e);

void hy_view_fechar(void)
{
    int dst = s_prev_view;
    if (dst < 0 || dst >= HY_V_N || dst == hy.view) dst = HY_V_SIGNAL;
    hy_set_view(dst);
}

static void ev_tap_vazio(lv_event_t *e)
{
    /* toque em LOCAL VAZIO dentro do view: sai (volta de onde veio) */
    if (lv_event_get_target(e) == hy_view_box[hy.view])
        hy_view_fechar();
}

void hy_cenario_toggle(void)
{
    s_limpo = !s_limpo;
    lv_obj_t *lis[3] = { hy_top_box, hy_nav_box, hy_hw_box };
    for (int i = 0; i < 3; i++) {
        if (lis[i]) {
            if (s_limpo) lv_obj_add_flag(lis[i], LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(lis[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    for (int i = 0; i < HY_V_N; i++) {
        if (hy_view_box[i]) {
            if (s_limpo) lv_obj_add_flag(hy_view_box[i], LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(hy_view_box[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    /* botao VOLTAR no canto sup. direito (sem ele nao ha volta) */
    if (!s_voltar) {
        s_voltar = hy_tbtn(hy_tela, "VOLTAR", HY_W - 118, 8, 110, 34,
                           ev_voltar, NULL);
        lv_obj_move_foreground(s_voltar);
    }
    if (s_limpo) lv_obj_clear_flag(s_voltar, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(s_voltar, LV_OBJ_FLAG_HIDDEN);
}

/* ───────────────────────────────────────────────────── zoom (#zoom web) ─── */
/* radial escuro + nome + sub + foto grande + X; fechar tocando fora */
static lv_obj_t *s_zoom, *s_zoom_name, *s_zoom_sub, *s_zoom_img;

static void zoom_fechar(lv_event_t *e)
{
    (void)e;
    if (s_zoom) lv_obj_add_flag(s_zoom, LV_OBJ_FLAG_HIDDEN);
}

static void zoom_montar(void)
{
    if (s_zoom) return;
    s_zoom = lv_obj_create(hy_tela);
    lv_obj_remove_style_all(s_zoom);
    lv_obj_set_pos(s_zoom, 0, 0);
    lv_obj_set_size(s_zoom, HY_W, HY_H);
    lv_obj_set_style_bg_color(s_zoom, HY_C(0x171208), 0);
    lv_obj_set_style_bg_opa(s_zoom, LV_OPA_50, 0);
    lv_obj_add_flag(s_zoom, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(s_zoom, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(s_zoom, zoom_fechar, LV_EVENT_CLICKED, NULL);

    s_zoom_name = hy_txt(s_zoom, "", &hy_fonte_c20, HY_OURO_CLARO, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_align(s_zoom_name, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_letter_space(s_zoom_name, 2, 0);
    s_zoom_sub = hy_txt(s_zoom, "", &hy_fonte_c20, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_align(s_zoom_sub, LV_ALIGN_TOP_MID, 0, 44);
    s_zoom_img = lv_img_create(s_zoom);           /* pivot = centro do src (default) */
    lv_obj_align(s_zoom_img, LV_ALIGN_CENTER, 0, 14);

    lv_obj_t *x = lv_btn_create(s_zoom);                    /* .zClose (touch 46px) */
    lv_obj_set_size(x, 46, 46);
    lv_obj_set_pos(x, HY_W - 54, 6);
    lv_obj_set_style_radius(x, 23, 0);
    lv_obj_set_style_bg_color(x, HY_C(0x332717), 0);
    lv_obj_set_style_border_color(x, HY_OURO_ESC, 0);
    lv_obj_set_style_border_width(x, 1, 0);
    lv_obj_set_style_shadow_width(x, 0, 0);
    hy_txt(x, "X", &hy_fonte_c16, HY_OURO_CLARO, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(x, zoom_fechar, LV_EVENT_CLICKED, NULL);
}

void hy_zoom_mostrar(lv_img_dsc_t *d, const char *nome, const char *sub,
                     uint16_t zoom_q)
{
    if (!d) return;
    zoom_montar();
    if (zoom_q == 0) {                       /* auto: cabe na tela, sem borrão */
        int32_t zx = 256 * (HY_W - 24) / d->header.w;
        int32_t zy = 256 * (HY_H - 110) / d->header.h;
        int32_t z = (zx < zy) ? zx : zy;
        int32_t cap = (d->header.w >= 400) ? 256 : 512;   /* nunca estourar foto */
        if (z > cap) z = cap;
        if (z < 256) z = 256;
        zoom_q = (uint16_t)z;
    }
    lv_label_set_text(s_zoom_name, nome);
    lv_label_set_text(s_zoom_sub, sub);
    lv_obj_set_width(s_zoom_name, HY_W - 80);
    lv_label_set_long_mode(s_zoom_name, LV_LABEL_LONG_SCROLL);
    lv_obj_set_style_text_align(s_zoom_name, LV_TEXT_ALIGN_CENTER, 0);
    lv_img_set_src(s_zoom_img, d);
    lv_img_set_zoom(s_zoom_img, zoom_q);
    lv_obj_clear_flag(s_zoom, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(s_zoom);
}

static lv_obj_t *s_menu_btn;                 /* MENU: so aparece ao tocar */
static void menu_tmr_cb(lv_timer_t *t)
{
    (void)t;
    if (!s_menu_btn) return;
    if (s_limpo) { lv_obj_add_flag(s_menu_btn, LV_OBJ_FLAG_HIDDEN); return; }
    if (lv_disp_get_inactive_time(NULL) < 4000)
        lv_obj_clear_flag(s_menu_btn, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(s_menu_btn, LV_OBJ_FLAG_HIDDEN);
}

static void ev_menu_top(lv_event_t *e)
{
    (void)e;
    hy_set_view(HY_V_CONFIG);        /* config = hub de tudo */
}

/* ─────────────────────────────────────────────────────────────── top bar ── */
static void top_montar(lv_obj_t *pai)
{
    hy_top_box = lv_obj_create(pai);
    lv_obj_remove_style_all(hy_top_box);
    lv_obj_set_pos(hy_top_box, 0, 0);
    lv_obj_set_size(hy_top_box, HY_W, HY_TOP_H);
    lv_obj_clear_flag(hy_top_box, LV_OBJ_FLAG_SCROLLABLE);
    hy_grad(hy_top_box, HY_PLATE_TOP_A, HY_PLATE_TOP_B);
    lv_obj_set_style_bg_opa(hy_top_box, LV_OPA_90, 0);
    /* linha inferior: 1px preta + 1px ouro fraco (box-shadow do CSS) */
    lv_obj_t *ln = lv_obj_create(hy_top_box);
    lv_obj_remove_style_all(ln);
    lv_obj_set_size(ln, HY_W, 1);
    lv_obj_align(ln, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(ln, HY_OURO, 0);
    lv_obj_set_style_bg_opa(ln, 18, 0);

    /* escudo: ◆ OLD+SKULL */
    lv_obj_t *gem = lv_img_create(hy_top_box);
    lv_img_set_src(gem, &hy_gem);
    lv_obj_align(gem, LV_ALIGN_TOP_LEFT, 10, 8);
    lv_obj_t *br1 = hy_txt(hy_top_box, "OLD", &hy_fonte_c20, HY_OURO_CLARO, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_set_pos(br1, 26, 4);
    lv_obj_t *br2 = hy_txt(hy_top_box, "SKULL", &hy_fonte_c20, HY_AMBAR, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_set_pos(br2, 26 + lv_obj_get_width(br1), 4);
    lv_obj_t *sub = hy_txt(hy_top_box, "HYBRIDSKULL · STAGE ENGINE", &hy_fonte_c16, HY_HINT,
                           LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_set_pos(sub, 252, 12);
    lv_obj_set_width(sub, 178);
    lv_label_set_long_mode(sub, LV_LABEL_LONG_DOT);

    /* nameplate do preset — faixa da cor do canal */
    /* tbtn: SALVAR | CENARIO (UNDO/REDO removidos: sem funcao F1) */
    s_menu_btn = hy_tbtn(hy_top_box, "MENU", 434, 8, 72, 24, ev_menu_top, NULL);
    hy_press_style(s_menu_btn);
    lv_obj_add_flag(s_menu_btn, LV_OBJ_FLAG_HIDDEN);   /* aparece ao toque */

    /* wifi ONLINE */
    hy_led(hy_top_box, 660, 12, 4, HY_VERDE, true);
    hy_txt(hy_top_box, "ONLINE", &hy_fonte_c16, HY_VERDE, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_t *wt = lv_obj_get_child(hy_top_box, -1);
    lv_obj_set_pos(wt, 672, 8);
}

/* ─────────────────────────────────────────────────────────────── nav ────── */
static void nav_montar(lv_obj_t *pai)
{
    hy_nav_box = lv_obj_create(pai);
    lv_obj_remove_style_all(hy_nav_box);
    lv_obj_set_pos(hy_nav_box, 0, HY_TOP_H);
    lv_obj_set_size(hy_nav_box, HY_W, HY_NAV_H);
    lv_obj_clear_flag(hy_nav_box, LV_OBJ_FLAG_SCROLLABLE);
    hy_grad(hy_nav_box, HY_NAV_A, HY_NAV_B);

    int passo = HY_W / HY_TAB_QTD;     /* 160 por aba - mais tap */
    for (int i = 0; i < HY_TAB_QTD; i++) {
        lv_obj_t *t = lv_btn_create(hy_nav_box);
        lv_obj_remove_style_all(t);
        lv_obj_set_pos(t, i * passo, 0);
        lv_obj_set_size(t, passo, HY_NAV_H);
        lv_obj_set_style_bg_opa(t, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_color(t, HY_C(0x1A140C), 0);
        lv_obj_set_style_bg_grad_color(t, HY_C(0x0E0B07), 0);
        lv_obj_set_style_bg_grad_dir(t, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_border_width(t, 0, 0);
        lv_obj_add_flag(t, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(t, ev_nav, LV_EVENT_CLICKED,
                            (void *)(intptr_t)HY_TAB_VIEW[i]);
        lv_obj_t *lb = hy_txt(t, HY_TAB_TXT[i], &hy_fonte_c20, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
        lv_obj_align(lb, LV_ALIGN_CENTER, 0, 0);
        hy_tab_btn[i] = t;
    }
}

void hy_set_view(int v)
{
    if (v < 0 || v >= HY_V_N) return;
    if (v != hy.view) s_prev_view = hy.view;       /* memoria do X */
    hy.view = v;
    for (int i = 0; i < HY_V_N; i++) {
        if (!hy_view_box[i]) continue;
        bool on = (i == v);
        if (on) lv_obj_clear_flag(hy_view_box[i], LV_OBJ_FLAG_HIDDEN);
        else lv_obj_add_flag(hy_view_box[i], LV_OBJ_FLAG_HIDDEN);
    }
    for (int k = 0; k < HY_TAB_QTD; k++) {
        if (!hy_tab_btn[k]) continue;
        bool on = (HY_TAB_VIEW[k] == v);
        lv_obj_t *lb = lv_obj_get_child(hy_tab_btn[k], 0);
        if (lb) lv_obj_set_style_text_color(lb, on ? HY_OURO_CLARO : HY_HINT, 0);
        lv_obj_set_style_border_color(hy_tab_btn[k],
                                      on ? hy_acc : lv_color_hex(0x241A10), 0);
    }
    /* montagem preguiçosa das abas (SIGNAL já vem montada do boot) */
    static bool s_montado[HY_V_N] = { false };
    if (!s_montado[v] && hy_view_box[v]) {
        switch (v) {
        case HY_V_PRESETS: hy_view_presets_montar(hy_view_box[v]); break;
        case HY_V_GEAR:    hy_view_gear_montar(hy_view_box[v]);    break;
        case HY_V_AMP:     hy_view_amp_montar(hy_view_box[v]);     break;
        case HY_V_AUTO:    hy_view_auto_montar(hy_view_box[v]);    break;
        case HY_V_CONFIG:  hy_view_config_montar(hy_view_box[v]);  break;
        default: break;
        }
        s_montado[v] = true;
    }
    hy_apply_accent();      /* recolore a aba ativa (estilo .tab.on) */
}

/* ─────────────────────────────────────────────────────────────── hw ────── */
static void ev_hw(lv_event_t *e)
{
    hy_set_slot((int)(intptr_t)lv_event_get_user_data(e), hy.cur_slot);
}

static void hw_montar(lv_obj_t *pai)
{
    hy_hw_box = lv_obj_create(pai);
    lv_obj_remove_style_all(hy_hw_box);
    lv_obj_set_pos(hy_hw_box, 0, HY_H - HY_HW_H);
    lv_obj_set_size(hy_hw_box, HY_W, HY_HW_H);
    lv_obj_clear_flag(hy_hw_box, LV_OBJ_FLAG_SCROLLABLE);
    hy_grad(hy_hw_box, HY_HW_A, HY_HW_B);
    lv_obj_set_style_border_width(hy_hw_box, 1, 0);
    lv_obj_set_style_border_color(hy_hw_box, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_side(hy_hw_box, LV_BORDER_SIDE_TOP, 0);

    int passo = HY_W / 8;
    for (int i = 0; i < 8; i++) {
        lv_obj_t *b = lv_btn_create(hy_hw_box);
        lv_obj_remove_style_all(b);
        lv_obj_set_pos(b, i * passo, 0);
        lv_obj_set_size(b, passo, HY_HW_H);
        lv_obj_set_style_bg_opa(b, LV_OPA_TRANSP, 0);
        lv_obj_add_flag(b, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(b, ev_hw, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        lv_obj_t *c = lv_obj_create(b);        /* .cap2 */
        lv_obj_remove_style_all(c);
        lv_obj_set_size(c, 26, 26);
        lv_obj_set_style_radius(c, 13, 0);
        lv_obj_set_style_border_width(c, 2, 0);
        lv_obj_set_style_border_color(c, HY_OURO_ESC, 0);
        lv_obj_set_style_bg_color(c, HY_C(0x261d11), 0);
        lv_obj_align(c, LV_ALIGN_TOP_MID, 0, 3);
        lv_obj_t *in = lv_obj_create(c);
        lv_obj_remove_style_all(in);
        lv_obj_set_size(in, 12, 12);
        lv_obj_set_style_radius(in, 6, 0);
        lv_obj_set_style_bg_color(in, HY_C(0x151009), 0);
        lv_obj_align(in, LV_ALIGN_CENTER, 0, 0);
        lv_obj_t *lb = hy_txt(b, "", &hy_fonte_c20, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
        char t[16];
        snprintf(t, sizeof(t), "BANCO %d", i + 1);
        lv_label_set_text(lb, t);
        lv_obj_align(lb, LV_ALIGN_BOTTOM_MID, 0, -1);
        hy_bank_chip[i] = b;
    }
}

void hb_bank_on(int idx)
{
    for (int i = 0; i < 8; i++) {
        lv_obj_t *b = hy_bank_chip[i];
        if (!b) continue;
        bool on = (i == idx);
        lv_obj_t *c = lv_obj_get_child(b, 0);
        if (c) {
            lv_obj_set_style_border_color(c, on ? HY_OURO : HY_OURO_ESC, 0);
            lv_obj_set_style_bg_color(c, on ? HY_C(0x302413) : HY_C(0x261d11), 0);
            if (on) {
                lv_obj_set_style_shadow_color(c, HY_OURO, 0);
                lv_obj_set_style_shadow_width(c, 10, 0);
            } else {
                lv_obj_set_style_shadow_width(c, 0, 0);
            }
        }
        lv_obj_t *lb = lv_obj_get_child(b, lv_obj_get_child_cnt(b) - 1);
        if (lb) lv_obj_set_style_text_color(lb, on ? HY_OURO_CLARO : HY_HINT, 0);
    }
}

/* ── TESTE DE TOUCH/TAMANHO (temporario, remover apos validar) ─────────── */
static void teste_touch_cb(lv_event_t *e)
{
    lv_indev_t *indev = lv_event_get_indev(e);
    if (!indev) return;
    lv_point_t pt;
    lv_indev_get_point(indev, &pt);
    printf("TOUCH %4d %4d %s\r\n", pt.x, pt.y,
           lv_event_get_code(e) == LV_EVENT_PRESSED ? "DOWN" : "MOVE");
}

static void teste_touch_montar(void)
{
    lv_obj_t *ov = lv_obj_create(hy_tela);              /* last child = topo */
    lv_obj_remove_style_all(ov);
    lv_obj_set_pos(ov, 0, 0);
    lv_obj_set_size(ov, HY_W, HY_H);                    /* TELA INTEIRA 800x480 */
    lv_obj_set_style_bg_color(ov, HY_C(0x000000), 0);
    lv_obj_set_style_bg_opa(ov, LV_OPA_20, 0);
    lv_obj_add_flag(ov, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ov, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ov, teste_touch_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(ov, teste_touch_cb, LV_EVENT_PRESSING, NULL);

    struct { int x, y, w, h; } seg[6] = {
        { 0,   239, HY_W, 2 },                          /* linha horizontal inteira */
        { 399, 0,   2, HY_H },                          /* linha vertical inteira */
        { 0,   0,   14, 14 },                           /* cantos */
        { HY_W - 14, 0, 14, 14 },
        { 0,   HY_H - 14, 14, 14 },
        { HY_W - 14, HY_H - 14, 14, 14 },
    };
    for (int i = 0; i < 6; i++) {
        lv_obj_t *l = lv_obj_create(ov);
        lv_obj_remove_style_all(l);
        lv_obj_set_pos(l, seg[i].x, seg[i].y);
        lv_obj_set_size(l, seg[i].w, seg[i].h);
        lv_obj_set_style_bg_color(l, HY_C(0xd9a24b), 0);
        lv_obj_set_style_bg_opa(l, LV_OPA_COVER, 0);
        lv_obj_clear_flag(l, LV_OBJ_FLAG_CLICKABLE);
    }
    printf("TESTE TOUCH ativo: 800x480, toque nos cantos/linhas\r\n");
}

/* ─────────────────────────────────────────────────────────────── boot ─── ── */
void hybridskull_boot(void)
{
    hy_init();
    hy.amp = 0;                                                              /* twostone_studio */
    {
        const HyAmp *a = hy_amp_slug("twostone_studio");
        if (a) hy.amp = (int)(a - hy_amps);
    }

    /* congela o video/decodificador para não brigar pelo motor JPEG-be
       (usa só a API existente de video_loop, sem editar arquivos velhos) */
    video_toggle_pause();

    /* aplica preset inicial (Banco 1 Slot 1 = Clean Studio) no estado */
    HySlot *s0 = &hy.banks[0][0];
    hy.chan = s0->chan;

    hy_tela = lv_obj_create(NULL);
    lv_obj_set_size(hy_tela, HY_W, HY_H);
    lv_obj_set_style_bg_color(hy_tela, HY_BG0, 0);

    hy_apply_bg();

    top_montar(hy_tela);
    /* hw strip removida a pedido: switches fisicos GPIO 29-35,49 ja sao os
       bancos; a tela ganhou 42px e nao duplica controle inutil */

    /* containers das 6 views */
    for (int i = 0; i < HY_V_N; i++) {
        hy_view_box[i] = lv_obj_create(hy_tela);
        lv_obj_remove_style_all(hy_view_box[i]);
        lv_obj_set_pos(hy_view_box[i], 0, HY_VIEW_Y);
        lv_obj_set_size(hy_view_box[i], HY_W, HY_VIEW_H);
        lv_obj_clear_flag(hy_view_box[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(hy_view_box[i], ev_tap_vazio, LV_EVENT_CLICKED, NULL);
    }

    hy_view_signal_montar(hy_view_box[HY_V_SIGNAL]);   /* aba inicial (rápida) */

    if (hy_lbl_preset) lv_label_set_text(hy_lbl_preset, s0->name);
    hb_bank_on(0);
    hy_set_view(HY_V_SIGNAL);
    hy_apply_accent();

    /* sobrepõe a tela do palco (que continua viva como referência) */
    lv_scr_load_anim(hy_tela, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    lv_timer_create(menu_tmr_cb, 400, NULL);    /* MENU some/reaparece ao toque */
    ESP_LOGI("hy_ui", "HybridSkull montado (amp: %s)", hy_amps[hy.amp].slug);
}
