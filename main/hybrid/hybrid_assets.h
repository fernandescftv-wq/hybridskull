/* hybrid_assets.h - decodificacao de assets (JPEG hardware P4 + raw RGB565A8).
 * Pools LRU independentes; buffers em PSRAM. Dimensoes definidas pelo
 * gen_assets.py e fixas por tipo de asset (1:1 na exibicao; nada escala).
 */
#pragma once
#include "lvgl.h"

/* fundo 800x480 / arte 616x264 / head 592x248 (1 por amp vigente) */
lv_img_dsc_t *hy_assets_bg  (const char *slug);
lv_img_dsc_t *hy_assets_art (const char *slug);
lv_img_dsc_t *hy_assets_head(const char *slug);

/* thumb 80x32 (tira do picker), slide m_ 320x88 (preview) */
lv_img_dsc_t *hy_assets_thumb(const char *slug);
lv_img_dsc_t *hy_assets_slide(const char *slug);

/* pedais: chain s_ 100x150 (raw alpha), zoom z_ 240x360, gear r_ 80x120,
 * p_ 80x120 (jpeg, legado) */
lv_img_dsc_t *hy_assets_pedal      (const char *id);
lv_img_dsc_t *hy_assets_pedal_zoom (const char *id);
lv_img_dsc_t *hy_assets_pedal_chain(const char *id);
lv_img_dsc_t *hy_assets_pedal_gear (const char *id);

/* libera amp vigente (troca) e tudo (teardown) */
void hy_assets_limpar_amp(void);
void hy_assets_teardown  (void);
