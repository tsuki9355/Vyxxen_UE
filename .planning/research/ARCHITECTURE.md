# Architecture Research

**Project:** Vyxxen
**Domain:** Unreal arcade flight controls and synchronized route camera, first implementation cut
**Researched:** 2026-10-07
**Confidence:** MEDIUM
**Scope:** Automatic world +X travel, independent Y steering and Z climb, editable speed/height tuning, pitch/bank, hold-to-slow, and Play In Editor delivery. Combat, cursor, fuel, hazards, fortress rules, respawn, and packaged delivery are deferred.

## Standard Architecture

### System Overview

Use one possessed ship Pawn with a custom `UFlightMovementComponent`, and one placed `ACameraActor` with a `URouteCameraComponent`. Keep the existing `Vyxxen` runtime module. The flight component owns forward advancement for this cut; the camera consumes its committed progress. The fortress stays where authored. This is a recommendation derived from the approved requirements, supported by Epic's Pawn, movement, input, and tick APIs [S1-S7].

```text
Enhanced Input assets / local player subsystem
  IA_Steer (Axis1D)   IA_Climb (Axis1D)   IA_Slow (Bool)
                 │ action values / terminal-event resets
                 ▼
Possessed ship Pawn: input binding and authored ship components
                 │ command state
                 ▼
UFlightMovementComponent: sole forward-progress writer
  slow ramp → world XYZ translation → height clamp → pitch / bank
                 │ committed forward progress, read only
                 ▼
Placed ACameraActor + URouteCameraComponent
  authored X + progress; authored Y/Z and rotation preserved
                 │ selected view target
                 ▼
Engine camera update → rendered PIE view

Authored MainGame scenery remains stationary throughout.
```

Do not introduce `AGameManager`, a custom PlayerController, stage phases, or a general event bus for this milestone. The long-term design's manager is justified by fuel/lives/rewards and replacement of the ship; those responsibilities are not active requirements. Keep the progress getter as a small boundary so a later approved milestone can deliberately move ownership if persistent stage progress becomes necessary.

### Existing Versus Proposed Architecture

| Status | Evidence / implication |
|---|---|
| Verified in source | `Source/Vyxxen/Public/PlayerShip.h` declares `APlayerShip : AActor`; its `.cpp` contains empty lifecycle hooks. This is not yet a possessed ship. |
| Verified in module rules | `Vyxxen.Build.cs` declares Core, CoreUObject, Engine, and InputCore; introducing native Enhanced Input requires declaring its module dependency. |
| Documented project association | The project context records engine association 5.8. Retain it; this research does not propose a migration or establish that a matching local editor is available. |
| Unverified binary assets | `BP_vyxxen-ship.uasset`, `MainGame.umap`, and scenery packages need editor inspection to establish parents, component hierarchy, possession, view target, and existing graph behavior. |
| Proposed target | Evolve the ship role to a Pawn and add flight/camera components after inspecting asset dependencies. Preserve the existing native class identity where practical; the design name `AShipPawn` does not require renaming `APlayerShip`. |

Pawn possession is the preferred control boundary, and `UPawnMovementComponent` requires components owned by a Pawn [S1, S2]. Do not attach that movement class to the current `AActor` and assume it works. Inspect the Blueprint parent, child assets, and existing scene roots before changing inheritance or reparenting. Names alone do not establish any connection.

### Component Responsibilities

| Component | Responsibility | Communication / implementation |
|---|---|---|
| Ship Pawn, existing class name retained if suitable | Hull/mesh ownership; bind actions; hold asset references; establish player control and camera reference | Sends `Steer`, `Climb`, and `SlowHeld` to its movement component. Uses normal Pawn input hooks. |
| `UFlightMovementComponent : UPawnMovementComponent` | Editable movement parameters; runtime command state; slowdown ramp; world XYZ displacement; altitude clamp; pitch and bank; committed progress | Owns the sole forward integrator. Moves an explicit `UpdatedComponent` and exposes read-only progress. |
| Ship hull / pose hierarchy | Movable scene body and visible ship, oriented so its nose is +X | Reuse authored meshes/materials after inspection. Pose changes must not rotate the world displacement basis. |
| `URouteCameraComponent : UActorComponent` | Cache authored camera transform; consume flight progress; assign X position | Reads flight state after movement. Never advances progress or reads steering/climb/slow input. |
| Placed `ACameraActor` | Authored composition, perspective/orthographic choice, fixed view rotation | Its route component translates only X. Selected through the existing player controller's view-target API [S7]. |
| `IA_Steer`, `IA_Climb`, `IA_Slow`, gameplay Mapping Context | Hardware bindings, action types, signs/modifiers | Native gameplay binds action assets rather than raw keys [S4]. |
| MainGame startup wiring | One active ship, one active route camera, mapping context and view target available in PIE | Prefer an inspected placed Pawn with Auto Possess Player 0 for this cut, or one verified existing spawn path. Avoid simultaneous placed and default-spawned ships. |

