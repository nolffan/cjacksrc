<!--
The release text (.github/workflows/public-archives.yml, the job "release"):
{{TAG}}, {{ARCHIVE}} and {{REPO}} are filled in, and this comment is left out.
-->
## Download

| File | What it is |
|---|---|
| `cjacksrc-{{ARCHIVE}}.tar.gz` | **This is what you want**: Contract J.A.C.K. (1.1) built from source, the modern build. |
| `cjacksrc-debug-{{ARCHIVE}}.tar.gz` | The same build with debug information. For bug reporting. |

**Work in progress.** The reconstruction is not finished: see [where it stands](https://github.com/{{REPO}}/blob/{{TAG}}/README.md#where-it-stands) before expecting a game that plays from start to end.

**No game data is included.** A platform specific installer is included that copies the game's data from your own images of the two discs of the European or the USA release, and from the English 1.1 update, `contractjack_update_en_10_11.exe` (read, not run).

Supported images, as redump names them (a plain `.iso`, redump's `.bin`/`.cue`, or redump's `.zip`):

- Europe: `Contract J.A.C.K. (Europe) (Disc 1)`, `Contract J.A.C.K. (Europe) (Disc 2)`
- USA: `Contract J.A.C.K. (USA) (Disc 1)`, `Contract J.A.C.K. (USA) (Disc 2)`

## Install

**This is the whole game, not a patch.** Unpack it into a new, empty directory, not over an earlier release or another installation. Copy your old `Save` and `Profiles` folders across to keep your saved games and profiles.

### Windows

Unpack the archive, drop both disc images and `contractjack_update_en_10_11.exe` onto `windows-install.bat`, then start `ContractJack.exe`.

### Linux

There's no native Linux build; the game is a Windows program, run under Wine or Proton.

```sh
tar xzf cjacksrc-{{ARCHIVE}}.tar.gz
cd cjacksrc-{{ARCHIVE}}
./linux-install.sh /path/to/disc1.zip /path/to/disc2.zip /path/to/contractjack_update_en_10_11.exe
./linux-run.sh
```

`./linux-run.sh --help` lists its options (a Wine prefix of its own, `~/.wine-cjack`; a window or full screen); `./linux-server.sh` runs a dedicated server, headless under `xvfb-run` when there is no display.

### Mac

The game runs in CrossOver (or another Wine for macOS). Unpack the archive, put both disc images and `contractjack_update_en_10_11.exe` in its folder and double-click `mac-install.command`, then run `ContractJack.exe` in a CrossOver bottle with Run Command. If macOS will not open the script (it is downloaded and not signed), allow it in System Settings > Privacy & Security ("Open Anyway"), or run it in Terminal: `./mac-install.command disc1.zip disc2.zip contractjack_update_en_10_11.exe`.

## What the modern build changes

Every change from the retail game, and what is planned, is in [`MODERN.md`](https://github.com/{{REPO}}/blob/{{TAG}}/MODERN.md); the retail game's own bugs it fixes are in [`RETAIL_BUGS.md`](https://github.com/{{REPO}}/blob/{{TAG}}/RETAIL_BUGS.md).

## Testing

The game is a 32-bit Windows program. It is tested on Linux with Wine; Windows and the Mac have had less testing (the Mac installer has been run on Linux, not yet on a Mac). Please report problems in the [issues](https://github.com/{{REPO}}/issues).
