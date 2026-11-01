---
description: Flash the OldSkull firmware to the ESP32-P4
agent: oldskull-flash
---

Flash the OldSkull firmware to the connected ESP32-P4.

Use port `/dev/ttyACM0` unless the user gives another. Before flashing, confirm with the
user and check that a binary is newer than the sources.

!`ls -l /dev/ttyACM* /dev/ttyUSB* 2>/dev/null`

!`cd /home/e/esp/esp32_p4_jc4880p433c_bsp && stat -c '%y %n' build/esp_lvgl_oldskull.bin main/palco.c main/app_main.c 2>/dev/null`
