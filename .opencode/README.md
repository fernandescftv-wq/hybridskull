# OldSkull — config do OpenCode

Fluxo de IA do projeto OldSkull (ESP32-P4) rodando no OpenCode V2.
O `AGENTS.md` do projeto carrega as regras mestre; este diretório guarda o resto.

## Estrutura

```
.opencode/
├── opencode.jsonc          # config do projeto: default_agent, instructions, references, permissions
├── README.md               # este arquivo
├── agents/                 # agents especializados (subagentes)
│   ├── oldskull.md         # agente primário do projeto
│   ├── oldskull-build.md   # build (não edita fontes)
│   ├── oldskull-flash.md   # flash/rollback (não edita fontes)
│   ├── oldskull-debug.md   # diagnóstico de panic via backtrace
│   └── oldskull-docs.md    # escreve no vault do Obsidian
├── commands/               # comandos de barra
│   ├── build.md            # /build
│   ├── flash.md            # /flash
│   ├── serial.md           # /serial
│   ├── backup.md           # /backup
│   ├── checkpoint.md       # /checkpoint
│   ├── review.md           # /review
│   ├── diagnose.md         # /diagnose
│   └── gitdiff.md          # /gitdiff
├── skills/                 # skills (contexto sob demanda)
│   ├── oldskull-projeto/
│   ├── oldskull-regras-mestre/
│   ├── oldskull-build/
│   ├── oldskull-display/
│   ├── oldskull-pedals/
│   ├── oldskull-wifi/
│   ├── oldskull-lvgl/
│   ├── oldskull-gpio/
│   └── oldskull-rx/
└── plugins/oldskull/index.ts   # tool `oldskull_status`
```

## Uso

Abra o OpenCode dentro de `/home/e/esp/esp32_p4_jc4880p433c_bsp`. A sessão já começa
com o agente `oldskull`. Para carregar uma skill, peça `@oldskull-pedals` ou simplesmente
mencione o assunto — o agente carrega a skill certa.

- Build/flash: peça `/build` e `/flash`.
- Diagnóstico: `/diagnose` (segue o fluxo de backtrace, sem chutar).
- Revisão: `/review`.
- Documentação: agente `oldskull-docs` (só escreve no vault).

## Permissões

- Baseline global (`~/.config/opencode/opencode.jsonc`): pergunta antes de todo shell;
  git de inspeção liberado; `.env` proibido.
- Projeto: build/flash liberados; arquivos críticos (`palco.c`, `app_main.c`, `wifi.c`,
  `esp_now_tx.c`, `c6_ota.c`) sempre pedem confirmação; acesso fora do projeto pede
  confirmação (exceto o vault do Obsidian).
- Policies globais (duros, não dá para liberar): `sudo` e `rm -rf`.

## Referências

- `oldskull-docs` (global) e `vault` (projeto) → `/home/e/Documents/Obsidian Vault/OldSkull`
