# Feature Research

**Project:** Vyxxen — first flight-and-camera cut
**Domain:** Single-player 3D arcade flight controls, playable in Unreal Editor
**Researched:** 2026-10-07
**Confidence:** MEDIUM — reused official Epic findings from the completed sibling research; `query classify-confidence --provider websearch --verified` returned MEDIUM. Feature scope comes directly from the approved PROJECT.md, rather than inferred market expectations. Runtime behavior remains unverified.

## Feature Landscape

This milestone validates one experience: steer and climb a continuously advancing ship while a fixed-angle camera shares its exact forward progress. The approved project scope overrides the broader first-fortress design. All features listed as active below are required; the differentiators are already approved characteristics of those controls, not optional additions.

### Table Stakes (Required for This Cut)

| ID | Feature | Why Expected | Complexity | Notes |
|----|---------|--------------|------------|-------|
| FLT-01 | Automatic forward travel | Forward progress is the core arcade flight behavior. | LOW | Ship moves along world +X without player input. Forward speed is editable on the ship class or movement component. Pitch and bank do not redirect travel. |
| FLT-02 | Independent steer and climb | Player must move laterally and change altitude while advancing. | MEDIUM | Enhanced Input actions drive Y steering and Z climb/descent. Simultaneous inputs retain their independent rates; do not normalize steer/climb together. Bind hardware through the Mapping Context. |
| FLT-03 | Editable speeds and height bounds | Developer needs to tune the flight directly in Unreal. | LOW | Forward, lateral, and climb speeds plus minimum/maximum height live on the ship class or movement component. Height bounds constrain Z. No separate settings or data-asset system is required. |
| FLT-04 | Pitch and bank with return to level | Visible pose communicates climb and steering. | MEDIUM | Steering banks around the nose/X axis; climb/descent pitches around Y. Neutral input eases the pose toward level. At a height limit, stopped climb should ease pitch toward level, consistent with the design. |
| FLT-05 | Ramped hold-to-slow | The approved controls include selective speed reduction. | MEDIUM | Holding Slow ramps toward a **40% reduction**, leaving **60% of normal forward and lateral speed**. Release restores normal speed through the ramp. Climb speed is unchanged. Ramp timing is editable. Fuel behavior is excluded. |
| FLT-06 | Input release and cancellation reset | Controls must stop responding when the action ends. | MEDIUM | Completed/Canceled handling clears steer, climb, and the held-slow request. Slow speed recovers through its normal release ramp; clearing the request must not bypass the ramp. |
| FLT-07 | Exact X-only camera following | The ship must remain framed through normal and slowed forward travel. | MEDIUM | Ship and camera consume one forward position/progress. Camera X preserves the initial X offset; camera Y, Z, and viewing rotation remain fixed. Camera ignores steering, altitude, pitch, and bank. Scenery stays authored in place. |
| FLT-08 | Start and play in Unreal Editor | Play In Editor is the selected delivery target. | MEDIUM | Existing ship/map assets must be inspected and wired so the player controls the intended ship and views the synchronized camera after starting PIE. Build success and asset presence alone do not prove this feature. |

Complexity estimates are planning judgments with MEDIUM confidence, not measured implementation durations. Existing Blueprint inheritance, possession, mapping assets, and camera wiring can change integration effort.

### Differentiators (Approved Control Characteristics)

| Feature | Value Proposition | Complexity | Notes |
|---------|-------------------|------------|-------|
| Selective, gradual slowdown | Changes forward pace and lateral response while preserving vertical control. | MEDIUM | Part of FLT-05, mandatory for this cut. The full effect is a 0.60 speed multiplier, not a 0.40 multiplier. No resource cost is implemented. |
| Fixed route-camera composition | Ship steering and altitude remain visible against the stationary scene. | MEDIUM | Part of FLT-07, mandatory. Equal average speed is insufficient: the camera must use the same committed X progress on each update. |
| Direct editor tuning | Speeds, altitude limits, and ramp timing can be adjusted without modifying gameplay code. | LOW | Part of FLT-03/05. Class/component Details are the tuning interface; no player-facing settings menu is implied. |

