---
name: OldSkull Build e Flash
description: Build and flash the OldSkull ESP32-P4 firmware with the correct ESP-IDF environment, including the fragile env requirements.
---

# OldSkull — Build & Flash

## Ambiente (crítico)

- **ESP-IDF**: `/home/e/esp/esp-idf` = **v5.5.3**. Não regenerar o `sdkconfig`.
- **Python env**: `/home/e/.espressif/python_env/idf5.5_py3.11_env`
- **Toolchain RISC-V**: `/home/e/.espressif/tools/riscv32-esp-elf/esp-14.2.0_20251107/riscv32-esp-elf/bin`
- **ROM ELF**: `ESP_ROM_ELF_DIR=/home/e/.espressif/tools/esp-rom-elfs/20241011/`
- **`unset PYTHONPATH`** é obrigatório (ou `env -i`).

## Variante validada (todos os builds passaram)

```sh
export IDF_PATH=/home/e/esp/esp-idf
export ESP_IDF_VERSION=5.5
export ESP_ROM_ELF_DIR=/home/e/.espressif/tools/esp-rom-elfs/20241011/
export PATH="/home/e/.espressif/tools/riscv32-esp-elf/esp-14.2.0_20251107/riscv32-esp-elf/bin:/home/e/.espressif/python_env/idf5.5_py3.11_env/bin:$PATH"
unset PYTHONPATH
cd /home/e/esp/esp32_p4_jc4880p433c_bsp
python3 /home/e/esp/esp-idf/tools/idf.py build
```

Scripts prontos no projeto: `./run.sh`, `./build_idf.sh`, `./build.sh`, `./final_build.sh`,
`./idf.sh` (build + flash).

## Verificação pós-build

- `build/esp_lvgl_oldskull.bin` deve ser **mais novo** que os `.c` alterados.
- `build/spiffs_image.bin` é gerado separadamente.
- `build/flash_args` traz: `bootloader.bin` (0x2000), `esp_lvgl_oldskull.bin` (0x10000),
  `partition-table.bin` (0x8000). **Não inclui** `spiffs_image.bin` — gravar em passo separado.

## Flash

```sh
idf.py -p /dev/ttyACM0 flash
idf.py -p /dev/ttyACM0 monitor
```

SPIFFS separado (ex.: `esp_spiffs_gen`/`spiffsgen`); conferir ordem dos argumentos.

## Regras de porta USB

- Porta típica: `/dev/ttyACM0`. Verificar com `ls /dev/ttyACM* /dev/ttyUSB*`.
- Monitor 115200; pulso DTR no reset faz o boot imprimir `MAC RX: ...`.

## Rollback

Preservar o estado **antes** de voltar: commit/tag no git, `rm -rf build`, rebuild+flash.
Rollback funcional já provado: ler o flash atual e `write_flash` de volta.
