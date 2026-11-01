/* hybrid_view_config.c - aba CONFIG: setTabs REDE/SKIN/SISTEMA/MIDI + setBody
 * (.srow k/v/chev + tog + ruler + corChip) — linhas copiadas do renderConfig().
 */
#include "hybrid_ui.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static lv_obj_t *s_hub, *s_body;
static int s_tab = -1;                      /* -1 = hub: coluna a ESQUERDA */

static void body_reconstruir(void);

static void tab_set(int i);

static void ev_body_vazio(lv_event_t *e)
{
    if (lv_event_get_target(e) == s_body)      /* clique no LOCAL VAZIO */
        tab_set(-1);
}

static void ev_tab(lv_event_t *e)
{
    tab_set((int)(intptr_t)lv_event_get_user_data(e));
}

static void ev_open_view(lv_event_t *e)
{
    hy_set_view((int)(intptr_t)lv_event_get_user_data(e));   /* sai do rail p/ tela */
}

/* ── hub do menu: 11 entradas em grade grande DISTRIBUIDA NA TELA
 *    (telas / submenus / acoes) ───────────────────────────────────────────── */
static const char *HUB_TXT[11] = { "SINAL", "PRESETS", "AMP", "GEAR", "AUTO",
                                   "REDE", "SKIN", "SISTEMA", "MIDI",
                                   "SALVAR", "CENARIO" };
static const int HUB_VIEW[5] = { HY_V_SIGNAL, HY_V_PRESETS, HY_V_AMP,
                                 HY_V_GEAR, HY_V_AUTO };
/* cor viva de cada tile (qualidade maxima: RGB cheio, sem tonzarada) */
static const uint32_t HUB_COR[11] = {
    0x2f7cd6, 0x9b4fd4, 0xe07b2e, 0x39a74a, 0xe0b32e,
    0x2eaaa7, 0xd44f92, 0x7a8ba6, 0xd95940,
    0x2ea44f, 0xd9a24b
};

static void salvo_tmr_cb(lv_timer_t *t)
{
    lv_obj_t *r = t->user_data;
    lv_obj_t *lb = lv_obj_get_child(r, -1);
    if (lb) lv_label_set_text(lb, "SALVAR");
    lv_timer_del(t);
}

static void salvar_row(lv_obj_t *r)
{
    lv_obj_t *lb = lv_obj_get_child(r, -1);
    if (lb) lv_label_set_text(lb, "OK!");
    lv_timer_create(salvo_tmr_cb, 1200, r);
}

static void ev_hub(lv_event_t *e)
{
    int i = (int)(intptr_t)lv_event_get_user_data(e);
    if (i == 9) {                              /* SALVAR: palco ja persiste */
        salvar_row(lv_event_get_target(e));
        return;
    }
    if (i == 10) {                             /* CENARIO: palco limpo */
        hy_cenario_toggle();
        return;
    }
    if (i < 5) {                               /* telas: SINAL..AUTO */
        hy_set_view(HUB_VIEW[i]);
        return;
    }
    tab_set(i - 5);                            /* submenus REDE..MIDI */
}

