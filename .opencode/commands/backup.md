---
description: Back up the OldSkull project
---

Back up the OldSkull project to a timestamped directory.

!`mkdir -p /home/e/esp && cp -r /home/e/esp/esp32_p4_jc4880p433c_bsp /home/e/esp/oldskull_backup_$(date +%Y%m%d_%H%M%S) && ls -d /home/e/esp/oldskull_backup_* | tail -1`

Never use `rm -rf`. Confirm the copy completed and report the new backup path.
