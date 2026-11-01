/* hybrid_view_signal.c - aba SIGNAL: amp no topo (headBig), cadeia de pedais
 * embaixo (fotos grandes), wires. A faixa base8 de loops foi removida
 * (obsoleta: os loops reais vieram/serao controlados no palco/ESP-NOW).
 */
#include "hybrid_ui.h"
#include "hybrid_assets.h"
#include "hybrid_accents.h"
#include <stdio.h>
#include <ctype.h>

lv_obj_t *hy_sig_head, *hy_sig_chain;

static void cadeia_reconstruir(void);

static lv_obj_t *s_amp_led;
static lv_obj_t *s_mod_img[8];
static lv_obj_t *s_io_in, *s_io_out;

void hy_zoom_pedal(int slot)
{
    const char *id = hy.banks[hy.cur_bank][hy.cur_slot].chain[slot];
    const HyPedal *p = hy_pedal_id(id);
    if (!p) return;
    lv_img_dsc_t *d = hy_assets_pedal_zoom(p->id);
    if (!d) return;
    char cap[40];
    snprintf(cap, sizeof(cap), "LOOP %d · %s", slot + 1, p->cat);
    for (char *c = cap; *c; c++) *c = (char)toupper((unsigned char)*c);
    hy_zoom_mostrar(d, p->name, cap, 0);                /* auto-fit */
}

static void ev_pedal(lv_event_t *e)
{
    hy_zoom_pedal((int)(intptr_t)lv_event_get_user_data(e));
}

static void ev_amp_zoom(lv_event_t *e)
{
    (void)e;
    const HyAmp *a = &hy_amps[hy.amp];
    lv_img_dsc_t *d = hy_assets_art(a->slug);
    if (!d) return;
    char cap[40];
    snprintf(cap, sizeof(cap), "CH1-%d · CONTROLES FICTICIOS NA FOTO", a->ch);
    hy_zoom_mostrar(d, a->name, cap, 0);                /* auto-fit */
}

/* ── cadeia (contemnte de #chainScroll) ──────────────────────────────────── */
static void cadeia_reconstruir(void)
{
    if (!hy_sig_chain) return;
    HySlot *s = &hy.banks[hy.cur_bank][hy.cur_slot];
    lv_obj_clean(hy_sig_chain);
    /* linha: IN wire mod wire mod wire ... OUT (todo flex row) */
    lv_obj_set_flex_flow(hy_sig_chain, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hy_sig_chain, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(hy_sig_chain, 10, 0);
    s_io_in = hy_txt(hy_sig_chain, "IN", &hy_fonte_c12, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
    for (int i = 0; i < 8 && s->chain[i][0]; i++) {
        const HyPedal *p = hy_pedal_id(s->chain[i]);
        if (!p) break;
        hy_wire(hy_sig_chain, 0, 0, 20, i == 0);
        hy_wire_no(hy_sig_chain, 0, 0, i == 0);

        lv_obj_t *m = lv_obj_create(hy_sig_chain);          /* .mod */
        lv_obj_remove_style_all(m);
        lv_obj_set_size(m, 78, 118);
        lv_obj_set_flex_flow(m, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(m, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(m, LV_OBJ_FLAG_SCROLLABLE);
        m->user_data = NULL;

        lv_obj_t *box = lv_obj_create(m);                   /* .box: foto 1:1 */
        lv_obj_remove_style_all(box);
        lv_obj_set_size(box, 76, 114);
        box->user_data = NULL;
        lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
        s_mod_img[i] = lv_img_create(box);
        lv_img_dsc_t *d = hy_assets_pedal_chain(p->id);   /* s_ raw alpha */
        if (d) {
            lv_img_set_src(s_mod_img[i], d);
            lv_obj_align(s_mod_img[i], LV_ALIGN_TOP_MID, 0, 0);
            lv_img_set_zoom(s_mod_img[i], 256);     /* 1:1 */
            lv_obj_set_style_shadow_width(s_mod_img[i], 12, 0);
            lv_obj_set_style_shadow_color(s_mod_img[i], lv_color_hex(0x000000), 0);
            lv_obj_set_style_shadow_opa(s_mod_img[i], LV_OPA_60, 0);
        }
        hy_led(box, 62, 3, 4, HY_VERDE, true);              /* .led do mod */

        /* clicar = zoom */
        lv_obj_add_flag(box, LV_OBJ_FLAG_CLICKABLE);
        box->user_data = (void *)(intptr_t)i;
        lv_obj_add_event_cb(box, ev_pedal, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
    hy_wire(hy_sig_chain, 0, 0, 20, false);
    s_io_out = hy_txt(hy_sig_chain, "OUT", &hy_fonte_c12, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
}

/* ── monta a view ────────────────────────────────────────────────────────── */
void hy_view_signal_novo_amp(void)
{
    /* amp mudou: head + cadeia (o padrão usado pela web: novo amp = redesenho) */
    if (hy_sig_head)
        lv_img_set_src(hy_sig_head, hy_assets_head(hy_amps[hy.amp].slug));
    cadeia_reconstruir();
}

void hy_view_signal_montar(lv_obj_t *pai)
{
    /* #sigTop > .ampTop: hero headBig 333x141, led accent (topo-centro) */
    hy_sig_head = lv_img_create(pai);
    lv_img_set_src(hy_sig_head, hy_assets_head(hy_amps[hy.amp].slug));
    lv_img_set_zoom(hy_sig_head, 256);        /* 1:1 (arquivo 480x200) */
    lv_obj_set_pos(hy_sig_head, (HY_W - 592) / 2, 0);
    lv_obj_add_flag(hy_sig_head, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(hy_sig_head, ev_amp_zoom, LV_EVENT_CLICKED, NULL);

    s_amp_led = hy_led(pai, 0, 0, 4, hy_acc, true);
    lv_obj_align(s_amp_led, LV_ALIGN_TOP_LEFT, (HY_W + 592) / 2 - 12, 8);

    /* #chainScroll (pedais 78x118 + caps), deitado horizontal */
    hy_sig_chain = lv_obj_create(pai);
    lv_obj_remove_style_all(hy_sig_chain);
    lv_obj_set_pos(hy_sig_chain, 0, 250);
    lv_obj_set_size(hy_sig_chain, HY_W, HY_VIEW_H - 250);   /* ate a beirada */
    lv_obj_set_style_bg_color(hy_sig_chain, HY_C(0x0c0a07), 0);
    lv_obj_set_style_bg_opa(hy_sig_chain, LV_OPA_20, 0);
    lv_obj_set_scroll_dir(hy_sig_chain, LV_DIR_HOR);
    lv_obj_set_style_pad_left(hy_sig_chain, 10, 0);
    hy_limpar_scroll(hy_sig_chain);
    cadeia_reconstruir();

}
