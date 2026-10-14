# Project Research Summary

**Project:** Vyxxen — first flight-and-camera cut
**Domain:** Single-player 3D arcade flight prototype in Unreal Engine
**Researched:** 2026-10-07
**Confidence:** MEDIUM

## Executive Summary

Vyxxen's approved first cut is a playable flight-control slice in Unreal Editor: automatic world +X travel, independent Y steering and Z climb, bank/pitch feedback, directly editable speeds and altitude limits, selective hold-to-slow, and a synchronized fixed-angle camera. Build it inside the existing `Vyxxen` module with a possessed ship Pawn, a small custom movement component, Enhanced Input actions/context, and a placed camera with a route-follow component. Preserve the existing scene and ship authoring. Firing, fuel, combat, fortress progression, additional HUD, and packaging are outside this milestone.

The central design decision is one movement authority. The flight component applies independent world-axis motion and publishes progress from the ship's actual resulting X position. The camera consumes that progress while preserving its authored Y/Z/rotation and starting X offset. One Slow blend ramps forward and lateral rates toward 60% of normal, then restores them on release; climb stays unchanged. Mesh pose expresses the controls without rotating the movement basis. These narrow boundaries satisfy the cut without introducing a stage manager, settings subsystem, or general flight framework.

The largest uncertainties are local integration and runtime evidence. The native ship is currently an `AActor` scaffold with empty lifecycle hooks; Blueprint inheritance, existing graphs, possession, view target, installed engine compatibility, and PIE input-focus behavior remain unverified. Inspect these before structural changes, choose one possession path, reset cached intent on ending/control-loss paths, and apply camera follow after actual movement but before view sampling. Completion requires a current build/load and observed, repeatable PIE behavior; research, filenames, and generated binaries establish no validated gameplay capability.

## Key Findings

### Recommended Stack

[STACK.md](STACK.md) recommends staying with the existing Unreal project and bundled modules. The descriptor associates with **UE 5.8**, but the installed editor revision and compatible MSVC/SDK must be checked before implementation. Keep engine and Enhanced Input from the same installation; no external package or service is needed.

**Core technologies:**

- **Unreal C++ and Blueprint assets:** native movement/input rules with existing authored meshes, references, and editor tuning.
- **`APawn` / existing PlayerController:** explicit player possession; adapt `APlayerShip` after Blueprint inspection rather than assuming its current Actor base is controllable.
- **Custom `UPawnMovementComponent`:** one owner for independent rates, altitude bounds, Slow state, pose, and applied forward progress.
- **Enhanced Input:** Axis1D Steer/Climb and Boolean Slow actions plus one flight Mapping Context; add the missing native `EnhancedInput` module dependency.
- **Placed `ACameraActor` and follow component:** fixed composition, X-only translation, and explicit selection as the active view target.
- **Reflected editable properties:** one ship/component tuning surface for speeds, height limits, and ramp timing; runtime validation supplements editor metadata.

### Expected Features

[FEATURES.md](FEATURES.md) defines eight required features. Its differentiators describe already approved control characteristics; they are mandatory parts of this cut.

**Must have (table stakes):**

- **FLT-01/02:** automatic world +X progress with independent signed Y steering and Z climb/descent, including simultaneous input.
- **FLT-03:** directly editable forward/lateral/climb speeds and enforced minimum/maximum height.
- **FLT-04:** nose-axis bank and climb pitch, with smooth return toward authored level when intent/motion stops.
- **FLT-05:** editable hold/release Slow ramp; full Slow leaves X/Y rates at **0.60**, with Z rate unchanged.
- **FLT-06:** release/cancellation clears intent; clearing Slow requests normal-speed recovery through the ramp.
- **FLT-07:** exact shared forward progress; constant camera–ship X offset and fixed camera world Y/Z/rotation.
- **FLT-08:** intended ship and camera work in MainGame through normal PIE startup.

**Approved differentiators:** selective gradual Slow, fixed route-camera framing, and immediate developer tuning through Details/Class Defaults. None is optional polish or a new product subsystem.

**Defer to a separately scoped milestone:** firing/projectiles/cursor, fuel and its slowdown penalty, enemies/hazards/damage, score/lives/respawn, fortress completion, new HUD, lateral corridor constraints, and packaged delivery. Do not create stubs or acceptance dependencies for them now.

### Architecture Approach

[ARCHITECTURE.md](ARCHITECTURE.md) recommends one possessed Pawn and two focused components. Input callbacks update command state; movement consumes that state once per tick, applies independent world-axis displacement, clamps height, and reads the actual resulting position. Pose remains separate from translation. Camera follow consumes committed progress in an ordered update before the rendered view is evaluated; scenery remains stationary.

**Major components:**

1. **Ship Pawn and input assets:** own the authored ship hierarchy, bind semantic actions, activate the local player's flight context, and send/reset intent.
2. **Flight movement component:** own tuning, Slow blend, world XYZ movement, height enforcement, visual pose, and read-only committed progress.
3. **Route camera component and placed camera:** capture authored composition once, assign X from actual ship progress, preserve Y/Z/rotation, and supply the selected player view.
4. **Existing MainGame wiring:** establish one controlled ship and one selected camera through inspected possession/startup configuration.

