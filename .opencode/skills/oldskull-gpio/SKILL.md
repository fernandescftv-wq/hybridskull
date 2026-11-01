---
name: OldSkull GPIO e Pinos
description: OldSkull JC4880P433C pinout - SD card, free GPIOs, LDO4, and footswitch mapping.
---

# OldSkull — GPIO / Pinos (JC4880P433C)

## SD card (CRÍTICO — confirmado pelo fabricante)

- Slot 0 SDMMC **4-bit nativo**, pinos fixos:
  - CLK = GPIO 43, CMD = GPIO 44, D0 = GPIO 39, D1 = GPIO 40, D2 = GPIO 41, D3 = GPIO 42
- **Não é 1-bit.** Usar `host.flags = 0` (default 4-bit) + `slot.width = 4`. Pull-up em CMD/D0-D3 ajuda.
- `format_if_mount_failed = true` formata o cartão se não tiver FAT32 (apaga conteúdo — só para teste).
  ESP-IDF FATFS **não** monta exFAT.
- **NUNCA** rodar scanner de pinos que chame `esp_vfs_fat_sdmmc_unmount()` sem argumento —
  crasha com Guru Meditation (Load access fault).

## LDO4 / VDD_IO_5 (causa raiz do timeout 0x107)

Os pinos 39-48 são alimentados pelo **LDO4 / VDD_IO_5**, que vem **desligado de fábrica**.
Sem ligar, o slot SD não recebe 3,3V e dá `0x107 timeout` mesmo com os pinos certos. Ligar antes de montar:

```c
#include "esp_ldo_regulator.h"
esp_ldo_channel_config_t ldo = { .chan_id = 4, .voltage_mv = 3300 };
esp_ldo_acquire_channel(&ldo, &g_ldo_chan);
```

Log esperado: `LDO4 ligado: VDD_IO_5 = 3.3V (pinos 39-48)` → depois `SD montado: SD04G`.

## GPIOs livres (header 2x13 traseiro)

- **Livres**: 4, 6, 9, 10, 11, 12, 13 (footswitch c/ pull-up interno, fechar no GND — 1 pino = 1 banco),
  36/37/38 (SPI extra).
- **Não livres**: 5 (LCD RST), 23 (LCD BACKLIGHT).
- 39-44 (SD) ficam livres enquanto `BSP_CAPS_SDCARD=0` — se reativar o SD, conflita.
- Ocupados sempre: 7/8 (touch I2C), 14-19 (SDIO do C6), 54 (reset C6), 2/3 (UART debug, evitar).
- Footswitches de banco: GPIO **29, 30, 31, 32, 33, 34, 35, 49** → BANCOS 1-8.
