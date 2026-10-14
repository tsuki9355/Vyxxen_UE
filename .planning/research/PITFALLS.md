# Pitfalls Research

**Project:** Vyxxen — first flight-and-camera cut
**Domain:** Unreal Engine arcade ship controls and an X-only route camera, delivered through Play In Editor
**Researched:** 2026-10-07
**Confidence:** MEDIUM

## Scope and Evidence Boundary

This research covers automatic world +X flight, independent Y steering and Z climb, mesh bank/pitch, directly editable movement/height tuning, ramped 40% forward/lateral slowdown, input cleanup, and camera synchronization. Firing, fuel, combat, hazards, scoring, multiplayer, and packaging are outside this cut.

Local evidence comes from `.planning/PROJECT.md`, `.planning/codebase/CONCERNS.md`, `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`, `Source/Vyxxen/Vyxxen.Build.cs`, and `Vyxxen.uproject`. The native ship currently derives from `AActor` and contains empty lifecycle scaffolding; its module does not declare `EnhancedInput`. These are observed integration gaps, not confirmed runtime bugs. The existing ship Blueprint, main map, World Settings, mesh orientation, and camera wiring have not been inspected in the editor.

`Vyxxen.uproject` associates with **5.8**. Epic's current retrieved documentation is labeled Unreal Engine 5.8, but no installed headers, toolchain, build, editor load, or PIE run was verified here. Documentation describes intended engine behavior; exact local API compatibility and asset behavior remain execution checks. Official sources generally have no displayed publication date; retrieval/search freshness was checked on the research date. No release-note-specific compatibility claim is made.

The GSD research-plan seam selected Context7. Context7 tools and the `ctx7` CLI were unavailable, so official Epic web search/API pages supplied the fallback. `query classify-confidence --provider websearch --verified` returned **MEDIUM**; all external findings and derived recommendations below use that tier. Local text observations are explicit facts; binary/runtime risks remain unverified. Phase labels below are suggested responsibilities, not imposed roadmap numbers.

## Critical Pitfalls

### Pitfall 1: Commands stick after release, cancellation, or focus loss

**What goes wrong:** A released steering/climb action continues moving the ship; Slow remains requested after the player releases it outside the PIE viewport. Mesh pose can remain tilted because the cached command never returns to neutral.

**Why it happens:** Code caches action values while handling only activation. `Completed` and `Canceled` represent different action transitions. Engine pressed-key flushing and an application's cached flight intent are separate state concerns. A delayed Hold trigger can also turn a simple hold-to-slow requirement into an unintended minimum press duration. [Epic's trigger-event API](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent) and [input settings API](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UInputSettings) support the distinction; they do not prove the existing project handles it.

**How to avoid:** Clear steering/climb values and the Slow request on both ending events. Keep the ending handler independent of the final action value. Reset cached intent on loss of possession, removal of this flight context, and the applicable focus/input lifecycle. For ordinary hold-to-slow, use a boolean pressed state without requiring a dwell-time Hold trigger unless explicitly wanted. Releasing Slow sets the ramp target back to normal; it should not leave a boolean latched. A single input-reset helper makes the lifecycle paths consistent.

**Warning signs:** Release works while focused but fails after Shift+F1 or switching windows; a quick Slow tap differs from a long press; restarting PIE changes the behavior; input remains nonzero in debug values after release.

**Recommended verification:** Hold each command, release, and observe zero intent. While holding steer/climb/Slow, release viewport capture or change window focus, release the physical control outside the viewport, then return. Repeat with two commands held together. Exercise a cancellation/lifecycle path deliberately rather than claiming ordinary release proves cancellation. Confirm the ship resumes normal-speed targeting and pose returns toward level without another key press.

**Phase to address:** Input and possession integration, with lifecycle regression checks in final PIE acceptance. **Confidence: MEDIUM; conditional project risk.**

---

### Pitfall 2: A compiling native ship is not the possessed playable ship

