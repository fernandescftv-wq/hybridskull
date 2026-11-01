/* hybrid_view_gear.c - aba GEAR: rail de categorias + grade de pedais da
 * categoria atual (foto real + nome); toque no card adiciona o pedal ao
 * primeiro slot vazio da cadeia (visual).
 */
#include "hybrid_ui.h"
#include "hybrid_assets.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static lv_obj_t *s_grid;
static lv_obj_t *s_cat_btn[7];

static void grid_reconstruir(void);

static void cat_set(int i)
{
    if (i < 0 || i >= hy_n_gear_cats) return;
    hy.gear_cat = i;
    for (int k = 0; k < hy_n_gear_cats; k++) {
        if (!s_cat_btn[k]) continue;
        bool on = (k == hy.gear_cat);
        lv_obj_set_style_border_color(s_cat_btn[k], on ? hy_acc : lv_color_hex(0x241a10), 0);
        lv_obj_set_style_border_width(s_cat_btn[k], on ? 2 : 0, 0);
        lv_obj_set_style_text_color(s_cat_btn[k], on ? HY_OURO_CLARO : HY_HINT, 0);
        lv_obj_set_style_bg_opa(s_cat_btn[k], on ? LV_OPA_60 : LV_OPA_20, 0);
    }
    grid_reconstruir();
}

static void ev_cat(lv_event_t *e)
{
    cat_set((int)(intptr_t)lv_event_get_user_data(e));
}

static void ev_gcard(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    HySlot *s = &hy.banks[hy.cur_bank][hy.cur_slot];
    bool ja = false;                          /* cada pedal 1x na cadeia */
    for (int i = 0; i < 8 && !ja; i++)
        if (strcmp(s->chain[i], hy_pedais[idx].id) == 0)
            ja = true;
    if (!ja) {
        for (int i = 0; i < 8; i++) {
            if (!s->chain[i][0]) {
                snprintf(s->chain[i], sizeof(s->chain[i]), "%s", hy_pedais[idx].id);
                break;
            }
        }
    }
    hy_view_signal_novo_amp();               /* reconstrói cadeia + base8 */
    hy_view_presets_refresh();
}

void hy_view_gear_montar(lv_obj_t *pai)
{
    /* ── rail das categorias (196px@1440 -> 110px) ────────────────────────── */
    lv_obj_t *rail = lv_obj_create(pai);
    lv_obj_remove_style_all(rail);
    lv_obj_set_pos(rail, 0, 0);
    lv_obj_set_size(rail, 110, HY_VIEW_H);
    lv_obj_clear_flag(rail, LV_OBJ_FLAG_SCROLLABLE);
    hy_grad(rail, HY_GCATA_A, HY_GCATA_B);
    lv_obj_set_style_border_width(rail, 1, 0);
    lv_obj_set_style_border_color(rail, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_side(rail, LV_BORDER_SIDE_RIGHT, 0);

    int rh = HY_VIEW_H / hy_n_gear_cats;
    for (int i = 0; i < hy_n_gear_cats; i++) {
        lv_obj_t *b = lv_btn_create(rail);
        lv_obj_set_pos(b, 2, i * rh + 2);
        lv_obj_set_size(b, 106, rh - 4);
        lv_obj_set_style_radius(b, 5, 0);
        lv_obj_set_style_bg_color(b, HY_C(0x171009), 0);
        lv_obj_set_style_bg_opa(b, LV_OPA_20, 0);
        lv_obj_set_style_border_width(b, 0, 0);
        lv_obj_set_style_shadow_width(b, 0, 0);
        lv_obj_clear_flag(b, LV_OBJ_FLAG_SCROLLABLE);
        hy_txt(b, hy_gear_cats[i].label, &hy_fonte_c20, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
        lv_obj_t *lb = lv_obj_get_child(b, -1);
        lv_obj_align(lb, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(b, ev_cat, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        s_cat_btn[i] = b;
    }

    /* ── grade (cards .gcard flex wrap) ──────────────────────────────────── */
    s_grid = lv_obj_create(pai);
    lv_obj_remove_style_all(s_grid);
    lv_obj_set_pos(s_grid, 114, 4);
    lv_obj_set_size(s_grid, HY_W - 118, HY_VIEW_H - 8);
    lv_obj_set_scroll_dir(s_grid, LV_DIR_VER);
    lv_obj_set_style_pad_column(s_grid, 9, 0);
    lv_obj_set_style_pad_row(s_grid, 9, 0);
    lv_obj_set_style_pad_left(s_grid, 4, 0);
    hy_limpar_scroll(s_grid);
    lv_obj_set_flex_flow(s_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(s_grid, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);

    cat_set(hy.gear_cat);
}

void hy_view_gear_refresh(void)
{
    for (int k = 0; k < hy_n_gear_cats; k++) {
        if (!s_cat_btn[k]) continue;
        bool on = (k == hy.gear_cat);
        lv_obj_set_style_border_color(s_cat_btn[k], on ? hy_acc : lv_color_hex(0x241a10), 0);
        lv_obj_set_style_border_width(s_cat_btn[k], on ? 2 : 0, 0);
        lv_obj_set_style_text_color(s_cat_btn[k], on ? HY_OURO_CLARO : HY_HINT, 0);
        lv_obj_set_style_bg_opa(s_cat_btn[k], on ? LV_OPA_60 : LV_OPA_20, 0);
    }
    grid_reconstruir();
}

static void grid_reconstruir(void)
{
    if (!s_grid) return;
    lv_obj_clean(s_grid);

    for (int i = 0; i < hy_n_pedais; i++) {
        if (strcmp(hy_pedais[i].cat, hy_gear_cats[hy.gear_cat].id)) continue;
        /* todos os pedais de volta (nenhuma familia pula) */
        lv_obj_t *card = lv_btn_create(s_grid);
        lv_obj_set_size(card, 96, 126);
        lv_obj_set_style_radius(card, 5, 0);
        lv_obj_set_style_bg_color(card, HY_C(0x1a140c), 0);
        lv_obj_set_style_bg_grad_color(card, HY_C(0x100c08), 0);
        lv_obj_set_style_bg_grad_dir(card, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_border_color(card, HY_OURO_ESC, 0);
        lv_obj_set_style_border_width(card, 0, 0);
        lv_obj_set_style_shadow_width(card, 0, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(card, ev_gcard, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        lv_obj_t *im = lv_img_create(card);         /* foto 56x88 1:1 */
        lv_img_dsc_t *d = hy_assets_pedal_gear(hy_pedais[i].id);
        if (d) {
            lv_img_set_src(im, d);
            lv_img_set_pivot(im, 0, 0);
            lv_img_set_zoom(im, 256);     /* 1:1 (arquivo p_ 80x120) */
            lv_img_set_pivot(im, 0, 0);
            lv_obj_align(im, LV_ALIGN_TOP_MID, 0, 0);
        }

    }
}
