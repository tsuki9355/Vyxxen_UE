---
last_mapped_commit: 97fba1f2e8fc0676f4f28f5e659f174a01754612
last_mapped_at: 2026-10-07
---
<!-- refreshed: 2026-10-07 -->

# Architecture

**Analysis Date:** 2026-10-07

## System Overview

```text
Unreal project descriptor: Vyxxen.uproject
  |-- Game target: Source/Vyxxen.Target.cs
  |-- Editor target: Source/VyxxenEditor.Target.cs
  v
Single runtime module: Source/Vyxxen/Vyxxen.cpp
  |-- Actor declaration: Source/Vyxxen/Public/PlayerShip.h
  |-- Actor lifecycle: Source/Vyxxen/Private/PlayerShip.cpp

Config/DefaultEngine.ini --> Content/Levels/MainGame.umap
Content/Units/PlayerShip/ + Content/Scenery/ --> available assets
  (map-to-asset references and Blueprint inheritance unverified)
```

- Scope is the local Unreal repository, including untracked `Source/` and assets in `Content/Scenery/` and `Content/Units/`; tracked files alone do not represent the working project.
- `README.md` describes a Zaxxon-like game. Native gameplay implementation in `Source/Vyxxen/Private/PlayerShip.cpp` is a scaffold; playable mechanics cannot be inferred from that description.
- `Content/Levels/MainGame.umap` and `.uasset` files are binary packages. Their internal graphs, actor instances, asset references, and editor wiring are unverified. No editor session, build, or runtime test establishes those relationships in this analysis.

## Component Responsibilities

| Component | Responsibility | File |
|-----------|----------------|------|
| Project descriptor | Associates engine 5.8, declares runtime module, enables editor modeling plugin | `Vyxxen.uproject` |
| Build targets | Select game versus editor target; include Vyxxen module | `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs` |
| Module rules | Declare Core, CoreUObject, Engine, and InputCore dependencies | `Source/Vyxxen/Vyxxen.Build.cs` |
| Module entry | Register default primary game module | `Source/Vyxxen/Vyxxen.cpp` |
| PlayerShip | Declare AActor subclass and empty lifecycle hooks | `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp` |
| Startup configuration | Select MainGame for game and editor startup | `Config/DefaultEngine.ini` |
| Input configuration | Select Enhanced Input engine classes and axis defaults | `Config/DefaultInput.ini` |
| Level package | Configured startup level; contents unverified | `Content/Levels/MainGame.umap` |
| Ship and scenery packages | Hold named ship, terrain, power-line, material, and texture assets; package types inferred only from filenames | `Content/Units/PlayerShip/`, `Content/Scenery/` |

## Pattern Overview

**Overall:** Single Unreal runtime module plus editor-authored content (`Vyxxen.uproject`, `Source/Vyxxen/`, `Content/`).

**Key Characteristics:**
- Follow Unreal reflection and lifecycle conventions: `UCLASS`, `GENERATED_BODY`, exported `VYXXEN_API`, and engine overrides in `Source/Vyxxen/Public/PlayerShip.h`.
- Keep class interfaces in `Source/Vyxxen/Public/` and implementations in `Source/Vyxxen/Private/`.
- Treat `Config/` as project defaults and `Content/` as binary authoring data, rather than searching generated `Intermediate/` for authoritative gameplay code.
- No native GameMode, Pawn, PlayerController, subsystem, save system, service layer, or networking implementation is detected in `Source/`.

## Layers

**Project and build configuration:**
- Purpose: engine discovery, target selection, module dependencies.
- Location: `Vyxxen.uproject`, `Source/*.Target.cs`, `Source/Vyxxen/Vyxxen.Build.cs`.
- Depends on UnrealBuildTool and engine modules; used by Unreal builds and editor loading.

**Native gameplay:**
- Purpose: reflected actor type and lifecycle extension points.
- Location: `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`.
- Contains only `APlayerShip`; depends on `AActor` and CoreMinimal. No component construction, input binding, movement, combat, or persistent fields are defined.
- Whether a level or Blueprint uses this class is unverified in `Content/Levels/MainGame.umap` and `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`.

**Content and presentation:**
- Purpose: level, ship, and environment authoring.
- Location: `Content/Levels/`, `Content/Units/PlayerShip/`, `Content/Scenery/`.
- Contains binary packages with Blueprint-style names such as `Content/Scenery/BP_level1_scenery.uasset`; dependencies and execution behavior require Unreal editor inspection.

## Data Flow

### Primary Request Path

This is a configured startup path, not a runtime-tested request pipeline.

1. Engine discovers the default-loading runtime module in `Vyxxen.uproject`.
2. Module registration uses `FDefaultGameModuleImpl` (`Source/Vyxxen/Vyxxen.cpp:6`).
3. Both startup map settings select `/Game/Levels/MainGame.MainGame` (`Config/DefaultEngine.ini:4`, `Config/DefaultEngine.ini:5`), corresponding to `Content/Levels/MainGame.umap`.
4. Map actor creation, GameMode selection, possession, camera setup, and Blueprint execution remain unverified inside `Content/Levels/MainGame.umap`.