**What goes wrong:** Automatic flight appears but input has no effect, a default pawn receives commands instead, or two ships exist. Reworking the `AActor` base into a Pawn also risks conflicting Blueprint movement, stale inherited nodes/components, or lost authoring defaults.

**Why it happens:** Possession, input component binding, local-player Mapping Context activation, default pawn spawning, and placed-actor setup are different links. The current C++ class is an Actor scaffold; the binary Blueprint's parent and graph behavior are unknown. Pawn input setup occurs upon possession, so an unchecked `BeginPlay` controller assumption is fragile. [Pawn input setup](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/APawn/SetupPlayerInputComponent) and [Pawn possession properties](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/APawn) describe these links.

**How to avoid:** Inspect the existing ship Blueprint and MainGame World Settings before defining migration. Choose one spawn/possession route for this cut: the intended placed pawn or the intended GameMode default pawn. Bind conceptual Input Actions through the possessed Pawn input component; activate the Mapping Context for the actual local player. Add `EnhancedInput` to the native module dependency when its APIs are used. Retain existing authored components and overrides intentionally, remove duplicate movement ownership, and compile/save the Blueprint after structural changes. Perform a fresh editor build/reopen for acceptance following a base-class migration rather than relying entirely on a patch session.

**Warning signs:** `GetPawn()` names a different actor; possession callback/input setup is never reached; action/context references are empty; C++ settings differ from Blueprint defaults; duplicate ticks move the same ship; behavior disappears after editor restart.

**Recommended verification:** In MainGame PIE identify the PlayerController, its one intended pawn, active context, and action callbacks. Confirm the visible mesh is owned by that pawn. Restart PIE and reopen the editor after the migration, then compile the Blueprint and repeat. Preserve original asset authoring while comparing before/after settings.

**Phase to address:** Existing asset inspection and native/Pawn integration before movement acceptance. **Confidence: MEDIUM; local scaffold/dependency evidence confirmed, Blueprint outcome unknown.**

---

### Pitfall 3: Camera speed matching hides drift or a one-frame lag

**What goes wrong:** Camera and ship start aligned but slowly separate, change spacing during slowdown, or jitter when speed changes. Following the whole ship transform also moves camera Y/Z or inherits bank/pitch, breaking the fixed view.

**Why it happens:** Two ticks independently integrate supposedly identical speed, maintain separate Slow ramps, or execute in an unspecified order. Camera lag/interpolation introduces separation by design. Attaching a camera normally to the ship inherits more than X translation. A follower component prerequisite on the actor tick alone does not necessarily order that component's own tick. Epic documents prerequisites and camera-update scheduling in [Actor Ticking](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-ticking-in-unreal-engine); its [actor prerequisite node](https://dev.epicgames.com/documentation/unreal-engine/BlueprintAPI/Actor/Tick/AddTickPrerequisiteComponent) explicitly distinguishes owned component dependencies.

**How to avoid:** Derive camera X from the ship's resulting authoritative forward position and the authored starting offset, for example `CameraX = StartCameraX + (ShipX - StartShipX)`. Preserve the camera's starting world Y/Z and viewing rotation. Do not integrate camera speed or apply a second Slow ramp. Make the tick that actually updates the follower depend on the tick that actually finalizes ship movement, or publish the final displacement directly within the same update flow. Complete the follower before engine camera evaluation; `TG_PostUpdateWork` is after camera updates and is a poor default for this follower. Use the placed route camera as the active view target and verify the rendered view, not only that actor's transform. [Set View Target](https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/Game/Player/SetViewTargetwithBlend) provides the explicit controller link.

**Warning signs:** Camera–ship X offset changes; follower reads the previous frame's ship location; camera actor coordinates are correct but PIE shows another view; turning/climbing alters view position or angle; offset jumps when PIE begins at a nonzero X.

**Recommended verification:** Observe/log `(CameraX - ShipX) - InitialOffset` after both updates and confirm it remains within the chosen floating-point tolerance. Also confirm constant camera Y/Z/rotation. Exercise normal flight, both Slow ramps, repeated toggles, simultaneous steer/climb, and an authored start location away from X=0. Check rendered stability at multiple frame rates and after a long enough run to expose accumulated drift.