These are project-specific distinctions from generic free-flight behavior. This research makes no claim about competitive uniqueness or market demand.

### Anti-Features (Deliberate Exclusions)

| Feature / approach | Why It Might Be Requested | Why It Is Excluded Here | Current Approach |
|--------------------|---------------------------|-------------------------|------------------|
| Firing, projectiles, muzzle aiming, and aim cursor | Present in the longer-term fortress design. | User explicitly selected flight controls only. | Implement only Steer, Climb, and Slow input actions for this cut. |
| Fuel capacity, drain, refill, and slowdown fuel penalty | Slowdown eventually interacts with fuel airpods. | Resource gameplay is deferred; its 20% drain penalty is not a prerequisite for flight slowdown. | Slow affects forward/lateral rates only. |
| Enemies, turrets, missiles, hazards, armor, damage, and crash rules | Required for a future fortress encounter. | They create combat and failure-state dependencies outside the approved slice. | Validate movement and framing in the existing scene without adding those systems. |
| Scoring, rewards, plane count, lives, respawn, and stage completion | Broader arcade loop needs progression. | Current completion is usable editor flight; there is no active stage-loop requirement. | Use the existing map as the PIE verification scene. |
| New HUD, altimeter, aim indicator, shadow system, or cosmetic effects | May improve a later gameplay presentation. | Not part of the approved first-cut requirements. | Verify height and motion with the existing view and editor observations; do not add presentation requirements. |
| Camera follow on Y/Z, rotation tracking, or X smoothing | Common third-person camera behavior. | Would violate the fixed composition or exact shared progress requirement. | Preserve initial camera Y/Z/rotation and assign X from shared progress. |
| Moving scenery as a conveyor | Can visually suggest forward flight. | Conflicts with the selected world model and can introduce another scrolling source. | Move ship and camera through stationary authored scenery. |
| Aerodynamic flight, normalized diagonal control, or heading-based translation | Common in general flight templates. | Would change the independent world-axis arcade controls. | Automatic +X, independent Y steering/Z climb, with pose expressing input. |
| Input-rebinding UI, generalized settings assets, multiplayer, behavior trees, and packaged delivery | Useful in larger products. | No approved requirement depends on these systems. | Enhanced Input assets, direct component tuning, and local PIE delivery suffice. |

Exclusions are scope decisions, not assertions that Unreal cannot support these features. They must not appear as tasks, stubs, or acceptance gates in this milestone.

## Feature Dependencies

```text
FLT-08 playable editor integration
    requires inspected ship/map, player control, mapping context, and view target

FLT-01 automatic progress + FLT-03 editable rates
    -> FLT-02 independent steering/climb
        -> FLT-04 visible pitch/bank
        -> FLT-05 selective slowdown
        -> FLT-06 terminal-event reset

FLT-01 committed forward progress
    -> FLT-07 exact X-only camera following

FLT-05 slowdown transitions
    -> FLT-07 synchronization verification during hold/release

All FLT-01..08
    -> observed, usable PIE flight slice
```

### Dependency Notes

- **Editor integration is a foundation and a final gate:** inspect existing binary assets before changing inheritance or wiring. Establish a controllable ship and selected camera early, then validate the complete slice through PIE at the end.
- **Controls require a live input path:** action assets and a Mapping Context must reach the controlled ship. Merely declaring actions or selecting Enhanced Input defaults is insufficient. Epic documents the action/context and possession lifecycle in the sources below.
- **Pose and slowdown depend on the movement contract:** use the independent world-axis rates before adding visual response or selective scaling. Otherwise a template's coupled flight behavior can obscure whether the required controls work.
- **Camera depends on committed progress:** use the actual ship X change from one source. It does not depend on bank/climb input and must not independently integrate speed. No fuel, score, or stage manager is required for this dependency.
- **Reset is part of each input feature:** integrate and verify Completed/Canceled behavior while building the controls, rather than treating input cleanup as optional polish.
- **No lateral boundary feature is added:** the active project requirements define editable altitude bounds. A lateral corridor clamp from the broad design is not a new acceptance requirement for this cut.