void hy_view_config_montar(lv_obj_t *pai)
{
    /* hub DISTRIBUIDO NA TELA: grade 6x2 com tiles grandes (texto c20) */
    s_hub = lv_obj_create(pai);
    lv_obj_remove_style_all(s_hub);
    lv_obj_set_pos(s_hub, 0, 0);
    lv_obj_set_size(s_hub, HY_W, HY_VIEW_H);
    lv_obj_clear_flag(s_hub, LV_OBJ_FLAG_SCROLLABLE);

    int cols = 6, pad = 6, gap = 8;
    int tw = (HY_W - 2 * pad - (cols - 1) * gap) / cols;
    int th = (HY_VIEW_H - 2 * pad - gap) / 2;
    for (int i = 0; i < 11; i++) {
        int col = i % cols, row = i / cols;
        lv_obj_t *b = lv_btn_create(s_hub);
        lv_obj_set_pos(b, pad + col * (tw + gap), pad + row * (th + gap));
        lv_obj_set_size(b, tw, th);
        lv_obj_set_style_radius(b, 12, 0);
        lv_obj_set_style_bg_color(b, lv_color_hex(HUB_COR[i]), 0);
        lv_obj_set_style_bg_opa(b, LV_OPA_40, 0);
        lv_obj_set_style_border_width(b, 2, 0);
        lv_obj_set_style_border_color(b, lv_color_hex(HUB_COR[i]), 0);
        lv_obj_set_style_border_opa(b, LV_OPA_MAX, 0);
        lv_obj_set_style_shadow_width(b, 0, 0);
        lv_obj_clear_flag(b, LV_OBJ_FLAG_SCROLLABLE);
        hy_txt(b, HUB_TXT[i], &hy_fonte_c20, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
        lv_obj_t *lb = lv_obj_get_child(b, -1);
        lv_obj_align(lb, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_text_color(lb, lv_color_hex(0xfdf6e8), 0);
        hy_press_style(b);
        lv_obj_add_event_cb(b, ev_hub, LV_EVENT_PRESSED, (void *)(intptr_t)i);
    }

    /* corpo dos submenus: a direita do rail */
    s_body = lv_obj_create(pai);
    lv_obj_remove_style_all(s_body);
    lv_obj_set_pos(s_body, 6, 6);
    lv_obj_set_size(s_body, HY_W - 12, HY_VIEW_H - 12);
    lv_obj_set_scroll_dir(s_body, LV_DIR_VER);
    lv_obj_set_style_pad_row(s_body, 6, 0);
    lv_obj_add_event_cb(s_body, ev_body_vazio, LV_EVENT_CLICKED, NULL);
    hy_limpar_scroll(s_body);
    lv_obj_set_flex_flow(s_body, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(s_body, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);

    tab_set(s_tab);
}

void hy_view_config_refresh(void)
{
    tab_set(s_tab);
}

static void tab_set(int i)
{
    s_tab = i;
    body_reconstruir();
}

/* ── .srow: k (esq) | v (dir) | chev ›  — devolve a row p/ extras ─────────── */
static lv_obj_t *srow(lv_obj_t *s_body, const char *k, const char *v,
                      lv_event_cb_t cb, void *ud)
{
    lv_obj_t *r = lv_btn_create(s_body);
    lv_obj_remove_style_all(r);
    lv_obj_set_size(r, HY_W - 44, 34);
    lv_obj_set_style_radius(r, 5, 0);
    lv_obj_set_style_bg_color(r, HY_C(0x171009), 0);
    lv_obj_set_style_bg_opa(r, LV_OPA_80, 0);
    lv_obj_set_style_border_color(r, HY_OURO_ESC, 0);
    lv_obj_set_style_border_opa(r, LV_OPA_40, 0);
    lv_obj_set_style_border_width(r, 1, 0);
    lv_obj_clear_flag(r, LV_OBJ_FLAG_SCROLLABLE);
    hy_press_style(r);
    if (cb) {
        lv_obj_add_flag(r, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(r, cb, LV_EVENT_CLICKED, ud);
    }
    lv_obj_t *lk = hy_txt(r, k, &hy_fonte_c20, HY_TXT, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_align(lk, LV_ALIGN_LEFT_MID, 9, 0);
    lv_obj_t *lv = hy_txt(r, v, &hy_fonte_c20, HY_HINT, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_align(lv, LV_ALIGN_RIGHT_MID, -16, 0);
    lv_obj_t *chev = hy_txt(r, ">", &hy_fonte_c20, HY_OURO_ESC, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_align(chev, LV_ALIGN_RIGHT_MID, -5, 0);
    return r;
}

/* .tog 26x14 alinhado a direita da row */
static void row_tog(lv_obj_t *r, bool on)
{
    lv_obj_t *t = hy_tog(r, 0, 0, 26, on);
    lv_obj_align(t, LV_ALIGN_RIGHT_MID, -17, 0);
}

static void ev_rede_wifi(lv_event_t *e)      { (void)e; hy.rede_wifi = !hy.rede_wifi;              body_reconstruir(); }
static void ev_rede_parear(lv_event_t *e)    { (void)e; hy.rede_pareando = !hy.rede_pareando;      body_reconstruir(); }
static void ev_skin_vel(lv_event_t *e)       { (void)e; hy.skin_vel = (hy.skin_vel + 1) % 3;       body_reconstruir(); }
static void ev_menu(lv_event_t *e)           { (void)e; hy.menu_fb = !hy.menu_fb;                  body_reconstruir(); }
static void ev_brilho(lv_event_t *e)         { (void)e; hy.cfg_brilho = (hy.cfg_brilho >= 100) ? 50 : 100; body_reconstruir(); }
static void ev_estilo(lv_event_t *e)         { (void)e; hy.cfg_estilo = (hy.cfg_estilo + 1) % 4;   body_reconstruir(); }
static void ev_midi_ch(lv_event_t *e)        { (void)e; hy.midi = (hy.midi % 16) + 1;              body_reconstruir(); }
static void ev_midi_clock(lv_event_t *e)     { (void)e; hy.midi_clock = !hy.midi_clock;            body_reconstruir(); }
static void ev_midi_scenes(lv_event_t *e)    { (void)e; hy.midi_scenes = !hy.midi_scenes;          body_reconstruir(); }
static void ev_cor(lv_event_t *e)            { hy.cor_letras = (int)(intptr_t)lv_event_get_user_data(e); body_reconstruir(); }

/* régua de brilho (20 ticks como .ruler do CSS) dentro da row Brilho */
static void ruler(lv_obj_t *r)
{
    for (int i = 1; i <= 20; i++) {
        lv_obj_t *t = lv_obj_create(r);
        lv_obj_remove_style_all(t);
        int h = 6 + (i / 3) * 3;                    /* CSS height */
        if (h > 20) h = 20;
        lv_obj_set_size(t, 2, h);
        lv_obj_set_pos(t, 200 + (i - 1) * 7, 26 - h - 2);
        lv_obj_set_style_bg_color(t, (i * 5 <= hy.cfg_brilho) ? HY_OURO : HY_C(0x2a2113), 0);
    }
}

/* cor das letras: 5 chips Ouro/Cobre/Ambar/Marfim/Bronze */
static void cor_row(lv_obj_t *s_body_)
{
    lv_obj_t *r = srow(s_body_, "Cor das letras", "", NULL, NULL);
    static const int maps[5] = { 0, 2, 3, 4, 5 };   /* índices LETTER_COLORS */
    for (int i = 0; i < 5; i++) {
        lv_obj_t *c = lv_obj_create(r);
        lv_obj_remove_style_all(c);
        lv_obj_set_size(c, 14, 14);
        lv_obj_set_style_radius(c, 7, 0);
        lv_obj_set_style_bg_color(c, lv_color_hex(hy_letter_colors[maps[i]]), 0);
        lv_obj_set_style_border_color(c, (hy.cor_letras == maps[i]) ? HY_OURO
                                     : lv_color_hex(0x241d13), 0);
        lv_obj_set_style_border_width(c, 2, 0);
        lv_obj_add_flag(c, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(c, ev_cor, LV_EVENT_CLICKED, (void *)(intptr_t)maps[i]);
        lv_obj_align(c, LV_ALIGN_RIGHT_MID, -56 + i * 20, 0);
    }
    hy_txt(r, hy_letter_names[hy.cor_letras], &hy_fonte_c12, HY_HINT,
           LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_t *nm = lv_obj_get_child(r, -1);
    lv_obj_align(nm, LV_ALIGN_RIGHT_MID, -220, 0);
}

static void body_reconstruir(void)
{
    if (!s_body) return;
    lv_obj_clean(s_body);

    if (!s_body) return;
    if (s_tab < 0) {                        /* hub: corpo some (grade na tela) */
        lv_obj_add_flag(s_body, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_clear_flag(s_body, LV_OBJ_FLAG_HIDDEN);
    srow(s_body, "< MENU", "", ev_tab, (void *)(intptr_t)-1);
    if (s_tab == 0) {                       /* REDE */
        lv_obj_t *r = srow(s_body, "WiFi", "", ev_rede_wifi, NULL);
        row_tog(r, hy.rede_wifi);
        srow(s_body, "Web Server", hy.rede_wifi ? "ONLINE :8775" : "OFFLINE", NULL, NULL);
        srow(s_body, "ESP-NOW", "PAREADO · 2 PEERS", NULL, NULL);
        lv_obj_t *rp = srow(s_body, "Pareamento",
                            hy.rede_pareando ? "AGUARDANDO RX..." : "INICIAR",
                            ev_rede_parear, NULL);
        row_tog(rp, hy.rede_pareando);
        srow(s_body, "Status", hy.rede_wifi ? "RX -52 dBm · GW 192.168.0.1"
                                            : "SEM CONEXAO", NULL, NULL);
    } else if (s_tab == 1) {                /* SKIN */
        const char *vel[3] = { "LENTO", "NORMAL", "RAPIDO" };
        srow(s_body, "Video", vel[hy.skin_vel], ev_skin_vel, NULL);
        srow(s_body, "Fundo", "AMP ATUAL (VIGENTE)", NULL, NULL);
    } else if (s_tab == 2) {                /* SISTEMA */
        lv_obj_t *rpz = srow(s_body, "Preset",
                             hy.banks[hy.cur_bank][hy.cur_slot].name, NULL, NULL);
        lv_obj_set_size(rpz, HY_W - 44, 36);
        lv_obj_t *vz = lv_obj_get_child(rpz, 1);
        if (vz) {
            lv_obj_set_style_text_font(vz, &hy_fonte_c20, 0);
            hy_lbl_preset = vz;             /* casco atualiza o nome aqui */
        }
        srow(s_body, "Menu", hy.menu_fb ? "ESQUERDA" : "ABAIXO", ev_menu, NULL);
        lv_obj_t *rb = srow(s_body, "Brilho", "ruler >", ev_brilho, NULL);
        ruler(rb);
        cor_row(s_body);
        const char *est[4] = { "MANUSCRITA", "PLAYFAIR", "CINZEL", "ROBUSTA" };
        srow(s_body, "Estilo das letras", est[hy.cfg_estilo], ev_estilo, NULL);
        /* AUTO: funcao rodando aqui (nao ha aba propria na nav) */
        const char *iv = (hy.auto_iv == 0) ? "2s" : (hy.auto_iv == 1) ? "4s"
                        : (hy.auto_iv == 2) ? "8s" : "16s";
        srow(s_body, "Modo Auto Stomp", hy.auto_run ? "RODANDO" : "OFF", NULL, NULL);
        srow(s_body, "Intervalo Auto", iv, NULL, NULL);
    } else {                                /* MIDI */
        char ch[6];
        snprintf(ch, sizeof(ch), "%02d", hy.midi);
        srow(s_body, "Canal MIDI", ch, ev_midi_ch, NULL);
        lv_obj_t *rc = srow(s_body, "Clock Sync", "", ev_midi_clock, NULL);
        row_tog(rc, hy.midi_clock != 0);
        srow(s_body, "Scenes", hy.midi_scenes ? "LIST" : "VISUAL",
             ev_midi_scenes, NULL);
    }
    srow(s_body, "VOLTAR <", "", ev_tab, (void *)(intptr_t)-1);
}
