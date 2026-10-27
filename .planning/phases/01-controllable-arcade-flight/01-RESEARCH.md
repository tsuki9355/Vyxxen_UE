<user_constraints>
## User Constraints (from CONTEXT.md)

### Phase Boundary
Deliver a playable Phase 1 flight-control slice in Unreal Editor PIE. The intended ship advances automatically along world +X while the player steers along Y, climbs or descends along Z, and can combine those inputs independently. Expose movement rates, world-Z height limits, pose tuning, and slowdown timing directly on the ship class or its movement component. Pitch and bank provide readable feedback; Slow ramps forward and lateral speed to 60% while leaving climb speed unchanged. Enhanced Input release, cancellation, and control loss must clear stored requests.

Firing and projectiles remain deferred. Exact X-synchronized camera movement and repeatable camera delivery are owned by Phase 2. Keep scenery stationary and do not add later fortress systems.

### Locked Decisions

### Default device bindings
- **D-01:** Map both WASD and arrow keys to the same semantic steer and climb actions.
- **D-02:** Include gamepad controls in the first cut. The left stick steers and climbs; pushing it forward/up increases altitude, matching W and Up Arrow.
- **D-03:** Slow is hold-to-use, bound to Left Shift on keyboard and LT/L2 on gamepad. Release and cancellation still follow the approved recovery and input-clearing behavior.

### Starting speed and height values
- **D-04:** Choose editable first-pass movement and height values, then tune them in PIE rather than treating them as measured arcade constants.
- **D-05:** Keep forward travel steady while lateral and vertical control feel responsive. Preserve independent rates when steering and climbing together; do not normalize the combined input.
- **D-06:** Set the editable minimum and maximum world-Z heights from the existing level's apparent playable height band. Preserve an authored starting height when one exists; otherwise start near the midpoint of that band.

### Pitch, bank, and slowdown response
- **D-07:** Keep pitch and bank moderate and clearly readable, responding to actual climb/descent and steer respectively.
- **D-08:** Make pitch and bank responsive with a brief smooth ease back toward neutral when their corresponding motion stops.
- **D-09:** Use an editable one-second Slow ramp toward the 60% X/Y speed level and the same one-second ramp back to normal on release. Climb/descent speed remains unchanged.

### the agent's Discretion
Choose concrete initial movement rates, height values within the inspected level band, and pitch/bank limits during PIE tuning to match these preferences. Preserve the authored ship start height if present; only use the band midpoint when no authored start height is available. Keep the values directly editable on the ship class or movement component.

### Deferred Ideas (OUT OF SCOPE)

No new deferred ideas arose during discussion. Fire/projectiles remain out of scope for this cut, and synchronized camera behavior remains assigned to Phase 2.
</user_constraints>

<phase_requirements>
## Phase Requirements

The requirement descriptions below are copied verbatim from `.planning/REQUIREMENTS.md` (lines 11–34); the planning support column identifies the research and inspection needed to implement and verify them.

| ID | Description | Research Support |
|----|-------------|------------------|
| MOVE-01 | “The ship advances automatically along world +X without requiring player input.” | One movement owner must advance X independently of action callbacks; verify in PIE at neutral input. |
| MOVE-02 | “The player can steer in both lateral directions along world Y while forward travel continues.” | World-axis movement and signed Steer action; inspect the mesh/root basis before tuning. |
| MOVE-03 | “The player can climb and descend along world Z while forward travel continues.” | Signed Climb action, editable Z rate, and editable world-Z bounds; inspect the authored start and level band in Editor. |
| MOVE-04 | “The player can steer and climb simultaneously without either input reducing the other's configured rate through diagonal normalization.” | Keep Y and Z commands independent and do not normalize their combined vector. |
| TUNE-01 | “The developer can edit forward, lateral, and climb speeds directly on the ship class or movement component, and those values control the corresponding movement rates.” | Expose reflected editor properties on the chosen ship/component surface; inspect class defaults and placed-instance overrides, then change one value at a time in PIE. |
| TUNE-02 | “The developer can edit minimum and maximum flight height on that class or component; the ship remains within those world-Z bounds and can move inward from either limit.” | Resolve the existing level height band and authored start first; test both bounds, inward motion, and continued X/Y motion. |
| POSE-01 | “The ship pitches with actual climb/descent and eases toward level when vertical movement stops, including at a height limit, without redirecting its world-axis translation.” | Drive the visual pose from actual clamped Z displacement/velocity, not just held input; keep the visual pivot separate from the world-axis movement basis. |
| POSE-02 | “The ship banks around its nose when steering and eases toward level when steering stops, without redirecting forward travel.” | Inspect mesh forward basis and apply bank to a visual pivot while movement remains world-axis based. |
| SLOW-01 | “Holding Slow ramps forward and lateral rates toward 60% of their normal values; release ramps them back to normal, using editable ramp timing on the ship class or component.” | Keep one normalized blend state in the movement owner; ramp from the current blend value toward held/released targets so mid-ramp reversals remain continuous. |
| SLOW-02 | “Climb/descent speed remains unchanged throughout slowdown and recovery.” | Apply the Slow multiplier to X/Y only; compare settled X, Y, and Z rates at equal input. |
| INPUT-01 | “Steer, Climb, and Slow are semantic Enhanced Input actions with their device bindings authored in a Mapping Context rather than raw key checks in gameplay C++.” | Use Enhanced Input action assets and a Mapping Context; bind the action values in the controlled Pawn, with no raw key polling. |
| INPUT-02 | “Releasing or canceling an action clears its stored control request; control/focus loss cannot leave steering, climb, or Slow latched when play resumes. Clearing Slow uses its normal recovery ramp.” | Clear state on Completed/Canceled and possession/context teardown; verify focus-loss flush and re-entry in PIE for both digital and analog controls. |
</phase_requirements>

