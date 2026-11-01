# Porta HybridSkull v2 → ESP32-P4 (JC4880P433C, 800×480)

> Plano de execução do porte da interface web (`~/HybridSkull_UI`, 1560×1040) para o
> firmware LVGL do OldSkull. Leitura obrigatória antes de qualquer patch em `palco.c`.

> **STATUS (26/10/2026) — F1 codificada e compilando:**
> - Módulos novos em `main/hybrid/` completos (casco + 6 views + componentes + assets + dados + fontes + hy_gem).
> - Integração: `CMakeLists.txt` (SRCS híbridos), `Kconfig.projbuild` (menu **HybridSkull UI**, `CONFIG_HYBRID_UI=y`) e 3 linhas protegidas em `app_main.c` após `lv_scr_load(tela)`.
> - **Binário: `build/esp_lvgl_oldskull.bin` 4.079.392 B (0x3E3F20), 114 KB livres (3%)**; `build/spiffs_image.bin` 12.517.376 B (10,3 MB de assets em 11,9 MB).
> - **Pendente: gravar/validar no hardware** (porta serial ausente no momento) — flash do app + flash separado do SPIFFS (`build/spiffs_image.bin` @ 0x410000, `idf.py flash` NÃO grava SPIFFS).
> - Fases seguintes (F2+): ESP-NOW/preset real, AUTO com timer, SALVAR/CENÁRIO/modais.

## 1. Base de hardware — NÃO MEXER

Tudo abaixo já está implementado e validado em hardware. O porte é **só UI**; estas
camadas ficam intocadas (`AGENTS.md` Regra 5).

| Item | Valor | Arquivo |
|---|---|---|
| Display | ST7701S MIPI-DSI, nativo 480×800, **landscape 800×480 por software** (`sw_rotate` + `LV_DISP_ROT_90`) | `src/bsp_display.c`, `main/app_main.c` |
| Touch | GT911 — I2C **SDA=GPIO7, SCL=GPIO8** | `src/bsp_touch.c`, `src/bsp_i2c.c` |
| Bootstrap C6 | SDIO **CMD=19, CLK=18, D0=14, D1=15, D2=16, D3=17, RESET=54** | `sdkconfig` (ESP-Hosted) |
| WiFi | AP **`old_skull`**, canal **1**, sem senha; HTTP :80; UDP **9999** (TX) / **9998** (RX) | `main/wifi.c`, `main/web_server.c` |
| ESP-NOW | P4 → `peer_data` (msg `0x4F53`, ack `0x4B43`) → C6 → RX; MAC TX clonado `30:ED:A0:E2:35:16` | `main/esp_now_tx.c` |
| Pacote RX | `CommandPacket` 7 B packed `{cmd, bank, fxMask, ch, seq}`; 1-5 aplica banco, 101 SAVE+aplica, 200/201 status | `main/esp_now_tx.h` |
| Botões de banco | **GPIO 29,30,31,32,33,34,35,49 → BANCO 1..8** (pull-up, ativo baixo, debounce 50 ms) | `main/banco_hw.c` |
| SD | SDMMC 4-bit **CLK=43, CMD=44, D0-D3=39-42** + **LDO4/VDD_IO_5 = 3,3 V** antes de montar | `main/sd_fundos.c` |
| Flash | 16 MB: app `factory` **4 MB** (offset 0x10000) + SPIFFS **11,9 MB** (0x410000) | `partitions_oldskull.csv` |
| Base atual | binário **3.996.272 B** → **5% livres (198 KB)**; SPIFFS em uso **5,9 MB** | `build/esp_lvgl_oldskull.bin` |

## 2. O que muda na transição web → tela

- Web: 1560×1040, fonte 11-28 px, 6 abas horizontais.
- ESP: **800×480**. Escala 0,51× → texto de 11 px viraria 5,6 px (ilegível).
  **Regra de condensação:** manter a *linguagem visual* (Cinzel, ouro `#F3C969`,
  fundo `#0E0B08`, chapas com borda 1 px e sombra interna), **não** a proporção.
  - Escala tipográfica mínima: **10 px** (rótulos), 14 px (títulos de seção), 22;28 px (nome de preset/amp).
  - Nada de texto abaixo de 10 px; cortar campos secundários em vez de encolher.

