# What is whose

This repository mixes code from different origins under different terms.
This file says which is which; it is not legal advice and grants nothing.
Sections are added as each package brings code of a new origin in.

## GameSpy SDK (`src/Game/Libs/GameSpy/GameSpySDK/`)

`GameSpy.dll` is Monolith's `Game\Libs\GameSpy` wrapper around GameSpy's C
SDK (server browsing, query reporting 2, NAT negotiation, HTTP, patching
and tracking, CD key), which the retail image names by its file paths
(`GameSpySDK\serverbrowsing\sb_serverlist.c`, ...).

| What | Origin | Terms |
|---|---|---|
| `GameSpySDK/` | **GameSpy Industries / IGN Entertainment**: the GameSpy SDK's C source as IGN released it in 2011 under a BSD licence, in the snapshot mirrored at github.com/nitrocaster/GameSpy (commit `d1deb2d1a951`; serverbrowsing 2.07.00, December 2007). Only the modules `GameSpy.dll` links are kept, each with its own `changelog.txt`; they are rolled back toward the July-October 2003 versions the retail image was built from (guided by the changelogs and the retail code), and the 2007 tree's `common/` platform layer is folded back into a 2003-style `nonport.h`/`nonport.c`, written for this project | the three-clause BSD licence in `GameSpySDK/LICENSE.txt` (copyright IGN Entertainment, 2011): keep the notice with the source and reproduce it with binaries |
| `src/Game/Libs/GameSpy/*.cpp`, `*.h` | **Monolith Productions**' wrapper, written for this project: no source of it was ever released; it is reconstructed from the retail `GameSpy.dll` (1.1) so that it compiles back to it | written for this project; no licence is declared |
