<!-- GSD:project-start source:PROJECT.md -->

## Project

**Vyxxen**

Vyxxen is a fully 3D Zaxxon-inspired arcade game in Unreal Engine for the Vertex School AI Bootcamp. The first implementation cut delivers ship flight controls and a synchronized route camera, playable in Unreal Editor. The longer-term first-fortress design includes combat, hazards, fuel airpods, rewards, and stage progression; those systems are deferred beyond this cut.

**Core Value:** The player can steer and climb a continuously advancing ship while a stable camera moves with exactly the same forward progress.

### Constraints

- **Milestone scope:** Flight controls and X-only route-camera movement — keep later fortress systems out of this first cut.
- **Scene basis:** Nose/forward is +X, lateral offset is Y, altitude is Z; scene axes do not define raw-key bindings.
- **Editable tuning:** Movement speeds and minimum/maximum height must be editable on the ship class or actor component. A separate data-asset or settings system is unnecessary for this cut.
- **Input:** Use Enhanced Input actions and a Mapping Context; no raw key names in native gameplay logic. Independent steer and climb inputs are not normalized together.
- **Slowdown:** Full slowdown reduces forward and lateral speed by 40%, with ramp timing editable. It does not reduce climb speed.
- **Camera:** One forward position feeds the ship and camera; avoid separate accumulated movement that can drift or double-scroll. The camera does not follow lateral movement, altitude, pitch, or bank.
- **World:** Fortress scenery stays where authored. Move the ship and camera rather than the terrain.
- **Implementation:** Follow Unreal actor/component conventions; reuse and inspect existing assets before changing them. The design favors a pawn plus movement component and a placed camera plus route-following component.
- **Verification:** Successful editor build/load and observed Play In Editor behavior establish completion. Generated binaries or asset filenames alone do not.
- **AI configuration:** Use `gpt-6-luna` with `xhigh` reasoning for all work and GSD agents, as explicitly requested by the user.

<!-- GSD:project-end -->

<!-- GSD:stack-start source:codebase/STACK.md -->

## Technology Stack

## Languages

- Unreal C++ gameplay module in `Source/Vyxxen/`; `Source/Vyxxen/Public/PlayerShip.h` declares a reflected `AActor`, and `Source/Vyxxen/Private/PlayerShip.cpp` implements constructor, BeginPlay, and Tick scaffolding.
- Blueprint assets are present at `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Scenery/BP_level1_scenery.uasset`. Binary graph contents, inheritance, and executable behavior are unverified.
- C++ language standard is not explicitly pinned in `Source/Vyxxen/Vyxxen.Build.cs` or either `Source/*.Target.cs`; use the selected engine/toolchain defaults until verified.
- C# UnrealBuildTool declarations in `Source/Vyxxen/Vyxxen.Build.cs`, `Source/Vyxxen.Target.cs`, and `Source/VyxxenEditor.Target.cs`; no standalone C# application is declared.
- Unreal INI configuration in `Config/DefaultEngine.ini`, `Config/DefaultGame.ini`, `Config/DefaultInput.ini`, and `Config/DefaultEditor.ini`.

## Runtime

- Unreal Engine association `5.8` in `Vyxxen.uproject`; exact installed engine revision is not established by that association.
- One runtime module, `Vyxxen`, loads at `Default` phase in `Vyxxen.uproject`; registration uses `IMPLEMENT_PRIMARY_GAME_MODULE` in `Source/Vyxxen/Vyxxen.cpp`.
- Local Win64 editor module artifacts exist in `Binaries/Win64/`; presence does not prove they match current source or run successfully.
- UnrealBuildTool resolves engine module dependencies through `Source/Vyxxen/Vyxxen.Build.cs`; no application package manager or lockfile is detected in the repository root.
- Engine/plugin association is declared in `Vyxxen.uproject`, without an independent dependency lockfile.

## Frameworks