The required invariant is `CameraX = StartCameraX + (ActualShipX - StartShipX)`. This preserves the initial offset even when applied movement differs from requested motion. Give the updating camera component a movement prerequisite; the proposed PrePhysics movement / PostPhysics follow ordering places follow before camera sampling. An actor-only prerequisite does not order work executed by an owned component. Avoid separate camera integration, X lag, and whole-transform attachment.

### Critical Pitfalls

[PITFALLS.md](PITFALLS.md) identifies these highest-impact risks:

1. **Wrong possessed ship or destructive Blueprint migration:** inspect parent/components/graphs and World Settings first; retain authored work, choose one spawn/possession route, remove duplicate movement, compile/save affected assets.
2. **Latched input after cancellation or focus loss:** bind Completed and Canceled resets, centralize lifecycle cleanup, and reproduce outside-viewport release in the actual PIE environment; engine flushing alone does not prove cached intent clears.
3. **Camera drift or previous-frame following:** read applied ship X, retain the initial offset, order the actual follower tick before view evaluation, and verify the active rendered camera.
4. **Coupled axes, incorrect Slow, or pose-driven motion:** use independent world XYZ arithmetic, one seconds-based Slow blend scaling only X/Y, and a visual pose pivot; verify 0.6/0.6/1.0 settled rate ratios and mid-ramp reversals.
5. **Unsafe tuning or patch-only success:** validate rates/bounds/duration, clamp initial and proposed Z, define zero-duration behavior, and repeat acceptance after a fresh editor load with saved asset references.

## Implications for Roadmap

Recommend **two phases**, grouped by the movement-to-camera dependency. Asset inspection and PIE wiring belong inside these phases, rather than separate infrastructure milestones. All eight flight features remain required for milestone completion.

### Phase 1: Controllable Arcade Flight

**Rationale:** camera correctness needs an actual possessed ship and authoritative applied movement, including Slow. Existing binary assets must be inspected before Pawn integration.
**Delivers:** inspected ship/map ownership; a single possessed ship; Enhanced Input actions/context; automatic +X, independent Y/Z, editable rates/height limits, bank/pitch recovery, ramped selective Slow, and release/cancellation cleanup.
**Addresses:** FLT-01 through FLT-06; establishes the control/startup portion of FLT-08.
**Uses:** existing `Vyxxen` module, Pawn lifecycle, custom movement component, reflected tuning, and Enhanced Input.
**Avoids:** duplicate/default pawns, duplicate Blueprint movement, mesh-basis errors, normalized diagonal rates, incorrect Slow percentages, latched input, and invalid tuning.
**Verification gate:** current editor build/load and observed controlled flight; direct tuning affects the intended ship, height bounds retain inward movement, neutral pose recovers, and settled Slow rates are 0.6 X/Y with unchanged Z.

### Phase 2: Synchronized Camera and Repeatable PIE Delivery

**Rationale:** exact framing must be checked against completed normal/Slow movement and the active player view.
**Delivers:** placed route camera bound to applied ship progress, fixed authored composition, component update ordering, selected view target, saved asset wiring, and repeatable integrated PIE acceptance.
**Addresses:** FLT-07 and completion of FLT-08; regression verification of FLT-01 through FLT-06.
**Uses / implements:** `ACameraActor`, route-follow component, movement prerequisite, and existing PlayerController view-target selection.
**Avoids:** independent camera accumulation, camera lag, Y/Z/pose inheritance, late follow, stale references/defaults, and accepting a compile-only result.
**Verification gate:** constant camera–ship X offset through normal flight, Slow ramp-in/hold/release/reversal, steering/climbing, and a nonzero starting X; fixed camera Y/Z/rotation; deliberate cancellation/focus checks; repeatable behavior after editor reopening.

### Phase Ordering Rationale

- Inspect binary assets and establish possession at the start of Phase 1; do not migrate blind or defer input cleanup to final polish.
- Keep all movement policy together so Slow, limits, and pose share one state owner before camera integration.
- Select an observation view early as needed, but complete synchronized-camera acceptance only after movement works.
- Phase 2 consumes actual position and finishes the PIE delivery gate; it adds no fortress systems or packaged build requirement.

### Research Flags

- **Phase 1 — targeted local research/inspection needed:** installed UE/toolchain, Blueprint parent/hierarchy/graphs, MainGame possession overrides, imported mesh basis, and actual Enhanced Input focus/flush behavior. Use research-phase support only for unresolved integration details; broad ecosystem research is unnecessary.
- **Phase 2 — standard pattern:** applied-X offset following and explicit tick dependencies are documented. Skip broad research; inspect active view target/auto-camera behavior and locally reproduce timing/focus issues if acceptance exposes them.
- **Standard across both phases:** independent world-axis arithmetic, one Slow blend, direct reflected tuning, and existing module integration. Research cannot replace the build/editor/PIE checks.

