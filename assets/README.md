# The 1.1 update's data

What the free English 1.1 update for *Contract J.A.C.K.*
(`contractjack_update_en_10_11.exe`, SHA-256
`7511a4a731a5d4eae31c669522c2a97ce1c73c5f72ef832f3db77cb51ef992b6`, as it
was published for download; its readme is dated October 30, 2003) adds to
the game, other than code. It ships with this project's releases, so
installing needs only the player's two discs.

- `ext/rez/`: the contents of the update's `Update_v1x1.rez` (SHA-256
  `d8fb90293de3d21b2a03fa373b70b4a79129c6198429fadbb2cdac6bf7bc4280`), as
  its archive has them, less the three game binaries (`CSHELL.DLL`,
  `OBJECT.LTO`, `CRES.DLL`), which this project builds from source: 4
  files -- the attribute files `FX.TXT` and `LAYOUT.TXT`, and the
  multiplayer menu's GameSpy button (`GAMESPYPB.SPR` and its texture).
  `build/all.sh` packs them into the game directory's `cjacksrc.rez` with
  lithtools' `lithrez` (`build/lithrez.sh`), with a fixed time
  (1067472000, the 1.1 readme's date) so that every build makes the same
  bytes. The game loads it where the update's installer put its archive:
  after the discs' archives, before `custom/` (`build/cjackcmds.sh`).

The rest of the update -- its engine, launcher, dedicated server and
GameSpy library, and its two readmes -- is either built from source here
or not needed; the settings and default profile the game starts from come
from the discs, which the update does not change. Taken out of ext/rez/'s copy
of the update (`Update_v1x1.rez` of the `contract-jack-eu-1.1` install,
`ext/rez/README.md`); the files are unchanged (`.gitattributes` keeps git from
converting their line endings). They are Monolith's and the publisher's,
as the rest of the game's data is (`docs/LICENSE_NOTES.md`).
