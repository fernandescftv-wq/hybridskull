---
description: Diagnose an OldSkull panic or freeze
agent: oldskull-debug
---

Diagnose an OldSkull panic or freeze using the proven backtrace workflow.

1. Get the panic `Backtrace:` from the serial log.
2. Enable `CONFIG_ESP_SYSTEM_USE_FRAME_POINTER=y`.
3. Run `addr2line` against `build/esp_lvgl_oldskull.elf` for the addresses.
4. Correct the function the backtrace points to, not the suspected one.

Do not guess — each wrong guess has cost hours. Confirm before changing code.