`UPawnMovementComponent` gives the custom flight code Unreal's Pawn association and movement helpers; it does not supply this game's route behavior automatically [S2]. A generic free-flight component would still need substantial policy changes to enforce independent world axes and automatic forward movement, so use the small custom component.

## Recommended Project Structure

Preserve the mapped Public/Private layout. The following are proposed additions, not existing implemented classes:

```text
Source/Vyxxen/
├── Public/
│   ├── PlayerShip.h                 # inspected existing ship → Pawn role
│   ├── FlightMovementComponent.h    # movement rules and editable tuning
│   └── RouteCameraComponent.h       # camera's read-only progress consumer
├── Private/
│   ├── PlayerShip.cpp
│   ├── FlightMovementComponent.cpp
│   └── RouteCameraComponent.cpp
└── Vyxxen.Build.cs                  # native EnhancedInput dependency

Content/
├── Input/                          # proposed action / mapping assets
├── Units/PlayerShip/               # existing ship art and Blueprint
└── Levels/MainGame.umap             # existing PIE scene
```

Do not copy the design reference's example `Source/ArcadeClone/` module or scaffold its Combat, Props, Rules, and UI folders. Three gameplay types and input assets are sufficient. Component defaults or instance overrides are the tuning surface; a separate settings singleton or data-asset system adds no needed behavior here.

## Architectural Patterns

### Pattern 1: One Committed Forward Position

**What:** Compute forward displacement once in the flight component. After movement, publish progress from the actual ship X position relative to its startup X. The camera assigns its X from that value each frame.

**When:** Always for this cut, including Slow press, hold, and release.

**Trade-off:** Couples this camera to the active flight source, which is appropriate for one ship. Future replacement of the ship will need an explicit route-continuity decision, not a second integrator hidden in the camera.

The proposed invariant is:

```text
Progress = CommittedShipWorldX - ShipStartWorldX
CameraWorldX = CameraStartWorldX + Progress
CameraWorldY = CameraStartWorldY
CameraWorldZ = CameraStartWorldZ
CameraRotation = CameraStartRotation

Therefore:
CameraWorldX - CommittedShipWorldX
    = CameraStartWorldX - ShipStartWorldX
```

This is a project-specific design invariant, not a claim that Unreal guarantees it automatically. Deriving progress after applying movement keeps the camera correct if a movement operation commits less than its requested displacement. Never advance one route counter and independently advance camera X with `Speed * DeltaTime`; duplicated calculations can diverge. Do not interpolate camera X: exact frame progress is the requirement.

### Pattern 2: Independent Commands, Editable Rates, Bounded Height

**What:** Store `SteerInput`, `ClimbInput`, and `SlowHeld` separately. Interpret Y and Z independently, without diagonal normalization. Expose `ForwardSpeed`, `LateralSpeed`, `ClimbSpeed`, `MinHeight`, `MaxHeight`, slowdown ramp timing, pose limits, and pose response on the movement component's reflected editable properties.

**When:** Flight update and editor tuning.

**Trade-off:** Custom policy is a few arithmetic operations but requires explicit validation. Reject inverted height bounds and negative rates; handle zero ramp duration and zero climb speed safely. Define height as the ship reference point's world Z in this cut and label it accordingly. Additional full-hull clearance rules belong to the later collision milestone.

Project-derived update sketch, not drop-in production code:

```cpp
// Flight component is the only code that integrates forward displacement.
// SlowCut is the fixed first-cut requirement, not a freely changed default.
constexpr float SlowCut = 0.40f;
if (DeltaSeconds <= 0.0f) return;

const float SlowTarget = bSlowHeld ? 1.0f : 0.0f;
SlowAlpha = RampSeconds > 0.0f
    ? FMath::FInterpConstantTo(SlowAlpha, SlowTarget,
                             DeltaSeconds, 1.0f / RampSeconds)
    : SlowTarget;
const float Scale = 1.0f - SlowCut * SlowAlpha;

const FVector Before = UpdatedComponent->GetComponentLocation();
FVector Desired = Before;
Desired.X += ForwardSpeed * Scale * DeltaSeconds;
Desired.Y += SteerInput * LateralSpeed * Scale * DeltaSeconds;
Desired.Z = FMath::Clamp(Before.Z + ClimbInput * ClimbSpeed * DeltaSeconds,
                        MinHeight, MaxHeight);
// Apply Desired through the selected movement operation, then read actual pose.
// Publish Actual.X - ShipStartX. The camera consumes this committed result.
```

At full Slow, forward and lateral movement are 60% of normal: the requirement is a 40% reduction. Climb speed stays unchanged. Pitch follows actual vertical displacement per time step, so reaching a height limit returns its target toward level. Bank follows steer input; neutral steer returns its target toward level. Apply pitch around Y and bank around the +X nose while preserving authored mesh orientation. Pose must never turn forward movement into local-forward aerodynamic motion. Any lateral clamp from the longer-term design is optional future scope, not an approved minimum requirement.

### Pattern 3: Input Callbacks Set State; Movement Tick Consumes It

**What:** Axis action callbacks write normalized scalar command values; the Slow action writes a held flag. `Completed` and `Canceled` clear their respective state. Native input setup binds actions and installs the gameplay Mapping Context through the local player's subsystem [S4].

**When:** Possession/input setup and every input event.

**Trade-off:** Clean movement arithmetic requires lifecycle hygiene. Use a shared reset helper for unpossession, removal of the gameplay context, and other observed control-loss paths. Do not assume every focus-loss path emits the same terminal event: Epic exposes a setting affecting actions when input is flushed [S8]. Validate viewport focus loss in PIE and use the verified engine behavior or an explicit reset hook.

Use immediate continuous triggers appropriate to steering, climbing, and held Slow; an Enhanced Input timed Hold trigger can delay activation and is not the slowdown ramp. The ramp belongs in flight state. Avoid mixing input-event movement with tick movement, and remove only this system's Mapping Context during cleanup rather than clearing unrelated contexts.

### Pattern 4: Camera Consumes the Current Frame Before View Sampling

**What:** Flight runs in `TG_PrePhysics`; route-camera application runs in `TG_PostPhysics`, before the engine camera update. Register an explicit prerequisite from the route camera **component** to the flight component. Epic documents cameras updating after `TG_PostPhysics` and before `TG_PostUpdateWork` [S5].

**When:** Startup binding of a valid flight source, and whenever that source is changed later.

**Trade-off:** Tick dependencies are cheap and explicit but belong to the tick function that executes the work. Adding a dependency only to the camera Actor does not order its owned component [S6]. Differing groups already order these ticks; the explicit component prerequisite documents the data dependency and protects against later regrouping.

Proposed wiring:

```cpp
// UFlightMovementComponent constructor
PrimaryComponentTick.TickGroup = TG_PrePhysics;

// URouteCameraComponent initialization, after resolving a valid flight source
PrimaryComponentTick.TickGroup = TG_PostPhysics;
AddTickPrerequisiteComponent(FlightMovement);
```

Cache the camera's authored transform once, before route translation, and use the movement component's recorded startup ship X. Synchronize the camera immediately when binding; do not capture a new ship/camera baseline on every tick. Resolve references during startup with validity checks, rather than relying on one Actor's `BeginPlay` occurring first. Select the placed camera after possession has established the local controller, using zero blend time for initial setup [S7]. Inspect existing auto-camera management and camera Blueprint ticks if the view target changes unexpectedly.

## Data Flow

### Per-Frame Update Order

