# Stack Research

**Project:** Vyxxen — first flight-and-camera cut
**Domain:** Single-player 3D arcade flight prototype, playable in Unreal Editor
**Researched:** 2026-10-07
**Confidence:** MEDIUM — code-classified verified websearch tier; official Epic documentation cross-checked with local declarations. Installed engine, compilation, binary asset graphs, and Play In Editor behavior remain unverified.

## Recommended Stack

Keep the existing Unreal project and `Vyxxen` runtime module. Use a possessable ship pawn, a small native movement component, Enhanced Input assets, and a placed camera with a route-follow component. This is a recommendation derived from the approved requirements and Epic's framework contracts, rather than a claim that Epic supplies this exact arcade flight implementation. No external package, additional gameplay framework, database, or service is needed for this cut.

### Core Technologies

| Technology | Version | Purpose | Why Recommended |
|------------|---------|---------|-----------------|
| Unreal Engine | Project association **5.8**; exact installed version unverified | Existing editor, rendering, reflected types, scene and asset runtime | Fixed project constraint. `Vyxxen.uproject` already declares the engine association and runtime module; keep the current map and authored scenery. |
| Unreal C++ plus Blueprint assets | Selected engine's defaults; independent language standard unpinned | Native flight behavior; editor asset references and tuning | Existing `Source/Vyxxen/` is the integration point. Blueprint assets keep meshes, material assignments, input references, and tuned values reviewable in the editor. |
| `APawn` and `APlayerController` | Engine-provided | Player possession and input dispatch | `APawn` is the possessable actor base; it fits a ship without requiring character walking behavior. Inspect existing Blueprint inheritance before adapting the current `AActor` scaffold. [Pawn API](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/APawn), [Controllers](https://dev.epicgames.com/documentation/unreal-engine/controllers-in-unreal-engine). |
| Custom `UPawnMovementComponent` subclass | Engine-provided base; project implementation | Automatic world +X advance, independent Y steering/Z climbing, speed ramp, altitude bounds | Base supports pawn ownership and directional input; a small subclass can enforce this project's independent speeds and selective slowdown explicitly. [Pawn movement API](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UPawnMovementComponent). |
| Enhanced Input | Bundled engine plugin; match installed engine | Two Axis1D actions for Steer/Climb and a Boolean Slow action, assigned through one flight Mapping Context | Already selected in input configuration. Keep device/key mappings in assets, bind native semantic actions, and register the context for the local player. [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine), [Input Overview](https://dev.epicgames.com/documentation/unreal-engine/input-overview-in-unreal-engine). |
| Placed `ACameraActor` plus route-follow component | Engine-provided base; project follow implementation | Authored viewpoint translated only along world X | A camera actor supports level placement. Copy applied ship X progress into camera X while retaining captured camera Y/Z/rotation; select it as the player's view target after possession. [Camera API](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/ACameraActor), [Set View Target](https://dev.epicgames.com/documentation/unreal-engine/BlueprintAPI/Game/Player/SetViewTargetwithBlend?lang=en-US). |
| Reflected `UPROPERTY` tuning | Engine-provided | Speeds, min/max altitude, ramp timing and visual pose settings in Details/Class Defaults | `EditAnywhere` supports both class defaults and placed instances. Put tuning directly on the ship's movement component; no separate settings asset is required. Validate relationships such as minimum <= maximum in runtime logic as well as editor metadata. [Properties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties). |

### Supporting Libraries

| Library / module | Version | Purpose | When to Use |
|------------------|---------|---------|-------------|
| `Core`, `CoreUObject`, `Engine`, `InputCore` | Selected engine's bundled versions | Math, UObject reflection, gameplay and input types | Already public dependencies in `Source/Vyxxen/Vyxxen.Build.cs`; retain. |
| `EnhancedInput` | Same engine installation | `UEnhancedInputComponent`, actions, mapping contexts and local-player subsystem | Add a native module dependency when implementing bindings. Input defaults alone do not declare a C++ build dependency. [EnhancedInput module](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/EnhancedInput), [Module dependencies](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules). |
| `UMovementComponent` utilities | Part of `Engine` | Move the configured UpdatedComponent through Unreal's movement API | Use within the custom pawn movement component. Select the moved root/component deliberately; if sweeps are used, its collision is authoritative. This does not add damage, hazards, or collision outcomes to this milestone. [Movement API](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UMovementComponent). |
| Actor/component tick dependencies | Part of `Engine` | Camera consumes current-frame flight progress | Use an explicit movement-before-follow dependency, or invoke follow immediately after movement, before camera evaluation. An arbitrary independent Tick can sample last-frame movement. [Actor Ticking](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-ticking-in-unreal-engine). |

### Development Tools

| Tool | Purpose | Notes |
|------|---------|-------|
| UnrealBuildTool / UnrealHeaderTool | Native editor build and reflected type generation | `Build.cs` and `Target.cs` are build authority. `VyxxenEditor.Target.cs` already defines an Editor target and `BuildSettingsVersion.V7`; retain the selected engine's settings. |
| Visual Studio and engine-compatible MSVC/Windows SDK | Windows native development | `.vsconfig` requests native game/desktop workloads, Unreal integrations, MSVC component `14.50.18.0`, and SDK `22621`. These are declarations, not installation verification. Epic's current UE5.8 specifications name Visual Studio 2026 for general development; verify the actual engine's supported toolchain before implementation. [Hardware and Software Specifications](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine). |
| Unreal Editor, asset Details, World Settings, Play In Editor | Blueprint inheritance, possession, camera selection, and acceptance | Inspect existing ship and map assets first. Successful editor load/build and observed controls establish completion; existing DLLs and `.uasset` filenames do not. [Playing and Simulating](https://dev.epicgames.com/documentation/en-us/unreal-engine/playing-and-simulating-in-unreal-engine). |

## Installation / Integration

There is no npm/pip installation step. All recommended APIs are engine modules or local project classes. This research changes documentation only; the following describes future implementation work.

1. Resolve the project association to the installed engine and inspect its version/build metadata and relevant headers. Verify its toolchain support rather than assuming a compiler version from generated IDE files.
2. Confirm Enhanced Input is enabled in that installation/project. `Config/DefaultInput.ini` selects `EnhancedPlayerInput` and `EnhancedInputComponent`; the descriptor does not explicitly list Enhanced Input.
3. Add `EnhancedInput` to `Vyxxen.Build.cs`: prefer a private dependency when Enhanced Input headers are included only in implementation files and public headers use forward declarations. Use a public dependency if public headers require the module's definitions. [Unreal Engine Modules](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules).
4. Inspect `BP_vyxxen-ship` and `MainGame` before changing inheritance, possession or map overrides. Bind flight actions and assign one flight Mapping Context. Clear stored intent on both Completed and Canceled; avoid a timed Hold trigger that delays a slowdown intended to begin immediately. [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine).
5. Regenerate project files after dependency/class layout changes, then build the existing Editor target during implementation. Validate flight and the selected camera in PIE. [Creating a Gameplay Module](https://dev.epicgames.com/documentation/en-us/unreal-engine/how-to-make-a-gameplay-module-in-unreal-engine).

## Alternatives Considered

| Recommended | Alternative | When to Use Alternative |
|-------------|-------------|-------------------------|
| Custom pawn movement component | `UFloatingPawnMovement` | Appropriate for generic free movement with acceleration/deceleration and one maximum velocity magnitude. Here, independent forward/lateral/climb rates and slowdown affecting only two axes make a small custom policy clearer. [Floating pawn movement](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UFloatingPawnMovement). |
| Native pawn/movement plus Blueprint tuning | Blueprint-only flight | Reasonable if later asset inspection reveals a substantial working graph worth preserving. Current native source is a scaffold and binary behavior is unknown; decide integration after inspection. |
| Pawn-local flight input | Custom PlayerController hosting all input | Useful for changing vehicles, persistent input modes or multiple possessed avatars. This first cut has one ship; pawn input binding is sufficient, with controller possession and view-target selection. [Player Controllers](https://dev.epicgames.com/documentation/unreal-engine/player-controllers-in-unreal-engine?lang=en-US). |
| Placed camera following an absolute X value | Camera component hosted by a dedicated route actor | Reasonable if existing scene organization already uses a route actor. Preserve the same independent Y/Z/rotation and shared forward-progress invariant. [Cameras](https://dev.epicgames.com/documentation/unreal-engine/cameras-in-unreal-engine). |

## What NOT to Use for This Cut

| Avoid | Why | Use Instead |
|-------|-----|-------------|
| Raw key checks / new legacy AxisMappings gameplay | Bypasses the selected Enhanced Input contract and mixes device layout with ship logic | Semantic actions and one Mapping Context. |
| `ACharacter` or aerodynamic/physics flight framework | Adds behavior and configuration outside the approved arcade control problem | Kinematic pawn movement with direct editor tuning. This is a scope choice, not a claim these frameworks cannot fly. |
| Generic normalize-and-cap treatment of combined X/Y/Z input | Couples climb/lateral speeds to simultaneous input and can reduce forward progress | Independent per-axis movement terms; slow multiplier affects X/Y only. |
| Spring-arm lag or camera attached directly to the ship mesh | Introduces follow behavior that conflicts with fixed Y/Z/rotation and exact X synchronization | Placed camera using applied ship progress, with explicit update ordering. |
| Separate camera speed integration | Identical configured speed can still drift or differ for a frame | `CameraX = InitialCameraX + (AppliedShipX - InitialShipX)`; derive from applied motion rather than requested motion. |
| Fuel, firing, damage, UI framework, online modules, database or service dependencies | None is required to demonstrate the authorized flight-and-camera cut | Defer until a later scoped milestone requires them. |

## Stack Patterns by Variant

**For the current straight +X route:** use world-axis motion, editable speed/altitude fields, and a visual mesh that banks/pitches independently of the translation basis. Keep authored scenery stationary. Full Slow blends forward/lateral scale to 0.6; climb rate remains unchanged. These are project decisions recorded in `PROJECT.md`.

**If the ship Blueprint already has working components/graphs:** preserve useful asset setup, remove duplicate movement owners during implementation, and adapt the native integration deliberately. Do not infer inheritance or working possession from an asset filename.

**If camera follow uses its own component tick:** order it after movement and before the engine evaluates the view. Match actual ship X after movement, including any movement constraint; setting nominal speed twice is insufficient. Epic documents prerequisites and camera timing; the exact matching formula is this project's design inference. [Actor Ticking](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-ticking-in-unreal-engine).

## Version Compatibility and Confidence

| Evidence / pairing | Confidence | Notes |
|--------------------|------------|-------|
| Descriptor association `5.8`, runtime module `Vyxxen`, local native dependencies and input defaults | MEDIUM | Directly read local declarations and cross-checked with the existing codebase map. Engine association does not establish the installed binary revision. |
| Epic current API/concept pages and recommended pawn/input/camera primitives | MEDIUM | Current pages identify UE5.8; accessed 2026-10-07. Search crawl dates ranged from today to three weeks earlier; crawl time is not a publication date. No precise publication dates were exposed. |
| All Engine and EnhancedInput modules from one installation | MEDIUM | Recommended compatibility policy; do not independently download/pin a plugin version. Compile against that installation's headers to verify concrete API signatures. |
| `.vsconfig` versus installed engine/toolchain | Unverified | The file describes requested components. No installation inventory, compiler invocation, build or editor launch was performed. |
| Existing Blueprint parent, graph behavior, map-level overrides and camera ownership | Unverified | Binary assets require Unreal Editor inspection before implementation. |

The research-plan seam selected Context7 for documentation questions, but neither a Context7 MCP tool nor `ctx7` was available. Official Epic websearch was the fallback. `classify-confidence --provider websearch --verified` returned **MEDIUM**, used for cross-checked findings and cached digests. Direct-open results alone are not treated as installed-version verification. Questions and digests were processed through the GSD research-plan/research-store seam.

## Sources

All web sources above are primary Epic documentation. Local evidence inspected: `.planning/PROJECT.md`, `.planning/codebase/STACK.md`, `Vyxxen.uproject`, `Source/Vyxxen/Vyxxen.Build.cs`, both `Source/*.Target.cs`, `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`, selected input-default lines in `Config/DefaultInput.ini`, and `.vsconfig`. The full engine configuration was not printed.

---
*Stack research for: Vyxxen first flight-and-camera cut*
*Researched: 2026-10-07*