- Unreal Engine 5.8 association: actors, reflection, rendering, assets, and editor integration (`Vyxxen.uproject`, `Source/Vyxxen/Public/PlayerShip.h`).
- Enhanced Input classes are configured as default player input and input component in `Config/DefaultInput.ini`. `EnhancedInput` is not an explicit native dependency in `Source/Vyxxen/Vyxxen.Build.cs`.
- CommonUI settings exist in `Config/DefaultEngine.ini`; explicit plugin enablement and native usage are not demonstrated by `Vyxxen.uproject` or `Source/Vyxxen/Vyxxen.Build.cs`.
- No project-authored test framework, automation tests, or test target detected in `Source/`; `Source/Vyxxen.Target.cs` and `Source/VyxxenEditor.Target.cs` define Game and Editor targets only.
- Engine test infrastructure availability is not evidence of project tests; binary content in `Content/` remains uninspected.
- UnrealBuildTool targets select `BuildSettingsVersion.V7` in `Source/Vyxxen.Target.cs` and `Source/VyxxenEditor.Target.cs`.
- Explicit/shared precompiled headers are selected in `Source/Vyxxen/Vyxxen.Build.cs`.
- `.vsconfig` requests Visual Studio native game/desktop workloads, Unreal IDE/debugger components, MSVC 14.50.18.0 components, LLVM Clang, Windows SDK 22621, and .NET Framework 4.6.2 targeting pack. These are declared component requirements, not an installation inventory.
- Generated IDE solutions exist at `Vyxxen.sln`, `Vyxxen.slnx`, `Automation_Vyxxen.sln`, and `Automation_Vyxxen.slnx`; use `Source/` declarations as build configuration authority.

## Key Dependencies

- `Core`, `CoreUObject`, `Engine`, and `InputCore` are public native module dependencies in `Source/Vyxxen/Vyxxen.Build.cs`; versions follow the selected Unreal engine.
- `Engine` is also listed under the runtime module's additional dependencies in `Vyxxen.uproject`.
- `Content/Levels/MainGame.umap` is the configured startup/game map in `Config/DefaultEngine.ini`; world contents and map-level class overrides are unverified.
- `ModelingToolsEditorMode` is explicitly enabled for Editor targets only in `Vyxxen.uproject`.
- `Slate`, `SlateCore`, `OnlineSubsystem`, and Steam are mentioned only in comments in `Source/Vyxxen/Vyxxen.Build.cs`; do not treat them as active declared dependencies.
- No project `Plugins/` directory or separate third-party SDK manifest is detected alongside `Vyxxen.uproject`.

## Configuration

