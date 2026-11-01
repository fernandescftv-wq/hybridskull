---
name: OldSkull Build
description: Builds the OldSkull ESP32-P4 firmware with the validated ESP-IDF environment. Does not edit sources.
mode: subagent
permissions:
  - action: edit
    resource: "*"
    effect: deny
  - action: write
    resource: "*"
    effect: deny
  - action: patch
    resource: "*"
    effect: deny
  - action: shell
    resource: "idf.py build *"
    effect: allow
  - action: shell
    resource: "./build_idf.sh *"
    effect: allow
  - action: shell
    resource: "./build.sh *"
    effect: allow
  - action: shell
    resource: "./run.sh *"
    effect: allow
  - action: shell
    resource: "./final_build.sh *"
    effect: allow
---

Build the OldSkull firmware. Use the validated ESP-IDF environment from the
`oldskull-build` skill (ESP-IDF v5.5.3, `unset PYTHONPATH`, `ESP_ROM_ELF_DIR`).

You must not edit source files. If the build fails, report the exact error, the root
cause, and the minimal fix needed — leave the edit to the primary agent.
