---
last_mapped_commit: 97fba1f2e8fc0676f4f28f5e659f174a01754612
last_mapped_at: 2026-10-07
---
# Codebase Structure

**Analysis Date:** 2026-10-07

## Directory Layout

```text
Vyxxen/
├── Vyxxen.uproject              # Unreal project descriptor
├── Source/
│   ├── Vyxxen.Target.cs         # Game target
│   ├── VyxxenEditor.Target.cs   # Editor target
│   └── Vyxxen/
│       ├── Vyxxen.Build.cs      # Module rules
│       ├── Vyxxen.cpp           # Module registration
│       ├── Vyxxen.h             # Core include header
│       ├── Public/PlayerShip.h
│       └── Private/PlayerShip.cpp
├── Config/                     # Default engine, input, game, editor settings
├── Content/
│   ├── Levels/MainGame.umap
│   ├── Scenery/
│   │   ├── BP_level1_scenery.uasset
│   │   ├── ground-tile/         # Tile and surface packages
│   │   └── power-lines/         # Power-line segment and surface packages
│   ├── Units/PlayerShip/
│   │   ├── BP_vyxxen-ship.uasset
│   │   ├── vyxxen-ship.uasset
│   │   ├── reference-block.uasset
│   │   └── Textures/
│   ├── Collections/             # Empty inventory
│   └── Developers/Breno/Collections/ # Empty inventory
├── .planning/codebase/          # Codebase reference documents
├── Binaries/                   # Local compiled output
├── Intermediate/               # Generated build/reflection data
├── DerivedDataCache/           # Generated asset cache
├── Saved/                      # Editor/runtime local output
├── .vs/ and .idea/              # IDE working data
└── README.md                   # One-line project purpose
```

The authoritative root is `C:/Users/Breno/Documents/Unreal Projects/Vyxxen`. This inventory includes local `Source/` and asset directories, not just Git-tracked files. Binary package contents and editor wiring in `Content/` are unverified.

## Directory Purposes

**`Source/`:**
- Purpose: UnrealBuildTool targets and native runtime module.
- Contains: C# build rules and C++ headers/implementations.
- Key files: `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs`, `Source/Vyxxen/Vyxxen.Build.cs`.

**`Source/Vyxxen/Public/` and `Source/Vyxxen/Private/`:**
- Purpose: separate class declarations from implementation.
- Contains: the only project actor declaration/implementation.
- Key files: `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`.

**`Config/`:**
- Purpose: project defaults consumed by Unreal.
- Contains: startup/render settings, input classes and axis defaults, project metadata, editor preview profiles.
- Key files: `Config/DefaultEngine.ini`, `Config/DefaultInput.ini`, `Config/DefaultGame.ini`, `Config/DefaultEditor.ini`.
- Handle `Config/DefaultEngine.ini` selectively: it also contains a credential-like configuration setting; do not reproduce sensitive values in documentation or logs.

**`Content/Levels/`:**
- Purpose: map packages.
- Contains: the configured startup map `Content/Levels/MainGame.umap`.
- `Config/DefaultEngine.ini` uses the corresponding Unreal object path `/Game/Levels/MainGame.MainGame` for both startup settings.

**`Content/Scenery/`:**
- Purpose: environmental content, grouped by prop family.
- Key files: `Content/Scenery/BP_level1_scenery.uasset`, `Content/Scenery/ground-tile/ground-tile.uasset`, `Content/Scenery/ground-tile/M_ground-tile.uasset`.
- `Content/Scenery/ground-tile/` includes diffuse, normal, and ORM-named surface packages.
- `Content/Scenery/power-lines/` includes connector, terminator, vertical, straight, paired, and curved segment names plus `PL_Graphite.uasset`, `PL_Silver.uasset`, `PL_WarmMetal.uasset`, and `PL_PowerWhite.uasset`.
- These are filename-based organizational descriptions; package classes and material/mesh links are unverified.

**`Content/Units/PlayerShip/`:**
- Purpose: one unit's asset family.
- Key files: `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`, `Content/Units/PlayerShip/vyxxen-ship.uasset`, `Content/Units/PlayerShip/reference-block.uasset`.
- `Content/Units/PlayerShip/Textures/` contains `playership_diffuse.uasset`, `playership_normal.uasset`, `playership_occlusion.uasset`, and `vyxxen-ship_mat.uasset`; do not infer exact asset types from the folder alone.

## Key File Locations

**Entry Points:**
- `Vyxxen.uproject`: engine association, module loading, enabled modeling editor plugin.
- `Source/Vyxxen/Vyxxen.cpp`: primary game module registration.
- `Content/Levels/MainGame.umap`: configured editor/game startup map; scene execution unverified.