## 3. Geometria da tela (800×480)

```
y   0.. 44  BARRA SUPERIOR   44px  logo+preset(canal)+wifi+DSP+SALVAR
y  44.. 78  NAV              34px  6 abas (133px cada)
y  78..404  CONTEÚDO        326px  view ativa
y 404..480  RODAPÉ           76px  8 footswitches (banco / bypass)
```

| View | Conteúdo em 800×480 |
|---|---|
| **SIGNAL** | amp 520×221 em (16,84) + cadeia de 8 pedais 88×86 em y=312 (pitch 98) + chips dos 8 bancos |
| **AMP** | arte 760×323 em (20,80) + nome sobreposto + `CH 1..4` (44×22, canto inf. dir.) + picker horizontal (thumb 190×81 → 72×31, 10 visíveis, scroll) |
| **PRESETS** | 8 chips de banco 92×28 + 8 linhas 720×30 (nome + amp + canal) |
| **GEAR** | rail de categorias 96 px + grade 5×3 de pedais (72×108 por célula) |
| **AUTO** | caixa central 520×250: nome (Digital7 48), intervalo (4 botões), barra, TOCAR/PARAR |
| **CONFIG** | 3 sub-abas: TEMA (cor da letra, fonte, brilho) · REDE (SSID/IP/scan) · SISTEMA (versão, fps, reiniciar) |

Rodapé: 8 teclas de 80×76 (BANCO 1..8), acesas conforme o banco atual; em `SIGNAL`
funcionam também como bypass/pedal, como na web (`#hw`).

### 3.1 Limite de legibilidade do painel do amp

A arte tem 2048×870; em 760×323 (escala 0,37×) os rótulos dos knobs (15 px na web)
caem para **~5,5 px** — viram textura, não texto. Para ler um rótulo de 15 px seria
preciso escala ≥ 0,67×, ou seja mostrar só **58% da largura** do painel por vez.

| Opção | Como fica | Custo |
|---|---|---|
| **A — arte inteira** (recomendado p/ F1) | amp reconhecível como "foto"; nome e canal em texto LVGL por cima | zero |
| B — arte inteira + zoom ao tocar | toque abre o painel de controles em 2× (arrasta na horizontal) | médio |
| C — painel rolável | já abre ampliado, 3-4 knobs por tela com rótulo legível | alto (perde a "cara" de amp inteiro) |

Mockups 1:1 em `/tmp/opencode/mock_amp.png` e `/tmp/opencode/mock_signal.png`
(gerados por `/tmp/opencode/mock_esp.py`, usam as artes já convertidas).

## 4. Assets — pipeline

Fonte: `~/HybridSkull_UI/assets/`. Destino: `spiffs/` (o app lê JPEG por `esp_jpeg`
em hardware, igual `sd_fundos.c`).

| Asset | Origem | Destino | Dim. | Formato | Qtd | Total |
|---|---|---|---|---|---|---|
| Arte dos amps | `amps/<slug>.png` 2048×870 | `spiffs/amps/<slug>.jpg` | 760×323 | JPEG q82 | 31 | **1.708 KB** (55 KB médio) |
| Thumbs do picker | idem | `spiffs/amps/t_<slug>.jpg` | 190×81 | JPEG q78 | 31 | **136 KB** (4 KB médio) |
| Pedais | `panels/DSP.*.Pedal.BG.png` 117×177 | `spiffs/pedais/<id>.jpg` | 117×177 | JPEG q88 (fundo `#0E0B08`) | 52 | **700 KB** (13 KB médio) |
| Fundos | `fundos/*.jpg` (8) | `spiffs/images/` | 800×480 | já existe | 8 | 596 KB |
| Fontes | `fonts/Cinzel.ttf`, `digital-7.ttf` | `main/fonte_*.c` | 10/12/14/16/22/28 e 48 | LVGL bpp4 | 7 | ~250 KB |

Gerador: `tools/gen_assets.py` (roda em ~6 s, idempotente, `--force` regenera).
Total novo no SPIFFS: **2,5 MB** → 5,9 + 2,5 = **8,4 MB de 11,9 MB** (3,5 MB livres).
O logo "OLD◆SKULL" é desenhado em LVGL (Cinzel) — não precisa de imagem.

