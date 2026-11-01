---
name: OldSkull Regras Mestre
description: OldSkull master rules - confirm before acting, surgical patches, hardware validation, backups. Highest priority for any OldSkull task.
---

# OldSkull — Regras Mestre

## REGRA DE OURO — Confirme antes de agir

Antes de **qualquer** modificação, skill, patch, build ou ação no projeto OldSkull,
confirme com o usuário. Nenhuma skill ou agente sobrepõe isso. Se outra skill
conflitar, esta prevalece e a skill conflitante deve ser corrigida.

## Regras

1. **Nunca mexer na tela 1** (vídeo + carrossel) sem ordem direta explícita.
2. **Correções pontuais**, nunca reescrever o arquivo inteiro por erro isolado
   (exceção só com autorização).
3. **Nunca substituir/excluir/sobrescrever** código existente sem confirmação.
4. **Código limpo:** sem código morto/comentários obsoletos/variáveis órfãs.
   Arquivos grandes (>~8K tokens) são lidos em partes. Vários `defined but not used`
   no linker = suspeitar de gravação truncada.
5. **Backup persistente ao finalizar** ("ficou perfeito"):
   `cp -r /home/e/esp/esp32_p4_jc4880p433c_bsp /home/e/esp/oldskull_backup_$(date +%Y%m%d_%H%M%S)`
   — **nunca `rm -rf`**.
6. **Reescrita total: jamais sem ordem direta** (inclui `app_main.c`, telas, `video_loop.c`).
7. **WiFi/ESP-NOW:** só mexer com ordem explícita (AP `old_skull` já funciona).
8. **Estilo de resposta:** PT-BR, direto, causa raiz + ação, curto.
9. **Investigar e validar ANTES de gravar:** `git diff` do trecho, confirmar que
   `build/esp_lvgl_oldskull.bin` é mais novo que os `.c`, gravar **uma** vez; se o
   sintoma persistir, verificar o estado real (serial/porta/binário), não re-gravar
   no mesmo rumo.

## Conflitos com skills antigas

Qualquer skill que contradiga estas regras deve ser atualizada ou marcada obsoleta —
estas são a fonte única de verdade.

## Diagnosticar antes de chutar

Projeto isolado em `~/Área de trabalho/tests/`, coletar log `TOQUE(x,y)` em tempo real,
gerar `zones.h`, integrar. O vault do Obsidian é a fonte de verdade.
