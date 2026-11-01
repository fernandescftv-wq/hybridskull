/* hybrid_view_presets.c - aba PRESETS: ornamento + bankRow (8 chips BANCO)
 * + pList (8 slots: nº, nome, CHn·amp) — cliques aplicam o slot (visual).
 */
#include "hybrid_ui.h"
#include <stdio.h>

static lv_obj_t *s_bank_chip[8], *s_prow_nm[8], *s_prow_sub[8], *s_prow_chip[8];
static lv_obj_t *s_prow_btn[8];
static int s_slot_h = 38;

static void ev_bank(lv_event_t *e)
{
    hy_set_slot((int)(intptr_t)lv_event_get_user_data(e), hy.cur_slot);
}

static void ev_prow(lv_event_t *e)
{
    hy_set_slot(hy.cur_bank, (int)(intptr_t)lv_event_get_user_data(e));
}

static void hl(void)                       /* marca banco/slot correntes */
{
    for (int i = 0; i < 8; i++) {
        if (s_bank_chip[i])
            hy_chip_on(s_bank_chip[i], i == hy.cur_bank, HY_OURO);
        bool on = (i == hy.cur_slot);
        for (int k = 0; k < 3; k++) {
            lv_obj_t *o = (k == 0) ? s_prow_nm[i] : (k == 1) ? s_prow_sub[i]
                                                             : s_prow_chip[i];
            if (!o) continue;
            if (k < 2)
                lv_obj_set_style_text_color(o, on ? HY_OURO_CLARO
                                                  : (k ? HY_HINT : HY_TXT), 0);
            else
                lv_obj_set_style_border_color(o, on ? HY_OURO : HY_OURO_ESC, 0);
        }
        if (s_prow_btn[i]) {
            lv_obj_set_style_border_color(s_prow_btn[i], on ? HY_OURO : HY_OURO_ESC, 0);
            lv_obj_set_style_border_width(s_prow_btn[i], on ? 2 : 1, 0);
        }
    }
}

void hy_view_presets_montar(lv_obj_t *pai)
{
    lv_obj_set_style_pad_top(pai, 10, 0);

    /* ornamento do topo */
    hy_ornamento(pai, 0, "BANCO DE PRESETS", HY_W - 40);
    lv_obj_set_pos(lv_obj_get_child(pai, lv_obj_get_child_cnt(pai) - 1), 20, 2);

    /* #bankRow: 8 chips */
    int bw = (HY_W - 28 - 7 * 5) / 8;
    for (int i = 0; i < 8; i++) {
        char t[12];
        snprintf(t, sizeof(t), "Banco %d", i + 1);
        lv_obj_t *c = hy_chip(pai, t, 14 + i * (bw + 5), 20, bw, 30, &hy_fonte_c20);
        lv_obj_add_event_cb(c, ev_bank, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        s_bank_chip[i] = c;
    }

    /* #pList: 8 linhas (.prow) */
    int py = 58;
    for (int i = 0; i < 8; i++) {
        lv_obj_t *r = lv_btn_create(pai);
        lv_obj_remove_style_all(r);
        lv_obj_set_pos(r, 14, py - 4 + i * (s_slot_h + 5));
        s_prow_btn[i] = r;        lv_obj_set_size(r, HY_W - 28, s_slot_h);
        lv_obj_set_style_radius(r, 6, 0);
        lv_obj_set_style_bg_color(r, HY_C(0x171009), 0);
        lv_obj_set_style_bg_opa(r, LV_OPA_80, 0);
        lv_obj_set_style_border_color(r, HY_OURO_ESC, 0);
        lv_obj_set_style_border_opa(r, LV_OPA_50, 0);
        lv_obj_set_style_border_width(r, 1, 0);
        lv_obj_set_style_pad_left(r, 26, 0);
        lv_obj_clear_flag(r, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(r, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(r, ev_prow, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        char t8[4];
        snprintf(t8, sizeof(t8), "%d", i + 1);              /* .no (badge) */
        lv_obj_t *no = lv_obj_create(r);
        lv_obj_remove_style_all(no);
        lv_obj_set_size(no, 14, 14);
        lv_obj_set_style_radius(no, 7, 0);
        lv_obj_set_style_bg_color(no, HY_C(0x2b2113), 0);
        lv_obj_align(no, LV_ALIGN_LEFT_MID, 4, 0);
        lv_obj_clear_flag(no, LV_OBJ_FLAG_SCROLLABLE);
        hy_txt(no, t8, &hy_fonte_c12, HY_K, LV_ALIGN_CENTER, 0, 0);

        s_prow_nm[i] = hy_txt(r, "", &hy_fonte_c16, HY_TXT, LV_ALIGN_DEFAULT, 0, 0);
        lv_obj_set_pos(s_prow_nm[i], 30, 5);
        s_prow_sub[i] = hy_txt(r, "", &hy_fonte_c12, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
        lv_obj_set_pos(s_prow_sub[i], 30, 24);

        s_prow_chip[i] = hy_chip(pai, "CH1", HY_W - 74, 0, 46, 20, &hy_fonte_c12);
        (void)s_prow_sub[i];                       /* capa: nome forte c16 */
        lv_obj_set_parent(s_prow_chip[i], r);
        lv_obj_align(s_prow_chip[i], LV_ALIGN_RIGHT_MID, -8, 0);
        s_prow_chip[i]->user_data = NULL;
        lv_obj_add_flag(s_prow_chip[i], LV_OBJ_FLAG_HIDDEN);
    }

    hy_presets_row = NULL;                  /* (referência p/ debug externo) */
    hy_view_presets_refresh();
}

void hy_view_presets_refresh(void)
{
    for (int i = 0; i < 8; i++) {
        HySlot *s = &hy.banks[hy.cur_bank][i];
        bool empty = !s->name[0];
        if (s_prow_nm[i]) {
            lv_label_set_text(s_prow_nm[i], empty ? "- vazio -" : s->name);
            lv_obj_set_style_opa(s_prow_nm[i], empty ? LV_OPA_40 : LV_OPA_COVER, 0);
        }
        if (s_prow_sub[i]) {
            char sub[56];
            if (empty) {
                snprintf(sub, sizeof(sub), "slot livre");
            } else {
                const char *an = (s->amp >= 0) ? hy_amps[s->amp].name : "—";
                snprintf(sub, sizeof(sub), "CH%d · %s", s->chan, an);
            }
            lv_label_set_text(s_prow_sub[i], sub);
        }
        if (s_prow_chip[i]) {
            if (empty) {
                lv_obj_add_flag(s_prow_chip[i], LV_OBJ_FLAG_HIDDEN);
            } else {
                char t[6];
                snprintf(t, sizeof(t), "CH%d", s->chan);
                lv_label_set_text(lv_obj_get_child(s_prow_chip[i],
                                  lv_obj_get_child_cnt(s_prow_chip[i]) - 1), t);
                lv_obj_clear_flag(s_prow_chip[i], LV_OBJ_FLAG_HIDDEN);
                hy_chip_on(s_prow_chip[i], false, HY_OURO);
                /* borda extra no chip = cor do canal do slot */
                lv_obj_set_style_border_color(s_prow_chip[i], hy_chan_cor(s->chan), 0);
            }
        }
    }
    hl();
}