- Configure project behavior through `Config/Default*.ini` and engine/plugin selection through `Vyxxen.uproject`; no application environment-variable contract is detected in `Source/`.
- `Config/DefaultEngine.ini` selects desktop/maximum graphics, DX12 with SM6 on Windows, Vulkan SM6 for Linux, and Metal SM6 for Mac. These settings do not verify builds on those platforms.
- `Config/DefaultEngine.ini` enables virtual shadows, mesh distance fields, ray tracing, Substrate, and GI/reflection method value 1; static lighting is disabled. Runtime rendering and hardware compatibility are not tested by this map.
- `Config/DefaultInput.ini` configures device axes, mouse capture, and Enhanced Input defaults; source gameplay does not implement input bindings in `Source/Vyxxen/Private/PlayerShip.cpp`.
- `Config/DefaultEditor.ini` holds asset preview profiles; treat these as editor configuration, not gameplay state.
- `Config/DefaultEngine.ini` contains an Android development file-server authentication field; omit its value from documentation and logs.
- Use `Source/Vyxxen.Target.cs` for Game target configuration, `Source/VyxxenEditor.Target.cs` for Editor target configuration, and `Source/Vyxxen/Vyxxen.Build.cs` for module dependencies.
- `.gitignore` excludes `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, and generated `.sln` files; `.gitattributes` requests automatic text normalization and does not configure Git LFS.

## Platform Requirements

- Use an Unreal installation compatible with the `5.8` association in `Vyxxen.uproject` and a compatible native toolchain matching `.vsconfig` requirements.
- Windows editor build artifacts are present at `Binaries/Win64/UnrealEditor-Vyxxen.dll`; no build or editor launch is performed for this analysis.
- No repository project skills are detected under `.codex/skills/` or `.agents/skills/`, and `.planning/config.json` is absent.
- Native desktop game is the configured target (`Source/Vyxxen.Target.cs`, `Config/DefaultEngine.ini`). Shipping packaging, distribution/store destination, dedicated server, and deployment pipeline are not detected.
- Validate packaging and binary asset references in Unreal before assuming `Content/Levels/MainGame.umap` is a shippable playable experience.

<!-- GSD:stack-end -->

<!-- GSD:conventions-start source:CONVENTIONS.md -->

## Conventions

## Naming Patterns

- Use matching PascalCase C++ class stems, with declarations in `Source/Vyxxen/Public/PlayerShip.h` and definitions in `Source/Vyxxen/Private/PlayerShip.cpp`.
- Keep Unreal build rule suffixes: `Source/Vyxxen/Vyxxen.Build.cs`, `Source/Vyxxen.Target.cs`, and `Source/VyxxenEditor.Target.cs`.
- Asset names mix prefixes and hyphenated imported names: `Content/Scenery/BP_level1_scenery.uasset`, `Content/Scenery/ground-tile/M_ground-tile.uasset`, and `Content/Units/PlayerShip/vyxxen-ship.uasset`. No uniform asset naming validator is detected.
- Use PascalCase Unreal lifecycle names and explicit overrides, as in `BeginPlay()` and `Tick(float DeltaTime)` in `Source/Vyxxen/Public/PlayerShip.h`.
- Match the visible Unreal API style: `DeltaTime` and `PrimaryActorTick.bCanEverTick` in `Source/Vyxxen/Private/PlayerShip.cpp`.
- No project-owned member variable naming pattern can be established from the scaffold in `Source/Vyxxen/Public/PlayerShip.h`.
- Use the Unreal actor prefix and module export macro: `class VYXXEN_API APlayerShip : public AActor` in `Source/Vyxxen/Public/PlayerShip.h`.
- Retain reflection declarations `UCLASS()` and `GENERATED_BODY()` in reflected headers such as `Source/Vyxxen/Public/PlayerShip.h`.

## Code Style

- Follow the visible tab indentation and opening braces on separate lines in `Source/Vyxxen/Private/PlayerShip.cpp` and `Source/Vyxxen/Vyxxen.Build.cs`.
- Whitespace is not fully normalized: the scaffold contains trailing tabs, extra blank lines, and mixed spacing in `Source/Vyxxen/Public/PlayerShip.h` and `Source/Vyxxen/Vyxxen.cpp`.
- No dedicated formatter or editor style configuration is detected alongside `Vyxxen.uproject`; `.gitattributes` sets `* text=auto`.
- No dedicated lint configuration is detected alongside `Vyxxen.uproject` or in `Source/`.
- `Source/Vyxxen.Target.cs` and `Source/VyxxenEditor.Target.cs` select `BuildSettingsVersion.V7`; do not interpret this as evidence of a passing build or an enforced project lint gate.

## Import Organization

- No application path aliases are detected. Includes use module-visible header names such as `PlayerShip.h` in `Source/Vyxxen/Private/PlayerShip.cpp`.
- Declare module dependencies in `Source/Vyxxen/Vyxxen.Build.cs`; the current public dependencies are `Core`, `CoreUObject`, `Engine`, and `InputCore`.

## Error Handling

- No project error-handling, assertions, `ensure`, `check`, or exception pattern is detected in `Source/Vyxxen/`.
- `Source/Vyxxen/Private/PlayerShip.cpp` only sets ticking and delegates lifecycle methods to the base class; it supplies no substantive failure path to copy.

## Logging

- `Source/Vyxxen/Vyxxen.cpp` registers the default game module and does not define a shared logger. Establish any future category explicitly rather than assuming one exists.

## Comments

- The comments in `Source/Vyxxen/Public/PlayerShip.h` and `Source/Vyxxen/Private/PlayerShip.cpp` explain generated lifecycle scaffolding; no project-specific rationale convention is established.
- `Source/Vyxxen/Vyxxen.Build.cs` includes commented suggestions for optional Slate and online subsystem dependencies; these are instructions in comments, not active dependencies.
- Not applicable to the native sources under `Source/Vyxxen/`; no custom API documentation markup is detected there.

## Function Design

## Module Design

- Export reflected actor classes with `VYXXEN_API` as in `Source/Vyxxen/Public/PlayerShip.h`.
- Keep public declarations in `Source/Vyxxen/Public/` and implementations in `Source/Vyxxen/Private/`.
- `Source/Vyxxen/Vyxxen.cpp` uses `IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Vyxxen, "Vyxxen")`; `Vyxxen.uproject` declares a single runtime module.
- No barrel pattern is present. `Source/Vyxxen/Vyxxen.h` is a minimal module header that includes `CoreMinimal.h`.
- No repository-local skill folders or `AGENTS.md` are detected alongside `Vyxxen.uproject`; conventions here derive from the inspected files.
- The current working-tree `Source/` is untracked; describe its present contents without assuming they are available in a clean checkout.
- Binary Blueprint internals, parent classes, and editor wiring in `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Levels/MainGame.umap` are unverified.

<!-- GSD:conventions-end -->

<!-- GSD:architecture-start source:ARCHITECTURE.md -->

## Architecture

## System Overview

```text

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

- Follow Unreal reflection and lifecycle conventions: `UCLASS`, `GENERATED_BODY`, exported `VYXXEN_API`, and engine overrides in `Source/Vyxxen/Public/PlayerShip.h`.
- Keep class interfaces in `Source/Vyxxen/Public/` and implementations in `Source/Vyxxen/Private/`.
- Treat `Config/` as project defaults and `Content/` as binary authoring data, rather than searching generated `Intermediate/` for authoritative gameplay code.
- No native GameMode, Pawn, PlayerController, subsystem, save system, service layer, or networking implementation is detected in `Source/`.

