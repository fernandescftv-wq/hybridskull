---
name: OldSkull WiFi e ESP-NOW
description: OldSkull WiFi AP, web server, UDP, and the ESP-NOW TX-to-RX protocol (P4 to C6 to RX). Do not change without explicit order.
---

# OldSkull — WiFi / ESP-NOW / Protocolo TX→RX

## Arquitetura (o P4 não tem rádio)

O ESP32-P4 usa o **ESP32-C6 soldado** via ESP-Hosted/SDIO (canal `peer_data`).
O C6 reenvia por ESP-NOW ao RX. O P4 **não** fala ESP-NOW diretamente.

## WiFi / Web

- AP `old_skull`, canal 1, aberto, `max_connection=4`, DHCP `192.168.4.1`.
- Servidor UDP nas portas **9999** (recebe respostas do RX) e **9998** (envia comandos).
- HTTP server na porta 80 servindo `WEB_INDEX_HTML` em `/`, `/index.html`, `/palco` e assets em `/assets/*`.
- Arquivos: `wifi.c/.h`, `web_server.c/.h`. Pode ser ligado/desligado pelo menu REDE.

## CommandPacket (não alterar)

```c
typedef struct __attribute__((packed)) {
    uint8_t cmd;      // 1..5 = aplica banco, 101 = SAVE+aplica, 200 = STATUS_REQ, 201 = STATUS_RESP
    uint8_t bank;     // 1..5
    uint8_t fxMask;   // bits 0..4 = FX1..FX4+BOOST
    uint8_t ch;       // 1=CLEAN, 2=CRUNCH (AMP3=CH2)
    uint32_t seq;     // sequência
} CommandPacket;     // 7 bytes
```

- Enviar com `esp_now_tx_aplicar(banco+1, fxMask, ch)`. O que faz o RX agir é o **cmd 101**.
- `fxMask` ainda é `0x1F` fixo no caminho de toque — reverter para por-pedal quando validado.

## MACs (atualizado após queima do RX)

- TX/C6 (interface gráfica): `30:ED:A0:E2:35:16` (C6 clona via `esp_base_mac_addr_set`)
- RX **novo** (devkit, reposição do queimado em 12/out): `EC:E3:34:14:8B:C4`
  — igual ao `RX_MAC` hardcoded no bridge C6 (`esp_c6_espnow/cp/main/main.c`)
- MAC antigo `80:F3:DA:63:51:90` morreu com a placa.
- Confirmar RX novo: abrir o serial 115200 e resetar (pulso DTR); o boot imprime `MAC RX: XX:XX...`.
  Bateu com o bridge → não mexer em nada.

## C6

- `c6_ota.c` grava `c6_fw.bin` no C6 via CP-OTA **uma única vez** (flag NVS `c6_fw_v15`) e reinicia.
- Bridge ESP-NOW: `esp_c6_espnow/cp/main/main.c`.

## Relés (RX)

- ACTIVE LOW. Pinos GPIO 16-19 e 21-25 (conforme `RX-Protocol.md` / `Reles.md` do vault).
