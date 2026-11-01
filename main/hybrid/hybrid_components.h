/* hybrid_components.h - primitivas visuais do HybridSkull no LVGL 8
 * Estilos extraídos 1:1 do CSS do index.html (.plate, .tbtn, .chip, .orn...).
 */
#ifndef HY_COMPONENTS_H
#define HY_COMPONENTS_H

#include "lvgl.h"
#include "hy_theme.h"

/* texto rápido: label com fonte/cor/ancoragem */
lv_obj_t *hy_txt(lv_obj_t *pai, const char *txt, const lv_font_t *f, lv_color_t cor,
                 lv_align_t al, int dx, int dy);

/* .plate: placaa de latão com rebites + moldura gravada dupla */
lv_obj_t *hy_placa(lv_obj_t *pai, int x, int y, int w, int h);

/* .orn : ornamento ◆ ───── texto ───── ◆ */
lv_obj_t *hy_ornamento(lv_obj_t *pai, int y, const char *txt, int w);

/* .tbtn : botão de latão da top bar */
lv_obj_t *hy_tbtn(lv_obj_t *pai, const char *txt, int x, int y, int w, int h,
                  lv_event_cb_t cb, void *user);

/* .chip / .banco / .ivchip : chip retangular com borda ouro-esc */
lv_obj_t *hy_chip(lv_obj_t *pai, const char *txt, int x, int y, int w, int h,
                  const lv_font_t *f);

/* acesa/desligada num chip (.on) */
void hy_chip_on(lv_obj_t *chip, bool on, lv_color_t cor);

/* .tog : toggle 40x22 (26x14 no ESP) */
lv_obj_t *hy_tog(lv_obj_t *pai, int x, int y, int w, bool on);

/* linha com "wire" do fio de sinal (44px, gradiente bronze→acento) */
lv_obj_t *hy_wire(lv_obj_t *pai, int x, int y, int w, bool live);

/* no do fio (::after): bola 6px na ponta */
lv_obj_t *hy_wire_no(lv_obj_t *pai, int x, int y, bool live);

/* LED redondo com glow (.led): r = raio em px */
lv_obj_t *hy_led(lv_obj_t *pai, int x, int y, int r, lv_color_t cor, bool on);

/* barra fina (dsp bar / autoBar): fundo #0b0906 + fill bronze→ambar */
lv_obj_t *hy_barra(lv_obj_t *pai, int x, int y, int w, int h);

/* fundo degradê genérico */
void hy_grad(lv_obj_t *o, lv_color_t a, lv_color_t b);

/* remove scroll bars e clip */
void hy_limpar_scroll(lv_obj_t *o);

#endif
