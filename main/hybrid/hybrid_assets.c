/* hybrid_assets.c - decode JPEG (hardware do P4) -> buffers RGB565 em PSRAM.
 * Mesma receita do sd_fundos.c (que já roda no hardware):
 *   jpeg_new_decoder_engine + jpeg_alloc_decoder_mem + jpeg_decoder_process.
 * Diferenca: cada decodada é COPIADA para um buffer próprio em PSRAM, porque
 * mantemos varios thumbs/pedais vivos (LRU) além do fundo e da arte.
 */
#include "hybrid_assets.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/jpeg_decode.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

static const char *TAG = "hy_assets";

#define HY_IN_MAX     (192 * 1024)
#define HY_OUT_MAX    ((800 + 16) * (480 + 16) * 2)

static jpeg_decoder_handle_t s_dec = NULL;
static uint8_t *s_jpeg = NULL;      /* entrada */
static uint8_t *s_rgb = NULL;       /* saida temporaria */
static size_t   s_rgb_sz = 0;

typedef struct {
    char key[24];
    int  w, h;
    uint8_t *buf;                   /* PSRAM, w*h*2 */
    lv_img_dsc_t dsc;
    uint32_t stamp;
} HyEntry;

static HyEntry s_art, s_head, s_bg;   /* 1 de cada (amp vigente) */
#define HY_N_THUMB 32     /* picker (31 thumbs) + margem */
#define HY_N_PEDAL 8
#define HY_N_PZOOM 2     /* modal mostra 1; margem para transicao */
#define HY_N_PGEAR 8
#define HY_N_SLIDE 2     /* preview raw alpha do slide (1 em uso) */
static HyEntry s_thumb[HY_N_THUMB];
static HyEntry s_pedal[HY_N_PEDAL];
static HyEntry s_pzoom[HY_N_PZOOM];
static HyEntry s_pgear[HY_N_PGEAR];
static HyEntry s_slide[HY_N_SLIDE];
static HyEntry s_pchain[HY_N_PEDAL];
static uint32_t s_clock;

static bool engine(void)
{
    if (s_dec) return true;
    jpeg_decode_engine_cfg_t eng = { .intr_priority = 0, .timeout_ms = 400 };
    if (jpeg_new_decoder_engine(&eng, &s_dec) != ESP_OK) {
        ESP_LOGE(TAG, "engine JPEG falhou");
        return false;
    }
    jpeg_decode_memory_alloc_cfg_t tx = { .buffer_direction = JPEG_DEC_ALLOC_INPUT_BUFFER };
    jpeg_decode_memory_alloc_cfg_t rx = { .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER };
    size_t txs = 0;
    s_jpeg = jpeg_alloc_decoder_mem(HY_IN_MAX, &tx, &txs);
    s_rgb  = jpeg_alloc_decoder_mem(HY_OUT_MAX, &rx, &s_rgb_sz);
    if (!s_jpeg || !s_rgb) {
        ESP_LOGE(TAG, "sem memoria para buffers JPEG");
        return false;
    }
    return true;
}

/* decodifica path->(w,h) e devolve buffer próprio em PSRAM (NULL em falha) */
static uint8_t *decode_file(const char *path, int *w, int *h)
{
    if (!engine()) return NULL;
    struct stat st;
    if (stat(path, &st) != 0 || st.st_size <= 0 || st.st_size > HY_IN_MAX) {
        ESP_LOGE(TAG, "arquivo invalido: %s", path);
        return NULL;
    }
    FILE *f = fopen(path, "rb");
    if (!f) { ESP_LOGE(TAG, "abrir falhou: %s", path); return NULL; }
    size_t sz = (size_t)st.st_size;
    if (fread(s_jpeg, 1, sz, f) != sz) { fclose(f); return NULL; }
    fclose(f);

    jpeg_decode_cfg_t cfg = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB565,
        .rgb_order = JPEG_DEC_RGB_ELEMENT_ORDER_BGR,
    };
    uint32_t saida = 0;
    if (jpeg_decoder_process(s_dec, &cfg, s_jpeg, (uint32_t)sz,
                             s_rgb, s_rgb_sz, &saida) != ESP_OK || saida == 0) {
        ESP_LOGE(TAG, "decode falhou: %s", path);
        return NULL;
    }

    /* dims reais: o driver preenche a saida linha a linha; w/h vem do JPEG.
     * usamos as dims conhecidas de cada asset — o chamador informa. */
    size_t need = (size_t)(*w) * (*h) * 2;
    uint8_t *dst = heap_caps_malloc(need, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!dst) { ESP_LOGE(TAG, "PSRAM cheia para %s (%u KB)", path, (unsigned)(need / 1024)); return NULL; }
    memcpy(dst, s_rgb, need);
    return dst;
}