1. Enhanced Input evaluates action assets; callbacks update steer/climb/held-Slow state.
2. Flight tick ramps `SlowAlpha`, computes independent world-axis displacement, and clamps proposed height.
3. Flight applies movement; the actual transform becomes the committed state. Determine vertical velocity from the actual height change, then settle pitch/bank toward their targets.
4. Route camera runs after flight and copies committed progress into its authored X baseline; Y/Z and viewing rotation retain their startup values.
5. Engine samples the selected camera for the frame.

This is deterministic update ordering, not a claim of deterministic cross-machine floating-point simulation. Nothing in this cut requires lockstep networking, a fixed-step simulation framework, physics flight, or replication.

### State Ownership

| State | Owner | Consumers |
|---|---|---|
| Action asset references / Mapping Context | Ship input boundary | Enhanced Input setup |
| Steer, climb, held Slow | Flight component, set through command methods | Flight update |
| Speeds, height bounds, ramp timing, pose tuning | Editable flight properties | Flight update and editor |
| Slow alpha, initial ship X, committed progress, pose state | Flight component | Camera reads committed progress only |
| Initial camera transform / active flight reference | Camera component | Camera assignment |
| Scene art and camera projection | Existing level/Blueprint authoring | Rendering |

There is no separate game state store or external service. Do not duplicate editable speeds on both ship and component; pick the component as the single tuning surface.

## Scaling Considerations

| Growth | Architecture adjustment |
|---|---|
| Current one-ship PIE cut | One module, flight component, placed camera component; constant per-frame work. |
| Later fortress systems | Add approved components incrementally; retain camera progress consumption and stationary scenery. |
| Later ship replacement / stage transitions | Decide how route progress survives replacement; migrate its writer deliberately while preserving one authority. |
| Very large future levels | Profile content streaming and precision only when demonstrated; no world-rebasing framework now. |

The immediate risks are duplicated Blueprint movement and late camera updates, not user-count scalability. Collision and full posed-hull clearance will require deeper research when they enter scope. Epic's movement helpers sweep only the `UpdatedComponent`; attached shapes are not automatically swept [S3]. Keep a clear hull boundary now without implementing crash rules, armor, hazards, or death state.

## Anti-Patterns

| Mistake | Consequence | Preferred architecture |
|---|---|---|
| Ship, camera, and manager each integrate progress | Drift and double advancement, especially through Slow | One flight writer; camera reads committed progress |
| Camera uses full Pawn transform or follows a spring arm inheriting pose | Steering/climb/pose changes the supposedly fixed composition | Placed camera with X-only assignment |
| Camera route tick in `TG_PostUpdateWork` | Camera may be sampled before its new transform | Apply route camera in `TG_PostPhysics` [S5] |
| Camera Actor prerequisite while work lives in its component | Component execution remains unordered | Set the prerequisite on that component [S6] |
| Hard-coded keys, default free-flight input, or movement in callbacks | Binding changes break behavior; movement may run twice | Enhanced Input action boundary and one movement tick |
| Normalize combined steer/climb input or multiply all speeds by Slow | Diagonal climb changes rate; slowdown violates unchanged climb | Independent Y/Z state, scale only forward/lateral |
| Modify native inheritance without inspecting Blueprint dependencies | Existing components/graphs and map wiring may break | Inspect editor assets, preserve work, then adapt the ship |
| Scaffold long-term manager, weapon, fuel, HUD, and fortress systems | Inflates the first cut and verification burden | Implement flight and route camera only |
| Move scenery to simulate scrolling while also moving ship/camera | Multiple scroll sources invalidate composition | Keep authored scenery stationary |

## Integration Points

### Internal Boundaries

| Boundary | Communication | Notes |
|---|---|---|
| Input → flight | Small command setters and reset helper | No raw hardware lookup in flight code |
| Flight → hull | Explicit movement/pose calls | `UpdatedComponent` must be movable and belong to the Pawn |
| Flight → route camera | Read-only committed progress getter | No independent camera speed or slowdown state |
| Ship startup → local controller | Existing possession and view-target APIs | Inspect current level wiring; choose one ship startup path |
| Native rules ↔ Blueprint assets | Reflected editable properties and asset references | Blueprint supplies art/tuning; inspect and eliminate duplicate rule execution |

