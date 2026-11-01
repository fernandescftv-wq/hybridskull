#!/bin/bash
# Grava o HybridSkull completo numa placa ESP32-P4 nova (16 MB flash).
# Uso: ./GRAVAR.sh            (auto-detecta /dev/ttyACM*)
set -e
PORT="${1:-$(ls /dev/ttyACM* 2>/dev/null | head -1)}"
[ -z "$PORT" ] && { echo "ERRO: nenhuma porta /dev/ttyACM* encontrada"; exit 1; }
echo "Gravando em $PORT ..."
python -m esptool --chip esp32p4 -b 460800 --before default_reset \
  --after hard_reset -p "$PORT" \
  write_flash 0x0 HYBRIDSKULL_COMPLETO_flash_em_0x0.bin
echo "OK - boot o ESP32-P4 (reconecte/ressete se preciso)"
