---
description: Build the OldSkull ESP32-P4 firmware
agent: oldskull-build
---

Build the OldSkull firmware using the validated ESP-IDF environment.

Run the project build script and report the result. If it fails, capture the exact
error, point at the root cause, and propose a surgical fix — do not rewrite files.

!`cd /home/e/esp/esp32_p4_jc4880p433c_bsp && ./run.sh 2>&1 | tail -40`

After the build, confirm `build/esp_lvgl_oldskull.bin` is newer than the last edited
`.c` file.
