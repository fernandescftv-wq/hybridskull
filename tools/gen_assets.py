#!/usr/bin/env python3
"""Gera os assets do HybridSkull para o ESP32-P4 (JPEG no SPIFFS).

Entrada : ~/HybridSkull_UI/assets/{amps,panels}  +  data.js (catalogo)
Saida   : spiffs/amps/<slug>.jpg        760x323  arte do amp (palco / picker)
          spiffs/amps/t_<slug>.jpg      190x81   thumb do seletor
          spiffs/amps/bg_<slug>.jpg     800x480  FUNDO já acabado como no HTML
          spiffs/pedais/<id>.jpg        117x177  pedal achatado no fundo #0E0B08

O fundo bg_<slug>.jpg reproduz em um único JPEG a composição CSS do HTML
(S.skin.fundo='amp'):
    #bgBlur : arte em cover, blur, brightness .45, saturate 1.25
    #bgFit  : arte inteira em contain (800x340) centrada
    #bg::after : gradiente escuro 180deg rgba(8,6,3,.46) -> .74
    #wm     : 'OLDSKULL' Cinzel 900, rotate -4deg, tracking .22em, ouro alpha .05
Assim o LVGL decodifica só 1 imagem e não gasta CPU com blur/zoom.

Uso:  python3 tools/gen_assets.py [--force]

JPEG baseline (PIL salva baseline) -> compatível com o decoder de hardware do
P4 usado em sd_funtos... ver main/sd_fundos.c. Idempotente (--force regenera).
"""
import json
import os
import subprocess
import sys

from PIL import Image

WEB = os.path.expanduser('~/HybridSkull_UI')
PROJ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_AMPS = os.path.join(PROJ, 'spiffs', 'amps')
OUT_PEDAIS = os.path.join(PROJ, 'spiffs', 'pedais')

_BG_GRAD = None                           # gradiente #bg.video (sem arte)

ART_W, ART_H = 744, 304          # arte (multiplo de 8) — view 398 (abas fora)
HEAD_W, HEAD_H = 592, 248          # head GRANDE 1:1 (sem upscale = sem marcas)
THUMB_W, THUMB_H = 152, 56     # t_: tira do picker (nomes c16 embaixo)
PEDAL_WH = (80, 120)            # chain 1:1 (nitido, sem transform)
PEDAL_Z_WH = (240, 360)         # zoom do pedal (240x360 no modal)
PEDAL_G_WH = (56, 88)           # cards do GEAR (1:1)
PEDAL_BG = (21, 16, 10)            # p_ (GEAR fallback: card)
Z_BG = (23, 18, 8)                 # z_ (veu do modal 0x171208)
CHAIN_W, CHAIN_H = 76, 114         # chain: raw RGB565A8 com ALPHA (sem barra!)
GEAR_W, GEAR_H = 80, 120           # r_ GEAR: raw RGB565A8 com ALPHA