## Layers

- Purpose: engine discovery, target selection, module dependencies.
- Location: `Vyxxen.uproject`, `Source/*.Target.cs`, `Source/Vyxxen/Vyxxen.Build.cs`.
- Depends on UnrealBuildTool and engine modules; used by Unreal builds and editor loading.
- Purpose: reflected actor type and lifecycle extension points.
- Location: `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`.
- Contains only `APlayerShip`; depends on `AActor` and CoreMinimal. No component construction, input binding, movement, combat, or persistent fields are defined.
- Whether a level or Blueprint uses this class is unverified in `Content/Levels/MainGame.umap` and `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`.
- Purpose: level, ship, and environment authoring.
- Location: `Content/Levels/`, `Content/Units/PlayerShip/`, `Content/Scenery/`.
- Contains binary packages with Blueprint-style names such as `Content/Scenery/BP_level1_scenery.uasset`; dependencies and execution behavior require Unreal editor inspection.

## Data Flow

### Primary Request Path

### Native Actor Lifecycle

- No custom member state or reflected properties exist in `Source/Vyxxen/Public/PlayerShip.h`.
- Input engine classes are configured in `Config/DefaultInput.ini:79`; no native bindings or input asset packages are detected in `Source/` or the `Content/` inventory.
- Blueprint state and level-owned state are unverified in `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Levels/MainGame.umap`.

## Key Abstractions

- Purpose: named ship actor extension point, not a verified controllable Pawn.
- Examples: `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`.
- Pattern: `AActor` inheritance with public constructor/Tick and protected BeginPlay; retain superclass calls when extending lifecycle hooks.
- Purpose: one game module shared by game and editor targets.
- Examples: `Source/Vyxxen/Vyxxen.cpp`, `Source/Vyxxen/Vyxxen.Build.cs`.
- Pattern: default engine module implementation; no custom startup/shutdown code.

## Entry Points

## Architectural Constraints

- **Threading:** No custom task/thread code exists in `Source/Vyxxen/`; do not infer worker execution from the presence of Tick in `Source/Vyxxen/Private/PlayerShip.cpp`.
- **Global state:** No custom singleton or shared mutable global is defined in `Source/Vyxxen/Vyxxen.cpp` or `Source/Vyxxen/Private/PlayerShip.cpp`.
- **Circular imports:** No local cycle is present in the small native include graph (`Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`, `Source/Vyxxen/Vyxxen.cpp`).
- **Input boundary:** `Config/DefaultInput.ini` names Enhanced Input classes, while `Source/Vyxxen/Vyxxen.Build.cs` has InputCore but no EnhancedInput dependency. Check/add the module dependency when introducing native Enhanced Input APIs.
- **Asset boundary:** Never assume `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` inherits `APlayerShip` from matching names. Verify its parent and level references in the editor first.
- **Project skills:** No project-local `.codex/skills/` or `.agents/skills/` directory and no `.planning/config.json` are present in this root; no project skill constraints were detected.

## Anti-Patterns

### Empty per-frame override

### Equating input configuration with control implementation

## Error Handling

## Cross-Cutting Concerns

<!-- GSD:architecture-end -->

<!-- GSD:skills-start source:skills/ -->

## Project Skills

No project skills found. Add skills to any of: `.claude/skills/`, `.agents/skills/`, `.cursor/skills/`, `.github/skills/`, or `.codex/skills/` with a `SKILL.md` index file.
<!-- GSD:skills-end -->

<!-- GSD:workflow-start source:GSD defaults -->

## GSD Workflow Enforcement

Before using Edit, Write, or other file-changing tools, start work through a GSD command so planning artifacts and execution context stay in sync.

Use these entry points:
- `$gsd-fast` for a trivial task inline, with no subagents and no PLAN.md
- `$gsd-quick` for small fixes, doc updates, and ad-hoc tasks
- `$gsd-debug` for investigation and bug fixing
- `$gsd-execute-phase` for planned phase work

Do not make direct repo edits outside a GSD workflow unless the user explicitly asks to bypass it.
<!-- GSD:workflow-end -->

<!-- GSD:profile-start -->

## Developer Profile

> Profile not yet configured. Run `$gsd-profile-user` to generate your developer profile.
> This section is managed by `generate-claude-profile` -- do not edit manually.
<!-- GSD:profile-end -->