**Phase to address:** Camera synchronization after authoritative movement exists, repeated in final acceptance. **Confidence: MEDIUM; invariant is a project requirement and implementation recommendation.**

---

### Pitfall 4: Visual rotation changes movement directions

**What goes wrong:** Climbing diverts automatic forward progress into Z; banking produces diagonal lateral motion; steering yaws instead of rolling; the mesh's imported axis causes the ship to tilt about the wrong apparent axis.

**Why it happens:** Movement uses a rotated `GetActorForwardVector()`/local offset while pose rotates the movement root. Imported meshes can have an orientation correction that differs from their parent. A rotation's field order and axis meaning are also easy to confuse. Epic specifies a Z-up world, X forward, Y lateral in [Coordinate System and Spaces](https://dev.epicgames.com/documentation/en-us/unreal-engine/coordinate-system-and-spaces-in-unreal-engine); [TRotator](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/TRotator) defines Roll around X and Pitch around Y.

**How to avoid:** Treat world X/Y/Z translation as the flight contract. Keep a stable movement root and apply pose to a visual child/pivot, preserving the existing mesh's authored base transform. Confirm its nose aligns with the pose pivot's +X before choosing tilt signs. Roll follows steering intent; pitch follows climb intent; neutral intent drives pose back to its authored level. Clamp independent action values separately rather than normalizing a combined Y/Z vector: simultaneous full steering and climb must retain their own speeds. Raw-key names belong in the Mapping Context, not the native axis logic.

**Warning signs:** Forward X speed changes when pose tilts; diagonal input reduces climb speed; ship nose swings around Z; neutral pose erases the mesh's import correction; camera changes orientation with the mesh.

**Recommended verification:** Compare X progress while level, fully banked, and fully pitched. Test each signed input separately and both axes together. Inspect the mesh visually for correct nose-axis bank and climb pitch, then release both and confirm level recovery without a location jump.

**Phase to address:** Movement basis and visual pose implementation. **Confidence: MEDIUM; exact imported orientation/sign must be inspected.**

---

### Pitfall 5: Editable bounds are exposed but unsafe or ineffective

**What goes wrong:** The ship exceeds altitude limits during a hitch, spawns outside the allowed range, reverses automatically due to a negative speed, or produces a divide-by-zero ramp. Changing the Blueprint property appears to do nothing because a different instance/component owns the effective setting.

**Why it happens:** Editor metadata is mistaken for runtime validation, two height limits are not validated together, or code rejects movement only after reaching a limit instead of clamping the candidate position. Blueprint/placed-instance overrides can differ from native defaults. Epic's [Metadata Specifiers](https://dev.epicgames.com/documentation/unreal-engine/metadata-specifiers-in-unreal-engine) describes property-entry clamps; no individual numeric metadata tag establishes `MinHeight <= MaxHeight`.

**How to avoid:** Expose speeds, height bounds, and ramp timing directly on the one ship class/component that consumes them. State units (normally Unreal world units per second for speeds) and the altitude reference. For this world-axis cut, absolute world Z is the simplest recommendation; confirm that it matches MainGame authoring. Validate finite/nonnegative speeds, ordered finite bounds, and ramp durations before use. Define a visible safe policy for invalid values, such as reporting once and using sanitized effective values; do not silently rely on an invalid clamp interval. Clamp the proposed Z after displacement and constrain initial altitude. Define zero-duration ramp behavior explicitly, preferably immediate targeting; never divide blindly by duration.

**Warning signs:** Reversed Min/Max yields a different failure at different frame rates; a long frame passes through a height bound; edited values are duplicated in several classes; restarting PIE restores an unexpected native/default override.

**Recommended verification:** Edit valid speeds/bounds in the supported authoring location and observe their effect in PIE. Hold climb/descend at each limit; move inward again; start beyond a limit. Check equal bounds, reversed bounds, zero ramp timing, and invalid speed handling. Confirm X/Y flight continues at a Z limit. These checks may be manual or focused deterministic checks of the arithmetic once it exists.

