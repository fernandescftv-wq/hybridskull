---
name: OldSkull Display e Touch
description: OldSkull display ST7701 800x480, GT911 touch, and the software-rotation rules that must never change.
---

# OldSkull — Display & Touch

## Display ST7701S

- Interface: **MIPI-DSI 2-lane** (500 Mbps), RGB565, painel nativo **480×800**.
- Landscape **800×480 por software**: `sw_rotate=true` + `lv_disp_set_rotation(disp, LV_DISP_ROT_90)`,
  com `swap_xy=false` no `disp_cfg.rotation` (não duplicar rotação).
- **NUNCA** usar MADCTL/rotação por hardware — causa telas divididas/duplicadas.
- Backlight: LEDC PWM no GPIO 23 (10-bit, 20 kHz).
- Arquivos: `src/bsp_display.c`, `src/jc4880p433c_bsp.c`, `main/Kconfig.projbuild`.
- `esp_lvgl_port`: usar `on_color_trans_done` (não `on_frame_buf_complete`).

## Touch GT911

- I2C compartilhado: SDA = **GPIO 7**, SCL = **GPIO 8**, 400 kHz.
- Registrado via `lvgl_port_add_touch`; flags `swap_xy=0, mirror_x=0, mirror_y=0`.
- **Máximos pós-rotação: `x_max=800`, `y_max=480`.**
- Arquivos: `src/bsp_touch.c`.

## Fundos e vídeo

- `sd_fundos.c`: SDMMC 4-bit; lê JPG do SD com fallback para `/spiffs/fundo1.jpg`.
- **NUNCA usar `lv_obj_move_background`** para o fundo — o vídeo cobre e nada muda.
  Usar `video_hide()`/`video_show()`.
- Pinos 39-48 são alimentados pelo **LDO4/VDD_IO_5**, desligado de fábrica.
  Ligar antes de montar (`esp_ldo_channel_config_t { .chan_id = 4, .voltage_mv = 3300 }`),
  senão dá timeout `0x107` mesmo com os pinos certos.

## Diagnóstico de UI

- Colisão de posições Y entre widgets; widgets duplicados; remover item de `menus[]`
  exige renumerar os `#define MENU_*`.
- Painel sobre vídeo: usar `video_menu_pause`/`video_menu_resume`.
- Tela 1 congela ao tocar na faixa superior (`zona_oldskull` chama `video_toggle_pause`).
