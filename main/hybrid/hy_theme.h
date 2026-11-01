/* hy_theme.h - paleta/cores exatas do :root do index.html (compartilhado) */
#ifndef HY_THEME_H
#define HY_THEME_H

#include "lvgl.h"

/* fontes Geradas (Cinzel 700/900 + Digital7) e o diamante ◆ */
extern const lv_font_t hy_fonte_c10;   /* micro-capas, labels 10px          */
extern const lv_font_t hy_fonte_c12;   /* abas, chips, botões               */
extern const lv_font_t hy_fonte_c16;   /* nameplate, presets, srow          */
extern const lv_font_t hy_fonte_c20;   /* ampName, zoom                     */
extern const lv_font_t hy_fonte_d31;   /* Digital7 31px (AUTO .big)         */
extern const lv_img_dsc_t hy_gem;      /* diamante ◆ 10x10 (hy_gem.h)       */

/* ── paleta (:root do index.html, valores EXATOS) ───────────────────────────── */
#define HY_C(hex) lv_color_hex(hex)
#define HY_OURO          HY_C(0xF3C969)
#define HY_OURO_CLARO    HY_C(0xF7DD9A)
#define HY_OURO_ESC      HY_C(0xC9922B)
#define HY_BRONZE        HY_C(0x8A6420)
#define HY_COBRE         HY_C(0xB87333)
#define HY_AMBAR         HY_C(0xFFA030)
#define HY_MARFIM        HY_C(0xEDE0C8)
#define HY_K             HY_C(0xE6D3A8)
#define HY_HINT          HY_C(0xB8A898)
#define HY_VERDE         HY_C(0x4CD964)
#define HY_VERMELHO      HY_C(0xE04A3F)
#define HY_LILA          HY_C(0x7A3FA8)
#define HY_PAINEL        HY_C(0x0E0B08)
#define HY_BG0           HY_C(0x0b0a08)
#define HY_TXT           HY_OURO            /* cfg.cor troca isto */

/* cores de canal (palco.c CH1-4) */
#define HY_CH1  HY_C(0x1F7A34)
#define HY_CH2  HY_C(0xB8901A)
#define HY_CH3  HY_C(0xA82418)
#define HY_CH4  HY_C(0x7A3FA8)

/* fundos/latão recorrentes do CSS */
#define HY_PLATE_TOP_A   HY_C(0x241C10)
#define HY_PLATE_TOP_B   HY_C(0x141008)
#define HY_NAV_A         HY_C(0x1A140C)
#define HY_NAV_B         HY_C(0x0E0B07)
#define HY_BTN_A         HY_C(0x312512)
#define HY_BTN_B         HY_C(0x1C150C)
#define HY_SLOT_BG       HY_C(0x151009)
#define HY_PNAME_BG      HY_C(0x100C07)
#define HY_BASE_A        HY_C(0x241B0F)
#define HY_BASE_B        HY_C(0x120D08)
#define HY_PICK_A        HY_C(0x231A0E)
#define HY_PICK_B        HY_C(0x120D08)
#define HY_HW_A          HY_C(0x231A0E)
#define HY_HW_B          HY_C(0x0D0A06)
#define HY_GCATA_A       HY_C(0x1B150C)
#define HY_GCATA_B       HY_C(0x0D0A07)

#endif /* HY_THEME_H */
