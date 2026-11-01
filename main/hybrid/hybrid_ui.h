/* hybrid_ui.h - casco HybridSkull (fundo/top bar/nav/hw) */
#ifndef HY_UI_H
#define HY_UI_H

#include "hy_types.h"

/* monta a tela HybridSkull e carrega (sem apagar a tela do palco: referência).
 * Congela o video_loop (toggle da API existente, sem editar arquivos velhos). */
void hybridskull_boot(void);

/* aplicação de estado global às views */
void hy_apply_bg(void);
void hy_apply_accent(void);
void hy_apply_chan(void);
void hy_set_view(int v);
void hy_set_amp(int idx);
void hy_set_chan(int n);
void hy_set_slot(int bank, int slot);      /* aplica preset (visual) */
void hy_press_style(lv_obj_t *o);          /* flash ambar no toque (feedback) */
void hy_cenario_toggle(void);              /* modo palco limpo (CENARIO) */
void hy_view_fechar(void);                 /* X: volta pro view anterior */

/* views (uma função de montagem por aba + refresh de valores) */
void hy_view_signal_montar(lv_obj_t *pai);
void hy_view_signal_novo_amp(void);        /* amp mudou: head + cadeia        */
void hy_view_amp_montar(lv_obj_t *pai);
void hy_view_amp_refresh(void);
void hy_view_presets_montar(lv_obj_t *pai);
void hy_view_presets_refresh(void);
void hy_view_gear_montar(lv_obj_t *pai);
void hy_view_gear_refresh(void);
void hy_view_auto_montar(lv_obj_t *pai);
void hy_view_auto_refresh(void);
void hy_view_config_montar(lv_obj_t *pai);
void hy_view_config_refresh(void);

/* refs do casco */
extern lv_obj_t   *hy_tela, *hy_bg_img, *hy_top_box, *hy_nav_box, *hy_hw_box;
extern lv_obj_t   *hy_view_box[HY_V_N];
extern lv_obj_t   *hy_tab_btn[HY_V_N];
extern lv_obj_t   *hy_lbl_preset;          /* nameplate nm */
extern lv_obj_t   *hy_lbl_amp_sig;         /* nome atual no SIGNAL? (cap do amp) */
extern lv_color_t  hy_acc;

/* refs ds views (signal: cache dos objetos do chain p/ novo amp) */
extern lv_obj_t *hy_sig_head, *hy_sig_chain;
extern lv_obj_t *hy_amp_name, *hy_amp_all, *hy_amp_chunks[4];
extern lv_obj_t *hy_amp_picker_box;
extern lv_obj_t *hy_presets_row;
extern lv_obj_t *hy_bank_chip[8], *hy_prow[8][8];

/* zoom (#zoom da web): pedal/amp em destaque; fechar = toque fora ou X */
void hy_zoom_mostrar(lv_img_dsc_t *d, const char *nome, const char *sub,
                     uint16_t zoom_q);
void hy_zoom_pedal(int slot);            /* zoom do pedal do slot do preset */

#endif