**Buffers:** arte 760×323 RGB565 = **491 KB** em PSRAM (decodifica 1 por vez);
thumbs 190×81 = 31 KB (cache de 8). Nada de arte em C array.

## 5. Código

```
main/hybrid/
  hybrid_theme.h    cores, fontes, espaçamentos (espelha o :root do index.html)
  hybrid_ui.c/.h    shell: barra, nav, roteamento entre views, rodapé (8 teclas)
  hybrid_asset.c/.h JPEG do SPIFFS → lv_img_dsc_t (PSRAM) + cache de thumbs
  view_signal.c     palco (amp + cadeia + bancos)
  view_amp.c        arte + CH + picker
  view_presets.c    8 bancos × 8 slots
  view_gear.c       grade de pedais
  view_auto.c       troca automática
  view_config.c     tema / rede / sistema
```

Integração: `app_main.c` passa a chamar `hybrid_ui_montar(tela)` **no lugar de**
`palco_montar(tela)`. `video_loop`, `wifi`, `web_server`, `esp_now_tx`, `banco_hw`,
`c6_ota` e `sd_fundos` continuam como estão.

**Sai do build (libera ~1,3 MB):** `amp_lonestar.c`, `amp_soldano.c`, `amp_jp2c.c`
(606×330/330×330 `TRUE_COLOR_ALPHA`, 1,25 MB no binário). Saem também do
`CMakeLists.txt` os `.c` órfãos que hoje só engordam o build: `tela3_img.c`,
`tela_palco_img.c`, `tela_palco3_img.c`, `oldskull_pedal_img.c`, `pedal_*_img.c`.
`palco.c` permanece no repositório (branch) durante a transição e sai no fim.

**Camada de dados:** os 8 bancos/presets e o `CommandPacket` já existem em
`palco.c` (NVS). O porte **reusa** `banco_selecionar()`, `palco_amp_mudou()` e
`esp_now_tx_aplicar()` — a UI nova não reimplementa nada de hardware.

## 6. Fases

| Fase | Entrega | Aceite |
|---|---|---|
| **F0** | este plano + `tools/gen_assets.py` + assets gerados | ✅ feito: `spiffs/amps` 31 jpg + 31 thumbs, `spiffs/pedais` 52 jpg (2,5 MB), build atual intacto (3.996.272 B) |
| **F1** | shell (barra + nav + rodapé) + **view AMP** | boot no ESP mostra a moldura nova, troca de amp pelo picker, `CH n/4` |
| **F2** | **view SIGNAL** (cadeia 8 pedais + bancos) | toque em pedal liga/desliga e envia `cmd` ao RX; teclas 1-8 trocam banco |
| **F3** | PRESETS + GEAR | 8×8 navegável; grade de 52 pedais com scroll |
| **F4** | AUTO + CONFIG | intervalo/tocar/parar; tema e rede (SSID/IP reais) |
| **F5** | limpeza: remove `palco.c`, `amp_*.c`, telas antigas; commit final | binário < 3 MB, SPIFFS < 8 MB |

## 7. Riscos e rollback

- **Binário no limite (5% livres)** — a F1 já remove os 3 amps do build, então há folga
  antes de acrescentar código.
- **PSRAM** — 491 KB por arte; nunca manter duas artes full-size ao mesmo tempo.
- **Regressão de hardware** — camadas de WiFi/ESP-NOW/NVS não são tocadas; qualquer
  necessidade de mudança ali é discutida antes (`AGENTS.md` Regra 5).
- **Rollback** — branch `porta-hybridskull` + backup `/home/e/esp/oldskull_backup_<data>`;
  gravar o `build/esp_lvgl_oldskull.bin` atual (3.996.272 B, funcionando) antes do 1º flash.

## 8. Pendências do usuário

1. **Conectar a placa** — agora não há `/dev/ttyACM*` (sem porta serial).
2. **Autorizar a troca** de `palco_montar()` por `hybrid_ui_montar()` no `app_main.c`
   (Regra 8: confirmar antes de modificar).
3. **Vídeo de fundo**: sai de vez (mais fps/CPU) ou fica como uma skin?