# Phase 1: Controllable Arcade Flight — Research

**Researched:** 2026-10-07  
**Domain:** Unreal Engine 5.8 C++ arcade flight, Enhanced Input, Blueprint integration, and PIE verification  
**Confidence:** MEDIUM

## Summary

Use the installed Unreal Engine 5.8 toolchain and the built-in Enhanced Input plugin; no external package is needed. Keep one controlled Pawn and one movement owner. Input handlers should only update semantic scalar requests and a held Slow target; the movement owner applies world +X/Y/Z movement, height clamping, selective slowdown, and visual-pose signals. Expose movement, bounds, pose, and ramp properties directly on the ship class or movement component as requested. [CITED: [Enhanced Input in Unreal Engine](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine)] [VERIFIED: `.planning/ROADMAP.md:32`]

The local native scaffold is an `AActor`, and the Blueprint/map binary contents are not established by their filenames. The `.uproject` selects UE 5.8; the installed engine is 5.8.3 CL 58210709, with VS 2026, MSVC 14.50/14.51, and Windows SDK 10.0.26100 found locally. Those versions align with Epic's UE 5.8 recommendations, but this research did not establish a successful project build or PIE run. Blueprint parentage, effective class/instance tuning, actual height band, possession path, and runtime focus-loss recovery remain editor/runtime checkpoints for the plan. [VERIFIED: `Vyxxen.uproject:3`; `Source/Vyxxen/Public/PlayerShip.h:9-12`; local engine/toolchain probes] [CITED: [Unreal Engine 5.8 Release Notes](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5-8-release-notes?lang=en-US)]

