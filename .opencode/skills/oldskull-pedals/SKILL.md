---
name: OldSkull Pedais e Bancos
description: OldSkull pedal slots, effects, banks, presets and the NVS persistence model.
---

# OldSkull — Pedais, Bancos e Presets

## Pedais

- **8 slots** de pedal na base da tela. 3 modelos com imagem real: OVERDRIVE, CHORUS, DELAY
  (`pedal_od_img.c`, `pedal_ch_img.c`, `pedal_dl_img.c`).
- Arrastar entre slots (threshold 18 px), handler único de drag.
- Toque curto alterna ON/OFF. Pedal `i` toggle independente (XOR mask, `bit = slot % 5` → FX1..FX4+BOOST).
- REVERB/BOOST foram **removidos** (imagens bugadas) — não reintroduzir sem asset válido.

## Bancos de preset (3 níveis, NVS)

- Namespace NVS `oldskull`. Chaves: `b<N>nome`, `b<N>ped`, `b<N>amp`, `b<N>modo`, `ativo`, `fxmask`.
- Estado: `banco_nome[3][24]`, `banco_pedais[3][2]`, `banco_amp[3]`, `banco_ativo`.
- Menu PRESETS tem chips BANCO 1/2/3 → painel do banco (USAR aplica tudo; NOMEAR abre o teclado
  roteado por `teclado_para_banco`; toggles de pedais; AMP cicla LONESTAR/SOLDANO/JP-2C;
  SALVAR POSIÇÃO captura pedais+amp da tela).
- Modo STOMP/CENA por banco (`b<N>modo`, toggle no painel). STOMP reduz backlight.

## Carrossel de amps

- 3 amps TRUE_COLOR_ALPHA (RGB565+alpha): `amp_lonestar.c` (606×330), `amp_soldano.c` / `amp_jp2c.c` (330×330).
- Um central (cy=275, 100% opaco) + dois laterais (cx=140/660, cy=175, 50% opaco).
- Animação de deslize 350ms ease-out. Troca por menu AMP ou swipe vertical.
- **NUNCA** aplicar o amp do banco com laço `while (app_amp_atual() != alvo) app_trocar_amp();` —
  cada `app_trocar_amp()` rearranja o carrossel inteiro e dá panic no boot. Usar `app_amp_ir_para(i)`.

## Estado dos relés (fxmask)

- 5 bits: FX1..FX4 + BOOST. Persistido em NVS pelo P4, com retry até ACK do RX.
- Restauração no boot com timer de 8 s (`ev_restaura_rx`).
- AMP trocado move o relé do canal (CH1/CH2 conforme Documento Mestre).