## MVP Definition

### Launch With (First Editor Cut)

- [ ] FLT-01: automatic world +X progress at the configured rate.
- [ ] FLT-02: separate steer/climb actions that work simultaneously.
- [ ] FLT-03: direct editable movement speeds and enforced minimum/maximum height.
- [ ] FLT-04: steer bank and climb pitch, easing toward level when input ends.
- [ ] FLT-05: editable slowdown ramp, 40% forward/lateral reduction, unchanged climb rate.
- [ ] FLT-06: clear control requests on release/cancellation, with no stuck movement or slowdown.
- [ ] FLT-07: exactly shared X progress and fixed camera Y/Z/rotation.
- [ ] FLT-08: build/load and observed Play In Editor control through the intended camera.

### Add After Validation / Future Consideration

There is no approved P2/P3 backlog in this research. Completion of flight controls does not authorize adding fortress features. The broader design remains a reference for a separately scoped future milestone; do not convert its build order into current requirements.

## Feature Prioritization Matrix

| Feature | User Value | Implementation Cost | Priority |
|---------|------------|---------------------|----------|
| FLT-01 automatic +X progress | HIGH | LOW | P1 |
| FLT-02 independent Y/Z controls | HIGH | MEDIUM | P1 |
| FLT-03 direct speed/height tuning | HIGH | LOW | P1 |
| FLT-04 pitch/bank and leveling | HIGH | MEDIUM | P1 |
| FLT-05 selective ramped slowdown | HIGH | MEDIUM | P1 |
| FLT-06 release/cancellation reset | HIGH | MEDIUM | P1 |
| FLT-07 synchronized fixed camera | HIGH | MEDIUM | P1 |
| FLT-08 usable PIE delivery | HIGH | MEDIUM | P1 |

**Priority key:** P1 is required for this approved cut. Cost is an estimate with MEDIUM confidence; runtime and binary asset integration remain open. Phase ordering can separate foundation, controls, and camera verification, but cannot drop any P1 feature from completion.

## User-Visible Verification

These are acceptance checks to execute during implementation, not claims that the current project passes. Editor transform inspection or temporary developer measurements may support observation; no permanent debug UI or test harness is a requested feature.

| Feature | Check in Unreal Editor / PIE | Passing Behavior |
|---------|-----------------------------|------------------|
| FLT-01 | Start PIE with no control held; compare ship X before/after a known interval. Repeat with a changed forward-speed value. | Positive X advance at the configured rate. Pose changes do not turn automatic movement into a different world direction. |
| FLT-02 | Steer each way, climb/descend, then steer and climb together. | Y/Z respond independently; simultaneous input does not reduce either rate through vector normalization. Automatic X continues. |
| FLT-03 | Change each speed in the chosen class/component tuning surface. Hold climb at maximum height and descent at minimum. | Edited values affect their rates; Z stays within the configured bounds. Moving away from either bound works. |
| FLT-04 | Observe steer, climb, descent, and neutral input; hold climb/descent at a height limit. | Steering visibly banks around the nose; climb/descent pitches appropriately. Neutral input eases toward level; stopped vertical motion at a height limit does not leave sustained climb pitch. |
| FLT-05 | Measure full-input rates after Slow has finished ramping; release Slow. Compare climb at equal input away from bounds. Change ramp timing and repeat. | Forward/lateral rates settle at 60% of baseline; climb matches baseline. Hold/release transitions follow editable ramp timing instead of snapping at normal nonzero settings. |
| FLT-06 | Release each held action; exercise action cancellation. Leave/re-enter PIE viewport focus with controls held where this produces cancellation or a reset path. | Lateral/climb requests clear, bank/pitch return toward level, and held-slow clears so recovery proceeds. No latched input survives the terminal/reset path. Actual viewport behavior must be checked, not assumed from engine defaults. |
| FLT-07 | Record ship/camera world transforms during ordinary travel, Slow ramp-in, full Slow, recovery, steer, climb, and bank/pitch. | `CameraX - ShipX` remains the initial offset within numeric inspection precision on each sampled update. Camera Y/Z and rotation retain initial values. No delayed follow, growing drift, or second scenery-scroll source appears. |
| FLT-08 | Load the intended map and start PIE using normal editor controls after a successful build/load. | Intended ship receives the actions and the game view uses the synchronized camera. Repeatable editor playability is the delivery result. |

