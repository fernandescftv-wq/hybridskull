/* hybrid_view_amp.c - aba AMP: nome, arte 600x255 com glow do accent,
 * chips CH1-4 e seletor rolável com as 31 skins (thumbs 190x81 -> 70x30).
 */
#include "hybrid_ui.h"
#include "hybrid_assets.h"
#include "hybrid_accents.h"
#include <stdio.h>

static lv_obj_t *s_art_box, *s_art_img, *s_ch_row;
static lv_obj_t *s_pick[31], *s_pick_img[31], *s_pick_lb[31];

/* toque na arte do amp = zoom (mesma interação do SIGNAL) */
static void ev_art_zoom(lv_event_t *e)
{
    (void)e;
    const HyAmp *a = &hy_amps[hy.amp];
    lv_img_dsc_t *d = hy_assets_art(a->slug);
    if (!d) return;
    char cap[40];
    snprintf(cap, sizeof(cap), "CH1-%d · CONTROLES FICTICIOS NA FOTO", a->ch);
    hy_zoom_mostrar(d, a->name, cap, 0);
}

static void ev_pick(lv_event_t *e)
{
    hy_set_amp((int)(intptr_t)lv_event_get_user_data(e));
}

static void ev_chan(lv_event_t *e)
{
    hy_set_chan((int)(intptr_t)lv_event_get_user_data(e) + 1);
}

