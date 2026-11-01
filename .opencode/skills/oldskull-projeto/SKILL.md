---
name: OldSkull Projeto
description: OldSkull ESP32-P4 guitar controller - architecture, current status, and pending items. Load before any OldSkull firmware work.
---

# OldSkull — Projeto (ESP32-P4)

## Projeto vivo

`/home/e/esp/esp32_p4_jc4880p433c_bsp` — CMake `project(esp_lvgl_oldskull)`, alvo `esp32p4`,
16 MB flash, partição `partitions_oldskull.csv` (app 4 MB + SPIFFS ~11,9 MB).
Git: branch `main`, em sincronia com `origin/main`.

## Dois hardwares (não confundir)

1. **ATUAL**: ESP32-P4 único na placa **JC4880P433C** (display ST7701 800×480, touch GT911).
2. **ORIGINAL (Doc Mestre V1)**: 3 módulos — CYD + TX (ESP32-S3) + RX (ESP32). Código Arduino `.ino`.

## Stack (verificado no código)

- ESP-IDF **v5.5.3** em `/home/e/esp/esp-idf` (builds); checkout v5.5.4 em `~/esp-idf-v5.5.4`
- LVGL **8.4.0** + `esp_lvgl_port` 2.8.0~1
- Display ST7701S via MIPI-DSI 2-lane, nativo 480×800, landscape 800×480 **por software**
  (`sw_rotate=true` + `LV_DISP_ROT_90`, `swap_xy=false`, nunca MADCTL)
- Touch GT911 (I2C, SDA=GPIO7/SCL=GPIO8)
- Co-processador ESP32-C6 soldado via ESP-Hosted/SDIO — o P4 **não tem rádio**
- SDMMC 4-bit (CLK=43, CMD=44, D0-D3=39-42); exige ligar **LDO4/VDD_IO_5 = 3,3V**

## Módulos (`main/`)

- `app_main.c` — boot: vídeo + carrossel de 3 amps + palco; C6 OTA; WiFi/HTTP/ESP-NOW
- `palco.c` — UI steampunk (tela 1 = palco, tela 2 = menus), 8 bancos em NVS, 8 slots de pedais
- `oldskull_pedal.c`, `oldskull_pedal_img.c` — pedal com zoom/rotação
- `video_loop.c` — vídeo em loop (frames JPEG do SPIFFS, JPEG HW)
- `sd_fundos.c` — fundos do SD/SPIFFS (JPEG HW)
- `banco_hw.c` — switches físicos GPIO 29-35,49 → bancos 1-8 (pull-up, ativo baixo, debounce 50ms)
- `wifi.c` / `web_server.c` — AP `old_skull` + HTTP porta 80 + UDP 9998/9999
- `esp_now_tx.c` / `c6_ota.c` — ponte P4→C6→ESP-NOW→RX; flash do firmware C6

## Estado atual

- Firmware da UI nova **gravado e validado em hardware** (boot limpo, bancos NVS OK, sem panic);
  rollback funcional provado (ler flash antes + `write_flash` de volta)
- ESP-NOW TX→RX funcionando com `cmd 101` (SAVE+aplica)
- Menus: PRESETS (8 bancos), PEDAIS, AMP, REDE, SKIN (fps + fundos), SISTEMA, MIDI (visual-only)

## Pendências

1. Otimizar o render LVGL do carrossel (full-refresh vs direct-mode)
2. Nome do preset não atualiza na tela 1 ao renomear (callback `atualizar_nome_tela1` não pega:
   `lbl_preset_nome` criado **depois** de `palco_registrar_nome` chamar o cb com NULL)
3. Copiar fundos para o SD de verdade (tirar cartão e copiar no PC, ou o ESP copiar da SPIFFS se não existir)
4. Empurrar o git para o remoto (se ainda houver algo pendente)

## Tom das respostas

PT-BR, direto, causa raiz + ação, curto. Faz somente o que foi mandado — não inventar.