static void entry_init(HyEntry *e, const char *key, int w, int h, const char *path)
{
    if (!strcmp(e->key, key) && e->buf) return;     /* já decodificado */
    if (e->buf) { heap_caps_free(e->buf); e->buf = NULL; }
    snprintf(e->key, sizeof(e->key), "%s", key);
    e->w = w; e->h = h;
    e->buf = decode_file(path, &w, &h);
    if (!e->buf) { e->key[0] = 0; return; }
    e->dsc.header.w = w;
    e->dsc.header.h = h;
    e->dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    e->dsc.data_size = w * h * 2;
    e->dsc.data = e->buf;
    e->stamp = ++s_clock;
    ESP_LOGI(TAG, "%s ok (%dx%d)", key, w, h);
}

/* raw RGB565A8 (LV_IMG_CF_TRUE_COLOR_ALPHA, LV_COLOR_16_SWAP=0) */
static void entry_raw(HyEntry *e, const char *key, int w, int h, const char *path)
{
    if (!strcmp(e->key, key) && e->buf) return;
    if (e->buf) { heap_caps_free(e->buf); e->buf = NULL; }
    snprintf(e->key, sizeof(e->key), "%s", key);
    e->w = w; e->h = h;
    FILE *f = fopen(path, "rb");
    if (!f) { ESP_LOGE(TAG, "abrir falhou: %s", path); e->key[0] = 0; return; }
    size_t sz = (size_t)w * h * 3;
    uint8_t *dst = heap_caps_malloc(sz, MALLOC_CAP_SPIRAM);
    if (!dst) { ESP_LOGE(TAG, "PSRAM cheia (%s)", path); e->key[0] = 0; fclose(f); return; }
    if (fread(dst, 1, sz, f) != sz) {
        ESP_LOGE(TAG, "ler falhou: %s", path);
        heap_caps_free(dst); fclose(f); e->key[0] = 0; return;
    }
    fclose(f);
    e->buf = dst;
    e->dsc.header.w = w; e->dsc.header.h = h;
    e->dsc.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    e->dsc.header.always_zero = 0;
    e->dsc.data_size = sz;
    e->dsc.data = e->buf;
    e->stamp = ++s_clock;
    ESP_LOGI(TAG, "%s ok (raw %dx%d)", key, w, h);
}

/* LRU simples: substitui a entrada com stamp mais antigo */
static HyEntry *lru(HyEntry *v, int n)
{
    HyEntry *best = &v[0];
    for (int i = 0; i < n; i++) {
        if (!v[i].buf) return &v[i];
        if (v[i].stamp < best->stamp) best = &v[i];
    }
    if (best) { heap_caps_free(best->buf); best->buf = NULL; best->key[0] = 0; }
    return best;
}

lv_img_dsc_t *hy_assets_bg(const char *slug)
{
    char key[24], path[64];
    snprintf(key, sizeof(key), "bg_%s", slug);
    snprintf(path, sizeof(path), "/spiffs/amps/%s.jpg", key);
    entry_init(&s_bg, key, 800, 480, path);
    return s_bg.buf ? &s_bg.dsc : NULL;
}

lv_img_dsc_t *hy_assets_art(const char *slug)
{
    char key[24], path[64];
    snprintf(key, sizeof(key), "art_%s", slug);
    snprintf(path, sizeof(path), "/spiffs/amps/%s.jpg", slug);
    entry_init(&s_art, key, 744, 304, path);
    return s_art.buf ? &s_art.dsc : NULL;
}

lv_img_dsc_t *hy_assets_head(const char *slug)
{
    char key[24], path[64];
    snprintf(key, sizeof(key), "a_%s", slug);
    snprintf(path, sizeof(path), "/spiffs/amps/a_%s.jpg", slug);
    entry_init(&s_head, key, 592, 248, path);
    return s_head.buf ? &s_head.dsc : NULL;
}

lv_img_dsc_t *hy_assets_thumb(const char *slug)
{
    char key[24], path[64];
    snprintf(key, sizeof(key), "t_%s", slug);
    snprintf(path, sizeof(path), "/spiffs/amps/%s.jpg", key);
    /* se já está no cache devolve */
    for (int i = 0; i < HY_N_THUMB; i++)
        if (s_thumb[i].buf && !strcmp(s_thumb[i].key, key)) {
            s_thumb[i].stamp = ++s_clock;
            return &s_thumb[i].dsc;
        }
    HyEntry *e = lru(s_thumb, HY_N_THUMB);
    entry_init(e, key, 152, 56, path);
    return e->buf ? &e->dsc : NULL;
}

lv_img_dsc_t *hy_assets_pedal(const char *id)
{
    char key[24], path[64];
    snprintf(key, sizeof(key), "p_%s", id);
    snprintf(path, sizeof(path), "/spiffs/pedais/p_%s.jpg", id);
    for (int i = 0; i < HY_N_PEDAL; i++)
        if (s_pedal[i].buf && !strcmp(s_pedal[i].key, key)) {
            s_pedal[i].stamp = ++s_clock;
            return &s_pedal[i].dsc;
        }
    HyEntry *e = lru(s_pedal, HY_N_PEDAL);
    entry_init(e, key, 80, 120, path);
    return e->buf ? &e->dsc : NULL;
}