/* chip .chip do CSS: dot 5px + label */
static lv_obj_t *ch_chip(lv_obj_t *pai, int n, int x, int y)
{
    lv_obj_t *c = lv_btn_create(pai);
    lv_obj_set_pos(c, x, y);
    lv_obj_set_size(c, 64, 26);
    lv_obj_set_style_radius(c, 5, 0);
    lv_obj_set_style_bg_color(c, HY_C(0x2b1f10), 0);
    lv_obj_set_style_bg_grad_color(c, HY_C(0x160f08), 0);
    lv_obj_set_style_bg_grad_dir(c, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(c, LV_OPA_70, 0);
    lv_obj_set_style_border_color(c, HY_OURO_ESC, 0);
    lv_obj_set_style_border_width(c, 1, 0);
    lv_obj_set_style_shadow_width(c, 0, 0);
    lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *dot = lv_obj_create(c);                       /* .dot */
    lv_obj_remove_style_all(dot);
    lv_obj_set_size(dot, 5, 5);
    lv_obj_set_style_radius(dot, 3, 0);
    lv_obj_set_style_bg_color(dot, HY_C(0x333333), 0);
    lv_obj_align(dot, LV_ALIGN_LEFT_MID, 6, 0);
    dot->user_data = NULL;
    char t[8];
    snprintf(t, sizeof(t), "CH%d", n);
    hy_txt(c, t, &hy_fonte_c20, HY_K, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_t *lb = lv_obj_get_child(c, -1);
    lv_obj_align(lb, LV_ALIGN_CENTER, 2, 0);
    lv_obj_add_event_cb(c, ev_chan, LV_EVENT_CLICKED, (void *)(intptr_t)(n - 1));
    hy_amp_chunks[n - 1] = c;
    return c;
}

void hy_view_amp_montar(lv_obj_t *pai)
{
    /* ── ampSkin: glow = --acc (drop-shadow do CSS); sem nome no topo ── */
    s_art_box = lv_obj_create(pai);
    lv_obj_remove_style_all(s_art_box);
    lv_obj_set_size(s_art_box, 744, 304);
    lv_obj_set_pos(s_art_box, (HY_W - 744) / 2, 2);
    lv_obj_set_style_radius(s_art_box, 12, 0);
    lv_obj_set_style_border_width(s_art_box, 0, 0);        /* sem borda dura (marcas) */
    lv_obj_add_flag(s_art_box, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s_art_box, ev_art_zoom, LV_EVENT_CLICKED, NULL);
    lv_obj_clear_flag(s_art_box, LV_OBJ_FLAG_SCROLLABLE);

    s_art_img = lv_img_create(s_art_box);
    lv_img_set_pivot(s_art_img, 0, 0);
    lv_img_set_zoom(s_art_img, 256);      /* arte 560x240 1:1: sem borrao */
    lv_obj_align(s_art_img, LV_ALIGN_TOP_LEFT, 0, 0);

    /* ── chRow ───────────────────────────────────────────────────────────── */
    s_ch_row = lv_obj_create(pai);
    lv_obj_remove_style_all(s_ch_row);
    lv_obj_set_size(s_ch_row, 336, 30);
    lv_obj_set_pos(s_ch_row, (HY_W - 336) / 2, 308);   /* linha propria */
    lv_obj_clear_flag(s_ch_row, LV_OBJ_FLAG_SCROLLABLE);
    ch_chip(s_ch_row, 1, 0, 1);
    ch_chip(s_ch_row, 2, 70, 1);
    ch_chip(s_ch_row, 3, 140, 1);
    ch_chip(s_ch_row, 4, 210, 1);

    /* ── picker: tira rolavel com TODAS as thumbs (web .apick) ──────────── */
    hy_amp_picker_box = lv_obj_create(pai);
    lv_obj_remove_style_all(hy_amp_picker_box);
    lv_obj_set_pos(hy_amp_picker_box, 0, 340);          /* fechado nos 398 da view */
    lv_obj_set_size(hy_amp_picker_box, HY_W, HY_VIEW_H - 340);
    lv_obj_set_scroll_dir(hy_amp_picker_box, LV_DIR_HOR);
    lv_obj_set_scrollbar_mode(hy_amp_picker_box, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(hy_amp_picker_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hy_amp_picker_box, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_left(hy_amp_picker_box, 17, 0);
    lv_obj_set_style_pad_top(hy_amp_picker_box, 3, 0);
    lv_obj_set_style_pad_bottom(hy_amp_picker_box, 3, 0);   /* 3+84+3 = 90 */
    lv_obj_set_style_pad_gap(hy_amp_picker_box, 7, 0);
    hy_grad(hy_amp_picker_box, HY_PICK_A, HY_PICK_B);
    lv_obj_set_style_border_width(hy_amp_picker_box, 1, 0);
    lv_obj_set_style_border_color(hy_amp_picker_box, HY_OURO_ESC, 0);
    lv_obj_set_style_border_side(hy_amp_picker_box, LV_BORDER_SIDE_TOP, 0);

    for (int i = 0; i < hy_n_amps && i < 31; i++) {
        lv_obj_t *it = lv_btn_create(hy_amp_picker_box);
        lv_obj_remove_style_all(it);
        lv_obj_set_size(it, 156, 84);
        lv_obj_clear_flag(it, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *im = lv_img_create(it);
        lv_img_set_pivot(im, 0, 0);
        lv_img_dsc_t *td = hy_assets_thumb(hy_amps[i].slug);
        if (td) lv_img_set_src(im, td);
        lv_obj_align(im, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_t *lb = hy_txt(it, hy_amps[i].name, &hy_fonte_c16, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
        lv_obj_align(lb, LV_ALIGN_BOTTOM_MID, 0, -3);
        lv_obj_set_size(lb, 156, 24);
        lv_obj_set_style_text_align(lb, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(lb, LV_LABEL_LONG_DOT);
        it->user_data = (void *)(intptr_t)i;
        lv_obj_add_event_cb(it, ev_pick, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        s_pick[i] = it; s_pick_img[i] = im; s_pick_lb[i] = lb;
    }

    hy_view_amp_refresh();
}

/* refresh: arte, glow, chips e cursor da tira de thumbs (casco chama) */
void hy_view_amp_refresh(void)
{
    const HyAmp *a = &hy_amps[hy.amp];
    if (!s_art_box) return;
    for (int i = 0; i < hy_n_amps && i < 31; i++) {
        if (!s_pick[i]) continue;
        bool on = (i == hy.amp);
        lv_color_t ac = lv_color_hex(hy_amp_accent_hex(hy_amps[hy.amp].slug));
        lv_obj_set_style_border_color(s_pick_img[i], on ? ac : HY_OURO, 0);
        lv_obj_set_style_border_opa(s_pick_img[i], on ? LV_OPA_MAX : LV_OPA_20, 0);
        lv_obj_set_style_shadow_color(s_pick_img[i], ac, 0);
        lv_obj_set_style_shadow_width(s_pick_img[i], on ? 14 : 0, 0);
        lv_obj_set_style_text_color(s_pick_lb[i], on ? HY_OURO_CLARO : HY_HINT, 0);
    }
    if (s_pick[hy.amp]) lv_obj_scroll_to_view(s_pick[hy.amp], LV_ANIM_ON);
    lv_img_dsc_t *d = hy_assets_art(a->slug);
    if (d) lv_img_set_src(s_art_img, d);
    lv_color_t aacc = lv_color_hex(hy_amp_accent_hex(a->slug)); /* glow segue o amp */
    lv_obj_set_style_shadow_color(s_art_box, aacc, 0);
    lv_obj_set_style_border_color(s_art_box, aacc, 0);
    for (int i = 0; i < 4; i++)
        if (hy_amp_chunks[i])
            hy_chip_on(hy_amp_chunks[i], i + 1 == hy.chan, hy_chan_cor(hy.chan));
}