**Phase to address:** Tunable movement and altitude constraints before camera acceptance. **Confidence: MEDIUM; validation policy/reference are recommendations needing implementation decisions.**

---

### Pitfall 6: Slow becomes 40% remaining speed, affects climb, or changes with frame rate

**What goes wrong:** Full Slow travels at 0.4 of normal instead of 0.6; climb also slows; lateral and forward ramps diverge; releasing snaps unexpectedly; quick alternating presses restart ramps from endpoints and pop the motion.

**Why it happens:** A 40% reduction is confused with a 40% multiplier, one uniform velocity scale affects every axis, movement is applied per input callback, or a fixed interpolation alpha is used per frame. An eased interpolation parameter is mistaken for an exact ramp duration. [FInterp to Constant](https://dev.epicgames.com/documentation/unreal-engine/BlueprintAPI/Math/Interpolation/FInterptoConstant) documents a constant rate with Delta Time; using it correctly is still an application choice.

**How to avoid:** Use one bounded Slow blend `s` from 0 to 1, with `m = 1 - 0.4*s`. Apply `m` only to forward and lateral speed; keep climb independently unchanged. Integrate movement once per update using elapsed seconds. Ramp from the current blend toward its target, including reversal midway. If tuning is expressed as duration, use a duration-based constant-rate/defined curve approach and specify whether it means a full 0-to-1 transition; do not promise an exact duration from an asymptotic interpolation speed. Camera consumes the resulting ship X and has no Slow state of its own.

**Warning signs:** Full Slow is too severe; Z displacement differs at steady Slow; behavior depends on input event frequency or FPS; short tap sequences cause sudden velocity changes; ramp time cannot be predicted from its editable field.

**Recommended verification:** At comparable initial conditions after each ramp settles, measure X and Y displacement over equal elapsed intervals: Slow/normal should be 0.6 within the declared measurement tolerance; climb should remain 1.0. Test ramp-in, ramp-out, reversal mid-ramp, repeated toggles, and 30/60/120 FPS where practical. During transitions compare the shared blend and camera offset, since short-window distance ratios include the ramp itself.

**Phase to address:** Movement timing and slowdown, then combined flight/camera acceptance. **Confidence: MEDIUM; ratios are derived from the approved requirement.**

---

### Pitfall 7: A successful code patch conceals stale asset/default state

**What goes wrong:** A Live Coding session looks playable but restart restores old settings or reveals Blueprint compile/reference errors. Existing placed objects retain constructor defaults from before the change.

**Why it happens:** Structural native changes, Blueprint inheritance, and constructor default changes are treated like ordinary function-body edits. Epic documents object reinstancing and notes that changed `.cpp` constructor defaults do not update existing objects in [Live Coding](https://dev.epicgames.com/documentation/unreal-engine/using-live-coding-to-recompile-unreal-engine-applications-at-runtime).

**How to avoid:** Use Live Coding for suitable iteration, then validate a fresh editor load/build after structural migration. Inspect inherited Blueprint components and intentionally preserve/reset overridden values. Ensure newly created actions/context and the migrated ship asset are saved and referenced by MainGame. Avoid interpreting a patch-session success as proof of durable project state.

**Warning signs:** Different behavior before/after reopening; null references only after restart; reinstanced class names in diagnostics; changed constructor defaults do not reach the placed ship.

**Recommended verification:** Build the editor target against the declared engine, reopen the project/map, compile affected Blueprints, and run the same flight checklist. Record current-source build/load results and the saved asset references.

**Phase to address:** Asset/native integration and final PIE delivery. **Confidence: MEDIUM; recommendation rather than a claim that Live Coding cannot migrate classes.**

---

### Pitfall 8: A plausible demo is accepted without testing the actual contract

**What goes wrong:** The player can fly briefly, so completion is claimed despite untested cancellation, speed ratios, altitude bounds, camera rotation, or map integration. File presence/generated binaries substitute for current build/play evidence.

**Why it happens:** Current codebase mapping established filenames/text only; no runtime behavior is validated. Manual viewing detects dramatic failures but can miss constant-offset drift and ratios. Adding deferred gameplay to make the demo look fuller distracts from flight verification.

**How to avoid:** Make successful current editor build/load and observed MainGame PIE behavior the acceptance evidence. Use a short repeatable checklist and lightweight numeric diagnostics for movement/camera invariants; add focused math regression checks where useful after implementation. Keep the first cut's acceptance limited to its authorized controls and camera. Packaging, fuel, firing, and combat do not belong in its completion gate.

**Warning signs:** Validation says only that classes/assets exist; tests run only at neutral input; Slow judged by feel; no focus/cancel scenario; the camera is inspected through the editor viewport rather than the active player view.

**Phase to address:** Acceptance criteria established early and executed after integration. **Confidence: MEDIUM for inferred risks; lack of validation is confirmed local evidence.**

## Technical Debt Patterns

All entries below are project-specific recommendations at **MEDIUM** confidence.

| Shortcut | Immediate Benefit | Long-term Cost | When Acceptable |
|----------|-------------------|----------------|-----------------|
| Integrate ship and camera speed independently | Two simple ticks | Drift, duplicated Slow logic, frame-order failures | Throwaway experiment only; violates final exact-progress contract |
| Bind raw physical keys in native flight logic | Quick keyboard demo | Conflates scene axes with device mapping; obstructs required Enhanced Input asset flow | Temporary debugging only |
| Keep separate native and Blueprint movement ticks | Avoid Blueprint cleanup | Double displacement, unclear tuning owner | Never for the accepted cut; inspect Blueprint first |
| Use several tuning owners or a new data-asset settings subsystem | Flexible configuration | Overrides become ambiguous; exceeds the requested direct editing model | Direct ship/component fields are sufficient for this cut |
| Rely on patch-session defaults | Fast iteration | Durable saved state remains unknown | Suitable iteration, followed by fresh-load acceptance |

## Integration Gotchas

There are no external service integrations required by this cut. Relevant boundaries are local Unreal systems; confidence is **MEDIUM**.

| Integration | Common Mistake | Correct Approach |
|-------------|----------------|------------------|
| Enhanced Input to native module | Input-class config treated as complete wiring | Declare module use, assign typed actions/context, activate local-player context, bind the possessed pawn |
| C++ to existing ship Blueprint | Replace the asset before inspecting its graphs/components | Inspect ownership and defaults, migrate deliberately, compile/save, preserve authoring |
| MainGame to pawn/controller | Placed ship and default pawn compete | One intended spawn/possession path, verified at runtime |
| Follower to movement component | Actor tick prerequisite used while follower work occurs in its component tick | Order the actual updating tick functions, before camera evaluation |
| Route camera to rendered view | Correct camera transform assumed to mean active player view | Verify the PlayerController view target and actual PIE view |

## Performance and Timing Traps

This is a single-ship PIE slice. No actor-count threshold or device budget has been measured; multiplayer/server scaling is irrelevant here. Confidence is **MEDIUM**.

| Trap | Symptoms | Prevention | When It Breaks |
|------|----------|------------|----------------|
| Fixed displacement/interpolation alpha per frame | Faster flight or tighter pose at high FPS | Seconds-based movement and defined ramp/pose time behavior | Any material frame-rate change |
| Discarding elapsed time by clamping DeltaTime without an explicit simulation policy | Slower real-time progress through hitches | Keep elapsed-time semantics clear; final-Z clamp is separate from time integration | Long frame/editor hitch; verify rather than promise exact timing |
| Per-frame actor searches or context rebuilding | Avoidable overhead, unclear lifetime | Resolve/cache intended ship/camera references at setup; activate context through lifecycle | Unnecessary even at this cut's size; no measured bottleneck asserted |
| Full existing renderer baseline mistaken for a movement defect | Uneven presentation obscures input/camera behavior | Measure frame timing during PIE; diagnose before changing rendering scope | Depends on the actual device/map; unmeasured |

## Security Mistakes

The existing concerns audit identifies a token field in `DefaultEngine.ini`. Its value was not read or reproduced for this research. Avoid pasting entire configuration files into research, debug output, or build reports. That existing housekeeping concern does not justify adding an online service or security subsystem to the flight milestone. No new network/security claim is established here.

## UX Pitfalls

Recommendations below have **MEDIUM** confidence and follow the approved interaction contract.

| Pitfall | User Impact | Better Approach |
|---------|-------------|-----------------|
| Sudden tilt or permanently banked neutral pose | Controls feel sticky or unstable | Visual target pose from intent, smooth return to authored level |
| Camera follows Y/Z or rotates with bank/pitch | Stable arcade framing is lost | Fixed authored Y/Z/rotation and shared X displacement |
| Slow delays until a dwell threshold | Holding Slow feels inconsistent | Ordinary boolean held state unless a threshold is explicitly wanted |
| Altitude cap stops all movement | Ship stalls when pressing climb at limit | Clamp Z while maintaining X and independent Y movement |
| Unknown PIE focus/controls | Correct integration looks broken | Document actual mappings and how to capture/release the PIE viewport |

## "Looks Done But Isn't" Checklist

- [ ] **Native integration:** Current source builds/loads with the declared engine; existing ship Blueprint compiles and is saved after migration.
- [ ] **Possession:** MainGame PIE controls the one visible intended ship; no competing default pawn or duplicate movement owner.
- [ ] **Automatic flight:** +X progresses without player input and retains the expected speed while visual pose changes.
- [ ] **Input lifecycle:** Each signed steer/climb input releases to neutral; cancellation and outside-viewport release are deliberately verified.
- [ ] **Independent axes:** Simultaneous Y/Z input preserves independent speeds; controls use actions/context, not raw keys in native gameplay.
- [ ] **Editable settings:** Authoring edits affect the actual ship; invalid/equal height bounds and zero ramp timing have defined behavior.
- [ ] **Altitude:** Initial and candidate Z stay constrained; reaching a bound permits moving inward and does not stop X/Y.
- [ ] **Pose:** Correct roll/pitch around the nose/right axes, preserved mesh base transform, neutral level recovery.
- [ ] **Slow:** Settled forward/lateral ratios are 0.6, climb ratio is 1.0; both ramps and mid-ramp reversals work across relevant frame rates.
- [ ] **Camera:** Constant X offset during ordinary flight and both Slow ramps; constant world Y/Z/rotation; active player view is the intended route camera.
- [ ] **Persistence:** Fresh editor reopening and repeated PIE produce the same behavior with saved assets/references.
- [ ] **Delivery scope:** Acceptance evidence addresses only the authorized flight-and-camera cut; no deferred system is silently required.

## Recovery Strategies

Costs describe relative effort for this cut, not measured estimates. Recommendations have **MEDIUM** confidence.

| Pitfall | Recovery Cost | Recovery Steps |
|---------|---------------|----------------|
| Sticky input | LOW–MEDIUM | Inspect ending events/lifecycle, centralize intent reset, reproduce outside-focus release |
| Wrong pawn or Blueprint migration conflict | MEDIUM–HIGH | Inspect original hierarchy/graph/defaults, identify controlled pawn, restore authored content deliberately, choose one ownership path |
| Camera drift/lag | LOW–MEDIUM | Remove separate integration/ramp, use final ship X plus initial offset, order actual follower tick, inspect active view |
| Pose alters flight axes | MEDIUM | Separate mesh/pivot pose from movement root; preserve import correction; retest signs and diagonal input |
| Unsafe tuning | LOW–MEDIUM | Validate consumed values/relations, clamp initial/final Z, define invalid/zero-duration policy, inspect instance overrides |
| Patch-only success | MEDIUM | Fresh build/load, Blueprint compilation, inspect saved references/defaults, repeat acceptance |

## Pitfall-to-Phase Mapping

These are candidate phase responsibilities for roadmap synthesis, not fixed phase numbers.

| Pitfall | Prevention Responsibility | Verification |
|---------|---------------------------|--------------|
| Possession and binary asset migration | Foundation/asset inspection | Intended pawn/controller/Blueprint identity, compile/save, restart |
| Sticky action state | Input integration | Release, Canceled path, focus-loss release, repeated PIE |
| World/local axis confusion | Flight and visual pose | +X invariant under tilt, signed Y/Z motion, independent simultaneous axes |
| Unsafe tuning and timestep handling | Movement/bounds/Slow | Property edits, limits, invalid inputs, duration behavior, FPS comparison |
| Slow percentage and synchronization | Movement/Slow | Settled 0.6 X/Y and 1.0 Z; ramp reversals |
| Camera drift or early read | Route-camera integration | Constant offset and Y/Z/rotation, ordered updates and active view |
| Stale patch/asset state | PIE delivery verification | Fresh build/load, Blueprint compile, saved assets, repeatable play checklist |

**Deeper research/inspection flags:** Existing Blueprint hierarchy, map overrides, imported mesh basis, and installed 5.8 API compatibility require phase-specific local inspection. Input focus-loss behavior requires reproduction in the actual PIE environment. World-axis arithmetic, a single Slow blend, and a shared camera-X invariant are standard implementation patterns and do not justify broad new ecosystem research.

## Sources

All official sources below were retrieved/checked on 2026-10-07 and carry **MEDIUM** confidence under the GSD verified-websearch classification. Recommendations are derived from documented semantics plus the project contract; no confirmed local runtime failure is asserted.

- [Epic — ETriggerEvent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent): Completed/Canceled semantics.
- [Epic — Enhanced Input](https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine): typed actions, Mapping Contexts, local-player subsystem. Search returned current documentation; an alternate version-query URL failed, so installed compatibility remains unverified.
- [Epic — UInputSettings](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UInputSettings): pressed-key flush behavior on viewport focus loss/input mode changes.
- [Epic — APawn::SetupPlayerInputComponent](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/APawn/SetupPlayerInputComponent): possession-driven binding lifecycle.
- [Epic — APawn](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/APawn): auto-possession and pawn properties.
- [Epic — Actor Ticking](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-ticking-in-unreal-engine): tick dependencies, groups, camera update ordering.
- [Epic — Add Tick Prerequisite Component](https://dev.epicgames.com/documentation/unreal-engine/BlueprintAPI/Actor/Tick/AddTickPrerequisiteComponent): actor dependency does not automatically establish owned component dependencies.
- [Epic — Set View Target with Blend](https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/Game/Player/SetViewTargetwithBlend): PlayerController view-target wiring.
- [Epic — Coordinate System and Spaces](https://dev.epicgames.com/documentation/en-us/unreal-engine/coordinate-system-and-spaces-in-unreal-engine): world/local basis and import-axis considerations.
- [Epic — TRotator](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/TRotator): Roll/Pitch/Yaw axes.
- [Epic — Metadata Specifiers](https://dev.epicgames.com/documentation/unreal-engine/metadata-specifiers-in-unreal-engine): editor numeric-entry metadata. Search retrieved the relevant table; alternate direct opens failed. No runtime relational-validation capability is inferred.
- [Epic — FInterp to Constant](https://dev.epicgames.com/documentation/unreal-engine/BlueprintAPI/Math/Interpolation/FInterptoConstant): constant-rate interpolation and Delta Time inputs.
- [Epic — Live Coding](https://dev.epicgames.com/documentation/unreal-engine/using-live-coding-to-recompile-unreal-engine-applications-at-runtime): object reinstancing and constructor-default caveats.
- Local `.planning/PROJECT.md`, `.planning/codebase/CONCERNS.md`, current native ship/module source, and `Vyxxen.uproject`: scope, observed scaffold/module state, association, and validation boundary.

---
*Pitfalls research for Vyxxen's first flight-and-camera cut. No source/assets were mutated, no build/editor session was run, and no commit was made.*
