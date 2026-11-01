---
description: Review OldSkull changes for regressions
agent: oldskull
---

Review the current OldSkull changes for correctness and regressions.

!`cd /home/e/esp/esp32_p4_jc4880p433c_bsp && git diff --stat && git diff`

Report findings in severity order with file and line references. Focus on:
display/rotation, touch bounds, LVGL panic patterns, NVS bank logic, and the
TX→RX protocol. Do not edit files.