def _fit_contain(im, wh, bg):
    # encaixa a arte (contain) num canvas wh com fundo bg (o CSS e object-fit:contain)
    w, h = wh
    sc = min(w / im.width, h / im.height)
    nw, nh = round(im.width * sc), round(im.height * sc)
    im2 = im.resize((max(1, nw), max(1, nh)), Image.LANCZOS)
    canvas = Image.new('RGB', wh, bg)
    canvas.paste(im2, ((w - nw) // 2, (h - nh) // 2))
    return canvas          # #0E0B08 (--painel do index.html)
WM_TXT = 'OLDSKULL'
WM_PX = 116                     # 210px @1440 -> 800  (escala da tela)
WM_COR = (243, 201, 105)


def catalogo():
    """Extrai AMPS/PEDAIS do data.js via node (mesma fonte da UI web)."""
    js = (
        "global.window={};const fs=require('fs');"
        f"eval(fs.readFileSync('{WEB}/data.js','utf8'));"
        "console.log(JSON.stringify({amps:window.AMPS,pedais:window.PEDAIS}));"
    )
    out = subprocess.run(['node', '-e', js], capture_output=True, text=True, check=True)
    return json.loads(out.stdout)


def rgb565a8_bytes(im):
    # LVGL8 TRUE_COLOR_ALPHA @RGB565: [b0][b1][a] por pixel (little endian)
    px = im.load()
    out = bytearray(im.width * im.height * 3)
    i = 0
    for y in range(im.height):
        for x in range(im.width):
            r, g, b, a = px[x, y]
            v = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
            out[i] = v & 0xff; out[i+1] = v >> 8; out[i+2] = a
            i += 3
    return bytes(out)

def _fit_cover(im, wh):
    # fill: recorta o centro (nao deixa barra, nao distorce)
    w, h = wh
    sc = max(w / im.width, h / im.height)
    nw, nh = max(w, round(im.width * sc)), max(h, round(im.height * sc))
    im2 = im.convert('RGB').resize((nw, nh), Image.LANCZOS)
    x, y = (nw - w) // 2, (nh - h) // 2
    return im2.crop((x, y, x + w, y + h))

def _fit_alpha(im, wh):
    # contain preservando o fundo TRANSPARENTE (endling: sem barra preta)
    w, h = wh
    sc = min(w / im.width, h / im.height)
    nw, nh = max(1, round(im.width * sc)), max(1, round(im.height * sc))
    im2 = im.convert('RGBA').resize((nw, nh), Image.LANCZOS)
    c = Image.new('RGBA', wh, (0, 0, 0, 0))
    c.paste(im2, ((w - nw) // 2, (h - nh) // 2), im2)
    return c

def _accent(im):
    # cor dominante saturada do amp (para o glow)
    im2 = im.convert('RGB').resize((64, 36))
    q = im2.quantize(colors=12)
    pal = q.getpalette() or []
    for _cnt, idx in sorted(q.getcolors(), reverse=True):
        r, g, b = pal[idx*3: idx*3+3]
        mx, mn = max(r, g, b), min(r, g, b)
        if mx > 60 and (mx - mn) > 35:
            return (r << 16) | (g << 8) | b
    return 0xd9a24b

def salvar_jpeg(im, path, quality):
    # subsampling=0 (4:4:4): sem perda de cor = sem borrão nas bordas
    im.convert('RGB').save(path, 'JPEG', quality=quality, optimize=True,
                           progressive=False, subsampling=0)


# ─── composição do fundo (réplica do CSS do index.html) ──────────────────────

def _cover_800x480(im):
    """CSS cover: preenche 800x480 cortando o excesso (sem distorcer)."""
    ar_t, ar_s = 800 / 480, im.width / im.height
    if ar_s > ar_t:
        w = int(round(im.height * ar_t))
        x0 = (im.width - w) // 2
        im = im.crop((x0, 0, x0 + w, im.height))
    else:
        h = int(round(im.width / ar_t))
        y0 = (im.height - h) // 2
        im = im.crop((0, y0, im.width, y0 + h))
    return im.resize((800, 480), Image.LANCZOS)


def _wm_img():
    """Watermark 'OLDSKULL' em camada RGBA (Cinzel 900, tracking .22em)."""
    from PIL import ImageDraw, ImageFont
    f = ImageFont.truetype(os.path.join(WEB, 'assets/fonts/Cinzel.ttf'), WM_PX)
    try:
        f.set_variation_by_axes([900])
    except Exception:
        pass

    pad, trk = 40, int(WM_PX * 0.22)
    d = ImageDraw.Draw(Image.new('RGBA', (8, 8)))
    larguras = [d.textlength(c, font=f) for c in WM_TXT]
    w = int(sum(larguras) + trk * (len(WM_TXT) - 1)) + 2 * pad
    h = WM_PX + 2 * pad
    lay = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    d = ImageDraw.Draw(lay)
    x = pad
    for c, cw in zip(WM_TXT, larguras):
        d.text((x, pad), c, font=f, fill=WM_COR + (255,), anchor='lm')
        x += cw + trk
    return lay.rotate(4, expand=True, resample=Image.BICUBIC)   # CSS: -4deg


def _fit_contain_rgba(src, wh):
    w, h = wh
    sc = min(w / src.width, h / src.height)
    nw, nh = max(1, round(src.width * sc)), max(1, round(src.height * sc))
    return src.convert('RGBA').resize((nw, nh), Image.LANCZOS)

def _bake_bg_art(bg, rect, src, sombra, glow=None):
    # recorta a bg no retangulo exato onde a arte aparece e cola a arte com
    # alpha (o PNG transparente deixa o fundo real - nada de faixas pretas).
    from PIL import ImageFilter
    x, y, w, h = rect
    can = bg.crop((x, y, x + w, y + h)).convert('RGBA')
    art = _fit_contain_rgba(src, (w, h))
    pos = ((w - art.width) // 2, (h - art.height) // 2)
    if sombra:
        offy, blur, pct = sombra
        layer = Image.new('L', (w, h), 0)
        layer.paste(Image.new('L', art.size, int(255 * pct)), (pos[0], pos[1] + offy))
        layer = layer.filter(ImageFilter.GaussianBlur(blur))
        zl = Image.new('L', (w, h), 0)
        sh = Image.merge('RGBA', (zl, zl, zl, layer))
        can.alpha_composite(sh)
    if glow:
        # glow na cor dominante do amp: silhueta da arte borrada, assada no
        # JPEG (equivalente ao drop-shadow do CSS; segue o CONTORNO do amp)
        cor, blur = glow
        for fator, forca in ((1.6, 1.0), (0.8, 1.5)):   # largo e suave + denso
            b2 = max(4, int(blur * fator))
            a = art.split()[3].filter(ImageFilter.GaussianBlur(b2))
            if forca > 1.0:
                a = a.point(lambda v: min(255, int(v * forca)))
            layer = Image.new('L', (w, h), 0)
            layer.paste(a, pos)
            gl = Image.new('RGBA', (w, h), cor + (0,))
            gl.putalpha(layer)
            can.alpha_composite(gl)
    can.paste(art, pos, art)
    return can.convert('RGB')

def _compor_fundo(im):
    """BG #bg.video do HTML: conico em (50%,120%) + radial quente (50%,65%)
    + veu (#bg::after 8,6,3 .46->.74). Fundo UNICO (nao repete a arte)."""
    global _BG_GRAD
    if _BG_GRAD is not None:
        return _fundo_com_arte(_BG_GRAD, im)
    import math
    W, H = 800, 480
    cx, cy = 400, 576                      # conico em 50% / 120%
    stops = [(0, (0x1c, 0x12, 0x06), 1.00), (30, (0xc9, 0x92, 0x2b), 0.16),
             (60, (0x1c, 0x12, 0x06), 1.00), (120, (0x8a, 0x64, 0x20), 0.22),
             (150, (0x1a, 0x11, 0x07), 1.00), (210, (0xf3, 0xc9, 0x69), 0.10),
             (240, (0x1a, 0x11, 0x07), 1.00), (300, (0xff, 0xa0, 0x30), 0.14),
             (360, (0x1c, 0x12, 0x06), 1.00)]
    out = Image.new('RGB', (W, H))
    op = out.load()
    base = (7, 5, 3)
    for y in range(H):
        for x in range(W):
            dx, dy = x - cx, y - cy
            ang = (math.degrees(math.atan2(dx, -dy)) + 360.0) % 360.0
            r, g, b, opa = base[0], base[1], base[2], 0.0
            for k in range(len(stops) - 1):
                a0, c0, o0 = stops[k]
                a1, c1, o1 = stops[k + 1]
                if a0 <= ang <= a1:
                    f = 0.0 if a1 == a0 else (ang - a0) / (a1 - a0)
                    rr = c0[0] + (c1[0] - c0[0]) * f
                    gg = c0[1] + (c1[1] - c0[1]) * f
                    bb = c0[2] + (c1[2] - c0[2]) * f
                    opa = o0 + (o1 - o0) * f
                    r = rr * opa + r * (1 - opa)
                    g = gg * opa + g * (1 - opa)
                    b = bb * opa + b * (1 - opa)
                    break
            ddx, ddy = x - 400, y - 312          # radial 900x480 @0.555 ~ (500,266)
            t = math.sqrt((ddx / 500.0) ** 2 + (ddy / 266.0) ** 2)
            ra = 0.0 if t >= 0.70 else (1 - t / 0.70)
            r = r * (1 - ra) + 0x2a * ra
            g = g * (1 - ra) + 0x1a * ra
            b = b * (1 - ra) + 0x08 * ra
            va = 0.46 + (0.74 - 0.46) * y / 479.0        # veil #bg::after
            r = r * (1 - va) + 8 * va
            g = g * (1 - va) + 6 * va
            b = b * (1 - va) + 3 * va
            op[x, y] = (int(r), int(g), int(b))
    _BG_GRAD = out
    return _fundo_com_arte(out, im)


def _fundo_com_arte(grad, im):
    """CASCATA EXATA do HTML (1440->800):
       bg.video (grad conico+radial) -> #bgBlur (cover, blur 40px@1440~22,
       brightness .45, saturate 1.25, opacity .9) -> #bgFit (contain nitido
       em brilho cheio) -> #bg::after (veu 8,6,3 .46->.74 POR CIMA DE TUDO)."""
    from PIL import Image, ImageEnhance, ImageFilter
    # 1) bgBlur: cover + inset -12% (~992px) simplificado p/ cover 800x480
    sc = max(800 / im.width, 480 / im.height)
    bw, bh = round(im.width * sc), round(im.height * sc)
    blur = im.resize((max(bw, 800 + 24), max(bh, 480 + 24)), Image.LANCZOS)
    bx, by = (blur.width - 800) // 2, (blur.height - 480) // 2
    blur = blur.crop((bx, by, bx + 800, by + 480))
    blur = blur.filter(ImageFilter.GaussianBlur(22)).convert('RGB')
    blur = ImageEnhance.Brightness(blur).enhance(0.45)
    blur = ImageEnhance.Color(blur).enhance(1.25)
    out = Image.blend(grad, blur, 0.9)                 # opacity .9 do blur
    # 2) bgFit: contain nítido, brilho cheio
    fit_h = round(800 * im.height / im.width)
    fit = im.resize((800, fit_h), Image.LANCZOS)
    top = (480 - fit_h) // 2
    if fit.mode == 'RGBA':
        out.paste(fit, (0, top), fit.split()[3])
    else:
        out.paste(fit, (0, top))
    # 3) veil #bg::after vertical em TUDO
    W, H = 800, 480
    v = Image.new('RGB', (W, 1))
    px = v.load()
    for y in range(H):
        a = 0.46 + (0.74 - 0.46) * y / (H - 1)
        fx = out.crop((0, y, W, y + 1))
        cor = Image.new('RGB', (W, 1), (8, 6, 3))
        out.paste(Image.blend(fx, cor, a), (0, y))
    return out


# ─── pipeline ────────────────────────────────────────────────────────────────

def main():
    from PIL import Image

    force = '--force' in sys.argv
    os.makedirs(OUT_AMPS, exist_ok=True)
    os.makedirs(OUT_PEDAIS, exist_ok=True)
    cat = catalogo()
    t_a = t_t = t_b = t_p = t_h = 0
    n_a = n_t = n_b = n_p = n_h = 0
    faltando = []
    accents = {}

    for a in cat['amps']:
        src = os.path.join(WEB, 'assets', 'amps', a['slug'] + '.png')
        try:
            accents[a['slug']] = _accent(Image.open(src))
        except Exception as e:
            print('accent falhou', a['slug'], e)
        if not os.path.exists(src):
            faltando.append(src)
            continue
        im = Image.open(src)

        bg_full = _compor_fundo(im)

        # thumbs: tira do picker (web .apick, cover)
        dst = os.path.join(OUT_AMPS, 't_' + a['slug'] + '.jpg')
        if force or not os.path.exists(dst):
            salvar_jpeg(_fit_cover(im, (THUMB_W, THUMB_H)), dst, 85)
        t_t += os.path.getsize(dst); n_t += 1

        # arts (slug.jpg, AMP view): recorte da bg em (92, 96) tamanho 616x264,
        # art com alpha por cima (sem faixas); glow assado na cor do amp
        dst = os.path.join(OUT_AMPS, a['slug'] + '.jpg')
        ca = accents.get(a['slug'], 0xd9a24b)
        rgb = ((ca >> 16) & 255, (ca >> 8) & 255, ca & 255)
        if force or not os.path.exists(dst):
            salvar_jpeg(_bake_bg_art(bg_full, (28, 42, ART_W, ART_H), im, None,
                                     glow=(rgb, 14)), dst, 94)
        t_a += os.path.getsize(dst); n_a += 1

        # head (a_, SIGNAL): mesma coisa, em (104,70), 592x248 com o glow
        # na cor do amp (assado; sem retangulo de sombra do LVGL)
        dst = os.path.join(OUT_AMPS, 'a_' + a['slug'] + '.jpg')
        if force or not os.path.exists(dst):
            salvar_jpeg(_bake_bg_art(bg_full, (104, 40, HEAD_W, HEAD_H), im, None,
                                     glow=(rgb, 13)), dst, 94)
        t_h += os.path.getsize(dst); n_h += 1

        # fundo por skin (bg_)
        dst = os.path.join(OUT_AMPS, 'bg_' + a['slug'] + '.jpg')
        if force or not os.path.exists(dst):
            salvar_jpeg(bg_full, dst, 82)
        t_b += os.path.getsize(dst); n_b += 1

    for p in cat['pedais']:
        panel = p.get('panel')
        if not panel:
            continue
        src = os.path.join(WEB, 'assets', 'panels', panel)
        if not os.path.exists(src):
            faltando.append(src)
            continue
        jobs = [('z_', PEDAL_Z_WH, 'jpg', Z_BG, 88),
                ('s_', (CHAIN_W, CHAIN_H), 'raw', None, 0),
                ('r_', (GEAR_W, GEAR_H), 'raw', None, 0)]
        for pre, wh, kind, bg, q in jobs:
            if kind == 'raw':
                dst = os.path.join(OUT_PEDAIS, pre + p['id'] + '.bin')
                if force or not os.path.exists(dst):
                    salvar_jpeg  # (nao usado; raw abaixo)
                    dump = rgb565a8_bytes(_fit_alpha(Image.open(src), wh))
                    open(dst, 'wb').write(dump)
            else:
                dst = os.path.join(OUT_PEDAIS, pre + p['id'] + '.jpg')
                if force or not os.path.exists(dst):
                    im = Image.open(src).convert('RGBA')
                    fundo = Image.new('RGBA', im.size, bg + (255,))
                    achatado = Image.alpha_composite(fundo, im).convert('RGB')
                    salvar_jpeg(_fit_contain(achatado, wh, bg), dst, q)
            t_p += os.path.getsize(dst); n_p += 1

    kb = lambda b: b / 1024.0
    print(f'artes    : {n_a:3d}  {kb(t_a):8.1f} KB  (media {kb(t_a)/max(n_a,1):5.1f} KB)')
    print(f'heads    : {n_h:3d}  {kb(t_h):8.1f} KB  (a_<slug>, SIGNAL 512x216)')
    print(f'thumbs   : {n_t:3d}  {kb(t_t):8.1f} KB')
    print(f'fundos   : {n_b:3d}  {kb(t_b):8.1f} KB  (media {kb(t_b)/max(n_b,1):5.1f} KB)')
    print(f'pedais   : {n_p:3d}  {kb(t_p):8.1f} KB')
    tot = t_a + t_h + t_t + t_b + t_p
    print(f'TOTAL    : {kb(tot):8.1f} KB  -> spiffs/amps/ e spiffs/pedais/')

    # tabela de cor dominante por amp (glow segue o amp)
    hdir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'main', 'hybrid')
    with open(os.path.join(hdir, 'hybrid_accents.c'), 'w') as f:
        f.write('/* auto: cor dominante por amp (gerada por gen_assets.py) */\n')
        f.write('#include "hybrid_accents.h"\n#include <string.h>\n#include <stdint.h>\n\n')
        f.write('typedef struct { const char *slug; uint32_t hex; } AmAcc;\n')
        f.write('static const AmAcc ACC[] = {\n')
        for slug, hexv in sorted(accents.items()):
            f.write('    { "%s", 0x%06x },\n' % (slug, hexv))
        f.write('};\n\n')
        f.write('uint32_t hy_amp_accent_hex(const char *slug)\n{\n')
        f.write('    for (unsigned i = 0; i < sizeof(ACC)/sizeof(ACC[0]); i++)\n')
        f.write('        if (!strcmp(ACC[i].slug, slug)) return ACC[i].hex;\n')
        f.write('    return 0xd9a24b;\n}\n')
    with open(os.path.join(hdir, 'hybrid_accents.h'), 'w') as f:
        f.write('#ifndef HYBRID_ACCENTS_H\n#define HYBRID_ACCENTS_H\n#include <stdint.h>\n\n')
        f.write('#ifdef __cplusplus\nextern "C" {\n#endif\n\nuint32_t hy_amp_accent_hex(const char *slug);\n')
        f.write('#ifdef __cplusplus\n}\n#endif\n#endif\n')
    print('accents   :  %3d     (main/hybrid/hybrid_accents.c)' % len(accents))
    if faltando:
        print(f'\n!! {len(faltando)} origem(ns) ausente(s):')
        for f in faltando[:10]:
            print('   ', f)


if __name__ == '__main__':
    main()