### Native Actor Lifecycle

1. If instantiated, `APlayerShip` enables tick in its constructor (`Source/Vyxxen/Private/PlayerShip.cpp:7`).
2. `BeginPlay` delegates to its superclass without adding behavior (`Source/Vyxxen/Private/PlayerShip.cpp:15`).
3. `Tick` delegates to its superclass without adding behavior (`Source/Vyxxen/Private/PlayerShip.cpp:22`).

**State Management:**
- No custom member state or reflected properties exist in `Source/Vyxxen/Public/PlayerShip.h`.
- Input engine classes are configured in `Config/DefaultInput.ini:79`; no native bindings or input asset packages are detected in `Source/` or the `Content/` inventory.
- Blueprint state and level-owned state are unverified in `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Levels/MainGame.umap`.

## Key Abstractions

**APlayerShip:**
- Purpose: named ship actor extension point, not a verified controllable Pawn.
- Examples: `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`.
- Pattern: `AActor` inheritance with public constructor/Tick and protected BeginPlay; retain superclass calls when extending lifecycle hooks.

**Vyxxen module:**
- Purpose: one game module shared by game and editor targets.
- Examples: `Source/Vyxxen/Vyxxen.cpp`, `Source/Vyxxen/Vyxxen.Build.cs`.
- Pattern: default engine module implementation; no custom startup/shutdown code.

## Entry Points

**Editor/game launch:** `Vyxxen.uproject` selects the engine/module; `Config/DefaultEngine.ini` selects `Content/Levels/MainGame.umap`.

**Native module load:** `Source/Vyxxen/Vyxxen.cpp` registers the primary module when the engine loads it.

**Actor callbacks:** `Source/Vyxxen/Private/PlayerShip.cpp` receives construction, BeginPlay, and Tick only if this actor is instantiated; placement/spawning is unverified.

## Architectural Constraints

- **Threading:** No custom task/thread code exists in `Source/Vyxxen/`; do not infer worker execution from the presence of Tick in `Source/Vyxxen/Private/PlayerShip.cpp`.
- **Global state:** No custom singleton or shared mutable global is defined in `Source/Vyxxen/Vyxxen.cpp` or `Source/Vyxxen/Private/PlayerShip.cpp`.
- **Circular imports:** No local cycle is present in the small native include graph (`Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`, `Source/Vyxxen/Vyxxen.cpp`).
- **Input boundary:** `Config/DefaultInput.ini` names Enhanced Input classes, while `Source/Vyxxen/Vyxxen.Build.cs` has InputCore but no EnhancedInput dependency. Check/add the module dependency when introducing native Enhanced Input APIs.
- **Asset boundary:** Never assume `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` inherits `APlayerShip` from matching names. Verify its parent and level references in the editor first.
- **Project skills:** No project-local `.codex/skills/` or `.agents/skills/` directory and no `.planning/config.json` are present in this root; no project skill constraints were detected.

## Anti-Patterns

### Empty per-frame override

**What happens:** `Source/Vyxxen/Private/PlayerShip.cpp` enables tick while the override only calls `Super::Tick`.
**Why it's wrong:** It adds an unnecessary callback for native-only instances without establishing gameplay behavior; Blueprint descendants could still depend on tick and are unverified.
**Do this instead:** Before disabling tick in `Source/Vyxxen/Private/PlayerShip.cpp`, inspect descendants and their event graphs in the editor; otherwise attach the intended per-frame behavior to a deliberate tick requirement.

### Equating input configuration with control implementation

**What happens:** `Config/DefaultInput.ini` selects Enhanced Input engine classes, while `Source/Vyxxen/Public/PlayerShip.h` declares an Actor with no input setup.
**Why it's wrong:** The setting alone does not establish possession, mapping contexts, actions, or ship controls.
**Do this instead:** Verify control ownership and Blueprint mappings in `Content/Levels/MainGame.umap` and `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` before designing native controls around `Source/Vyxxen/Private/PlayerShip.cpp`.

## Error Handling

**Strategy:** No project-specific error handling exists in the native scaffold (`Source/Vyxxen/Private/PlayerShip.cpp`, `Source/Vyxxen/Vyxxen.cpp`).

**Patterns:** Lifecycle callbacks simply delegate to engine methods in `Source/Vyxxen/Private/PlayerShip.cpp`; no fallible project operations, validation branches, or recovery paths are implemented.

## Cross-Cutting Concerns

**Logging:** No custom log category or logging calls are defined in `Source/Vyxxen/`.
**Validation:** No project validation code is defined in `Source/Vyxxen/`; binary asset validity is unverified in `Content/`.
**Authentication:** Not detected in `Source/Vyxxen/`; online subsystem entries in `Source/Vyxxen/Vyxxen.Build.cs` are comments, not enabled dependencies.

---

*Architecture analysis: 2026-10-07*
