---
name: OldSkull LVGL
description: OldSkull LVGL (8.4) lessons: panic diagnosis with backtrace, rotation guards, rendering, and known crash patterns.
---

# OldSkull — LVGL (8.4)

## Versão

LVGL **8.4.0** (API `lv_img_*`). A migração para 9.3 foi **revertida** a pedido do usuário.
O mapa completo v8↔v9 fica em `esp32-p4-guition-lvgl` → `references/lvgl9_migration.md`.
O BSP (`bsp_display.c`) mantém guard `#if LVGL_VERSION_MAJOR >= 9` nos pontos de rotação.

## Fluxo de diagnóstico de panic/trava (usar ANTES de chutar)

Cada chute aqui custou horas. Ordem:

1. Logger de serial em background.
2. `CONFIG_ESP_SYSTEM_USE_FRAME_POINTER=y`.
3. `addr2line` no `Backtrace:` do panic.
4. Corrigir a função apontada pelo backtrace, **não** a suspeita.

## Causas de trava/corrupção já provadas por backtrace

- `lv_obj_del` do painel dentro do evento de um filho dele → usar `lv_obj_del_async` +
  `lv_anim_del(obj, NULL)`; zerar ponteiros de widgets que viviam dentro. Mesmo padrão
  para o teclado (READY/CANCEL).
- Animação de altura/largura com objeto `radius > 0` nascendo em 0 (LVGL 8: mask degenera
  em `lv_draw_sw_polygon` loop) → definir tamanho final direto, animar só a opacidade.
- `lv_img_set_zoom` em runtime + alfa (v8) → gerar o asset já no tamanho final, sem zoom.

## Render

- Pendência: otimizar full-refresh vs direct-mode para o carrossel ficar liso.
- `sw_rotate` saturado causou panic — cuidado com rotação e buffering no P4.
- Atenção ao bug TRIPLE_PARTIAL + rotação no P4 se migrar para `esp_lvgl_adapter`/LVGL 9.4+.

## Fontes

Fontes vitorianas (Cinzel, Playfair Display, manuscrita) embutidas como arrays C em
`main/fonte_*.c`. Não reescrever esses arquivos (são grandes e gerados).
