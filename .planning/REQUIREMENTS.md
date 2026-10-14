# Requirements: Vyxxen

**Defined:** 2026-10-07
**Milestone:** First implementation cut — flight controls and synchronized camera
**Core Value:** The player can steer and climb a continuously advancing ship while a stable camera moves with exactly the same forward progress.

## v1 Requirements

### Flight Movement

- [ ] **MOVE-01**: The ship advances automatically along world +X without requiring player input.
- [ ] **MOVE-02**: The player can steer in both lateral directions along world Y while forward travel continues.
- [ ] **MOVE-03**: The player can climb and descend along world Z while forward travel continues.
- [ ] **MOVE-04**: The player can steer and climb simultaneously without either input reducing the other's configured rate through diagonal normalization.

### Direct Editor Tuning

- [ ] **TUNE-01**: The developer can edit forward, lateral, and climb speeds directly on the ship class or movement component, and those values control the corresponding movement rates.
- [ ] **TUNE-02**: The developer can edit minimum and maximum flight height on that class or component; the ship remains within those world-Z bounds and can move inward from either limit.

### Ship Pose

- [ ] **POSE-01**: The ship pitches with actual climb/descent and eases toward level when vertical movement stops, including at a height limit, without redirecting its world-axis translation.
- [ ] **POSE-02**: The ship banks around its nose when steering and eases toward level when steering stops, without redirecting forward travel.

### Hold-to-Slow

- [ ] **SLOW-01**: Holding Slow ramps forward and lateral rates toward 60% of their normal values; release ramps them back to normal, using editable ramp timing on the ship class or component.
- [ ] **SLOW-02**: Climb/descent speed remains unchanged throughout slowdown and recovery.

### Input Behavior

- [ ] **INPUT-01**: Steer, Climb, and Slow are semantic Enhanced Input actions with their device bindings authored in a Mapping Context rather than raw key checks in gameplay C++.
- [ ] **INPUT-02**: Releasing or canceling an action clears its stored control request; control/focus loss cannot leave steering, climb, or Slow latched when play resumes. Clearing Slow uses its normal recovery ramp.

### Route Camera

- [ ] **CAM-01**: The camera consumes the ship's actual forward X progress and preserves its initial camera-to-ship X offset through normal movement, slowdown, recovery, and simultaneous steering/climb, without independent speed accumulation or a delayed frame of following.
- [ ] **CAM-02**: The camera's world Y, world Z, and viewing rotation stay fixed at their authored starting values regardless of ship steering, altitude, pitch, or bank; scenery remains stationary.

### Editor Delivery

- [ ] **PLAY-01**: After a successful current editor build/load, normal Play In Editor startup in the intended MainGame map gives the player control of the intended ship and displays the synchronized route camera, with repeatable behavior after a fresh editor session.

## v2 Requirements

Future work is not part of the current roadmap and requires separate milestone scoping.

- **FUTURE-01**: Firing, projectiles, and a scenery aim cursor.
- **FUTURE-02**: Fuel drain, refueling airpods, destruction penalty, and slowdown's fuel-drain modifier.
- **FUTURE-03**: Enemies, hazards, damage, rewards, scoring, plane count, lives, and respawning.
- **FUTURE-04**: Fortress entry/exit, completion, and stage presentation.

These are longer-term design topics, not an authorization to implement or scaffold them now.

## Out of Scope

| Feature | Reason |
|---------|--------|
| Fire input and combat implementation | User explicitly deferred firing; the active cut is flight controls only. |
| Fuel/resource systems and slowdown resource cost | Deferred fortress systems are unnecessary to verify the flight control change. |
| Aim cursor, new HUD, altimeter, shadow subsystem, or extra visual effects | Outside the approved flight-and-camera cut. |
| Lateral corridor limits or fortress crash rules | User requested altitude bounds; adding other gameplay limits expands scope. |
| Packaged Windows build | User selected Play In Editor delivery. |
| Multiplayer, replication, behavior trees, or aerodynamic simulation | The current experience is a local arcade flight prototype. |
| Input-rebinding UI, settings subsystem, or mandatory data assets | The requested tuning interface is the ship class or component itself. |
| Moving the fortress terrain or camera smoothing/rotation tracking | Violates the fixed world and exactly synchronized X-only camera contract. |

## Verification and Definition of Done

Requirements remain Pending until implemented and verified. Research, asset existence, and generated binaries do not establish completion.

- Inspect the existing ship Blueprint and MainGame map before changing class inheritance, input, possession, or camera wiring. Preserve their authored content.
- Observe automatic forward motion, both steer directions, climb/descent, and simultaneous inputs in PIE. Changing each exposed speed must affect its corresponding rate.
- Exercise both altitude limits and movement away from them. Verify pitch returns toward level when actual climb is stopped.
- Compare settled rates at equal input away from altitude bounds: full slowdown must yield X/Y multipliers of 0.60 and a Z multiplier of 1.00. Verify hold/release ramps, mid-ramp changes, and input cancellation/focus recovery.
- Sample the actual ship and active camera transforms from the same update. `CameraX - ShipX` must remain the initial offset within normal floating-point measurement precision; camera Y/Z/rotation must remain unchanged. Equal average speed alone is insufficient.
- Record a current editor build/load and repeatable PIE startup/controls/view after saving affected assets and reopening the editor. Any inaccessible runtime verification must be reported as pending.
- Complete required plan/result checks and commit only the intended implementation and planning changes while preserving unrelated working changes.

## Traceability

Each v1 requirement is assigned to exactly one phase in ROADMAP.md. All remain Pending until implementation and verification.

| Requirement | Phase | Status |
|-------------|-------|--------|
| MOVE-01 | Phase 1 | Pending |
| MOVE-02 | Phase 1 | Pending |
| MOVE-03 | Phase 1 | Pending |
| MOVE-04 | Phase 1 | Pending |
| TUNE-01 | Phase 1 | Pending |
| TUNE-02 | Phase 1 | Pending |
| POSE-01 | Phase 1 | Pending |
| POSE-02 | Phase 1 | Pending |
| SLOW-01 | Phase 1 | Pending |
| SLOW-02 | Phase 1 | Pending |
| INPUT-01 | Phase 1 | Pending |
| INPUT-02 | Phase 1 | Pending |
| CAM-01 | Phase 2 | Pending |
| CAM-02 | Phase 2 | Pending |
| PLAY-01 | Phase 2 | Pending |

**Coverage:**
- v1 requirements: 15 total
- Mapped to phases: 15
- Unmapped: 0
- Duplicate assignments: 0

## Sources

- User scope: all ship flight controls; minimum/maximum height and speeds directly editable on class/component; camera at exactly the ship's speed along X.
- User clarification: defer firing; deliver through Unreal Editor; create PROJECT.md; use small playable phases, parallel independent work, committed planning, plan/result checks, and research first.
- `.planning/PROJECT.md` — agreed scope and constraints.
- `.planning/research/SUMMARY.md` and its four source reviews — technical guidance, dependencies, limitations, and suggested verification.
- `C:/Users/Breno/My Drive/Vertex/{ Unreal AI }/Games/Vyxxen/Design/Vyxxen_First_Stage.md` — pitch/bank, input model, selective 40% slowdown, and stationary scenery; broader systems are deferred by the user's narrowed scope.

---
*Requirements defined: 2026-10-07*
*Last updated: 2026-10-07 — approved requirements mapped to the initial roadmap; roadmap review pending*