/* foto grande para o zoom (z_<id> 240x360, desenhada ~1:1 no modal) */
lv_img_dsc_t *hy_assets_pedal_zoom(const char *id)
{
    char key[26], path[64];
    snprintf(key, sizeof(key), "z_%s", id);
    snprintf(path, sizeof(path), "/spiffs/pedais/z_%s.jpg", id);
    for (int i = 0; i < HY_N_PZOOM; i++)
        if (s_pzoom[i].buf && !strcmp(s_pzoom[i].key, key)) {
            s_pzoom[i].stamp = ++s_clock;
            return &s_pzoom[i].dsc;
        }
    HyEntry *e = lru(s_pzoom, HY_N_PZOOM);
    entry_init(e, key, 240, 360, path);
    return e->buf ? &e->dsc : NULL;
}

/* preview do slide do AMP: m_<slug>.jpg 320x88 (cover: sem barras) */
lv_img_dsc_t *hy_assets_slide(const char *slug)
{
    char key[40], path[80];
    snprintf(key, sizeof(key), "m_%s", slug);
    snprintf(path, sizeof(path), "/spiffs/amps/m_%s.jpg", slug);
    for (int i = 0; i < HY_N_SLIDE; i++)
        if (s_slide[i].buf && !strcmp(s_slide[i].key, key)) {
            s_slide[i].stamp = ++s_clock;
            return &s_slide[i].dsc;
        }
    HyEntry *e = lru(s_slide, HY_N_SLIDE);
    entry_init(e, key, 320, 88, path);
    return e->buf ? &e->dsc : NULL;
}

/* chain: s_<id>.bin 76x114 raw com ALPHA (sem barra preta) */
lv_img_dsc_t *hy_assets_pedal_chain(const char *id)
{
    char key[26], path[64];
    snprintf(key, sizeof(key), "s_%s", id);
    snprintf(path, sizeof(path), "/spiffs/pedais/s_%s.bin", id);
    for (int i = 0; i < HY_N_PEDAL; i++)
        if (s_pchain[i].buf && !strcmp(s_pchain[i].key, key)) {
            s_pchain[i].stamp = ++s_clock;
            return &s_pchain[i].dsc;
        }
    HyEntry *e = lru(s_pchain, HY_N_PEDAL);
    entry_raw(e, key, 76, 114, path);
    return e->buf ? &e->dsc : NULL;
}

/* GEAR: r_<id>.bin 80x120 raw com ALPHA */
lv_img_dsc_t *hy_assets_pedal_gear(const char *id)
{
    char key[26], path[64];
    snprintf(key, sizeof(key), "r_%s", id);
    snprintf(path, sizeof(path), "/spiffs/pedais/r_%s.bin", id);
    for (int i = 0; i < HY_N_PGEAR; i++)
        if (s_pgear[i].buf && !strcmp(s_pgear[i].key, key)) {
            s_pgear[i].stamp = ++s_clock;
            return &s_pgear[i].dsc;
        }
    HyEntry *e = lru(s_pgear, HY_N_PGEAR);
    entry_raw(e, key, 80, 120, path);
    return e->buf ? &e->dsc : NULL;
}

void hy_assets_limpar_amp(void)
{
    if (s_bg.buf)  { heap_caps_free(s_bg.buf);  s_bg.buf = NULL; s_bg.key[0] = 0; }
    if (s_art.buf) { heap_caps_free(s_art.buf); s_art.buf = NULL; s_art.key[0] = 0; }
    if (s_head.buf){ heap_caps_free(s_head.buf);s_head.buf= NULL; s_head.key[0] = 0; }
}

void hy_assets_teardown(void)
{
    hy_assets_limpar_amp();
    for (int i = 0; i < HY_N_THUMB; i++)
        if (s_thumb[i].buf) { heap_caps_free(s_thumb[i].buf); s_thumb[i].buf = NULL; }
    for (int i = 0; i < HY_N_PEDAL; i++)
        if (s_pedal[i].buf) { heap_caps_free(s_pedal[i].buf); s_pedal[i].buf = NULL; }
    for (int i = 0; i < HY_N_PZOOM; i++)
        if (s_pzoom[i].buf) { heap_caps_free(s_pzoom[i].buf); s_pzoom[i].buf = NULL; }
    for (int i = 0; i < HY_N_PGEAR; i++)
        if (s_pgear[i].buf) { heap_caps_free(s_pgear[i].buf); s_pgear[i].buf = NULL; }
    for (int i = 0; i < HY_N_PEDAL; i++)
        if (s_pchain[i].buf) { heap_caps_free(s_pchain[i].buf); s_pchain[i].buf = NULL; }
    for (int i = 0; i < HY_N_SLIDE; i++)
        if (s_slide[i].buf) { heap_caps_free(s_slide[i].buf); s_slide[i].buf = NULL; }
    ESP_LOGI(TAG, "teardown ok");
}