## Confidence Assessment

| Area | Confidence | Notes |
|------|------------|-------|
| Stack | MEDIUM | Primary Epic APIs plus inspected declarations; installed engine/toolchain and compilation remain unverified. |
| Features | MEDIUM | Approved scope is explicit; effort, integration dependencies, and current runtime behavior are unverified. |
| Architecture | MEDIUM | Official Pawn/input/movement/tick contracts support the proposal; exact asset hierarchy and camera sampling need local validation. |
| Pitfalls | MEDIUM | Engine semantics support conditional risks; current scaffold/dependency gaps are observed, runtime failures are not confirmed. |

**Overall confidence:** MEDIUM. All four documents use the verified-websearch MEDIUM tier after Context7 was unavailable. Agreement comes largely from shared official sources and scope, rather than independent runtime evidence. No gameplay feature has been validated yet.

### Gaps to Address

- **Installed engine/API/toolchain:** resolve the 5.8 association and compile the existing Editor target against the actual installation; retain the project association unless an explicitly scoped change is needed.
- **Blueprint/map ownership:** inspect parent, movable root, pose pivot/import correction, graphs, instance defaults, GameMode/possession, and camera selection before adapting assets.
- **Authoring values:** choose initial speeds, world-Z bounds, pose limits, ramp duration, and authored camera composition in editor; these are tunables, not researched fixed arcade constants.
- **Input loss:** verify Completed/Canceled and focus-loss release separately; implement the reset hook required by observed local behavior.
- **Numeric/runtime acceptance:** define practical transform/rate measurement tolerances, invalid-setting behavior, and hitch/frame-rate checks; use lightweight diagnostics or focused arithmetic checks where useful, without adding a permanent debug subsystem.
- **Durable delivery:** record current build/load and fresh-session PIE evidence with saved Blueprints/input/map references. If runtime access is unavailable, report that verification boundary explicitly instead of marking the cut complete.

## Sources

### Project Authority and Local Evidence

- [PROJECT.md](../PROJECT.md) — approved first-cut scope, constraints, exclusions, repository evidence, and absence of runtime validation.
- [STACK.md](STACK.md), [FEATURES.md](FEATURES.md), [ARCHITECTURE.md](ARCHITECTURE.md), [PITFALLS.md](PITFALLS.md) — synthesized research and detailed source trails, 2026-10-07.
- Local declarations cited by the research: `Vyxxen.uproject`, `Source/Vyxxen/` ship/module/target files, selected input defaults, `.vsconfig`, and `.planning/codebase/`. Blueprint/map behavior remains uninspected.

### Primary Epic Documentation (MEDIUM Research Tier)

These references were checked by the source research on 2026-10-07. Source authority does not establish compatibility with the installed engine or prove Vyxxen's runtime behavior.

- [Pawn](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine), [Pawn input setup](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/APawn/SetupPlayerInputComponent) — controllable entity, possession and input lifecycle.
- [UPawnMovementComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UPawnMovementComponent), [UMovementComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UMovementComponent) — owner/UpdatedComponent boundary and applied motion.
- [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine), [ETriggerEvent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent) — actions, contexts and terminal events.
- [Input settings](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UInputSettings), [Enhanced Input developer settings](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/EnhancedInput/UEnhancedInputDeveloperSettings) — focus/input flushing and event behavior.
- [Actor Ticking](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-ticking-in-unreal-engine), [Component prerequisite](https://dev.epicgames.com/documentation/unreal-engine/BlueprintAPI/Actor/Tick/AddTickPrerequisiteComponent) — actual tick dependencies and view sampling order.
- [Set View Target](https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/Game/Player/SetViewTargetwithBlend) — selecting the placed camera for the player view.
- [Coordinate system](https://dev.epicgames.com/documentation/en-us/unreal-engine/coordinate-system-and-spaces-in-unreal-engine), [TRotator](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/TRotator) — world/visual bases and bank/pitch axes.
- [Properties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties), [Metadata](https://dev.epicgames.com/documentation/unreal-engine/metadata-specifiers-in-unreal-engine) — editor tuning versus runtime validation.
- [Constant interpolation](https://dev.epicgames.com/documentation/unreal-engine/BlueprintAPI/Math/Interpolation/FInterptoConstant), [Live Coding](https://dev.epicgames.com/documentation/unreal-engine/using-live-coding-to-recompile-unreal-engine-applications-at-runtime) — elapsed-time ramp semantics and fresh-state verification.
- [Hardware/software specifications](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine), [Unreal modules](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules) — engine/toolchain checks and native module dependencies.

### Secondary / Tertiary Sources

None required. The broader first-fortress design is future context; approved PROJECT.md scope controls this milestone.

---
*Research completed: 2026-10-07*
*Ready for roadmap: yes — scope and recommendations established; runtime verification remains required.*