**Primary recommendation:** Start with an Editor inspection/baseline task, then implement only the selected ship path after confirming Blueprint parentage, possession, effective overrides, mesh basis, and authored height band. Preserve authored content and validate through a current Editor build/load plus observed PIE behavior. [VERIFIED: `.planning/ROADMAP.md:32-34`; `.planning/REQUIREMENTS.md:73-79`]

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Device-to-command mapping | Enhanced Input asset/context | Controlled Pawn | Mapping Contexts associate hardware inputs and modifiers with semantic Input Actions; the Pawn binds those actions. [CITED: [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine); [Input Overview](https://dev.epicgames.com/documentation/unreal-engine/input-overview-in-unreal-engine)] |
| Automatic flight, steering, climbing, bounds, and Slow | Runtime movement component on the controlled Pawn | Pawn input handlers | One component owns the world-axis movement and tuning state; handlers only set command values. This is the project roadmap's chosen ownership direction. [VERIFIED: `.planning/ROADMAP.md:32`]
| Pitch and bank | Ship visual hierarchy/pivot | Movement component's actual motion feedback | Visual rotation must not alter the world-axis translation basis; pitch follows actual vertical motion, including a height cap. [VERIFIED: `.planning/REQUIREMENTS.md:23-24`; `.planning/ROADMAP.md:26`]
| Control-state cleanup | Controlled Pawn/shared input-state reset path | Enhanced Input action lifecycle and possession/context lifecycle | UE exposes Completed and Canceled action transitions; local request state still needs an explicit reset path. [CITED: [ETriggerEvent API](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent?lang=en-US)] |

## Standard Stack

### Core

| Library / component | Version | Purpose | Why standard |
|--------------------|---------|---------|--------------|
| Unreal Engine | Project association `5.8`; installed engine 5.8.3, CL 58210709 | Runtime, Editor, C++ build, and PIE | `Vyxxen.uproject` contains the exact line `"EngineAssociation": "5.8"`. The installed `Engine/Build/Build.version` contains `"MajorVersion": 5`, `"MinorVersion": 8`, `"PatchVersion": 3`, `"Changelist": 58210709`. [VERIFIED: `Vyxxen.uproject:3`; `C:/Program Files/Epic Games/UE_5.8/Engine/Build/Build.version:2-6`] |
| Enhanced Input plugin/module | UE 5.8 engine plugin | Semantic Steer, Climb, and Slow actions plus device mappings | Epic's current UE 5.8 guidance describes Input Actions, Mapping Contexts, modifiers, and per-player context application. The project already selects Enhanced Input classes in `DefaultInput.ini`. [CITED: [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine)] [VERIFIED: `Config/DefaultInput.ini:79-80`] |
| `APawn` and `UPawnMovementComponent` | UE 5.8 engine classes | Possessable ship and single movement owner | Epic documents Pawn as the base class for player/AI-controlled Actors and notes spawned Pawns are not automatically possessed. Confirm the existing map/GameMode path before altering class inheritance. [CITED: [Pawn in Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine)] |
| UnrealBuildTool module dependencies | Installed UE 5.8 | Declare native module dependencies | The current module's exact declaration is `PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });` and its private dependency declaration is `PrivateDependencyModuleNames.AddRange(new string[] {  });`. Add `EnhancedInput` at the narrowest visibility needed by its headers. [VERIFIED: `Source/Vyxxen/Vyxxen.Build.cs:11-13`] |

### Supporting

| Component | Version | Purpose | When to use |
|-----------|---------|---------|-------------|
| Reflected properties (`UPROPERTY`) | UE 5.8 | Expose movement/height/pose/Slow tuning in Details | Use an edit specifier matching the intended surface: Epic documents `EditAnywhere` as editable on archetypes and instances, and `EditDefaultsOnly` as archetypes only. Inspect whether placed instance overrides hide a changed class default. [CITED: [Unreal Engine UProperties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties)] |
| PIE Editor viewport | Installed Editor | Observe movement while Phase 2 camera work remains deferred | Use an observation view that can see the ship; Phase 1 does not need to deliver exact camera synchronization. [VERIFIED: `.planning/ROADMAP.md:32`]

### Alternatives Considered

| Instead of | Could use | Decision |
|------------|-----------|----------|
| Legacy/raw key checks | Enhanced Input Input Actions plus Mapping Context | Use Enhanced Input; raw keys in gameplay C++ conflict with locked `INPUT-01` and the project constraint. [VERIFIED: `AGENTS.md:16`; `.planning/REQUIREMENTS.md:33`] |
| Putting each movement rule in separate Actors or accumulating velocity in multiple places | One Pawn movement owner | Use one owner, which prevents competing movement and keeps the X/Y/Z rates and Slow multiplier coherent. [VERIFIED: `.planning/ROADMAP.md:32`]
| External flight/movement package | Project-specific movement component using engine primitives | Do not install a package: the required behavior is a small custom arcade policy over three independent world axes, and the repository is already a native Unreal C++ project. [VERIFIED: `Source/Vyxxen/Vyxxen.Build.cs:5-13`; `.planning/REQUIREMENTS.md:11-34`]

**Installation:** None. Use the engine's existing Enhanced Input plugin; no external packages are required. [VERIFIED: `Vyxxen.uproject:16-24`; `C:/Program Files/Epic Games/UE_5.8/Engine/Plugins/EnhancedInput/EnhancedInput.uplugin`]

## Architecture Patterns

### System Architecture Diagram

```text
Keyboard / gamepad
        │
        ▼
Enhanced Input Mapping Context ──► Steer / Climb axis actions + Slow bool
                                           │ action values / lifecycle events
                                           ▼
                                    Controlled Pawn
                               stores scalar requests/held target
                                           │
                                           ▼
                                One flight movement owner
                   ┌───────────────────────┼────────────────────────┐
                   ▼                       ▼                        ▼
             X forward rate          Y steer rate             Z climb rate
             Slow affects X          Slow affects Y            Slow unchanged
                   └───────────────────────┬────────────────────────┘
                                           ▼
                              Clamp resulting world Z to band
                                           │
                                           ▼
                        Apply world-axis translation + actual-motion feedback
                                           │
                                           ▼
                          Visual pitch/bank pivot eases toward neutral
```

The action assets/context are the device-binding boundary; the Pawn is the controlled entity; one movement owner applies all movement and returns actual clamped motion for pose. This is an implementation recommendation consistent with the phase requirements and roadmap, not evidence about the current Blueprint graph. [CITED: [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine)] [VERIFIED: `.planning/ROADMAP.md:32`]

### Recommended Project Structure

Preserve matching public/private class stems and existing project layout. A likely implementation shape, subject to the Editor asset inspection, is:

```text
Source/Vyxxen/Public/PlayerShip.h
Source/Vyxxen/Private/PlayerShip.cpp
Source/Vyxxen/Public/FlightMovementComponent.h
Source/Vyxxen/Private/FlightMovementComponent.cpp
Source/Vyxxen/Vyxxen.Build.cs
Content/Input/<Steer, Climb, Slow actions and flight Mapping Context>
```

This is a proposed mapping, not a claim that these files/assets already exist. Keep Enhanced Input includes private if public declarations do not expose its types; if an Enhanced Input type appears in a public header, declare the module with the visibility required by that public header. The current native ship header declares only `APlayerShip : public AActor`; inspect and preserve the actual Blueprint-controlled class path before adding or reparenting assets. [VERIFIED: `AGENTS.md:94-101,135-142`; `Source/Vyxxen/Public/PlayerShip.h:9-12`]

### Pattern 1: Inspect, then select the controlled Pawn path

**What:** Open the existing ship Blueprint and MainGame map in Unreal Editor. Inspect native/Blueprint parentage, components and graphs, mesh-forward basis, GameMode/default Pawn and placed-Pawn auto-possession, existing Mapping Contexts, class defaults, placed-instance overrides, and authored ship start/height band. Then choose one player-controlled Pawn path and one movement owner. 

**When to use:** Before changing inheritance, possession, input wiring, tuning, or visual hierarchy. Binary asset names and existence do not establish their internal graph or runtime state. [VERIFIED: `.planning/REQUIREMENTS.md:73`; `.planning/ROADMAP.md:32-34`]

Epic defines a Pawn as a controlled Actor, but a Pawn spawned during gameplay is not automatically possessed. The current native `PlayerShip.h` contains the exact declaration `class VYXXEN_API APlayerShip : public AActor`; therefore that native scaffold by itself is not a possessable Pawn. Do not infer that the Blueprint has the same parent. [CITED: [Pawn in Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine)] [VERIFIED: `Source/Vyxxen/Public/PlayerShip.h:9-12`]

### Pattern 2: Semantic inputs write requests; movement component owns motion

Create semantic `Steer` and `Climb` axis actions and a `Slow` bool action. Map both keyboard layouts and left-stick axes to those same actions in a flight Mapping Context. Use mapping-level modifiers where a source direction needs a sign change; tune the stick dead zone in the context, not in raw-key gameplay branches. Bind `Triggered` actions to update scalar requests, and clear each request on both `Completed` and `Canceled`. Epic documents Mapping Contexts as the place to associate actions with keys/buttons/axes and apply per-input modifiers. [CITED: [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine), [ETriggerEvent API](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent?lang=en-US)]

Keep movement rates on independent world axes: constant forward +X; signed steer on world Y; signed climb on world Z. Never normalize the combined Y/Z vector. Clamp the resulting world Z to the inspected band, not the input vector; then derive pitch from actual Z displacement/velocity so holding climb against a bound does not sustain false pitch. Apply bank and pitch to a visual pivot/mesh hierarchy so pose cannot rotate the movement basis. This is prescribed by the phase contract; the current mesh's local basis is unknown until inspected. [VERIFIED: `.planning/REQUIREMENTS.md:11-24`; `.planning/ROADMAP.md:24-27`]

### Pattern 3: One Slow blend, only X/Y are scaled

Maintain one normalized Slow blend that moves toward its held or released target from its current value. Use it to derive X/Y rates only; preserve the configured Z rate. This allows a release or re-press during a ramp without a discontinuous jump. The exact contract says, “Use an editable one-second Slow ramp toward the 60% X/Y speed level and the same one-second ramp back to normal on release. Climb/descent speed remains unchanged.” `[VERIFIED: .planning/phases/01-controllable-arcade-flight/01-CONTEXT.md, D-09]` A release/cancellation should set the target to normal and let that same ramp recover. Verify the full and intermediate ramp values in PIE. [VERIFIED: `.planning/REQUIREMENTS.md:28-29`; `.planning/ROADMAP.md:27`]

### Pattern 4: Tune the effective value, not only the C++ default

Expose rates, world-Z bounds, pitch/bank limits and response, and Slow duration on the selected ship or movement component. Epic documents `EditAnywhere` as editable on both archetypes and instances; a placed Blueprint/actor instance may therefore carry a value that differs from its class default. During PIE tuning, inspect the actual selected instance and reset/adjust stale overrides deliberately. Keep the level's authored start Z when present; if no start is authored, initialize near the midpoint of the measured playable height band. [CITED: [Unreal Engine UProperties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties)] [VERIFIED: `.planning/phases/01-controllable-arcade-flight/01-CONTEXT.md`, D-04/D-06]

## Don't Hand-Roll

| Problem | Don't build | Use instead | Why |
|---------|------------|-------------|-----|
| Device and semantic action routing | Raw key checks or a parallel legacy input system | Enhanced Input Actions and a Mapping Context | It is the locked action model and supports key/button/axis mappings and mapping modifiers. [CITED: [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine)] |
| Pawn ownership/possession | Treat an `AActor` or `.uasset` filename as proof of a playable Pawn | Existing Editor-confirmed Pawn/GameMode/placed-Pawn possession path | Possession is an explicit Controller/Pawn relationship; spawned Pawns are not automatically possessed. [CITED: [Pawn in Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine)] |
| Editor-tunable properties | Custom runtime settings menu or separate tuning service | Reflected class/component properties directly in Details | The user chose direct editing on the class or actor component; UProperties provide the Editor surface. [CITED: [Unreal Engine UProperties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties)] [VERIFIED: `AGENTS.md:15`]
| Arcade flight rules | Generic aerodynamic/character controller or third-party movement package | A small project-owned movement component using Unreal movement primitives | Required behavior is independent arcade rates, a height band, selective slowdown, and visual feedback—not aerodynamic simulation. [VERIFIED: `.planning/REQUIREMENTS.md:11-34,65`]

**Key insight:** The difficult risk is integration with the actual authored Blueprint/map and effective runtime state, not calculating three independent axis rates. Make the first plan task produce direct Editor observations before selecting the implementation seam. [VERIFIED: `.planning/ROADMAP.md:32-34`]

## Project Constraints (from AGENTS.md)

- Phase 1 scope is flight controls; the broader first cut also includes Phase 2's X-only route camera, while fortress systems remain out of scope. For this phase, exact synchronized camera work is deferred. [VERIFIED: `AGENTS.md:13`; `.planning/phases/01-controllable-arcade-flight/01-CONTEXT.md`, Phase Boundary]
- Use world axes +X forward, Y lateral, Z altitude. Scene axes do not imply raw-key bindings. [VERIFIED: `AGENTS.md:14`]
- Put speed and min/max height tuning directly on the ship class or actor component; no separate data-asset/settings system is needed for this cut. [VERIFIED: `AGENTS.md:15`]
- Use Enhanced Input actions and a Mapping Context; do not use raw key names in native gameplay logic; do not normalize independent steer/climb requests. [VERIFIED: `AGENTS.md:16`]
- Full Slow lowers X/Y by 40% and leaves Z unchanged; ramp timing is editable. Exact text: “Full slowdown reduces forward and lateral speed by 40%, with ramp timing editable. It does not reduce climb speed.” [VERIFIED: `AGENTS.md:17`]
- Keep scenery stationary. Inspect/reuse authored assets before changing them. The project favors one Pawn plus movement component; select the concrete controlled path only after Editor inspection. [VERIFIED: `AGENTS.md:19-20`]
- Use UE actor/component conventions, `VYXXEN_API`, public reflected declarations in `Source/Vyxxen/Public/`, and definitions in `Source/Vyxxen/Private/`. Keep module dependencies declared in `Vyxxen.Build.cs`; current include style is module-visible header names. [VERIFIED: `AGENTS.md:94-101,105,113-115,135-138`]
- Verify with current Editor build/load and observed PIE behavior. Generated binaries or asset filenames alone are not completion evidence. [VERIFIED: `AGENTS.md:21`; `.planning/REQUIREMENTS.md:71-78`]
- Execute edits through the active GSD workflow and preserve unrelated working changes. [VERIFIED: `AGENTS.md:243-253`; `.planning/REQUIREMENTS.md:79`]
- Do not expose the Android dev-server security token stored in project configuration; it is unrelated to Phase 1. [VERIFIED: `.planning/codebase/INTEGRATIONS.md`]

## Common Pitfalls

### Pitfall 1: Planning from filenames instead of Blueprint and map inspection

**What goes wrong:** A plan assumes the named ship Blueprint inherits the native scaffold, is possessed, owns input, or has no effective instance overrides. **Why:** Blueprint and map assets are binary and filenames reveal none of those facts. **Avoidance:** Make opening the actual assets in Editor the first implementation gate; record parent, components/graphs, GameMode/possession, active view, defaults/overrides, and authored start/band before edits. **Warning sign:** A plan assigns implementation to a class or map path without an Editor observation. [VERIFIED: `.planning/ROADMAP.md:32-34`; `.planning/codebase/ARCHITECTURE.md`]

### Pitfall 2: Assuming focus-loss key flushing clears game-owned values

**What goes wrong:** A stored steer/climb float or Slow-held bool remains latched after the viewport loses focus. **Why:** Engine flushing updates input key state; the game still owns its cached command variables. UE 5.8 source shows `UGameViewportClient::LostFocus` calls `FlushPressedKeys` when `bShouldFlushPressedKeysOnViewportFocusLost` is true (`GameViewportClient.cpp:2632-2647`). The project's `DefaultInput.ini` contains the exact line `bShouldFlushPressedKeysOnViewportFocusLost=True` (`Config/DefaultInput.ini:70`). The local Enhanced Input source initializes `bSendTriggeredEventsWhenInputIsFlushed` to `true` (`EnhancedInputDeveloperSettings.cpp:16`) and says flushed in-progress actions can emit Cancelled/Triggered events, while the false setting may leave key state retained (`EnhancedInputDeveloperSettings.h:101-108`). [VERIFIED: installed UE 5.8 source paths above; `Config/DefaultInput.ini:70`]

**How to avoid:** Bind both Completed and Canceled to a common request-clear helper, reset state on unpossess/end-play and context removal, and test PIE focus loss and return with keyboard and gamepad. Local engine code has special ignore-until-released logic for held digital mappings after flush; analog inputs are explicitly handled differently, so test analog reacquisition rather than assuming the digital behavior covers it (`EnhancedPlayerInput.cpp:1021-1066`). **Uncertainty:** this source inspection is not an observed PIE result; the project’s effective merged settings and actual action callbacks still require runtime verification. [VERIFIED: installed UE 5.8 source path above] [CITED: [UInputSettings API](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UInputSettings); [UEnhancedInputDeveloperSettings API](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/EnhancedInput/UEnhancedInputDeveloperSettings?lang=en-US)]

### Pitfall 3: Editing the wrong tuning layer

**What goes wrong:** A C++ or Blueprint class default changes but PIE behavior does not. **Why:** A placed actor or derived Blueprint can store overrides. **Avoidance:** Verify defaults and selected instance values in Details, then perform one-property-at-a-time PIE tuning and save/reopen the affected assets. [CITED: [Unreal Engine UProperties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties)] [VERIFIED: `.planning/REQUIREMENTS.md:18-19,73-78`]

### Pitfall 4: Rotating the movement basis or using requested input for pose

**What goes wrong:** Pitch/bank causes diagonal drift, forward speed changes, or pitch remains visible while held against a height limit. **Avoidance:** Move in world axes independently; apply pitch/bank to a visual child/pivot; derive pitch from actual clamped Z motion. Confirm the model's local forward direction in Editor before choosing rotation axes. [VERIFIED: `.planning/REQUIREMENTS.md:23-24`; `.planning/ROADMAP.md:26`]

### Pitfall 5: Scaling the wrong axes or creating an unreachable ramp

**What goes wrong:** Slow changes Z, multiplies a shared 3D vector, jumps on mid-ramp reversal, or approaches its target asymptotically instead of meeting the user-selected one-second duration. **Avoidance:** Keep one current blend alpha and target; apply only to X/Y. Move from the current alpha when the target changes and reach each target after the configured duration. Test press, release, and reversal at several ramp positions. [VERIFIED: `.planning/REQUIREMENTS.md:28-29`; `.planning/phases/01-controllable-arcade-flight/01-CONTEXT.md`, D-09]

### Pitfall 6: Guessing the height interval or overwriting authored start Z

**What goes wrong:** The ship is clamped into scenery, begins at the wrong altitude, or has no room to move away from a bound. **Avoidance:** Inspect the MainGame playable band and ship start in Editor, preserve its authored start, otherwise choose the midpoint, and tune limits during PIE. Test both bounds and inward movement. [VERIFIED: `.planning/phases/01-controllable-arcade-flight/01-CONTEXT.md`, D-04/D-06; `.planning/REQUIREMENTS.md:19,75`]

## Code Examples

The project contract says: “Full slowdown reduces forward and lateral speed by 40%, with ramp timing editable. It does not reduce climb speed.” The requirement says Slow targets “60% of their normal values.” Use a normalized ramp state and do not apply it to Z. [VERIFIED: `AGENTS.md:17`; `.planning/REQUIREMENTS.md:28-29`]

```text
On Steer/Climb Triggered: store the latest signed semantic action value.
On Slow Triggered/Started: set slow target to active.
On Steer/Climb Completed or Canceled: clear that axis request.
On Slow Completed or Canceled: set slow target to inactive.
On unpossess / action-context removal / teardown: clear all cached requests.

Each movement update:
  advance the current Slow blend toward its target using elapsed seconds;
  multiply X and Y rates by (1 - 0.40 * SlowBlend); preserve the normal Z rate;
  compute X, Y, and Z movement independently in world coordinates;
  clamp the resulting Z to the inspected minimum/maximum band;
  apply translation without allowing visual pitch/bank to rotate the movement axes;
  derive pitch from actual clamped Z motion, bank from steer, and ease both toward neutral.
```

The numerical `0.40` above is directly grounded in the quoted AGENTS contract; the target can also be computed as `0.60` from the verbatim SLOW-01 quote. Treat all concrete rate, bound, angle, and response-time values other than the locked one-second Slow ramp as first-pass PIE tuning choices, not established existing values. [VERIFIED: `AGENTS.md:17`; `.planning/phases/01-controllable-arcade-flight/01-CONTEXT.md`, D-04/D-06/D-07/D-09]

## State of the Art

| Older or unsuitable approach | Phase 1 approach | Evidence / impact |
|------------------------------|------------------|------------------|
| Direct raw-key gameplay branches or legacy action mapping | Enhanced Input semantic actions and a Mapping Context | UE 5.8 documentation describes Input Actions as semantic links to code and Mapping Contexts as device/action mappings applied to a local player. [CITED: [Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine)] |
| Assume input loss automatically clears arbitrary gameplay state | Use Completed/Canceled and explicit lifecycle resets, then verify focus recovery | UE exposes these trigger transitions; project state reset remains application-owned. [CITED: [ETriggerEvent API](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent?lang=en-US)] |

**Deprecated/outdated:** No project or Epic source inspected for this phase establishes a specific deprecated API. Avoid making deprecation claims. The project decision itself excludes raw key checks in gameplay C++. [VERIFIED: `AGENTS.md:16`]

## Assumptions Log

| # | Claim | Section | Risk if wrong |
|---|-------|---------|---------------|
| A1 | [ASSUMED] The intended playable ship may be an existing Blueprint child or wrapper distinct from native `APlayerShip`. | Architecture patterns | Reparenting the wrong class could break authored graphs or startup possession. |
| A2 | [ASSUMED] The existing MainGame level has a meaningful playable height band and possibly an authored start Z. | Pattern 4 / Common Pitfalls | Incorrect limits/start position can obstruct flight or intersect scenery. |
| A3 | [ASSUMED] A keyboard/gamepad Mapping Context or action assets may already exist in binary content. | Standard stack / Architecture | Duplicating them could create conflicting or redundant mappings. |
| A4 | [ASSUMED] The effective runtime values may be overridden at Blueprint class or placed-instance level. | Pattern 4 | Class-default edits may not affect PIE. |
| A5 | [ASSUMED] The ship mesh's local forward axis, pivot, and visual component hierarchy are not known from source declarations. | Pattern 2 / Pitfall 4 | Incorrect rotation axis or hierarchy can redirect movement or make pose unreadable. |
| A6 | [ASSUMED] Enhanced Input flush and focus restoration will emit the expected callbacks in actual PIE for both digital and analog controls. | Pitfall 2 | Held controls could remain latched or analog input could resume unexpectedly. |
| A7 | [ASSUMED] The selected controlled path can expose all requested tuning directly on its class/component without needing asset migration. | Standard stack | Existing Blueprint structure may require preserving a different component boundary. |

## Editor-Time Verification Prerequisites

No product or architecture question remains for planning. The following project facts are still unobserved and must be established in Unreal Editor during the listed implementation tasks; they are not claimed as resolved by this research.

| Unverified fact | Required Editor observation | Owning plan task |
|---|---|---|
| Actual possessed ship and MainGame path | Record Blueprint native parent, component tree/graphs, GameMode/default Pawn, placed-Pawn auto-possession, and runtime possessed Pawn before selecting the movement owner. | 01-01 Task 1 |
| Effective tuning and authored flight band | Inspect class defaults, child Blueprint defaults, placed-instance overrides, ship start Z, and surrounding playable scenery. Use the visible band and preserve an authored start. | 01-02 Task 2 |
| Mesh and visual-pivot basis | Verify model forward and rotation hierarchy before choosing pitch/bank axes; keep the movement root world-axis based. | 01-03 Task 1 |
| Focus-loss recovery | Verify effective Input/Enhanced Input settings; hold digital and analog controls, lose and restore PIE viewport focus, and confirm requests clear and Slow recovers. Engine source alone is not runtime evidence. | 01-03 Task 2 |
| Current Editor build/load | Run the Editor build/load after native and asset integration and record the observed result. Toolchain version alignment alone is not a build result. | 01-01 Task 1, then 01-02 Task 2 and 01-03 Task 2 after their changes |

## Environment Availability

| Dependency | Required by | Available | Version / observation | Fallback |
|------------|-------------|-----------|----------------------|----------|
| Unreal Engine Editor | Asset inspection, build/load, PIE | Yes | Project association `5.8`; installed `5.8.3`, CL `58210709` | No useful fallback for Blueprint/map verification; this is a required local Editor task. |
| Visual Studio / MSVC | Native C++ build | Yes | Local probes found VS version `18.10.12224.181`, MSVC `14.50.35717` and `14.51.36231` | Existing engine toolchain only; verify which compiler UBT selects during build. |
| Windows SDK | Native C++ build | Yes | Installed SDK folder `10.0.26100.0` | None needed based on UE 5.8 recommendations. |
| Enhanced Input | Semantic actions and Mapping Context | Yes | Engine plugin exists at `Engine/Plugins/EnhancedInput/EnhancedInput.uplugin`; `DefaultInput.ini` selects Enhanced Input player/input component classes. | No new package. Add module dependency if native headers are used. |
| Running Unreal Editor / PIE session | Runtime focus and movement tests | Not running during the probe [VERIFIED: local process probe] | No PIE behavior observed in this research pass. | Start the Editor and perform the planned PIE checks. |

Epic's UE 5.8 release notes recommend VS 2026, MSVC 14.50, and Windows SDK 10.0.26100.0; the discovered local versions meet those stated baselines. This is compatibility evidence only; successful build/load is still pending. [CITED: [Unreal Engine 5.8 Release Notes](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5-8-release-notes?lang=en-US)] [VERIFIED: local toolchain probes]

## Validation Architecture

### Test Framework

| Property | Value |
|----------|-------|
| Framework | No project-owned automated test framework/target was identified in the inspected project. [VERIFIED: `.planning/codebase/TESTING.md`] |
| Config file | None identified. |
| Quick run command | None established; perform focused checks in Unreal Editor PIE. |
| Full suite command | None established; the phase gate is current Editor build/load plus the complete manual PIE matrix below. |

### Phase Requirements → Test Map

| Req ID | Behavior | Test type | Automated command | Exists? |
|--------|----------|-----------|-------------------|---------|
| MOVE-01 | Automatic +X at neutral input | Manual PIE | None | No automated test |
| MOVE-02 | Both Y steer directions while X continues | Manual PIE | None | No automated test |
| MOVE-03 | Both Z directions and editable world-Z bounds | Manual PIE | None | No automated test |
| MOVE-04 | Simultaneous steer/climb retain independent configured rates | Manual PIE | None | No automated test |
| TUNE-01 | Each directly exposed rate changes only its corresponding movement | Manual Details + PIE | None | No automated test |
| TUNE-02 | Min/max limits hold; movement inward and X/Y continue | Manual PIE | None | No automated test |
| POSE-01 | Pitch follows actual climb/descent, eases neutral, and stops at bounds | Manual PIE | None | No automated test |
| POSE-02 | Readable bank and neutral return do not redirect X | Manual PIE | None | No automated test |
| SLOW-01 | One-second X/Y ramp to 60%, return, and mid-ramp reversal | Manual PIE with rate observation | None | No automated test |
| SLOW-02 | Z rate stays unchanged during Slow and recovery | Manual PIE with rate observation | None | No automated test |
| INPUT-01 | Keyboard layouts and gamepad feed the same semantic actions | Manual PIE, per device | None | No automated test |
| INPUT-02 | Release/cancel/focus loss/unpossess clears all state; Slow recovery remains smooth | Manual PIE, digital and analog | None | No automated test |

### Sampling and Phase Gate

- **Per task:** Compile/load the affected C++ or Blueprint asset, then perform the focused PIE check for the changed behavior.
- **Per wave:** Run the full movement/input matrix after integrating all actions and movement, including focus loss and mid-ramp reversal.
- **Phase gate:** Current Editor build/load and observed PIE evidence; preserve results as a concise checklist or verification record. Compilation alone is insufficient. [VERIFIED: `.planning/REQUIREMENTS.md:71-79`]

### Wave 0 Gaps

- No automated test framework/target was identified; do not create a test framework solely to mirror the movement implementation. Create a manual PIE acceptance checklist that names the twelve requirements, devices, rate observations, height-bound checks, and focus-loss sequence.
- First verification prerequisite is Editor inspection of the actual ship Blueprint and MainGame map, followed by a baseline current Editor build/load and a visible observation view. [VERIFIED: `.planning/ROADMAP.md:32`; `.planning/codebase/TESTING.md`]

## Security Domain

Phase 1 is local single-player movement with local input and Editor-authored tuning; no authentication, session service, access-control boundary, cryptography, network message, or persistence feature is part of the requirements. [VERIFIED: `.planning/REQUIREMENTS.md:56-67`]

| ASVS category | Applies | Standard control |
|---------------|---------|------------------|
| V2 Authentication | No | No identity/login feature in this phase. |
| V3 Session Management | No | No application session feature in this phase. |
| V4 Access Control | No | No multi-user/server access boundary in this phase. |
| V5 Input Validation | Yes, narrowly | Treat editor-tunable rates and bounds as configuration: reject non-finite/negative rates, ensure a valid min/max interval, clamp input action values to the expected axis range, and clamp world Z. This avoids invalid motion values, not a security boundary. |
| V6 Cryptography | No | No secret or cryptographic behavior is part of this phase. |

| Threat pattern | STRIDE | Mitigation |
|---------------|--------|------------|
| Invalid axis/tuning values cause excessive or undefined movement | Tampering | Clamp semantic input and resulting world Z; guard invalid rates/bounds in the movement component. |
| Focus loss leaves movement request active | Denial of service / state integrity | Clear stored input state on action cancellation/completion and control lifecycle loss; validate return-to-PIE behavior. |

Do not print or copy the unrelated Android dev-server security token while inspecting configuration. [VERIFIED: `.planning/codebase/INTEGRATIONS.md`]

## Sources

### Primary (HIGH confidence)

- Installed UE 5.8 source: `GameViewportClient.cpp:2632-2647`, `InputSettings.h:82-90`, `EnhancedInputDeveloperSettings.cpp:16`, `EnhancedInputDeveloperSettings.h:101-108`, `EnhancedPlayerInput.cpp:1021-1066` — local focus-loss flush, Enhanced Input event-on-flush default, and digital/analog handling.
- Project requirements/context/AGENTS and source files, cited inline — authoritative user constraints and current native/config declarations.

### Official Epic documentation (CITED)

- [Enhanced Input in Unreal Engine](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine) — semantic actions, value types, Mapping Contexts, mapping-level inputs/modifiers, local-player subsystem.
- [Input Overview in Unreal Engine](https://dev.epicgames.com/documentation/unreal-engine/input-overview-in-unreal-engine) — action binding to input components and trigger events.
- [ETriggerEvent API](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent?lang=en-US) — `Triggered`, `Completed`, and `Canceled` transitions.
- [UInputSettings API](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UInputSettings) — focus-loss key flushing setting.
- [UEnhancedInputDeveloperSettings API](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/EnhancedInput/UEnhancedInputDeveloperSettings?lang=en-US) — events when input is flushed.
- [Pawn in Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine) — Pawn control role and no automatic possession for spawned Pawns.
- [Unreal Engine UProperties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties) — archetype/instance editing semantics.
- [Unreal Engine 5.8 Release Notes](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5-8-release-notes?lang=en-US) — UE 5.8 Windows toolchain baselines.

### Secondary / tertiary

- None used for technical recommendations. No package or non-Epic movement library is recommended.

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH — project files and installed engine were inspected; official UE 5.8 docs confirm Enhanced Input and editor property behavior.
- Architecture: MEDIUM — the recommended Pawn/component/input ownership follows project constraints and official Unreal concepts, but Blueprint/map runtime ownership is unverified.
- Pitfalls: HIGH for UE 5.8 engine-source focus-flush behavior; MEDIUM for project-specific PIE risk until it is tested.
- Toolchain compatibility: MEDIUM — installed versions align with the official UE 5.8 recommendations; no build was run in this research pass.

**Research date:** 2026-10-07  
**Valid until:** 2026-11-06 for stable Unreal architecture guidance; re-check the installed engine/toolchain if the project engine association changes.
