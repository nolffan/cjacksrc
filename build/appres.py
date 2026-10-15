#!/usr/bin/env python3
"""The launcher's and the dedicated server's art, from the retail game.

  build/appres.py <retail game directory> <output directory>

Contract J.A.C.K.'s launcher (ContractJack.exe) and dedicated server
(ContractJackServer.exe) carry their art in their resources: the launcher's
backgrounds and buttons (76 bitmaps), its sounds (8 waves), its icon and
cursor; the server's splash screen and two icons. None of it was ever
released (NOLF2's source release carries NOLF2's), so, as the game's data,
it is taken from the retail game when the game is built, never kept in this
repository: this writes each one back out of the retail image as the file
the resource compiler reads -- a bitmap with its BITMAPFILEHEADER, a wave as
it is, an icon or cursor group as an .ico or .cur file -- into
<output>/Launcher/ and <output>/ServerApp/, where the projects' resource
scripts find them (their /i paths). Compiled again, each resource is the
retail one byte for byte.

build/compile.sh runs it into src/Game/built/retail/ before the compile; it
writes only what is missing or differs, so a rebuild recompiles nothing.
"""
import os
import struct
import sys

RT_CURSOR, RT_BITMAP, RT_ICON, RT_GROUP_CURSOR, RT_GROUP_ICON = 1, 2, 3, 12, 14

# Each image, and the file name its resource script gives each resource of
# it (named resources are written under their own name). The names are
# NOLF2's source release's for the same resources.
IMAGES = {
    "ContractJack.exe": ("Launcher", {
        (RT_BITMAP, 130): "MainAppBackground.bmp",
        (RT_BITMAP, 164): "OptionsBackground.bmp",
        (RT_BITMAP, 219): "DisplayBackground.bmp",
        (RT_BITMAP, 283): "BoxBackground.bmp",
        (RT_BITMAP, 284): "DetailSettingsBackground.bmp",
        (RT_BITMAP, 296): "Information.bmp",
        (RT_BITMAP, 297): "Error.bmp",
        (RT_BITMAP, 298): "Warning.bmp",
        (RT_BITMAP, 323): "CustomBackground.bmp",
        ("WAVE", 225): "Select.wav",
        ("WAVE", 226): "Click.wav",
        ("WAVE", 278): "buttondown.wav",
        ("WAVE", 279): "type1.wav",
        ("WAVE", 280): "type2.wav",
        ("WAVE", 281): "type3.wav",
        ("WAVE", 282): "TypeBack.wav",
        ("WAVE", 294): "Intro.wav",
        (RT_GROUP_ICON, 128): "Launcher.ico",
        (RT_GROUP_CURSOR, 308): "WebHand.cur",
    }),
    "ContractJackServer.exe": ("ServerApp", {
        (RT_BITMAP, 535): "splash.bmp",
        (RT_GROUP_ICON, 524): "ServerApp.ico",
        (RT_GROUP_ICON, 526): "selected.ico",
    }),
}


def resources(path):
    """{(type, name): data} for the first language of every resource; a
    type or name is an int or an upper-case string, as the image has it."""
    d = open(path, "rb").read()
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    optsz = struct.unpack_from("<H", d, pe + 20)[0]
    for i in range(nsec):
        s = pe + 24 + optsz + i * 40
        if d[s:s + 8].rstrip(b"\0") == b".rsrc":
            va, rp = struct.unpack_from("<II", d, s + 12)[0], struct.unpack_from("<I", d, s + 20)[0]
            break
    else:
        sys.exit(f"appres: {path} has no resources")

    def entries(off):
        nn, ni = struct.unpack_from("<HH", d, rp + off + 12)
        for i in range(nn + ni):
            name, data = struct.unpack_from("<II", d, rp + off + 16 + i * 8)
            if name & 0x80000000:
                o = rp + (name & 0x7FFFFFFF)
                n = struct.unpack_from("<H", d, o)[0]
                name = d[o + 2:o + 2 + n * 2].decode("utf-16le")
            yield name, data

    out = {}
    for t, td in entries(0):
        for n, nd in entries(td & 0x7FFFFFFF):
            _lang, ld = next(entries(nd & 0x7FFFFFFF))
            drva, size = struct.unpack_from("<II", d, rp + ld)
            out[(t, n)] = d[rp + drva - va:rp + drva - va + size]
    return out


def bitmap_file(dib):
    """A .bmp: the BITMAPFILEHEADER the resource compiler strips again."""
    hsize, _w, _h, _planes, bits, _comp, _isize, _xr, _yr, used = struct.unpack_from("<IiiHHIIiiI", dib)
    colours = used or (1 << bits if bits <= 8 else 0)
    return b"BM" + struct.pack("<IHHI", 14 + len(dib), 0, 0, 14 + hsize + 4 * colours) + dib


def group_file(res, group, kind):
    """An .ico (kind 1) or .cur (kind 2) of a group's images, in the group's
    order. A cursor's image starts with its hot spot, which the file keeps
    in the directory entry instead."""
    _r, _t, n = struct.unpack_from("<HHH", group)
    images, dirents = [], []
    off = 6 + 16 * n
    for i in range(n):
        e = group[6 + 14 * i:6 + 14 * (i + 1)]
        if kind == 1:
            w, h, colours, rsv, planes, bits, size, rid = struct.unpack("<BBBBHHIH", e)
            img = res[(RT_ICON, rid)]
            dirents.append(struct.pack("<BBBBHHII", w, h, colours, rsv, planes, bits, len(img), off))
        else:
            w, h2, planes, bits, size, rid = struct.unpack("<HHHHIH", e)
            raw = res[(RT_CURSOR, rid)]
            hx, hy = struct.unpack_from("<HH", raw)
            img = raw[4:]
            dirents.append(struct.pack("<BBBBHHII", w & 0xFF, (h2 // 2) & 0xFF, 0, 0, hx, hy, len(img), off))
        images.append(img)
        off += len(img)
    return struct.pack("<HHH", 0, kind, n) + b"".join(dirents) + b"".join(images)


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    game, outdir = sys.argv[1:]
    written = 0
    for image, (sub, names) in IMAGES.items():
        path = os.path.join(game, image)
        if not os.path.isfile(path):
            sys.exit(f"appres: {path} not found (the retail game, build/retail.sh)")
        res = resources(path)
        files = {}
        for (t, n), data in res.items():
            if t == RT_BITMAP:
                files[names.get((t, n)) or f"{n}.bmp"] = bitmap_file(data)
            elif (t, n) in names and t == "WAVE":
                files[names[(t, n)]] = data
            elif t in (RT_GROUP_ICON, RT_GROUP_CURSOR):
                files[names[(t, n)]] = group_file(res, data, 1 if t == RT_GROUP_ICON else 2)
        missing = [v for (t, n), v in names.items() if v not in files]
        if missing:
            sys.exit(f"appres: {image} lacks {', '.join(missing)}")
        d = os.path.join(outdir, sub)
        os.makedirs(d, exist_ok=True)
        for name, data in sorted(files.items()):
            p = os.path.join(d, name)
            if os.path.isfile(p) and open(p, "rb").read() == data:
                continue
            with open(p, "wb") as f:
                f.write(data)
            written += 1
    if written:
        print(f"   appres: {written} file(s) of the launcher's and the server's art from {game}")


if __name__ == "__main__":
    main()
