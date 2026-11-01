/* hybrid_components.c - primitivas visuais (estilos do index.html) */
#include "hybrid_components.h"
#include "hybrid_ui.h"
#include "hy_gem.h"

void hy_grad(lv_obj_t *o, lv_color_t a, lv_color_t b)
{
    lv_obj_set_style_bg_color(o, a, 0);
    lv_obj_set_style_bg_grad_color(o, b, 0);
    lv_obj_set_style_bg_grad_dir(o, LV_GRAD_DIR_VER, 0);
}

void hy_limpar_scroll(lv_obj_t *o)
{
    lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
}

lv_obj_t *hy_txt(lv_obj_t *pai, const char *txt, const lv_font_t *f, lv_color_t cor,
                 lv_align_t al, int dx, int dy)
{
    lv_obj_t *l = lv_label_create(pai);
    lv_label_set_text(l, txt);
    lv_obj_set_style_text_font(l, f, 0);
    lv_obj_set_style_text_color(l, cor, 0);
    if (al != LV_ALIGN_DEFAULT) lv_obj_align(l, al, dx, dy);
    return l;
}

/* rebite: circulo 3px dos cantos (radial-gradient no CSS) */
static void rebite(lv_obj_t *p, lv_align_t al, int dx, int dy)
{
    lv_obj_t *r = lv_obj_create(p);
    lv_obj_remove_style_all(r);
    lv_obj_set_size(r, 3, 3);
    lv_obj_set_style_radius(r, 2, 0);
    lv_obj_set_style_bg_color(r, HY_OURO_CLARO, 0);
    lv_obj_set_style_bg_opa(r, LV_OPA_90, 0);
    lv_obj_set_style_border_width(r, 1, 0);
    lv_obj_set_style_border_color(r, HY_C(0x5A4210), 0);
    lv_obj_align(r, al, dx, dy);
}