No external service, database, or new plugin architecture is needed.

## Build Order and Roadmap Implications

1. **Inspect and establish PIE ownership.** Verify BP parent/component hierarchy, current map actors, existing movement graphs, ship orientation, camera and view target; resolve the local engine/editor target. Adapt the existing scaffold to the Pawn role without replacing assets blindly. This is the highest asset-dependent research risk.
2. **Deliver flight as one coherent slice.** Add native Enhanced Input dependency and three action assets/context; implement automatic +X, independent Y/Z, editable speeds and height limits, pitch/bank, Slow ramp, and terminal-event/lifecycle resets. One component owns translation. Avoid splitting Slow across unrelated phases because camera correctness depends on its final progress.
3. **Deliver synchronized camera and integrated PIE validation.** Bind the placed camera to the flight source, capture authored composition, establish tick order, select view target, and verify the complete flight/camera interaction. Camera scaffolding can occur earlier, but final verification depends on normal and Slow movement being present.

Acceptance evidence should include normal versus full-Slow forward/lateral rates, unchanged climb rate, each height limit, neutral pose recovery, action cancellation/focus loss, and constant camera-minus-ship X offset through press/hold/release. Also verify fixed camera Y/Z/rotation during steering and climbing, a single active ship, and no duplicate Blueprint movement. Observe PIE; successful compilation alone does not establish the architecture's behavior. Do not require packaged delivery or deferred fortress mechanics to complete this milestone.

## Confidence and Gaps

The research-plan seam selected Context7. No Context7 MCP or `ctx7` executable was available, so Epic's official online documentation supplied the fallback. The runtime command `query classify-confidence --provider websearch --verified` returned **MEDIUM**; this tier applies to the external architectural/API findings and recommendations. The parent explicitly excluded cache sidecar writes for this targeted review.

Epic's fetched pages identify Unreal Engine 5.8 and were checked on 2026-10-07; publication dates are not provided on these API pages. No engine association change follows from browsing them. Local source facts were inspected, while binary Blueprint/map wiring, local engine compatibility, component transform axes, input flush behavior, and actual PIE camera sampling remain implementation-time validation gaps. No source changes, build, editor run, or gameplay validation were performed for this research.

## Sources

All external references are official Epic documentation, accessed 2026-10-07, confidence **MEDIUM** from the verified websearch classification. Recommendations and formulas specific to Vyxxen are design inferences from the approved project requirements, not additional Epic guarantees.

- **[S1]** [Pawn overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine) — controllable world entity and possession boundary.
- **[S2]** [UPawnMovementComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UPawnMovementComponent) — Pawn association and `SetUpdatedComponent` restriction.
- **[S3]** [UMovementComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UMovementComponent) and [MoveUpdatedComponent](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UMovementComponent/MoveUpdatedComponent?lang=en-US) — explicit updated component and movement operations.
- **[S4]** [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine) — actions, value types, contexts, trigger lifecycle.
- **[S5]** [Actor Ticking](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-ticking-in-unreal-engine) — tick groups, dependency ordering, and camera sampling position.
- **[S6]** [AActor::AddTickPrerequisiteComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/AActor/AddTickPrerequisiteComponent) — Actor prerequisites do not automatically order owned component ticks; [UActorComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UActorComponent) — component prerequisite API.
- **[S7]** [Set View Target with Blend](https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/Game/Player/SetViewTargetwithBlend) — existing PlayerController selects the camera actor.
- **[S8]** [UEnhancedInputDeveloperSettings](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/EnhancedInput/UEnhancedInputDeveloperSettings) — input-flush event behavior has an explicit setting.
- **Local scope/evidence:** `.planning/PROJECT.md`, `.planning/codebase/ARCHITECTURE.md`, `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`, `Source/Vyxxen/Vyxxen.Build.cs`.
- **Long-term design context:** `C:/Users/Breno/My Drive/Vertex/{ Unreal AI }/Games/Vyxxen/Design/Vyxxen_First_Stage.md` — used for future compatibility; current milestone exclusions take precedence.

---
*Architecture research for Vyxxen's first flight-and-camera implementation cut.*