**Configuration:**
- `Source/Vyxxen/Vyxxen.Build.cs`: runtime module dependencies.
- `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs`: game/editor build target types.
- `Config/DefaultEngine.ini`: map and rendering/platform defaults.
- `Config/DefaultInput.ini`: Enhanced Input class defaults and engine input settings.
- `.gitignore`: excludes build products, cache, saved data, and generated solutions.

**Core Logic:**
- `Source/Vyxxen/Public/PlayerShip.h`: reflected `AActor` subclass, with no custom state.
- `Source/Vyxxen/Private/PlayerShip.cpp`: constructor and empty BeginPlay/Tick overrides.
- `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`: Blueprint-style asset name; graph, parent type, and functional responsibilities unverified.

**Testing:**
- No test source, test directory, or custom test target is detected in `Source/`.
- No test map is detected in `Content/Levels/`; `Content/Levels/MainGame.umap` is the only inventoried map.

## Naming Conventions

**Files:**
- Use matching PascalCase `.h`/`.cpp` stems for native classes: `Source/Vyxxen/Public/PlayerShip.h` and `Source/Vyxxen/Private/PlayerShip.cpp`.
- Follow module/target suffixes: `Source/Vyxxen/Vyxxen.Build.cs`, `Source/VyxxenEditor.Target.cs`.
- Asset naming is mixed: Blueprint-style `BP_` at `Content/Scenery/BP_level1_scenery.uasset`, material-style `M_` at `Content/Scenery/ground-tile/M_ground-tile.uasset`, and hyphenated import names at `Content/Units/PlayerShip/vyxxen-ship.uasset`. Preserve existing references when renaming through Unreal.

**Directories:**
- Use Unreal's conventional `Source/<Module>/Public/` and `Source/<Module>/Private/` split, as in `Source/Vyxxen/`.
- Group unit assets by unit under `Content/Units/`, and scenery by family under `Content/Scenery/`.

## Where to Add New Code

**New Feature:**
- Primary native code: extend the existing `Source/Vyxxen/` module using public declarations and private implementations; declare additional module dependencies in `Source/Vyxxen/Vyxxen.Build.cs` when required.
- Asset implementation: use `Content/Units/PlayerShip/` for ship content, `Content/Scenery/` for environment content, and `Content/Levels/` for maps.
- Tests: no established location exists in `Source/`; choose a deliberate test layout when introducing a runner rather than copying an absent convention.

**New Component/Module:**
- For an ordinary gameplay class, follow `Source/Vyxxen/Public/PlayerShip.h` and `Source/Vyxxen/Private/PlayerShip.cpp`.
- No second runtime module or project plugin exists in this inventory; creating one requires explicit descriptor/build integration in `Vyxxen.uproject` and `Source/`.

**Utilities:**
- No utility directory is established under `Source/Vyxxen/`. Keep private helpers with their owning implementation; introduce a shared interface under `Source/Vyxxen/Public/` only when required by consumers.

## Special Directories

**`Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`:**
- Purpose: generated compiled modules, build intermediates, cache, and editor/runtime output.
- Generated: Yes. Committed: excluded by `.gitignore`.
- Do not place authored gameplay implementation in these directories; use `Source/` and `Content/`.

**`.vs/`, `.idea/`, root solution files:**
- Purpose: IDE/project workspace data, including `Vyxxen.sln`, `Vyxxen.slnx`, `Automation_Vyxxen.sln`, and `Automation_Vyxxen.slnx`.
- Generated/local: Yes for solution/workspace data. `.gitignore` explicitly excludes `.vs/` and `*.sln`; do not assume `.idea/` or `*.slnx` is excluded by the same rule.
- Tracked `.idea/` or `.slnx` entries were not detected in the Git inventory.

**`Source/`, `Content/Scenery/`, `Content/Units/`:**
- Purpose: local authored source and binary asset packages.
- Generated: Source contains Unreal-style class boilerplate but is editable project source; content packages are editor/import output and must be treated as authored assets.
- Committed: no tracked entries detected in these subtrees at analysis time. Preserve them and include them in future change/reproducibility reviews.

**`Content/Levels/`, `Config/`:**
- Purpose: main level and project defaults.
- Generated: editor-authored/serialized configuration and assets.
- Committed: `Content/Levels/MainGame.umap` and the four default INI files are tracked.

**`.planning/codebase/`:**
- Purpose: generated architecture and other analysis references.
- Generated: Yes. Commit handling belongs to the orchestrating workflow; these mapper documents do not change game runtime behavior.

---

*Structure analysis: 2026-10-07*