lv_obj_t *hy_placa(lv_obj_t *pai, int x, int y, int w, int h)
{
    lv_obj_t *p = lv_obj_create(pai);
    lv_obj_remove_style_all(p);
    lv_obj_set_pos(p, x, y);
    lv_obj_set_size(p, w, h);
    hy_grad(p, HY_C(0x2b2214), HY_C(0x251b0e));
    lv_obj_set_style_radius(p, 8, 0);
    lv_obj_set_style_border_color(p, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(p, 1, 0);
    lv_obj_set_style_shadow_width(p, 28, 0);
    lv_obj_set_style_shadow_color(p, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(p, LV_OPA_50, 0);
    /* moldura gravada dupla (::before inset 6 ouro .26; ::after inset 11 bronze) */
    lv_obj_t *m1 = lv_obj_create(p);
    lv_obj_remove_style_all(m1);
    lv_obj_set_pos(m1, 5, 5);
    lv_obj_set_size(m1, w - 10, h - 10);
    lv_obj_set_style_border_color(m1, HY_OURO, 0);
    lv_obj_set_style_border_opa(m1, LV_OPA_30, 0);
    lv_obj_set_style_border_width(m1, 1, 0);
    lv_obj_set_style_radius(m1, 6, 0);
    lv_obj_set_style_bg_opa(m1, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(m1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *m2 = lv_obj_create(p);
    lv_obj_remove_style_all(m2);
    lv_obj_set_pos(m2, 9, 9);
    lv_obj_set_size(m2, w - 18, h - 18);
    lv_obj_set_style_border_color(m2, HY_BRONZE, 0);
    lv_obj_set_style_border_opa(m2, LV_OPA_50, 0);
    lv_obj_set_style_border_width(m2, 1, 0);
    lv_obj_set_style_radius(m2, 4, 0);
    lv_obj_set_style_bg_opa(m2, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(m2, LV_OBJ_FLAG_SCROLLABLE);

    rebite(p, LV_ALIGN_TOP_LEFT, 14, 14);
    rebite(p, LV_ALIGN_TOP_RIGHT, -14, 14);
    rebite(p, LV_ALIGN_BOTTOM_LEFT, 14, -14);
    rebite(p, LV_ALIGN_BOTTOM_RIGHT, -14, -14);
    return p;
}

/* .orn: linha ───── ◆ TEXTO ───── (linhas gradientes dos lados) */
lv_obj_t *hy_ornamento(lv_obj_t *pai, int y, const char *txt, int w)
{
    lv_obj_t *row = lv_obj_create(pai);
    lv_obj_remove_style_all(row);
    lv_obj_set_pos(row, 0, y);
    lv_obj_set_size(row, w, 22);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *lab = hy_txt(row, txt, &hy_fonte_c16, HY_K, LV_ALIGN_DEFAULT, 0, 0);
    int lw = lv_obj_get_width(lab) + 24;
    int lh = (w - lw) / 2;
    if (lh > 0 && lh < 400) {
        lv_obj_t *l1 = lv_obj_create(row), *l2 = lv_obj_create(row);
        lv_obj_remove_style_all(l1);
        lv_obj_remove_style_all(l2);
        lv_obj_set_size(l1, lh, 1);
        lv_obj_set_size(l2, lh, 1);
        lv_obj_set_style_bg_color(l1, HY_OURO_ESC, 0);
        lv_obj_set_style_bg_color(l2, HY_OURO_ESC, 0);
        lv_obj_set_style_bg_opa(l1, LV_OPA_70, 0);
        lv_obj_set_style_bg_opa(l2, LV_OPA_70, 0);
        lv_obj_align(l1, LV_ALIGN_LEFT_MID, 0, 0);
        lv_obj_align(l2, LV_ALIGN_RIGHT_MID, 0, 0);
    }
    lv_obj_t *gem = lv_img_create(row);
    lv_img_set_src(gem, &hy_gem);
    lv_obj_align(gem, LV_ALIGN_CENTER, -lw / 2 - 9, 0);
    lv_obj_align(lab, LV_ALIGN_CENTER, 9, 0);
    return row;
}

/* .tbtn: botao de latao da top bar */
lv_obj_t *hy_tbtn(lv_obj_t *pai, const char *txt, int x, int y, int w, int h,
                  lv_event_cb_t cb, void *user)
{
    lv_obj_t *b = lv_btn_create(pai);
    lv_obj_set_pos(b, x, y);
    lv_obj_set_size(b, w, h);
    lv_obj_set_style_radius(b, 5, 0);
    lv_obj_set_style_bg_color(b, HY_BTN_A, 0);
    lv_obj_set_style_bg_grad_color(b, HY_BTN_B, 0);
    lv_obj_set_style_bg_grad_dir(b, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_color(b, HY_OURO_ESC, 0);
    lv_obj_set_style_border_width(b, 1, 0);
    lv_obj_set_style_shadow_width(b, 10, 0);
    lv_obj_set_style_shadow_color(b, lv_color_hex(0x000000), 0);
    lv_obj_t *l = hy_txt(b, txt, &hy_fonte_c16, HY_K, LV_ALIGN_CENTER, 0, 0);
    (void)l;
    hy_press_style(b);
    if (cb) lv_obj_add_event_cb(b, cb, LV_EVENT_CLICKED, user);
    return b;
}

/* .chip/.banco/.ivchip */
lv_obj_t *hy_chip(lv_obj_t *pai, const char *txt, int x, int y, int w, int h,
                  const lv_font_t *f)
{
    lv_obj_t *c = lv_btn_create(pai);
    lv_obj_set_pos(c, x, y);
    lv_obj_set_size(c, w, h);
    lv_obj_set_style_radius(c, 5, 0);
    lv_obj_set_style_bg_opa(c, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(c, HY_C(0x2b1f10), 0);
    lv_obj_set_style_bg_grad_color(c, HY_C(0x160f08), 0);
    lv_obj_set_style_bg_grad_dir(c, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_color(c, HY_OURO_ESC, 0);
    lv_obj_set_style_border_width(c, 1, 0);
    hy_txt(c, txt, f, HY_K, LV_ALIGN_CENTER, 0, 0);
    return c;
}

void hy_press_style(lv_obj_t *o)
{
    static lv_style_t st;
    static bool ok = false;
    if (!ok) {
        lv_style_init(&st);
        lv_style_set_bg_opa(&st, LV_OPA_90);
        lv_style_set_bg_color(&st, HY_C(0x7a5417));   /* flash ambar: click entrou */
        lv_style_set_border_color(&st, HY_OURO_CLARO);
        ok = true;
    }
    lv_obj_add_style(o, &st, LV_STATE_PRESSED);
}

void hy_chip_on(lv_obj_t *chip, bool on, lv_color_t cor)
{
    if (on) {
        lv_obj_set_style_bg_color(chip, cor, 0);
        lv_obj_set_style_bg_grad_color(chip, HY_C(0xffffff), 0);
        lv_obj_set_style_bg_grad_dir(chip, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_bg_opa(chip, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(chip, cor, 0);
        lv_obj_set_style_shadow_color(chip, cor, 0);
        lv_obj_set_style_shadow_width(chip, 14, 0);
        lv_obj_set_style_shadow_opa(chip, LV_OPA_50, 0);
        if (lv_obj_get_child_cnt(chip)) {
            lv_obj_t *l = lv_obj_get_child(chip, lv_obj_get_child_cnt(chip) - 1);
            lv_obj_set_style_text_color(l, lv_color_hex(0x120d06), 0);
        }
    } else {
        lv_obj_set_style_bg_color(chip, HY_C(0x2b1f10), 0);
        lv_obj_set_style_bg_grad_dir(chip, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_bg_grad_color(chip, HY_C(0x160f08), 0);
        lv_obj_set_style_bg_opa(chip, LV_OPA_70, 0);
        lv_obj_set_style_border_color(chip, HY_OURO_ESC, 0);
        lv_obj_set_style_shadow_width(chip, 0, 0);
        if (lv_obj_get_child_cnt(chip)) {
            lv_obj_t *l = lv_obj_get_child(chip, lv_obj_get_child_cnt(chip) - 1);
            lv_obj_set_style_text_color(l, HY_K, 0);
        }
    }
}

/* .tog: toggle pill */
lv_obj_t *hy_tog(lv_obj_t *pai, int x, int y, int w, bool on)
{
    int h = w * 22 / 40;
    lv_obj_t *o = lv_obj_create(pai);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_radius(o, h / 2, 0);
    lv_obj_set_style_border_color(o, on ? HY_OURO : HY_OURO_ESC, 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *b = lv_obj_create(o);
    lv_obj_remove_style_all(b);
    int d = h - 4;
    lv_obj_set_size(b, d, d);
    lv_obj_set_style_radius(b, d / 2, 0);
    lv_obj_align(b, on ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID,
                 on ? -2 : 2, 0);
    if (on) {
        lv_obj_set_style_bg_color(o, HY_OURO, 0);
        lv_obj_set_style_bg_color(b, lv_color_hex(0x120d06), 0);
    } else {
        lv_obj_set_style_bg_color(o, lv_color_hex(0x241d13), 0);
        lv_obj_set_style_bg_color(b, HY_MARFIM, 0);
    }
    return o;
}

/* .wire: fio de sinal (linha gradiente) — gradiente horizontal via 2 objs */
lv_obj_t *hy_wire(lv_obj_t *pai, int x, int y, int w, bool live)
{
    lv_obj_t *o = lv_obj_create(pai);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y - 1);
    lv_obj_set_size(o, w, 2);
    lv_obj_set_style_bg_color(o, live ? HY_OURO_ESC : HY_C(0x3a2d16), 0);
    return o;
}

/* no do fio (bola 6px da ponta) */
lv_obj_t *hy_wire_no(lv_obj_t *pai, int x, int y, bool live)
{
    lv_obj_t *o = lv_obj_create(pai);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y - 3);
    lv_obj_set_size(o, 6, 6);
    lv_obj_set_style_radius(o, 3, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_bg_color(o, live ? HY_OURO : lv_color_hex(0x2a2a2a), 0);
    return o;
}

/* .led com glow */
lv_obj_t *hy_led(lv_obj_t *pai, int x, int y, int r, lv_color_t cor, bool on)
{
    lv_obj_t *o = lv_obj_create(pai);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, r * 2, r * 2);
    lv_obj_set_style_radius(o, r, 0);
    if (on) {
        lv_obj_set_style_bg_color(o, cor, 0);
        lv_obj_set_style_shadow_color(o, cor, 0);
        lv_obj_set_style_shadow_width(o, 10, 0);
        lv_obj_set_style_shadow_opa(o, LV_OPA_80, 0);
    } else {
        lv_obj_set_style_bg_color(o, lv_color_hex(0x2c2212), 0);
    }
    return o;
}

/* .dsp/.autoBar: barra fina com fundo #0b0906 (o fill e um child do usuario) */
lv_obj_t *hy_barra(lv_obj_t *pai, int x, int y, int w, int h)
{
    lv_obj_t *o = lv_obj_create(pai);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_radius(o, h / 2, 0);
    lv_obj_set_style_bg_color(o, lv_color_hex(0x0b0906), 0);
    lv_obj_set_style_border_color(o, HY_OURO_ESC, 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    return o;
}