## Reference Feature Analysis

The template's competitor section is adapted to scope/reference comparison. This is a developer prototype with an explicit approved cut; competitor surveying would not change its required controls.

| Feature | Broader First-Fortress Design | Approved Current Approach |
|---------|------------------------------|---------------------------|
| Forward/lateral/altitude movement | Independent arcade axes with direct speed tuning. | Preserve these controls and explicit editable altitude bounds. |
| Slow | Ramped 40% forward/lateral reduction plus fuel-drain penalty. | Preserve motion reduction/ramp only; exclude fuel and its penalty. |
| Camera | Fixed composition consuming forward progress. | Enforce exact X synchronization and fixed Y/Z/rotation. |
| Firing and cursor | Nose-aligned projectiles and scenery decal. | Excluded by explicit narrowing. |
| Fortress loop | Hazards, fuel, rewards, enemies, death, and completion. | Deferred; not dependencies of editor flight playability. |

## Confidence and Open Verification

Approved behavior is established by `.planning/PROJECT.md` and the parent task's user-approved scope. Official Epic API semantics are reused from the already completed STACK/ARCHITECTURE/PITFALLS research, checked on 2026-10-07; this targeted feature pass did not duplicate browsing or write cache sidecars. The code seam returned MEDIUM for verified websearch findings. Complexity, dependency ordering, and checks are project-specific planning inferences at that tier.

No feature is runtime-validated by this document. Existing Blueprint parent/component layout, player possession, camera selection, action mapping, viewport cancellation/reset behavior, and installed-engine compatibility need implementation-time verification. Exact speed values, height values, and pose/ramp feel are developer tunables, not recovered arcade constants. Initial camera composition is authored in the editor; this cut does not require choosing a new projection or adding camera controls.

## Sources

- [Approved project scope](../PROJECT.md) — authority for all active requirements and exclusions; current as of 2026-10-07.
- [First-fortress design](<C:/Users/Breno/My Drive/Vertex/{ Unreal AI }/Games/Vyxxen/Design/Vyxxen_First_Stage.md>) — retained movement/control behavior and longer-term reference; narrower approved scope takes precedence.
- [Stack research](STACK.md), [Architecture research](ARCHITECTURE.md), [Pitfalls research](PITFALLS.md) — reused official-source verification and local integration gaps, dated 2026-10-07.
- [Epic — Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine) — semantic actions, Mapping Contexts, action values and lifecycle; MEDIUM, previously checked 2026-10-07.
- [Epic — ETriggerEvent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/EnhancedInput/ETriggerEvent) — Completed/Canceled semantics; MEDIUM, previously checked 2026-10-07.
- [Epic — Pawn](https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine) — player-controllable actor/possession boundary; MEDIUM, previously checked 2026-10-07.
- [Epic — Properties](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-uproperties) — reflected editor tuning surface; MEDIUM, previously checked 2026-10-07.
- [Epic — Actor Ticking](https://dev.epicgames.com/documentation/en-us/unreal-engine/actor-ticking-in-unreal-engine) — ordered movement/camera updates; MEDIUM, previously checked 2026-10-07. Exact shared-progress behavior is Vyxxen's contract, not an automatic guarantee from this API.

---
*Feature research for Vyxxen's approved flight-and-camera cut. Only FEATURES.md was authored; no implementation, build, editor session, cache sidecars, or commit was performed.*
