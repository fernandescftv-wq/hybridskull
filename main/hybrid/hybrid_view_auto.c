/* hybrid_view_auto.c - aba AUTO: placa "AUTO PILOTO" com display Digital7
 * (.big 56px@1440 -> 31px = fonte d31), INTERVALO (2/4/8/16s), autoBar,
 * TOCAR/PARAR. F1: visual + chaves de estado (rotação real na F2 com esp-now).
 */
#include "hybrid_ui.h"
#include <stdio.h>
#include <stdint.h>

static lv_obj_t *s_placa, *s_big, *s_sm, *s_fill;
static lv_obj_t *s_iv[4];
static lv_obj_t *s_tocar, *s_parar;
static const char *IV_TXT[4] = { "2s", "4s", "8s", "16s" };

static void iv_set(int i)
{
    hy.auto_iv = i;
    for (int k = 0; k < 4; k++)
        if (s_iv[k]) hy_chip_on(s_iv[k], k == i, HY_OURO);
}

static void ev_iv(lv_event_t *e)
{
    iv_set((int)(intptr_t)lv_event_get_user_data(e));
}

static void ev_tocar(lv_event_t *e)
{
    (void)e;
    hy.auto_run = 1;
    hy_chip_on(s_tocar, true, HY_OURO);
    hy_chip_on(s_parar, false, HY_OURO);
    lv_label_set_text(s_big, "AUTO");
    lv_label_set_text(s_sm, "RODANDO · PRESET A CADA INTERVALO");
    if (s_fill) lv_obj_set_width(s_fill, 176 * 38 / 100);
}

static void ev_parar(lv_event_t *e)
{
    (void)e;
    hy.auto_run = 0;
    hy_chip_on(s_tocar, false, HY_OURO);
    hy_chip_on(s_parar, true, HY_OURO);
    lv_label_set_text(s_big, "STANDBY");
    lv_label_set_text(s_sm, "AGUARDANDO · SEM TROCA AUTOMATICA");
    if (s_fill) lv_obj_set_width(s_fill, 0);
}

void hy_view_auto_montar(lv_obj_t *pai)
{
    s_placa = hy_placa(pai, (HY_W - 460) / 2, 8, 460, HY_VIEW_H - 16);

    /* ornamento interno (filho da placa) */
    lv_obj_t *orn = hy_ornamento(s_placa, 0, "AUTO PILOTO", 400);
    lv_obj_set_pos(orn, 30, 12);

    /* display: .big (Digital7 31px) + .sm */
    s_big = hy_txt(s_placa, "STANDBY", &hy_fonte_d31, HY_OURO, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_center(s_big);
    lv_obj_set_y(s_big, 62);
    lv_obj_set_style_text_letter_space(s_big, 3, 0);
    s_sm = hy_txt(s_placa, "AGUARDANDO · SEM TROCA AUTOMATICA", &hy_fonte_c20,
                  HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_center(s_sm);
    lv_obj_set_y(s_sm, 104);
    lv_obj_set_width(s_sm, 400);                       /* dots se pedirem muito */
    lv_label_set_long_mode(s_sm, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(s_sm, LV_TEXT_ALIGN_CENTER, 0);

    /* autoRow: INTERVALO + 4 chips */
    lv_obj_t *lb = hy_txt(s_placa, "INTERVALO", &hy_fonte_c20, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_align(lb, LV_ALIGN_TOP_MID, 0, 126);
    for (int i = 0; i < 4; i++) {
        lv_obj_t *c = hy_chip(s_placa, IV_TXT[i], 66 + i * 88, 158, 76, 24, &hy_fonte_c20);
        lv_obj_add_event_cb(c, ev_iv, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        s_iv[i] = c;
    }
    iv_set(hy.auto_iv);

    /* autoBar (fill 38% quando rodando) */
    lv_obj_t *barra = hy_barra(s_placa, 100, 188, 260, 10);
    s_fill = lv_obj_create(barra);
    lv_obj_remove_style_all(s_fill);
    lv_obj_set_style_radius(s_fill, 5, 0);
    lv_obj_set_style_bg_color(s_fill, HY_BRONZE, 0);
    lv_obj_set_style_bg_grad_color(s_fill, HY_AMBAR, 0);
    lv_obj_set_style_bg_grad_dir(s_fill, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_size(s_fill, 0, 6);
    lv_obj_align(s_fill, LV_ALIGN_LEFT_MID, 1, 0);
    if (hy.auto_run) lv_obj_set_width(s_fill, 176 * 38 / 100);

    /* TOCAR (latao cheio) / PARAR (escuro) */
    s_tocar = hy_tbtn(s_placa, "TOCAR", 120, 216, 106, 30, ev_tocar, NULL);
    s_parar = hy_tbtn(s_placa, "PARAR", 234, 216, 106, 30, ev_parar, NULL);
    hy_chip_on(s_tocar, hy.auto_run != 0, HY_OURO);
    hy_chip_on(s_parar, hy.auto_run == 0, HY_OURO);
    if (hy.auto_run) {
        lv_label_set_text(s_big, "AUTO");
        lv_label_set_text(s_sm, "RODANDO · PRESET A CADA INTERVALO");
        lv_obj_set_width(s_fill, 176 * 38 / 100);
    }
}

void hy_view_auto_refresh(void)
{
    /* F2: exibicao vivida (slots girando). F1: estado estatico. */
}
