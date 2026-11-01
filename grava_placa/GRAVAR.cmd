@echo off
REM Grava o HybridSkull completo numa placa ESP32-P4 nova (16 MB flash).
REM Uso: GRAVAR.cmd COM5   (ou COM6, COM7... - veja no Gerenciador de Dispositivos)
setlocal
if "%~1"=="" (
  echo ERRO: passe a porta, ex: GRAVAR.cmd COM5
  exit /b 1
)
python -m esptool --chip esp32p4 -b 460800 --before default_reset ^
  --after hard_reset -p %~1 ^
  write_flash 0x0 HYBRIDSKULL_COMPLETO_flash_em_0x0.bin
echo OK - reinicie a placa
