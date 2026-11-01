---
name: OldSkull RX e C6
description: OldSkull RX (ESP32) and C6 bridge - relay pins, MACs, CommandPacket, and the OTA flow.
---

# OldSkull — RX e Bridge C6

## RX (receptor)

- Sketch de referência: `/home/e/esp/esp_rx_receptor/rx_oldskull/rx_oldskull.ino` (Arduino).
- Protocolo/pinos/NVS idênticos à referência; o sketch **não** filtra MAC do remetente,
  então o `TX_MAC_CLONE` do C6 não importa para o RX novo.
- Relés ACTIVE LOW. Pinos: FX1-4 loops 16,17 / 18,19 / 21,22 / 23,25; BOOST 13,14; CH = 27.
- Bancos b1-b5 + `last` em NVS.
- RX original queimou em 12/out/2026; substituído por um devkit.

## MACs

- TX/C6: `30:ED:A0:E2:35:16` (clonado)
- RX novo: `EC:E3:34:14:8B:C4`
- MAC antigo: `80:F3:DA:63:51:90` (morto)
- Verificar: serial 115200 + reset (pulso DTR) → boot imprime `MAC RX: ...`.

## Bridge C6

- `/home/e/esp/esp_c6_espnow/cp/main/main.c` — recebe `peer_data` do P4 e converte em
  ESP-NOW unicast. Contém o `RX_MAC` hardcoded.
- `MSG_ID_OLDSKULL = 0x4F53`, `MSG_ID_ACK = 0x4B43`, `MSG_ID_RXRESP = 0x5258`.

## OTA do C6

- `c6_ota.c` grava `c6_fw.bin` no C6 via CP-OTA **uma única vez** (flag NVS `c6_fw_v15`) e reinicia.
- Versão atual gravada: **v15** (skill diz v15; checkpoint antigo cita v3 — vale confirmar).

## ACK / retry

- O P4 salva/restaura `fxmask` em NVS com retry até ACK do RX, restauração no boot com timer de 8 s.
- Se o RX não responde: verificar canal e filtro de MAC antes de mexer no protocolo.
